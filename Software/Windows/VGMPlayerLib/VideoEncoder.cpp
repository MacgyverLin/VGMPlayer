#include "VideoEncoder.h"
#include <assert.h>
#include <vector>

extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};

const char* get_err2str(int errnum)
{
	static char buf[AV_ERROR_MAX_STRING_SIZE] = { 0 };

	return av_make_error_string(buf, AV_ERROR_MAX_STRING_SIZE, errnum);
}


class VideoEncoderImpl
{
private:
	/////////////////////////////////////////////
	// audio
	AVStream* audio_st;
	AVCodec* audio_codec;
	double				audio_time;

	struct SwrContext* swr_ctx;
	float               t, tincr, tincr2;

	int                 max_dst_nb_samples;

	uint8_t** src_samples_data;
	int                 src_samples_linesize;
	int                 src_nb_samples;

	uint8_t** dst_samples_data;
	int                 dst_samples_linesize;
	int                 dst_samples_size;


	/////////////////////////////////////////////
	// video
	AVStream* video_st;
	AVCodec* video_codec;
	double				video_time;

	AVFrame* frame;
	AVPicture			src_picture, dst_picture;

	struct SwsContext* sws_ctx;

	////////////////////////////////////////////
	AVOutputFormat* fmt;
	AVFormatContext* oc;
private:

	/* Add an output stream. */
	AVStream* add_stream(AVFormatContext* oc, AVCodec** codec, enum AVCodecID codec_id, int width, int height)
	{
		AVCodecContext* c;
		AVStream* st;

		/* find the encoder */
		*codec = avcodec_find_encoder(codec_id);
		if (!(*codec))
		{
			vgm_log("Could not find encoder for '%s'\n", avcodec_get_name(codec_id));
			exit(1);
		}

		st = avformat_new_stream(oc, *codec);
		if (!st)
		{
			vgm_log("Could not allocate stream\n");
			exit(1);
		}

		st->id = oc->nb_streams - 1;
		c = st->codec;

		switch ((*codec)->type)
		{
		case AVMEDIA_TYPE_AUDIO:
			c->sample_fmt = AV_SAMPLE_FMT_S16;
			c->bit_rate = 64000;
			c->sample_rate = 44100;
			c->channels = 2;
			break;

		case AVMEDIA_TYPE_VIDEO:
			c->codec_id = codec_id;

			c->bit_rate = 400000;
			/* Resolution must be a multiple of two. */
			c->width = width;
			c->height = height;
			/* timebase: This is the fundamental unit of time (in seconds) in terms
			 * of which frame timestamps are represented. For fixed-fps content,
			 * timebase should be 1/framerate and timestamp increments should be
			 * identical to 1. */
			c->time_base.den = 43;
			c->time_base.num = 1;
			c->gop_size = 12; /* emit one intra frame every twelve frames at most */
			c->pix_fmt = AV_PIX_FMT_YUV420P;
			if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO)
			{
				/* just for testing, we also add B frames */
				c->max_b_frames = 2;
			}
			if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO)
			{
				/* Needed to avoid using macroblocks in which some coeffs overflow.
				 * This does not happen with normal video, it just happens here as
				 * the motion of the chroma plane does not match the luma plane. */
				c->mb_decision = 2;
			}
			break;

