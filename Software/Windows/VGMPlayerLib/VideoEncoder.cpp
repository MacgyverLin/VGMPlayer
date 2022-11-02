#include "VideoEncoder.h"
#include <assert.h>

// #define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavformat/avformat.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavformat/avformat.h>
#ifdef __cplusplus
};
#endif
#endif

/*
FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
"h264_mp4toannexb" bitstream filter (BSF)
  *Add SPS,PPS in front of IDR frame
  *Add start code ("0,0,0,1") in front of NALU
H.264 in some container (MPEG2TS) don't need this BSF.
*/
//'1': Use H.264 Bitstream Filter 
#define USE_H264BSF 0

/*
FIX:AAC in some container format (FLV, MP4, MKV etc.) need
"aac_adtstoasc" bitstream filter (BSF)
*/
//'1': Use AAC Bitstream Filter 
#define USE_AACBSF 0


class VideoEncoderImpl
{
public:
	VideoEncoderImpl()
	{
		ofmt = NULL;
		//Input AVFormatContext and Output AVFormatContext
		ifmt_ctx_v = NULL;
		ifmt_ctx_a = NULL;
		ofmt_ctx = NULL;

		//memset(&pkt, 0, sizeof(AVPacket));
		//int ret, i;
		videoindex_v = -1;
		videoindex_out = -1;
		audioindex_a = -1;
		audioindex_out = -1;
		frame_index = 0;
		cur_pts_v = 0;
		cur_pts_a = 0;

		//const char *in_filename_v = "cuc_ieschool.ts";//Input file URL
		in_filename_v = "cuc_ieschool.h264";
		//const char *in_filename_a = "cuc_ieschool.mp3";
		//const char *in_filename_a = "gowest.m4a";
		//const char *in_filename_a = "gowest.aac";
		in_filename_a = "huoyuanjia.mp3";

		out_filename = "cuc_ieschool.mp4";//Output file URL
	}

	~VideoEncoderImpl()
	{
	}

	AVOutputFormat* ofmt;
	//Input AVFormatContext and Output AVFormatContext
	AVFormatContext* ifmt_ctx_v, *ifmt_ctx_a, *ofmt_ctx;
	AVPacket pkt;

	int videoindex_v, videoindex_out;
	int audioindex_a, audioindex_out;
	int frame_index;
	int64_t cur_pts_v, cur_pts_a;

	const char* in_filename_v;
	const char* in_filename_a;

	const char* out_filename;
};

