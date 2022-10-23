#include "VGMMultiChannelNoteRenderer.h"
#include "FFT.h"
#include <GL/glu.h>
#include <GL/gl.h>

VGMMultiChannelNoteRenderer::VGMMultiChannelNoteRenderer(const char* name_, u32 x_, u32 y_, u32 width_, u32 height_, float waveScale_, const VGMMultiChannelNoteRenderer::Skin& skin_)
	: VGMRenderer(name_, x_, y_, width_, height_)
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

	texture.Load(info.texturePath);
}

void VGMMultiChannelNoteRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();
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
		if (channelsNotes.size() != systemChannels.GetChannelsCount())
			channelsNotes.resize(systemChannels.GetChannelsCount());

		for (int ch = 0; ch < systemChannels.GetChannelsCount() ; ch++)
		{
			const std::vector<VGMData::Channel::Note>& outputNotes = systemChannels.GetOutputNotes(ch);
			channelsNotes[ch].insert(channelsNotes[ch].end(), outputNotes.begin(), outputNotes.end());
		}

		/////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDevice::Constant::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, 1, 0, 1);
		videoDevice.MatrixMode(VideoDevice::Constant::MODELVIEW);

		/////////////////////////////////////////////////////////////////////
		SetViewport(0, 0, 1, 1);

		videoDevice.Disable(VideoDevice::Constant::BLEND);
		videoDevice.DrawSolidRectangle
		(
			Vector2(1, 0), Color(0.0f, 0.0f, 0.0f, 0.1f),
			Vector2(0, 0), Color(0.0f, 0.0f, 0.0f, 0.1f),
			Vector2(0, 1), Color(0.0f, 0.0f, 0.0f, 0.2f),
			Vector2(1, 1), Color(0.0f, 0.0f, 0.0f, 0.2f)
		);

		/////////////////////////////////////////////////////////////////////
		int startX = 0;
		int endX = 32;
		int startY = 0;
		int endY = VGM_FRAME_PER_SECOND * 5;
		f32 frameCounter = vgmData.GetFrameCounter();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(startX, endX, startY + frameCounter, endY + frameCounter);
		glMatrixMode(GL_MODELVIEW);

		SetViewport(0, 0, 1, 1);
		videoDevice.Enable(VideoDevice::Constant::BLEND);
		videoDevice.BlendFunc(VideoDevice::Constant::SRC_ALPHA, VideoDevice::Constant::ONE_MINUS_SRC_ALPHA);

		Color topColor = skin.leftColor; topColor.a = 0.3f;
		Color bottomColor = skin.leftColor; bottomColor.a = 0.9f;
		for (int ch = 0; ch < channelsNotes.size(); ch++)
		{
			float channelViewportWidth = 1.0f / systemChannels.GetChannelsCount();

			SetViewport(channelViewportWidth * ch, 0.0f, channelViewportWidth, 1.0f);

			for (s32 i = 0; i< channelsNotes[ch].size(); i++)
			{
				f32 y = channelsNotes[ch][i].frame + endY;
				// 30.86 	61.73 	123.46 	246.92 	493.84 	987.67 	1975.34 	3950.68 	7901.36 	15802.72 	31605.44 
				s32 x = channelsNotes[ch][i].frequency / 987.67 * (endX - startX);

				videoDevice.DrawSolidRectangle
				(
					Vector2(x + 1, y), topColor,
					Vector2(x + 0, y), topColor,
					Vector2(x + 0, y + 1), topColor,
					Vector2(x + 1, y + 1), topColor
				);
			}
		}
	}
}