		default:
			break;
		}

		/* Some formats want stream headers to be separate. */
		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
			c->flags |= CODEC_FLAG_GLOBAL_HEADER;

		return st;
	}

	/**************************************************************/
	/* audio output */
	void open_audio(AVFormatContext* oc, AVCodec* codec, AVStream* st)
	{
		AVCodecContext* c;
		int ret;

		c = st->codec;

		/* open it */
		ret = avcodec_open2(c, codec, NULL);
		if (ret < 0)
		{
			vgm_log("Could not open audio codec: %s\n", get_err2str(ret));
			exit(1);
		}

		/* init signal generator */
		t = 0;
		tincr = 2 * M_PI * 110.0 / c->sample_rate;
		/* increment frequency by 110 Hz per second */
		tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

		src_nb_samples = c->codec->capabilities& CODEC_CAP_VARIABLE_FRAME_SIZE ? 10000 : c->frame_size;

		ret = av_samples_alloc_array_and_samples(&src_samples_data, &src_samples_linesize, c->channels, src_nb_samples, c->sample_fmt, 0);
		if (ret < 0)
		{
			vgm_log("Could not allocate source samples\n");
			exit(1);
		}

		/* create resampler context */
		if (c->sample_fmt != AV_SAMPLE_FMT_S16)
		{
			swr_ctx = swr_alloc();
			if (!swr_ctx)
			{
				vgm_log("Could not allocate resampler context\n");
				exit(1);
			}

			/* set options */
			av_opt_set_int(swr_ctx, "in_channel_count", c->channels, 0);
			av_opt_set_int(swr_ctx, "in_sample_rate", c->sample_rate, 0);
			av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
			av_opt_set_int(swr_ctx, "out_channel_count", c->channels, 0);
			av_opt_set_int(swr_ctx, "out_sample_rate", c->sample_rate, 0);
			av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", c->sample_fmt, 0);

			/* initialize the resampling context */
			if ((ret = swr_init(swr_ctx)) < 0)
			{
				vgm_log("Failed to initialize the resampling context\n");
				exit(1);
			}
		}

		/* compute the number of converted samples: buffering is avoided
		 * ensuring that the output buffer will contain at least all the
		 * converted input samples */
		max_dst_nb_samples = src_nb_samples;
		ret = av_samples_alloc_array_and_samples(&dst_samples_data, &dst_samples_linesize, c->channels, max_dst_nb_samples, c->sample_fmt, 0);
		if (ret < 0)
		{
			vgm_log("Could not allocate destination samples\n");
			exit(1);
		}
		dst_samples_size = av_samples_get_buffer_size(NULL, c->channels, max_dst_nb_samples, c->sample_fmt, 0);
	}



	void write_audio_frame(AVFormatContext* oc, AVStream* st, const std::vector<s16>& audioBuffer)
	{
		AVCodecContext* c;
		AVPacket pkt = { 0 }; // data and size must be 0;
		AVFrame* frame = avcodec_alloc_frame();
		int got_packet, ret, dst_nb_samples;

		av_init_packet(&pkt);
		c = st->codec;

		// get_audio_frame((int16_t*)src_samples_data[0], src_nb_samples, c->channels);
		memcpy((int16_t*)src_samples_data[0], &audioBuffer[0], audioBuffer.size() * sizeof(s16));

		/* convert samples from native format to destination codec format, using the resampler */
		if (swr_ctx)
		{
			/* compute destination number of samples */
			dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, c->sample_rate) + src_nb_samples, c->sample_rate, c->sample_rate, AV_ROUND_UP);
			if (dst_nb_samples > max_dst_nb_samples)
			{
				av_free(dst_samples_data[0]);
				ret = av_samples_alloc(dst_samples_data, &dst_samples_linesize, c->channels, dst_nb_samples, c->sample_fmt, 0);
				if (ret < 0)
					exit(1);
				max_dst_nb_samples = dst_nb_samples;
				dst_samples_size = av_samples_get_buffer_size(NULL, c->channels, dst_nb_samples, c->sample_fmt, 0);
			}

			/* convert to destination format */
			ret = swr_convert(swr_ctx, dst_samples_data, dst_nb_samples, (const uint8_t**)src_samples_data, src_nb_samples);
			if (ret < 0)
			{
				vgm_log("Error while converting\n");
				exit(1);
			}
		}
		else
		{
			dst_samples_data[0] = src_samples_data[0];
			dst_nb_samples = src_nb_samples;
		}

		frame->nb_samples = dst_nb_samples;
		avcodec_fill_audio_frame(frame, c->channels, c->sample_fmt, dst_samples_data[0], dst_samples_size, 0);

		ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
		if (ret < 0)
		{
			vgm_log("Error encoding audio frame: %s\n", get_err2str(ret));
			exit(1);
		}

		if (!got_packet)
			return;

		pkt.stream_index = st->index;

		/* Write the compressed frame to the media file. */
		ret = av_interleaved_write_frame(oc, &pkt);
		if (ret != 0)
		{
			vgm_log("Error while writing audio frame: %s\n", get_err2str(ret));
			exit(1);
		}
		avcodec_free_frame(&frame);
	}

	void close_audio(AVFormatContext* oc, AVStream* st)
	{
		avcodec_close(st->codec);
		if (swr_ctx)
			av_free(dst_samples_data[0]);

		av_free(src_samples_data[0]);
	}

	/**************************************************************/
	/* video output */
	void open_video(AVFormatContext* oc, AVCodec* codec, AVStream* st)
	{
		int ret;
		AVCodecContext* c = st->codec;

		/* open the codec */
		ret = avcodec_open2(c, codec, NULL);
		if (ret < 0)
		{
			vgm_log("Could not open video codec: %s\n", get_err2str(ret));
			exit(1);
		}

		/* allocate and init a re-usable frame */
		frame = avcodec_alloc_frame();
		if (!frame)
		{
			vgm_log("Could not allocate video frame\n");
			exit(1);
		}

		/* Allocate the encoded raw picture. */
		ret = avpicture_alloc(&dst_picture, c->pix_fmt, c->width, c->height);
		if (ret < 0)
		{
			vgm_log("Could not allocate picture: %s\n", get_err2str(ret));
			exit(1);
		}

		/* If the output format is not YUV420P, then a temporary YUV420P
		 * picture is needed too. It is then converted to the required
		 * output format. */
		ret = avpicture_alloc(&src_picture, AV_PIX_FMT_RGB24, c->width, c->height);
		if (ret < 0)
		{
			vgm_log("Could not allocate temporary picture: %s\n", get_err2str(ret));
			exit(1);
		}

		/* copy data and linesize picture pointers to frame */
		*((AVPicture*)frame) = dst_picture;


			sws_ctx = sws_getContext(c->width, c->height, AV_PIX_FMT_RGB24, c->width, c->height, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
			if (!sws_ctx)
			{
				vgm_log("Could not initialize the conversion context\n");
				exit(1);
			}
	}

	void write_video_frame(AVFormatContext* oc, AVStream* st, const Vector<unsigned char>& videoBuffer)
	{
		int ret;
		AVCodecContext* c = st->codec;

		int w = c->width * 3;
		for (int y = 0; y < c->height; y++)
		{
			int srcStartIdx = (y                ) * w;
			int dstStartIdx = (c->height - 1 - y) * w;

			memcpy(&src_picture.data[0][srcStartIdx], &videoBuffer[dstStartIdx], w);
		}
		// memcpy(src_picture.data[0], &videoBuffer[0], videoBuffer.size());
		sws_scale(sws_ctx, (const uint8_t* const*)src_picture.data, src_picture.linesize, 0, c->height, dst_picture.data, dst_picture.linesize);

		if (oc->oformat->flags & AVFMT_RAWPICTURE)
		{
			/* Raw video case - directly store the picture in the packet */
			AVPacket pkt;
			av_init_packet(&pkt);

			pkt.flags |= AV_PKT_FLAG_KEY;
			pkt.stream_index = st->index;
			pkt.data = dst_picture.data[0];
			pkt.size = sizeof(AVPicture);

			ret = av_interleaved_write_frame(oc, &pkt);
		}
		else
		{
			AVPacket pkt = { 0 };
			int got_packet;
			av_init_packet(&pkt);

			/* encode the image */
			ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
			if (ret < 0) 
			{
				vgm_log("Error encoding video frame: %s\n", get_err2str(ret));
				exit(1);
			}
			/* If size is zero, it means the image was buffered. */

			if (!ret && got_packet && pkt.size) 
			{
				pkt.stream_index = st->index;

				/* Write the compressed frame to the media file. */
				ret = av_interleaved_write_frame(oc, &pkt);
			}
			else
			{
				ret = 0;
			}
		}

		if (ret != 0) 
		{
			vgm_log("Error while writing video frame: %s\n", get_err2str(ret));
			exit(1);
		}
	}

	void close_video(AVFormatContext* oc, AVStream* st)
	{
		av_free(src_picture.data[0]);
		av_free(dst_picture.data[0]);
		av_free(frame);

		avcodec_close(st->codec);
	}
public:
	VideoEncoderImpl()
	{
		audio_st = nullptr;
		audio_codec = nullptr;
		audio_time = 0;

		swr_ctx = nullptr;
		t = 0; 
		tincr = 0; 
		tincr2 = 0;

		max_dst_nb_samples = 0;

		src_samples_data = nullptr;
		src_samples_linesize = 0;
		src_nb_samples = 0;

		dst_samples_data = nullptr;
		dst_samples_linesize = 0;
		dst_samples_size = 0;

		/////////////////////////////////////////////
		// video
		video_st = nullptr;
		video_codec = nullptr;
		video_time = 0;

		frame = nullptr;

		sws_ctx = nullptr;
		
		////////////////////////////////////////////
		fmt = nullptr;
		oc = nullptr;
	}

	~VideoEncoderImpl()
	{
	}

	bool Initialize(const char* filename, int width, int height)
	{
		int ret;

		/* Initialize libavcodec, and register all codecs and formats. */
		av_register_all();

		/* allocate the output media context */
		avformat_alloc_output_context2(&oc, NULL, NULL, filename);
		if (!oc) {
			printf("Could not deduce output format from file extension: using MPEG.\n");
			avformat_alloc_output_context2(&oc, NULL, "mp4", filename);
		}
		if (!oc) {
			return false;
		}
		fmt = oc->oformat;

		/* Add the audio and video streams using the default format codecs
		 * and initialize the codecs. */
		video_st = NULL;
		audio_st = NULL;

		if (fmt->video_codec != AV_CODEC_ID_NONE) {
			video_st = add_stream(oc, &video_codec, fmt->video_codec, width, height);
		}
		if (fmt->audio_codec != AV_CODEC_ID_NONE) {
			audio_st = add_stream(oc, &audio_codec, fmt->audio_codec, 0, 0);
		}

		/* Now that all the parameters are set, we can open the audio and
		 * video codecs and allocate the necessary encode buffers. */
		if (video_st)
			open_video(oc, video_codec, video_st);
		if (audio_st)
			open_audio(oc, audio_codec, audio_st);

		av_dump_format(oc, 0, filename, 1);

		/* open the output file, if needed */
		if (!(fmt->flags & AVFMT_NOFILE)) 
		{
			ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
			if (ret < 0) 
			{
				vgm_log("Could not open '%s': %s\n", filename, get_err2str(ret));
				return false;
			}
		}

		/* Write the stream header, if any. */
		ret = avformat_write_header(oc, NULL);
		if (ret < 0) 
		{
			fprintf(stderr, "Error occurred when opening output file: %s\n", get_err2str(ret));
			return false;
		}

		if (frame)
			frame->pts = 0;

		return true;
	}

	bool Update(const Vector<unsigned char>& videoBuffer, const std::vector<s16>& audioBuffer)
	{
		/* Compute current audio and video time. */
		audio_time = audio_st ? audio_st->pts.val * av_q2d(audio_st->time_base) : 0.0;
		video_time = video_st ? video_st->pts.val * av_q2d(video_st->time_base) : 0.0;

		//printf("%f %f\n", video_time, audio_time);
		/*
		if ((!audio_st || audio_time >= STREAM_DURATION) &&
			(!video_st || video_time >= STREAM_DURATION))
			return false;
		*/

		if ((!audio_st) && (!video_st))
			return false;


		/* write interleaved audio and video frames */
		//if (!video_st || (video_st && audio_st && audio_time < video_time))
		// {
			write_audio_frame(oc, audio_st, audioBuffer);
			// }
		// else
		// {
			write_video_frame(oc, video_st, videoBuffer);
	
			frame->pts += av_rescale_q(1, video_st->codec->time_base, video_st->time_base);

			//frame->pts += av_rescale_q(1, video_st->codec->time_base, video_st->time_base);
			// }
		return true;
	}

	void Terminate()
	{
		/* Write the trailer, if any. The trailer must be written before you
		 * close the CodecContexts open when you wrote the header; otherwise
		 * av_write_trailer() may try to use memory that was freed on
		 * av_codec_close(). */
		av_write_trailer(oc);

		/* Close each codec. */
		if (video_st)
			close_video(oc, video_st);
		if (audio_st)
			close_audio(oc, audio_st);

		if (!(fmt->flags & AVFMT_NOFILE))
			/* Close the output file. */
			avio_close(oc->pb);

		/* free the stream */
		avformat_free_context(oc);
	}
private:
};


VideoEncoder::VideoEncoder()
{
	impl = new VideoEncoderImpl();
}

VideoEncoder::~VideoEncoder()
{
	if (impl)
	{
		delete impl;
		impl = nullptr;
	}
}

bool VideoEncoder::Initiate(const char* filename, int width, int height)
{
	assert(impl);

	if (!impl->Initialize(filename, width, height))
		return false;

	return true;
}

bool VideoEncoder::Update(const Vector<unsigned char>& videoBuffer, const std::vector<s16>& audioBuffer)
{
	assert(impl);

	if (!impl->Update(videoBuffer, audioBuffer))
		return false;

	return true;
}

void VideoEncoder::Terminate()
{
	assert(impl);

	impl->Terminate();
}