VideoEncoderImpl* ffmpeg_initialize()
{
	VideoEncoderImpl* impl = new VideoEncoderImpl();
	int ret, i;

	av_register_all();
	//Input
	if ((ret = avformat_open_input(&impl->ifmt_ctx_v, impl->in_filename_v, 0, 0)) < 0) {
		printf("Could not open input file.");
		return nullptr;
	}
	if ((ret = avformat_find_stream_info(impl->ifmt_ctx_v, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		return nullptr;
	}

	if ((ret = avformat_open_input(&impl->ifmt_ctx_a, impl->in_filename_a, 0, 0)) < 0) {
		printf("Could not open input file.");
		return nullptr;
	}
	if ((ret = avformat_find_stream_info(impl->ifmt_ctx_a, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		return nullptr;
	}
	printf("===========Input Information==========\n");
	av_dump_format(impl->ifmt_ctx_v, 0, impl->in_filename_v, 0);
	av_dump_format(impl->ifmt_ctx_a, 0, impl->in_filename_a, 0);
	printf("======================================\n");
	//Output
	avformat_alloc_output_context2(&impl->ofmt_ctx, NULL, NULL, impl->out_filename);
	if (!impl->ofmt_ctx) {
		printf("Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		return nullptr;
	}
	impl->ofmt = impl->ofmt_ctx->oformat;

	for (i = 0; i < impl->ifmt_ctx_v->nb_streams; i++) 
	{
		//Create output AVStream according to input AVStream
		if (impl->ifmt_ctx_v->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			AVStream* in_stream = impl->ifmt_ctx_v->streams[i];
			AVStream* out_stream = avformat_new_stream(impl->ofmt_ctx, in_stream->codec->codec);
			impl->videoindex_v = i;
			if (!out_stream) 
			{
				printf("Failed allocating output stream\n");
				ret = AVERROR_UNKNOWN;
				
				return nullptr;
			}
			impl->videoindex_out = out_stream->index;
			//Copy the settings of AVCodecContext
			if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
				printf("Failed to copy context from input to output stream codec context\n");
				
				return nullptr;
			}
			out_stream->codec->codec_tag = 0;
			if (impl->ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
				out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
			break;
		}
	}

	for (i = 0; i < impl->ifmt_ctx_a->nb_streams; i++) 
	{
		//Create output AVStream according to input AVStream
		if (impl->ifmt_ctx_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			AVStream* in_stream = impl->ifmt_ctx_a->streams[i];
			AVStream* out_stream = avformat_new_stream(impl->ofmt_ctx, in_stream->codec->codec);
			impl->audioindex_a = i;
			if (!out_stream) 
			{
				printf("Failed allocating output stream\n");
				ret = AVERROR_UNKNOWN;
				return nullptr;
			}
			impl->audioindex_out = out_stream->index;
			//Copy the settings of AVCodecContext
			if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
				printf("Failed to copy context from input to output stream codec context\n");
				return nullptr;
			}
			out_stream->codec->codec_tag = 0;
			if (impl->ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
				out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

			break;
		}
	}

	printf("==========Output Information==========\n");
	av_dump_format(impl->ofmt_ctx, 0, impl->out_filename, 1);
	printf("======================================\n");
	//Open output file
	if (!(impl->ofmt->flags & AVFMT_NOFILE)) {
		if (avio_open(&impl->ofmt_ctx->pb, impl->out_filename, AVIO_FLAG_WRITE) < 0) {
			printf("Could not open output file '%s'", impl->out_filename);
			return nullptr;
		}
	}
	//Write file header
	if (avformat_write_header(impl->ofmt_ctx, NULL) < 0) {
		printf("Error occurred when opening output file\n");
		return nullptr;
	}

	//FIX
#if USE_H264BSF
	AVBitStreamFilterContext* h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
	AVBitStreamFilterContext* aacbsfc = av_bitstream_filter_init("aac_adtstoasc");
#endif
	return impl;
}

bool ffmpeg_update(VideoEncoderImpl* impl)
{
	AVFormatContext* ifmt_ctx;
	int stream_index = 0;
	AVStream* in_stream, * out_stream;

	//Get an AVPacket
	if (av_compare_ts(impl->cur_pts_v, impl->ifmt_ctx_v->streams[impl->videoindex_v]->time_base, impl->cur_pts_a, impl->ifmt_ctx_a->streams[impl->audioindex_a]->time_base) <= 0) 
	{
		ifmt_ctx = impl->ifmt_ctx_v;
		stream_index = impl->videoindex_out;

		if (av_read_frame(ifmt_ctx, &impl->pkt) >= 0) 
		{
			do 
			{
				in_stream = ifmt_ctx->streams[impl->pkt.stream_index];
				out_stream = impl->ofmt_ctx->streams[stream_index];

				if (impl->pkt.stream_index == impl->videoindex_v) {
					//FIX£ºNo PTS (Example: Raw H.264)
					//Simple Write PTS
					if (impl->pkt.pts == AV_NOPTS_VALUE) {
						//Write PTS
						AVRational time_base1 = in_stream->time_base;
						//Duration between 2 frames (us)
						int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
						//Parameters
						impl->pkt.pts = (double)(impl->frame_index * calc_duration) / (double)(av_q2d(time_base1) * AV_TIME_BASE);
						impl->pkt.dts = impl->pkt.pts;
						impl->pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1) * AV_TIME_BASE);
						impl->frame_index++;
					}

					impl->cur_pts_v = impl->pkt.pts;
					break;
				}
			} while (av_read_frame(ifmt_ctx, &impl->pkt) >= 0);
		}
		else 
		{
			return false;
		}
	}
	else 
	{
		ifmt_ctx = impl->ifmt_ctx_a;
		stream_index = impl->audioindex_out;
		if (av_read_frame(ifmt_ctx, &impl->pkt) >= 0) 
		{
			do 
			{
				in_stream = ifmt_ctx->streams[impl->pkt.stream_index];
				out_stream = impl->ofmt_ctx->streams[stream_index];

				if (impl->pkt.stream_index == impl->audioindex_a)
				{
					//FIX£ºNo PTS
					//Simple Write PTS
					if (impl->pkt.pts == AV_NOPTS_VALUE) {
						//Write PTS
						AVRational time_base1 = in_stream->time_base;
						//Duration between 2 frames (us)
						int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
						//Parameters
						impl->pkt.pts = (double)(impl->frame_index * calc_duration) / (double)(av_q2d(time_base1) * AV_TIME_BASE);
						impl->pkt.dts = impl->pkt.pts;
						impl->pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1) * AV_TIME_BASE);
						impl->frame_index++;
					}
					impl->cur_pts_a = impl->pkt.pts;

					break;
				}
			} while (av_read_frame(ifmt_ctx, &impl->pkt) >= 0);
		}
		else 
		{
			return false;
		}
	}

	//FIX:Bitstream Filter
#if USE_H264BSF
	av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
#if USE_AACBSF
	av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif

	//Convert PTS/DTS
	impl->pkt.pts = av_rescale_q_rnd(impl->pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	impl->pkt.dts = av_rescale_q_rnd(impl->pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	impl->pkt.duration = av_rescale_q(impl->pkt.duration, in_stream->time_base, out_stream->time_base);
	impl->pkt.pos = -1;
	impl->pkt.stream_index = stream_index;

	printf("Write 1 Packet. size:%5d\tpts:%lld\n", impl->pkt.size, impl->pkt.pts);
	//Write
	if (av_interleaved_write_frame(impl->ofmt_ctx, &impl->pkt) < 0) 
	{
		printf("Error muxing packet\n");
		return false;
	}

	av_free_packet(&impl->pkt);

	return true;
}

void ffmpeg_terminate(VideoEncoderImpl* impl)
{
	int ret, i;

	//Write file trailer
	av_write_trailer(impl->ofmt_ctx);

#if USE_H264BSF
	av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
	av_bitstream_filter_close(aacbsfc);
#endif

	avformat_close_input(&impl->ifmt_ctx_v);
	avformat_close_input(&impl->ifmt_ctx_a);
	/* close output */
	if (impl->ofmt_ctx && !(impl->ofmt->flags & AVFMT_NOFILE))
		avio_close(impl->ofmt_ctx->pb);

	avformat_free_context(impl->ofmt_ctx);
	
	delete impl;
	impl = nullptr;
}

int test()
{
	VideoEncoderImpl* impl = ffmpeg_initialize();
	if (!impl)
		return false;

	while (ffmpeg_update(impl));

	ffmpeg_terminate(impl);
}


VideoEncoder::VideoEncoder()
{
	impl = new VideoEncoderImpl();

	test();
}

VideoEncoder::~VideoEncoder()
{
    Terminate();

	if (impl)
	{
		delete impl;
		impl = nullptr;
	}
}

bool VideoEncoder::Initiate(const char* filename)
{
	assert(impl);

    return true;
}

bool VideoEncoder::AddFrame(const char* imageBuffer, int imageBufferSize, const char* soundBuffer, int soundBufferSize)
{
    assert(impl);

    return true;
}

void VideoEncoder::Terminate()
{
    assert(impl);
}