#include "VGMFrequencyViewer.h"
#include "FFT.h"

VGMFrequencyViewer::VGMFrequencyViewer(const string& name_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_, const Color& bg_)
	: VGMDataObverser()
	, name(name_)
	, x(x_)
	, y(y_)
	, width(width_)
	, height(height_)
	, bg(bg_)
{
}

VGMFrequencyViewer::~VGMFrequencyViewer()
{
}

void VGMFrequencyViewer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFrequencyViewer::onNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.open(name.c_str(), x, y, width, height);
}

void VGMFrequencyViewer::onNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.close();
}

void VGMFrequencyViewer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFrequencyViewer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFrequencyViewer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFrequencyViewer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFrequencyViewer::onNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	Color gridColor(0.0f, 0.2f, 0.2f, 1.0f);
	Color axisColor(0.0f, 0.5f, 0.5f, 1.0f);
	Color leftColor(0.0f, 0.5f, 0.0f, 1.0f);
	Color rightColor(0.0f, 0.5f, 0.5f, 1.0f);

	videoDevice.makeCurrent();
	videoDevice.clear(bg);

	int sampleCount = 64;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPushMatrix();
	gluOrtho2D(sampleCount / 2, sampleCount, 0, 32767);
	glMatrixMode(GL_MODELVIEW);

	complex left[VGM_SAMPLE_COUNT];
	float step = ((float)sampleCount / VGM_SAMPLE_COUNT);
	for(float i = 0; i < VGM_SAMPLE_COUNT; i+= step)
	{
		left[(int)i].real = bufferInfo.outputSamples[i + 0].l;
		left[(int)i].imag = 0;
	}

	complex right[VGM_SAMPLE_COUNT];
	for (float i = 0; i < VGM_SAMPLE_COUNT; i += step)
	{
		right[(int)i].real = bufferInfo.outputSamples[i + 0].r;
		right[(int)i].imag = 0;
	}

	fft(sampleCount, left);
	fft(sampleCount, right);

	/////////////////////////////////////////////////////////////////////
	glViewport(0, 0, width, height / 2);
	videoDevice.drawLine(Vertex(0, 0), Vertex(sampleCount, 0), gridColor);
	for(INT32 i= sampleCount/2; i<sampleCount; i+=sampleCount/10)
	{
		videoDevice.drawLine(Vertex(i, -32767), Vertex(i, 32767), gridColor);
	}
	for(INT32 i=0; i < 32767; i += 32768 / 10)
	{
		videoDevice.drawLine(Vertex(sampleCount / 2, i), Vertex(sampleCount, i), gridColor);
	}
	videoDevice.drawLine(Vertex(0, 0), Vertex(sampleCount, 0), axisColor);
	for (INT32 i = sampleCount / 2; i < sampleCount; i++)
	{
		INT32 y0 = abs(left[i].real);

		videoDevice.drawSolidRectangle(Vertex(i + 0.1f, y0), Vertex(i + 0.9f, y0), Vertex(i + 0.9f, 0), Vertex(i + 0.1f, 0), leftColor);
	}

	/////////////////////////////////////////////////////////////////////
	glViewport(0, height / 2, width, height / 2);
	videoDevice.drawLine(Vertex(0, 0), Vertex(sampleCount, 0), gridColor);
	for (INT32 i = sampleCount / 2; i < sampleCount; i += sampleCount / 10)
	{
		videoDevice.drawLine(Vertex(i, -32767), Vertex(i, 32767), gridColor);
	}
	for (INT32 i = 0; i < 32767; i += 32768 / 10)
	{
		videoDevice.drawLine(Vertex(sampleCount / 2, i), Vertex(sampleCount, i), gridColor);
	}
	videoDevice.drawLine(Vertex(0, 0), Vertex(sampleCount, 0), axisColor);
	for (INT32 i = sampleCount / 2; i < sampleCount; i++)
	{
		INT32 y0 = abs(right[i].real);

		videoDevice.drawSolidRectangle(Vertex(i + 0.1f, y0), Vertex(i + 0.9f, y0), Vertex(i + 0.9f, 0), Vertex(i + 0.1f, 0), rightColor);
	}

	videoDevice.flush();
	glPopMatrix();
}