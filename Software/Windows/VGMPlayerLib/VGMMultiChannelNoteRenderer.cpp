#include "VGMMultiChannelNoteRenderer.h"
#include "FFT.h"

VGMMultiChannelNoteRenderer::VGMMultiChannelNoteRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMMultiChannelNoteRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, waveScale(waveScale_)
	, skin(skin_)
{
}

VGMMultiChannelNoteRenderer::~VGMMultiChannelNoteRenderer()
{
}

void VGMMultiChannelNoteRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	font = videoDevice.CreateFont("arial.ttf", 24);
}

void VGMMultiChannelNoteRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.DestroyFont(font);
}

void VGMMultiChannelNoteRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		/////////////////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();

		Rect region = GetRegion();
		videoDevice.Ortho2D(0, region.w, 0, region.h);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);
		SetViewport(0, 0, 1, 1);

		videoDevice.SetFont(font);
		videoDevice.SetFontColor(Color::White);
		videoDevice.SetFontScale(1.0f);
		videoDevice.DrawText("fuck you", 0, 0);
		videoDevice.DrawText("fuck you", 0, 20);

		/////////////////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, 1, 0, 1);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		SetViewport(0, 0, 1, 1);

		videoDevice.Disable(VideoDeviceEnum::BLEND);
		videoDevice.DrawWireRectangle
		(
			Vector2(0, 0), Color::Grey,
			Vector2(1, 0), Color::Grey,
			Vector2(1, 1), Color::Grey,
			Vector2(0, 1), Color::Grey
		);

		/////////////////////////////////////////////////////////////////////////////////
		if (channelsNotes.size() != systemChannels.GetChannelsCount())
			channelsNotes.resize(systemChannels.GetChannelsCount());

		for (int ch = 0; ch < systemChannels.GetChannelsCount() ; ch++)
		{
			const std::vector<VGMData::Channel::Note>& outputNotes = systemChannels.GetOutputNotes(ch);
			channelsNotes[ch].insert(channelsNotes[ch].end(), outputNotes.begin(), outputNotes.end());
		}

		/////////////////////////////////////////////////////////////////////
		int startX = 0;
		int endX = 32;
		int startY = 0;
		int endY = VGM_FRAME_PER_SECOND;
		f32 staffHeight = (endY - startY) * 0.01;
		f32 frameCounter = vgmData.GetFrameCounter();

		Color topColor = Color::Cyan;
		Color bottomColor = Color::Black;
		Color staffColor = Color::Yellow;

		videoDevice.Enable(VideoDeviceEnum::BLEND);
		videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE_MINUS_SRC_ALPHA);

		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, 1, 0, 1);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		int channelCount = 1;// systemChannels.GetChannelsCount();
		for (int ch = 0; ch < channelCount; ch++)
		{
			float channelViewportWidth = 1.0f / channelCount;
			SetViewport(channelViewportWidth * ch, 0.0f, channelViewportWidth, 1.0f);

			// float channelViewportHeight = 1.0f / systemChannels.GetChannelsCount();
			// SetViewport(0.0f, channelViewportHeight * ch, 1.0f, channelViewportHeight);

			videoDevice.DrawWireRectangle
			(
				Vector2(1, 0), bottomColor,
				Vector2(0, 0), bottomColor,
				Vector2(0, 1), topColor,
				Vector2(1, 1), topColor
			);

			videoDevice.Disable(VideoDeviceEnum::BLEND);

			videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
			videoDevice.LoadIdentity();
			videoDevice.Ortho2D(startX, endX, startY + frameCounter, endY + frameCounter);
			videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);
			videoDevice.Enable(VideoDeviceEnum::BLEND);

			for (s32 i = 0; i< channelsNotes[ch].size(); i++)
			{
				// 30.86 	61.73 	123.46 	246.92 	493.84 	987.67 	1975.34 	3950.68 	7901.36 	15802.72 	31605.44 
				f32 x = floor(channelsNotes[ch][i].frequency / 1000.0f) * (endX - startX);
				f32 y = floor(channelsNotes[ch][i].frame) + endY;

				videoDevice.DrawSolidRectangle
				(
					Vector2(x + 1, y + 0), staffColor,
					Vector2(x + 0, y + 0), staffColor,
					Vector2(x + 0, y + staffHeight), staffColor,
					Vector2(x + 1, y + staffHeight), staffColor
				);
			}
		}
	}
}