#include "VGMFireSpectrumViewer.h"
#include "FFT.h"
#include <GL/glu.h>
#include <GL/gl.h>

VGMFireSpectrumViewer::VGMFireSpectrumViewer(const string& name_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_, const Skin& skin_)
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

VGMFireSpectrumViewer::~VGMFireSpectrumViewer()
{
}

void VGMFireSpectrumViewer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFireSpectrumViewer::onNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.open(name.c_str(), x, y, width, height);
}

void VGMFireSpectrumViewer::onNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.close();
}

void VGMFireSpectrumViewer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFireSpectrumViewer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFireSpectrumViewer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFireSpectrumViewer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMFireSpectrumViewer::onNotifyUpdate(Obserable& observable)
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
		float stepX = ((float)(endX - startX)) / ((float)fftSampleCount / 2);

		int startY = 0;
		int endY = 1;
		float stepY = ((float)(endY - startY)) / 20;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glPushMatrix();
		gluOrtho2D(endX, startX, startY, endY);
		glMatrixMode(GL_MODELVIEW);

		float step = VGM_SAMPLE_COUNT / fftSampleCount;

		vector<complex> left;
		left.resize(fftSampleCount);
		float leftIdx = 0;
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
		float rightIdx = 0;
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
			FLOAT32 l = abs(left[i].real);
			if (maxLeft[i] < l)
				maxLeft[i] = l;
			else
				maxLeft[i] *= 0.96f;

			FLOAT32 r = abs(right[i].real);
			if (maxRight[i] < r)
				maxRight[i] = r;
			else
				maxRight[i] *= 0.96f;
		}

		/////////////////////////////////////////////////////////////////////
		glViewport(0, 0, width, height / 2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		{
			FLOAT32 bloom = 0.01f;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			Color topColor = skin.leftColor; topColor.a = 0.3f;
			Color bottomColor = skin.leftColor; bottomColor.a = 0.9f;
			for (INT32 i = startX; i < endX; i++)
			{
				FLOAT32 y0 = y0 = abs(maxLeft[i]) / (65536);
				videoDevice.drawSolidRectangle(
					Vertex(i + 0.0f, y0), topColor,
					Vertex(i + 1.0f, y0), topColor,
					Vertex(i + 1.0f, 0), bottomColor,
					Vertex(i + 0.0f, 0), bottomColor);
			}
		}

		/////////////////////////////////////////////////////////////////////
		glViewport(0, height / 2, width, height / 2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		{
			FLOAT32 bloom = 0.01f;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			Color topColor = skin.rightColor; topColor.a = 0.3f;
			Color bottomColor = skin.rightColor; bottomColor.a = 0.9f;
			for (INT32 i = startX; i < endX; i++)
			{
				FLOAT32 y0 = abs(maxRight[i]) / (65536);
				videoDevice.drawSolidRectangle(
					Vertex(i + 0.0f, y0), topColor,
					Vertex(i + 1.0f, y0), topColor,
					Vertex(i + 1.0f, 0), bottomColor,
					Vertex(i + 0.0f, 0), bottomColor);
			}
		}

		videoDevice.flush();
		glPopMatrix();
	}
}