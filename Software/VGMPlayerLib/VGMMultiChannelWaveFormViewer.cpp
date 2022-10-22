#include "VGMMultiChannelWaveFormViewer.h"
#include <GL/glu.h>
#include <GL/gl.h>

VGMMultiChannelWaveFormViewer::VGMMultiChannelWaveFormViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_, float waveScale_, const VGMMultiChannelWaveFormViewer::Skin& skin_)
	: VGMDataObverser()
	, name(name_)
	, x(x_)
	, y(y_)
	, width(width_)
	, height(height_)
	, waveScale(waveScale_)
	, skin(skin_)
{
}

VGMMultiChannelWaveFormViewer::~VGMMultiChannelWaveFormViewer()
{
}

void VGMMultiChannelWaveFormViewer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormViewer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.Open(name.c_str(), x, y, width, height);

	texture.Load(info.texturePath);
}

void VGMMultiChannelWaveFormViewer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.Close();
}

void VGMMultiChannelWaveFormViewer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormViewer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormViewer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}
 
void VGMMultiChannelWaveFormViewer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormViewer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	if (systemChannels.HasSampleUpdateEvent())
	{
		videoDevice.MakeCurrent();

		videoDevice.Clear(skin.bgColor);

		int startX = 0;
		int endX = VGM_SAMPLE_COUNT; //sampleCount;
		int divX = 10;

		int startY = -32767; //sampleCount/2;
		int endY = 32768; //sampleCount;
		int divY = 3;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glPushMatrix();
		gluOrtho2D(startX, endX, startY, endY);
		glMatrixMode(GL_MODELVIEW);
		glViewport(0, 0, width, height);

		glDisable(GL_BLEND);
		videoDevice.DrawTexSolidRectangle
		(
			texture,
			Vector2(startX, startY), skin.bgColor, Vector2(0, 0),
			Vector2(endX, startY), skin.bgColor, Vector2(1, 0),
			Vector2(endX, endY), skin.bgColor, Vector2(1, 1),
			Vector2(startX, endY), skin.bgColor, Vector2(0, 1)
		);

		glEnable(GL_BLEND);
		for (int ch = 0; ch < systemChannels.GetChannelsCount(); ch++)
		{
			int channelViewportHeight = height / systemChannels.GetChannelsCount() / 2;

			//////////////////////////////////////////////////////////////////////////////
			// draw grid
			glViewport(0, (ch * 2 + 0) * channelViewportHeight, width, channelViewportHeight);

			glBlendFunc(GL_ONE, GL_ONE);
			videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
			for (s32 i = startX; i < endX; i += (endX - startX) / divX)
			{
				videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
			}
			for (s32 i = startY; i < endY; i += (endY - startY) / divY)
			{
				videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
			}
			videoDevice.DrawLine(Vector2(startX, 0), skin.axisColor, Vector2(endX, 0), skin.axisColor);

			int step = 1;
			Color c = skin.leftColor;
			for (s32 i = startX; i < endX - step; i += step)
			{
				s32 y0 = systemChannels.GetChannelLeftSample(ch + 0, i +    0) * 5 * waveScale;
				s32 y1 = systemChannels.GetChannelLeftSample(ch + 0, i + step) * 5 * waveScale;
				videoDevice.DrawLine(Vector2(i + 0, y0), c, Vector2(i + step, y1), c);
			}

			//////////////////////////////////////////////////////////////////////////////
			// draw grid
			glViewport(0, (ch * 2 + 1) * channelViewportHeight, width, channelViewportHeight);

			glBlendFunc(GL_ONE, GL_ONE);
			videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
			for (s32 i = startX; i < endX; i += (endX - startX) / divX)
			{
				videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
			}
			for (s32 i = startY; i < endY; i += (endY - startY) / divY)
			{
				videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
			}
			videoDevice.DrawLine(Vector2(startX, 0), skin.axisColor, Vector2(endX, 0), skin.axisColor);

			step = 1;
			c = skin.rightColor;
			for (s32 i = startX; i < endX - step; i += step)
			{
				s32 y0 = systemChannels.GetChannelRightSample(ch + 0, i +    0) * 5 * waveScale;
				s32 y1 = systemChannels.GetChannelRightSample(ch + 0, i + step) * 5 * waveScale;
				videoDevice.DrawLine(Vector2(i + 0, y0), c, Vector2(i + step, y1), c);
			}
		}

		videoDevice.Flush();
		glPopMatrix();
	}
}
