#include "VGMMultiChannelNoteViewer.h"
#include "FFT.h"
#include <GL/glu.h>
#include <GL/gl.h>

VGMMultiChannelNoteViewer::VGMMultiChannelNoteViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_, float waveScale_, const Skin& skin_)
	: VGMDataObverser()
	, name(name_)
	, x(x_)
	, y(y_)
	, width(width_)
	, height(height_)
	, waveScale(waveScale_)
	, skin(skin_)
{
	maxLeft.resize(skin.numColumns * 2);
	maxRight.resize(skin.numColumns * 2);
}

VGMMultiChannelNoteViewer::~VGMMultiChannelNoteViewer()
{
}

void VGMMultiChannelNoteViewer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteViewer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.Open(name.c_str(), x, y, width, height);
	texture.Load(info.texturePath);
}

void VGMMultiChannelNoteViewer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.Close();
}

void VGMMultiChannelNoteViewer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteViewer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteViewer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteViewer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteViewer::OnNotifyUpdate(Obserable& observable)
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

		videoDevice.MakeCurrent();
		videoDevice.Clear(Color(0, 0, 0, 1));


		/////////////////////////////////////////////////////////////////////
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height);
		glMatrixMode(GL_MODELVIEW);

		glViewport(0, 0, width, height);
		glEnable(GL_BLEND);
		videoDevice.DrawSolidRectangle
		(
			Vector2(width, 0), Color(1.0f, 1.0f, 1.0f, 0.1f),
			Vector2(0, 0), Color(1.0f, 1.0f, 1.0f, 0.1f),
			Vector2(0, height), Color(1.0f, 1.0f, 1.0f, 0.2f),
			Vector2(width, height), Color(1.0f, 1.0f, 1.0f, 0.2f)
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

		glViewport(0, 0, width, height);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Color topColor = skin.leftColor; topColor.a = 0.3f;
		Color bottomColor = skin.leftColor; bottomColor.a = 0.9f;
		for (int ch = 0; ch < channelsNotes.size(); ch++)
		{
			int viewportWidth = width / (systemChannels.GetChannelsCount());
			glViewport(viewportWidth * ch, 0, viewportWidth, height);

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

		videoDevice.Flush();
	}
}