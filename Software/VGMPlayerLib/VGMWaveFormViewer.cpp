#include "VGMWaveFormViewer.h"
#ifdef STM32
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

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

void VGMWaveFormViewer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::onNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.open(name.c_str(), x, y, width, height);

	texture.Load(playInfo.texturePath);
}

void VGMWaveFormViewer::onNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.close();
}

void VGMWaveFormViewer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormViewer::onNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	if (bufferInfo.needQueueOutputSamples)
	{
		videoDevice.makeCurrent();

		videoDevice.clear(skin.bgColor);
#ifdef STM32
#else
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
#endif

#ifdef STM32
#else
		glViewport(0, 0, width, height);

		glDisable(GL_BLEND);
		videoDevice.drawTexSolidRectangle
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
			videoDevice.drawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
			for (s32 i = startX; i < endX; i += (endX - startX) / divX)
			{
				videoDevice.drawLine(Vector2(i, startY * skin.waveScale), skin.gridColor, Vector2(i, endY), skin.gridColor);
			}
			for (s32 i = startY; i < endY; i += (endY - startY) / 10)
			{
				videoDevice.drawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
			}
			videoDevice.drawLine(Vector2(startX, 0), skin.axisColor, Vector2(endX, 0), skin.axisColor);

			const Color& c = j % 2 ? skin.leftColor : skin.rightColor;
			for (s32 i = startX; i < endX - 3; i += 3)
			{
				s32 y0 = bufferInfo.outputSamples.Get(i + 0, j) * waveScale;
				s32 y1 = bufferInfo.outputSamples.Get(i + 3, j) * waveScale;
				videoDevice.drawLine(Vector2(i, y0), c, Vector2(i + 3, y1), c);
			}
		}
#endif

#ifdef STM32
#else
		videoDevice.flush();
		glPopMatrix();
#endif
	}
}
