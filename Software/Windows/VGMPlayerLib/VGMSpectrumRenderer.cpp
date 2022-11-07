#include "VGMSpectrumRenderer.h"
#include "FFT.h"

VGMSpectrumRenderer::VGMSpectrumRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMSpectrumRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, waveScale(waveScale_)
	, skin(skin_)
{
}

VGMSpectrumRenderer::~VGMSpectrumRenderer()
{
}

void VGMSpectrumRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMSpectrumRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();
}

void VGMSpectrumRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();
}

void VGMSpectrumRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMSpectrumRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMSpectrumRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMSpectrumRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMSpectrumRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		/////////////////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, 1, 0, 1);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		SetViewport(0, 0, 1, 1);

		videoDevice.Disable(VideoDeviceEnum::BLEND);
		videoDevice.DrawWireRectangle
		(
			Vector2(0, 0), Color::Grey,
			Vector2(1, 0), Color::Grey,
			Vector2(1, 1), Color::Grey,
			Vector2(0, 1), Color::Grey
		);

		/////////////////////////////////////////////////////////////////////////////////
		int fftSampleCount = skin.numColumns * 2;
		int startX = 0;// ;
		int endX = fftSampleCount / 2;
		f32 stepX = ((f32)(endX - startX)) / ((f32)fftSampleCount);

		int startY = 0;
		int endY = 1;
		f32 stepY = ((f32)(endY - startY)) / 20;

		f32 N = VGM_SAMPLE_BUFFER_SIZE / fftSampleCount;


		vector<complex> left(fftSampleCount);
		vector<complex> right(fftSampleCount);
		f32 leftIdx = 0;
		f32 rightIdx = 0;
		for (int i = 0; i < fftSampleCount; i++)
		{
			complex* l = &left[i];
			complex* r = &right[i];

			l->real = 0;
			l->imag = 0;
			r->real = 0;
			r->imag = 0;

			for (int j = 0; j < N; j++)
			{
				l->real += systemChannels.GetOutputSample(0, leftIdx  + j);
				r->real += systemChannels.GetOutputSample(1, rightIdx + j);
			}
			l->real /= N;
			r->real /= N;

			leftIdx += N;
			rightIdx += N;
		}

		fft(fftSampleCount, &left[0]);
		fft(fftSampleCount, &right[0]);

		vector<f32> displayLeft(fftSampleCount);
		vector<f32> displayRight(fftSampleCount);
		maxLeft.resize(fftSampleCount);
		maxRight.resize(fftSampleCount);
		for (int i = 0; i < fftSampleCount; i++)
		{
			displayLeft[i] = abs(left[i].real);
			displayRight[i] = abs(right[i].real);

			maxLeft[i] = maxLeft[i] < displayLeft[i] ? displayLeft[i] : (maxLeft[i] * 0.96f);
			maxRight[i] = maxRight[i] < displayRight[i] ? displayRight[i] : (maxRight[i] * 0.96f);
		}


		/////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(startX, endX, startY, endY);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		SetViewport(0, 0, 1, 0.5f);

		videoDevice.Enable(VideoDeviceEnum::BLEND);
		videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE_MINUS_SRC_ALPHA);
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

		// ------------------------------
		f32 bloom = 0.01f;
		videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE);
		Color topColor = skin.leftColor; topColor.a = 0.3f;
		Color bottomColor = skin.leftColor; bottomColor.a = 0.9f;

		f32 scale = waveScale / 65536.0f;
		for (s32 i = startX; i < endX; i++)
		{
			f32 y0 = displayLeft[i] * scale;
			videoDevice.DrawSolidRectangle(
				Vector2(i + 0.1f, y0), topColor,
				Vector2(i + 0.9f, y0), topColor,
				Vector2(i + 0.9f, 0), bottomColor,
				Vector2(i + 0.1f, 0), bottomColor);

			y0 = maxLeft[i] * scale;
			videoDevice.DrawSolidRectangle(
				Vector2(i + 0.1f, y0 - bloom), topColor,
				Vector2(i + 0.9f, y0 - bloom), topColor,
				Vector2(i + 0.9f, y0 + bloom), bottomColor,
				Vector2(i + 0.1f, y0 + bloom), bottomColor);
		}


		/////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(startX, endX, startY, endY);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		SetViewport(0, 0.5f, 1.0f, 0.5f);

		videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE_MINUS_SRC_ALPHA);
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

		// ------------------------------
		bloom = 0.01f;
		videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE);
		topColor = skin.rightColor; topColor.a = 0.3f;
		bottomColor = skin.rightColor; bottomColor.a = 0.9f;

		scale = waveScale / 65536.0f;
		for (s32 i = startX; i < endX; i++)
		{
			f32 y0 = displayRight[i] * scale;
			videoDevice.DrawSolidRectangle(
				Vector2(i + 0.1f, y0), topColor,
				Vector2(i + 0.9f, y0), topColor,
				Vector2(i + 0.9f, 0), bottomColor,
				Vector2(i + 0.1f, 0), bottomColor);

			y0 = maxRight[i] * scale;
			videoDevice.DrawSolidRectangle(
				Vector2(i + 0.1f, y0 - bloom), topColor,
				Vector2(i + 0.9f, y0 - bloom), topColor,
				Vector2(i + 0.9f, y0 + bloom), bottomColor,
				Vector2(i + 0.1f, y0 + bloom), bottomColor);
		}

	}
}