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
		int divX = fftSampleCount / 2;

		int startY = 0;
		int endY = 65536 * 2;
		int divY = fftSampleCount / 2;
		
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

		/////////////////////////////////////////////////////////////////////
		glViewport(0, 0, width, height / 2);
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.gridColor);
		for (INT32 i = startX; i < endX; i += (endX - startX) / divX)
		{
			videoDevice.drawLine(Vertex(i, startY), Vertex(i, endY), skin.gridColor);
		}
		for (INT32 i = startY; i < endY; i += (endY - startY) / divY)
		{
			videoDevice.drawLine(Vertex(startX, i), Vertex(endX, i), skin.gridColor);
		}
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.axisColor);

		for (INT32 i = startX; i < endX; i++)
		{
			INT32 y0 = abs(left[i].real);

			videoDevice.drawSolidRectangle(Vertex(i + 0.1f, y0), Vertex(i + 0.9f, y0), Vertex(i + 0.9f, 0), Vertex(i + 0.1f, 0), skin.leftColor);
		}

		/////////////////////////////////////////////////////////////////////
		glViewport(0, height / 2, width, height / 2);
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.gridColor);
		for (INT32 i = startX; i < endX; i += (endX - startX) / divX)
		{
			videoDevice.drawLine(Vertex(i, startY), Vertex(i, endY), skin.gridColor);
		}
		for (INT32 i = startY; i < endY; i += (endY - startY) / divY)
		{
			videoDevice.drawLine(Vertex(startX, i), Vertex(endX, i), skin.gridColor);
		}
		videoDevice.drawLine(Vertex(startX, 0), Vertex(endX, 0), skin.axisColor);

		for (INT32 i = startX; i < endX; i++)
		{
			INT32 y0 = abs(right[i].real);

			videoDevice.drawSolidRectangle(Vertex(i + 0.1f, y0), Vertex(i + 0.9f, y0), Vertex(i + 0.9f, 0), Vertex(i + 0.1f, 0), skin.rightColor);
		}

		videoDevice.flush();
		glPopMatrix();
	}
}