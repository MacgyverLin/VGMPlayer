#include "VGMSpectrumViewer.h"
#include "FFT.h"
#include <GL/glu.h>
#include <GL/gl.h>

VGMSpectrumViewer::VGMSpectrumViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_, float waveScale_, const Skin& skin_)
	: VGMDataObverser()
	, name(name_)
	, x(x_)
	, y(y_)
	, width(width_)
	, height(height_)
	, waveScale(waveScale_)
	, skin(skin_)
{
	maxLeft.resize(skin.numColumns * 2);
	maxRight.resize(skin.numColumns * 2);
}

VGMSpectrumViewer::~VGMSpectrumViewer()
{
}

void VGMSpectrumViewer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.Open(name.c_str(), x, y, width, height);

	texture.Load(info.texturePath);
}

void VGMSpectrumViewer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.Close();
}

void VGMSpectrumViewer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumViewer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		videoDevice.MakeCurrent();

		videoDevice.Clear(skin.bgColor);

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

		f32 step = VGM_SAMPLE_BUFFER_SIZE / fftSampleCount;

		vector<complex> left;
		left.resize(fftSampleCount);
		f32 leftIdx = 0;
		for (int i = 0; i < fftSampleCount; i++)
		{
			left[i].real = 0;
			left[i].imag = 0;
			for (int j = 0; j < step; j++)
			{
				left[i].real += systemChannels.GetOutputSample(0, (int)leftIdx + j);
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
				right[i].real += systemChannels.GetOutputSample(1, (int)rightIdx + j);
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
		glViewport(0, 0, width, height);

		glDisable(GL_BLEND);
		videoDevice.DrawTexSolidRectangle
		(
			texture,
			Vector2(endX, startY), skin.bgColor, Vector2(0, 0),
			Vector2(startX, startY), skin.bgColor, Vector2(1, 0),
			Vector2(startX, endY), skin.bgColor, Vector2(1, 1),
			Vector2(endX, endY), skin.bgColor, Vector2(0, 1)
		);

		glViewport(0, 0, width, height / 2);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
		for (f32 i = startX; i < endX; i += stepX)
		{
			videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
		}
		for (f32 i = startY; i < endY; i += stepY)
		{
			videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
		}
		videoDevice.DrawLine(Vector2(startX, 0.0f), skin.axisColor, Vector2(endX, 0.0f), skin.axisColor);

		{
			f32 bloom = 0.01f;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			Color topColor = skin.leftColor; topColor.a = 0.3f;
			Color bottomColor = skin.leftColor; bottomColor.a = 0.9f;
			for (s32 i = startX; i < endX; i++)
			{
				f32 y0 = abs(left[i].real) / (65536) * waveScale;
				videoDevice.DrawSolidRectangle(
					Vector2(i + 0.1f, y0), topColor,
					Vector2(i + 0.9f, y0), topColor,
					Vector2(i + 0.9f, 0), bottomColor,
					Vector2(i + 0.1f, 0), bottomColor);

				y0 = abs(maxLeft[i]) / (65536) * waveScale;
				videoDevice.DrawSolidRectangle(
					Vector2(i + 0.1f, y0 - bloom), topColor,
					Vector2(i + 0.9f, y0 - bloom), topColor,
					Vector2(i + 0.9f, y0 + bloom), bottomColor,
					Vector2(i + 0.1f, y0 + bloom), bottomColor);
			}
		}

		/////////////////////////////////////////////////////////////////////
		glViewport(0, height / 2, width, height / 2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
		for (f32 i = startX; i < endX; i += stepX)
		{
			videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
		}
		for (f32 i = startY; i < endY; i += stepY)
		{
			videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
		}
		videoDevice.DrawLine(Vector2(startX, 0.0f), skin.axisColor, Vector2(endX, 0.0f), skin.axisColor);

		{
			f32 bloom = 0.01f;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			Color topColor = skin.rightColor; topColor.a = 0.3f;
			Color bottomColor = skin.rightColor; bottomColor.a = 0.9f;
			for (s32 i = startX; i < endX; i++)
			{
				f32 y0 = abs(right[i].real) / (65536) * waveScale;
				videoDevice.DrawSolidRectangle(
					Vector2(i + 0.1f, y0), topColor,
					Vector2(i + 0.9f, y0), topColor,
					Vector2(i + 0.9f, 0), bottomColor,
					Vector2(i + 0.1f, 0), bottomColor);

				y0 = abs(maxRight[i]) / (65536) * waveScale;
				videoDevice.DrawSolidRectangle(
					Vector2(i + 0.1f, y0 - bloom), topColor,
					Vector2(i + 0.9f, y0 - bloom), topColor,
					Vector2(i + 0.9f, y0 + bloom), bottomColor,
					Vector2(i + 0.1f, y0 + bloom), bottomColor);
			}
		}

		videoDevice.Flush();
		glPopMatrix();
	}
}