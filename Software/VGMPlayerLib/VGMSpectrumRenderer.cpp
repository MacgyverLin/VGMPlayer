#include "VGMSpectrumRenderer.h"
#include "FFT.h"
#include <GL/glu.h>
#include <GL/gl.h>

VGMSpectrumRenderer::VGMSpectrumRenderer(const char* name_, u32 x_, u32 y_, u32 width_, u32 height_, float waveScale_, const VGMSpectrumRenderer::Skin& skin_)
	: VGMRenderer(name_, x_, y_, width_, height_)
	, waveScale(waveScale_)
	, skin(skin_)
{
	maxLeft.resize(skin.numColumns * 2);
	maxRight.resize(skin.numColumns * 2);
}

VGMSpectrumRenderer::~VGMSpectrumRenderer()
{
}

void VGMSpectrumRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();
	texture.Load(info.texturePath);
}

void VGMSpectrumRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();
}

void VGMSpectrumRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMSpectrumRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		int fftSampleCount = skin.numColumns * 2;
		int startX = fftSampleCount / 2;
		int endX = fftSampleCount; //sampleCount;
		f32 stepX = ((f32)(endX - startX)) / ((f32)fftSampleCount / 2);

		int startY = 0;
		int endY = 1;
		f32 stepY = ((f32)(endY - startY)) / 20;
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
		videoDevice.MatrixMode(VideoDevice::Constant::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(startX, endX, startY, endY);
		videoDevice.MatrixMode(VideoDevice::Constant::MODELVIEW);

		/////////////////////////////////////////////////////////////////////
		SetViewport(0, 0, 1, 1);

		videoDevice.Disable(VideoDevice::Constant::BLEND);
		videoDevice.DrawTexSolidRectangle
		(
			texture,
			Vector2(endX, startY), skin.bgColor, Vector2(1, 0),
			Vector2(startX, startY), skin.bgColor, Vector2(0, 0),
			Vector2(startX, endY), skin.bgColor, Vector2(0, 1),
			Vector2(endX, endY), skin.bgColor, Vector2(1, 1)
		);

		/////////////////////////////////////////////////////////////////////
		SetViewport(0, 0, 1, 0.5f);
		
		videoDevice.Enable(VideoDevice::Constant::BLEND);
		videoDevice.BlendFunc(VideoDevice::Constant::SRC_ALPHA, VideoDevice::Constant::ONE_MINUS_SRC_ALPHA);
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
			videoDevice.BlendFunc(VideoDevice::Constant::SRC_ALPHA, VideoDevice::Constant::ONE);
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
		SetViewport(0, 0.5f, 1.0f, 0.5f);
		
		videoDevice.BlendFunc(VideoDevice::Constant::SRC_ALPHA, VideoDevice::Constant::ONE_MINUS_SRC_ALPHA);
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
			videoDevice.BlendFunc(VideoDevice::Constant::SRC_ALPHA, VideoDevice::Constant::ONE);
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
	}
}