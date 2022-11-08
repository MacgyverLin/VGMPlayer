#include "VGMSpectrumRenderer.h"
#include "FFT.h"

VGMSpectrumRenderer::VGMSpectrumRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMSpectrumRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, waveScale(waveScale_)
	, skin(skin_)
	, font(nullptr)
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

	const VGMInfo& vgmInfo = vgmData.GetInfo();
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

	font = videoDevice.CreateFont("arial.ttf", 12);
}

void VGMSpectrumRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;


	const VGMInfo& vgmInfo = vgmData.GetInfo();
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

	videoDevice.DestroyFont(font);
	font = nullptr;
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
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();


	/////////////////////////////////////////////////////////////////////////////////
	int fftSampleCount = skin.numColumns * 2;
	int startX = 0;// ;
	int endX = fftSampleCount / 2;
	FLOAT32 stepX = ((FLOAT32)(endX - startX)) / ((FLOAT32)fftSampleCount);

	int startY = 0;
	int endY = 1;
	FLOAT32 stepY = ((FLOAT32)(endY - startY)) / 20;

	FLOAT32 N = VGM_SAMPLE_BUFFER_SIZE / fftSampleCount;


	vector<complex> left(fftSampleCount);
	vector<complex> right(fftSampleCount);
	FLOAT32 leftIdx = 0;
	FLOAT32 rightIdx = 0;
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
			l->real += outputChannels.GetOutputSample(0, leftIdx + j);
			r->real += outputChannels.GetOutputSample(1, rightIdx + j);
		}
		l->real /= N;
		r->real /= N;

		leftIdx += N;
		rightIdx += N;
	}

	fft(fftSampleCount, &left[0]);
	fft(fftSampleCount, &right[0]);

	vector<FLOAT32> displayLeft(fftSampleCount);
	vector<FLOAT32> displayRight(fftSampleCount);
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

	SetViewport(0, 0, 0.5f, 1.0f);

	videoDevice.Enable(VideoDeviceEnum::BLEND);
	videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE_MINUS_SRC_ALPHA);
	videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
	for (FLOAT32 i = startX; i < endX; i += stepX)
	{
		videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
	}
	for (FLOAT32 i = startY; i < endY; i += stepY)
	{
		videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
	}
	videoDevice.DrawLine(Vector2(startX, 0.0f), skin.axisColor, Vector2(endX, 0.0f), skin.axisColor);

	// ------------------------------
	FLOAT32 bloom = 0.01f;
	videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE);
	Color topColor = skin.leftColor; topColor.a = 0.3f;
	Color bottomColor = skin.leftColor; bottomColor.a = 0.9f;

	FLOAT32 scale = waveScale / 65536.0f;
	for (UINT32 i = startX; i < endX; i++)
	{
		FLOAT32 y0 = displayLeft[i] * scale;
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

	SetViewport(0.5f, 0.0f, 0.5f, 1.0f);

	videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE_MINUS_SRC_ALPHA);
	videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
	for (FLOAT32 i = startX; i < endX; i += stepX)
	{
		videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
	}
	for (FLOAT32 i = startY; i < endY; i += stepY)
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
	for (UINT32 i = startX; i < endX; i++)
	{
		FLOAT32 y0 = displayRight[i] * scale;
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

	//////////////////////////////////////////////////////////////////////////////
	// Draw Name
	videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
	videoDevice.LoadIdentity();
	videoDevice.Ortho2D(0, region.w, 0, region.h);
	videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);
	SetViewport(0, 0, 1, 1);

	videoDevice.SetFont(font);
	videoDevice.SetFontColor(Color::White);
	videoDevice.SetFontScale(1.0);
	videoDevice.DrawText(name.c_str(), 0, region.h - 12);

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
		Vector2(0.5, 0), Color::Grey,
		Vector2(0.5, 1), Color::Grey,
		Vector2(0, 1), Color::Grey
	);

	videoDevice.DrawWireRectangle
	(
		Vector2(0.5, 0), Color::Grey,
		Vector2(1.0, 0), Color::Grey,
		Vector2(1.0, 1), Color::Grey,
		Vector2(0.5, 1), Color::Grey
	);
}