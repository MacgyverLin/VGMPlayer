#include "VGMFrequencyViewer.h"
#include "FFT.h"

VGMFrequencyViewer::VGMFrequencyViewer(const string& name_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_, const Skin& skin_)
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
			if(maxLeft[i] < l)
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
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.gridColor);
		for (FLOAT32 i = startX; i < endX; i += stepX)
		{
			videoDevice.drawLine(Vertex(i, startY), Vertex(i, endY), skin.gridColor);
		}
		for (FLOAT32 i = startY; i < endY; i += stepY)
		{
			videoDevice.drawLine(Vertex(startX, i), Vertex(endX, i), skin.gridColor);
		}
		videoDevice.drawLine(Vertex(startX, 0.0f), Vertex(endX, 0.0f), skin.axisColor);

		FLOAT32 bloom = 0.01f;
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		Color leftColor(0.0f, 1.0f, 0.0f, 0.3f);
		for (INT32 i = startX; i < endX; i++)
		{
			FLOAT32 y0 = abs(left[i].real) / (65536);
			videoDevice.drawSolidRectangle(Vertex(i + 0.1f, y0        ), Vertex(i + 0.9f, y0        ), Vertex(i + 0.9f, 0), Vertex(i + 0.1f, 0), skin.leftColor);
			videoDevice.drawSolidRectangle(Vertex(i + 0.2f, y0 - bloom), Vertex(i + 0.8f, y0 - bloom), Vertex(i + 0.8f, 0), Vertex(i + 0.2f, 0), leftColor);

			y0 = abs(maxLeft[i]) / (65536);
			videoDevice.drawSolidRectangle(Vertex(i + 0.1f, y0 - bloom), Vertex(i + 0.9f, y0 - bloom), Vertex(i + 0.9f, y0 + bloom), Vertex(i + 0.1f, y0 + bloom), skin.leftColor);
			videoDevice.drawSolidRectangle(Vertex(i + 0.2f, y0 - bloom), Vertex(i + 0.8f, y0 - bloom), Vertex(i + 0.8f, y0 + bloom), Vertex(i + 0.2f, y0 + bloom), leftColor);
		}

		/////////////////////////////////////////////////////////////////////
		glViewport(0, height / 2, width, height / 2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.gridColor);
		for (FLOAT32 i = startX; i < endX; i += stepX)
		{
			videoDevice.drawLine(Vertex(i, startY), Vertex(i, endY), skin.gridColor);
		}
		for (FLOAT32 i = startY; i < endY; i += stepY)
		{
			videoDevice.drawLine(Vertex(startX, i), Vertex(endX, i), skin.gridColor);
		}
		videoDevice.drawLine(Vertex(startX, 0.0f), Vertex(endX, 0.0f), skin.axisColor);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		Color rightColor(0.0f, 1.0f, 1.0f, 0.3f);
		for (INT32 i = startX; i < endX; i++)
		{
			FLOAT32 y0 = abs(right[i].real) / (65536);
			videoDevice.drawSolidRectangle(Vertex(i + 0.1f, y0        ), Vertex(i + 0.9f, y0        ), Vertex(i + 0.9f, 0), Vertex(i + 0.1f, 0), skin.rightColor);
			videoDevice.drawSolidRectangle(Vertex(i + 0.2f, y0 - bloom), Vertex(i + 0.8f, y0 - bloom), Vertex(i + 0.8f, 0), Vertex(i + 0.2f, 0), rightColor);

			y0 = abs(maxRight[i]) / (65536);
			videoDevice.drawSolidRectangle(Vertex(i + 0.1f, y0 - bloom), Vertex(i + 0.9f, y0 - bloom), Vertex(i + 0.9f, y0 + bloom), Vertex(i + 0.1f, y0 + bloom), skin.rightColor);
			videoDevice.drawSolidRectangle(Vertex(i + 0.2f, y0 - bloom), Vertex(i + 0.8f, y0 - bloom), Vertex(i + 0.8f, y0 + bloom), Vertex(i + 0.2f, y0 + bloom), rightColor);
		}

		videoDevice.flush();
		glPopMatrix();
	}
}