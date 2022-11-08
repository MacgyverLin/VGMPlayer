#include "VGMWaveFormRenderer.h"

VGMWaveFormRenderer::VGMWaveFormRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMWaveFormRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, waveScale(waveScale_)
	, skin(skin_)
	, font(nullptr)
{
}

VGMWaveFormRenderer::~VGMWaveFormRenderer()
{
}

void VGMWaveFormRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

	font = videoDevice.CreateFont("arial.ttf", 12);
}

void VGMWaveFormRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMInfo& vgmInfo = vgmData.GetInfo();
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

	videoDevice.DestroyFont(font);
	font = nullptr;
}

void VGMWaveFormRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

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
	int startX = 0;
	int endX = VGM_SAMPLE_BUFFER_SIZE;
	int divX = 20;

	int startY = -32767;
	int endY = 32768;
	int divY = 10;

	/////////////////////////////////////////////////////////////////////////////////
	videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
	videoDevice.LoadIdentity();
	videoDevice.Ortho2D(startX, endX, startY, endY);
	videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

	videoDevice.Enable(VideoDeviceEnum::BLEND);
	for (int ch = 0; ch < 2; ch++)
	{
		SetViewport(0, ch * 1.0f / 2.0f, 1.0f, 1.0f / 2.0f);

		videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE_MINUS_SRC_ALPHA);
		videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
		for (UINT32 i = startX; i < endX; i += (endX - startX) / divX)
		{
			videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
		}
		for (UINT32 i = startY; i < endY; i += (endY - startY) / 10)
		{
			videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
		}
		videoDevice.DrawLine(Vector2(startX, 0), skin.axisColor, Vector2(endX, 0), skin.axisColor);

		videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE);
		const Color& c = (ch % 2) ? skin.rightColor : skin.leftColor;
		for (UINT32 i = startX; i < endX - 3; i += 3)
		{
			UINT32 y0 = outputChannels.GetOutputSample(ch, i + 0) * waveScale;
			UINT32 y1 = outputChannels.GetOutputSample(ch, i + 3) * waveScale;
			videoDevice.DrawLine(Vector2(i, y0), c, Vector2(i + 3, y1), c);
		}
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
}
