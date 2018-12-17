#include "VGMChannelViewer.h"
#include "FFT.h"

VGMChannelViewer::VGMChannelViewer(const string& name_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_, const Color& bg_)
	: VGMDataObverser()
	, name(name_)
	, x(x_)
	, y(y_)
	, width(width_)
	, height(height_)
	, bg(bg_)
{
}

VGMChannelViewer::~VGMChannelViewer()
{
}

void VGMChannelViewer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.open(name.c_str(), x, y, width, height);
}

void VGMChannelViewer::onNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.close();
}

void VGMChannelViewer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	if (bufferInfo.needQueueOutputSamples)
	{
		videoDevice.makeCurrent();

		videoDevice.clear(bg);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glPushMatrix();
		gluOrtho2D(0, VGM_SAMPLE_COUNT, -32767, 32767);
		glMatrixMode(GL_MODELVIEW);

		Color gridColor(0.0f, 0.2f, 0.2f, 1.0f);
		Color axisColor(0.0f, 0.5f, 0.5f, 1.0f);
		Color leftColor(0.0f, 0.5f, 0.0f, 1.0f);
		Color rightColor(0.0f, 0.5f, 0.5f, 1.0f);

		glViewport(0, 0, width, height / 2);
		videoDevice.drawLine(Vertex(0, 0), Vertex(VGM_SAMPLE_COUNT, 0), gridColor);
		for (INT32 i = 0; i < VGM_SAMPLE_COUNT; i += VGM_SAMPLE_COUNT / 10)
		{
			videoDevice.drawLine(Vertex(i, -32767), Vertex(i, 32767), gridColor);
		}
		for (INT32 i = -32767; i < 32767; i += 65536 / 10)
		{
			videoDevice.drawLine(Vertex(0, i), Vertex(VGM_SAMPLE_COUNT, i), gridColor);
		}
		videoDevice.drawLine(Vertex(0, 0), Vertex(VGM_SAMPLE_COUNT, 0), axisColor);
		for (INT32 i = 0; i < VGM_SAMPLE_COUNT - 1; i++)
		{
			INT32 y0 = bufferInfo.outputSamples[i + 0].l;
			INT32 y1 = bufferInfo.outputSamples[i + 1].l;
			videoDevice.drawLine(Vertex(i, y0), Vertex(i + 1, y1), leftColor);
		}

		glViewport(0, height / 2, width, height / 2);
		videoDevice.drawLine(Vertex(0, 0), Vertex(VGM_SAMPLE_COUNT, 0), gridColor);
		for (INT32 i = 0; i < VGM_SAMPLE_COUNT; i += VGM_SAMPLE_COUNT / 10)
		{
			videoDevice.drawLine(Vertex(i, -32767), Vertex(i, 32767), gridColor);
		}
		for (INT32 i = -32767; i < 32767; i += 65536 / 10)
		{
			videoDevice.drawLine(Vertex(0, i), Vertex(VGM_SAMPLE_COUNT, i), gridColor);
		}
		videoDevice.drawLine(Vertex(0, 0), Vertex(VGM_SAMPLE_COUNT, 0), axisColor);
		for (INT32 i = 0; i < VGM_SAMPLE_COUNT - 1; i++)
		{
			INT32 y0 = bufferInfo.outputSamples[i + 0].r;
			INT32 y1 = bufferInfo.outputSamples[i + 1].r;
			videoDevice.drawLine(Vertex(i, y0), Vertex(i + 1, y1), rightColor);
		}

		videoDevice.flush();
		glPopMatrix();
	}
}