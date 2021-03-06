#include "VGMWaveFormViewer.h"
#ifdef STM32
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

VGMWaveFormViewer::VGMWaveFormViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_, const Skin& skin_)
: VGMDataObverser()
, name(name_)
, x(x_)
, y(y_)
, width(width_)
, height(height_)
, skin(skin_)
{
}

VGMWaveFormViewer::~VGMWaveFormViewer()
{
}

void VGMWaveFormViewer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMWaveFormViewer::onNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.open(name.c_str(), x, y, width, height);
}

void VGMWaveFormViewer::onNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.close();
}

void VGMWaveFormViewer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMWaveFormViewer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMWaveFormViewer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMWaveFormViewer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMWaveFormViewer::onNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
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
		glViewport(0, 0, width, height / 2);

		glBlendFunc(GL_ONE, GL_ONE);
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.gridColor);
		for (s32 i=startX; i< endX; i += (endX - startX) / divX)
		{
			videoDevice.drawLine(Vertex(i, startY), Vertex(i, endY), skin.gridColor);
		}
		for (s32 i = startY; i < endY; i += (endY - startY) / 10)
		{
			videoDevice.drawLine(Vertex(startX, i), Vertex(endX, i), skin.gridColor);
		}
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.axisColor);

		for (s32 i = startX; i < endX - 3; i+=3)
		{
			s32 y0 = bufferInfo.outputSamples[i + 0].l;
			s32 y1 = bufferInfo.outputSamples[i + 3].l;
			videoDevice.drawLine(Vertex(i, y0), Vertex(i + 3, y1), skin.leftColor);
		}
#endif
		
#ifdef STM32
#else
		glViewport(0, height / 2, width, height / 2);

		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.gridColor);
		for (s32 i = startX; i < endX; i += (endX - startX) / divX)
		{
			videoDevice.drawLine(Vertex(i, startY), Vertex(i, endY), skin.gridColor);
		}
		for (s32 i = startY; i < endY; i += (endY - startY) / 10)
		{
			videoDevice.drawLine(Vertex(startX, i), Vertex(endX, i), skin.gridColor);
		}
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.axisColor);

		for (s32 i = startX; i < endX - 3; i+=3)
		{
			s32 y0 = bufferInfo.outputSamples[i + 0].r;
			s32 y1 = bufferInfo.outputSamples[i + 3].r;
			videoDevice.drawLine(Vertex(i, y0), Vertex(i + 3, y1), skin.rightColor);
		}
#endif		

#ifdef STM32
#else
		videoDevice.flush();
		glPopMatrix();
#endif
	}
}
