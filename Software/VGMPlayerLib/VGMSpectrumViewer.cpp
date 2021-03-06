#include "VGMSpectrumViewer.h"
#include "FFT.h"
#include <GL/glu.h>
#include <GL/gl.h>

VGMSpectrumViewer::VGMSpectrumViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_, const Skin& skin_)
	: VGMDataObverser()
	, name(name_)
	, x(x_)
	, y(y_)
	, width(width_)
	, height(height_)
	, skin(skin_)
{
	maxLeft.resize(skin.numColumns * 2);
	maxRight.resize(skin.numColumns * 2);
}

VGMSpectrumViewer::~VGMSpectrumViewer()
{
}

void VGMSpectrumViewer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::onNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.open(name.c_str(), x, y, width, height);
}

void VGMSpectrumViewer::onNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.close();
}

void VGMSpectrumViewer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::onNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	if (bufferInfo.needQueueOutputSamples)
	{
		videoDevice.makeCurrent();
		videoDevice.clear(skin.bgColor);

		int fftSampleCount = skin.numColumns * 2;
		int startX = fftSampleCount / 2;
		int endX = fftSampleCount; //sampleCount;
		f32 stepX = ((f32)(endX - startX)) / ((f32)fftSampleCount / 2);

		int startY = 0;
		int endY = 1;
		f32 stepY = ((f32)(endY - startY)) / 20;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glPushMatrix();
		gluOrtho2D(endX, startX, startY, endY);
		glMatrixMode(GL_MODELVIEW);

		f32 step = VGM_SAMPLE_COUNT / fftSampleCount;

		vector<complex> left;
		left.resize(fftSampleCount);
		f32 leftIdx = 0;
		for (int i = 0; i < fftSampleCount; i++)
		{
			left[i].real = 0;
			left[i].imag = 0;
			for (int j = 0; j < step; j++)
			{
				left[i].real += bufferInfo.outputSamples[(int)leftIdx + j].l;
			}
			left[i].real /= step;

			leftIdx += step;
		}

		vector<complex> right;
		right.resize(fftSampleCount);
		f32 rightIdx = 0;
		for (int i = 0; i < fftSampleCount; i++)
		{
			right[i].real = 0;
			right[i].imag = 0;
			for (int j = 0; j < step; j++)
			{
				right[i].real += bufferInfo.outputSamples[(int)rightIdx + j].r;
			}
			right[i].real /= step;

			rightIdx += step;
		}

		fft(fftSampleCount, &left[0]);
		fft(fftSampleCount, &right[0]);

		for (int i = 0; i < fftSampleCount; i++)
		{
			f32 l = abs(left[i].real);
			if (maxLeft[i] < l)
				maxLeft[i] = l;
			else
				maxLeft[i] *= 0.96f;

			f32 r = abs(right[i].real);
			if (maxRight[i] < r)
				maxRight[i] = r;
			else
				maxRight[i] *= 0.96f;
		}

		/////////////////////////////////////////////////////////////////////
		glViewport(0, 0, width, height / 2);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.gridColor);
		for (f32 i = startX; i < endX; i += stepX)
		{
			videoDevice.drawLine(Vertex(i, startY), Vertex(i, endY), skin.gridColor);
		}
		for (f32 i = startY; i < endY; i += stepY)
		{
			videoDevice.drawLine(Vertex(startX, i), Vertex(endX, i), skin.gridColor);
		}
		videoDevice.drawLine(Vertex(startX, 0.0f), Vertex(endX, 0.0f), skin.axisColor);

		{
			f32 bloom = 0.01f;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			Color topColor = skin.leftColor; topColor.a = 0.3f;
			Color bottomColor = skin.leftColor; bottomColor.a = 0.9f;
			for (s32 i = startX; i < endX; i++)
			{
				f32 y0 = abs(left[i].real) / (65536);
				videoDevice.drawSolidRectangle(
					Vertex(i + 0.1f, y0), topColor,
					Vertex(i + 0.9f, y0), topColor,
					Vertex(i + 0.9f, 0), bottomColor,
					Vertex(i + 0.1f, 0), bottomColor);

				y0 = abs(maxLeft[i]) / (65536);
				videoDevice.drawSolidRectangle(
					Vertex(i + 0.1f, y0 - bloom), topColor,
					Vertex(i + 0.9f, y0 - bloom), topColor,
					Vertex(i + 0.9f, y0 + bloom), bottomColor,
					Vertex(i + 0.1f, y0 + bloom), bottomColor);
			}
		}

		/////////////////////////////////////////////////////////////////////
		glViewport(0, height / 2, width, height / 2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.gridColor);
		for (f32 i = startX; i < endX; i += stepX)
		{
			videoDevice.drawLine(Vertex(i, startY), Vertex(i, endY), skin.gridColor);
		}
		for (f32 i = startY; i < endY; i += stepY)
		{
			videoDevice.drawLine(Vertex(startX, i), Vertex(endX, i), skin.gridColor);
		}
		videoDevice.drawLine(Vertex(startX, 0.0f), Vertex(endX, 0.0f), skin.axisColor);

		{
			f32 bloom = 0.01f;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			Color topColor = skin.rightColor; topColor.a = 0.3f;
			Color bottomColor = skin.rightColor; bottomColor.a = 0.9f;
			for (s32 i = startX; i < endX; i++)
			{
				f32 y0 = abs(right[i].real) / (65536);
				videoDevice.drawSolidRectangle(
					Vertex(i + 0.1f, y0), topColor,
					Vertex(i + 0.9f, y0), topColor,
					Vertex(i + 0.9f, 0), bottomColor,
					Vertex(i + 0.1f, 0), bottomColor);

				y0 = abs(maxRight[i]) / (65536);
				videoDevice.drawSolidRectangle(
					Vertex(i + 0.1f, y0 - bloom), topColor,
					Vertex(i + 0.9f, y0 - bloom), topColor,
					Vertex(i + 0.9f, y0 + bloom), bottomColor,
					Vertex(i + 0.1f, y0 + bloom), bottomColor);
			}
		}

		videoDevice.flush();
		glPopMatrix();
	}
}