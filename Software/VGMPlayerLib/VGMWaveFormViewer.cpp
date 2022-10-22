#include "VGMWaveFormViewer.h"
#include <GL/glu.h>
#include <GL/gl.h>

VGMWaveFormViewer::VGMWaveFormViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_, float waveScale_, const Skin& skin_)
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

VGMWaveFormViewer::~VGMWaveFormViewer()
{
}

void VGMWaveFormViewer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.Open(name.c_str(), x, y, width, height);

	texture.Load(info.texturePath);
}

void VGMWaveFormViewer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.Close();
}

void VGMWaveFormViewer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
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
		int divY = 10;

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
			Vector2(  endX, startY), skin.bgColor, Vector2(1, 0),
			Vector2(  endX,   endY), skin.bgColor, Vector2(1, 1),
			Vector2(startX,   endY), skin.bgColor, Vector2(0, 1)
		);

		glEnable(GL_BLEND);
		for (int j = 0; j < 2; j++)
		{
			glViewport(0, j * height / 2, width, height / 2);

			glBlendFunc(GL_ONE, GL_ONE);
			videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
			for (s32 i = startX; i < endX; i += (endX - startX) / divX)
			{
				videoDevice.DrawLine(Vector2(i, startY * skin.waveScale), skin.gridColor, Vector2(i, endY), skin.gridColor);
			}
			for (s32 i = startY; i < endY; i += (endY - startY) / 10)
			{
				videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
			}
			videoDevice.DrawLine(Vector2(startX, 0), skin.axisColor, Vector2(endX, 0), skin.axisColor);

			const Color& c = j % 2 ? skin.leftColor : skin.rightColor;
			for (s32 i = startX; i < endX - 3; i += 3)
			{
				s32 y0 = systemChannels.GetOutputSample(i + 0, j) * waveScale;
				s32 y1 = systemChannels.GetOutputSample(i + 3, j) * waveScale;
				videoDevice.DrawLine(Vector2(i, y0), c, Vector2(i + 3, y1), c);
			}
		}

		videoDevice.Flush();
		glPopMatrix();
	}
}
