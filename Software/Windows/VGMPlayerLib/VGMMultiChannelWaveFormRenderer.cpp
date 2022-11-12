#include "VGMMultiChannelWaveFormRenderer.h"

VGMMultiChannelWaveFormRenderer::VGMMultiChannelWaveFormRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMMultiChannelWaveFormRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, waveScale(waveScale_)
	, skin(skin_)
	, font(nullptr)
{
}

VGMMultiChannelWaveFormRenderer::~VGMMultiChannelWaveFormRenderer()
{
}

void VGMMultiChannelWaveFormRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMInfo& vgmInfo = vgmData.GetInfo();

	font = videoDevice.CreateFont("arial.ttf", 12);
}

void VGMMultiChannelWaveFormRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMInfo& vgmInfo = vgmData.GetInfo();

	videoDevice.DestroyFont(font);
	font = nullptr;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& vgmInfo = vgmData.GetInfo();

	/////////////////////////////////////////////////////////////////////////////////
	videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
	videoDevice.LoadIdentity();
	videoDevice.Ortho2D(0, 1, 0, 1);
	videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

	SetViewport(0, 0, 1, 1);

	videoDevice.Disable(VideoDeviceEnum::BLEND);
	videoDevice.DrawWireRectangle
	(
		Vector2(0.0, 0.0), Color::Grey,
		Vector2(1.0, 0.0), Color::Grey,
		Vector2(1.0, 1.0), Color::Grey,
		Vector2(0.0, 1.0), Color::Grey
	);

	/////////////////////////////////////////////////////////////////////////////////
	int startX = 0;
	int endX = VGM_SAMPLE_BUFFER_SIZE; //sampleCount;
	int divX = 20;

	int startY = -32767; //sampleCount/2;
	int endY = 32768; //sampleCount;
	int divY = 3;

	videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
	videoDevice.LoadIdentity();
	videoDevice.Ortho2D(startX, endX, startY, endY);
	videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

	//////////////////////////////////////////////////////////////////////////////
	// draw grid
	videoDevice.Enable(VideoDeviceEnum::BLEND);
	videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE_MINUS_SRC_ALPHA);

	int xCount = 4;
	int yCount = vgmInfo.GetOutputChannelBufferCount() / xCount;

	for (int ch = 0; ch < vgmInfo.GetOutputChannelBufferCount(); ch++)
	{
		float channelViewportWidth = 1.0f / xCount;
		float channelViewportHeight = 1.0f / yCount;
		int x = ch % xCount;
		int y = ch / xCount;

		SetViewport(x * channelViewportWidth, y * channelViewportHeight, channelViewportWidth, channelViewportHeight);

		for (UINT32 i = startX; i < endX; i += (endX - startX) / divX)
		{
			videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
		}
		for (UINT32 i = startY; i < endY; i += (endY - startY) / 10)
		{
			videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
		}

		videoDevice.DrawWireRectangle
		(
			Vector2(startX, startY), Color::Grey,
			Vector2(startX, endY), Color::Grey,
			Vector2(endX, endY), Color::Grey,
			Vector2(endX, startY), Color::Grey
		);
	}

	//////////////////////////////////////////////////////////////////////////////
	// draw channel
	int stepX = 3;
	for (int ch = 0; ch < vgmInfo.GetOutputChannelBufferCount(); ch++)
	{
		float channelViewportWidth = 1.0f / xCount;
		float channelViewportHeight = 1.0f / yCount;
		int x = ch % xCount;
		int y = ch / xCount;

		SetViewport(x * channelViewportWidth, y * channelViewportHeight, channelViewportWidth, channelViewportHeight);

		for (UINT32 i = startX; i < endX - stepX; i += stepX)
		{
			FLOAT32 y0 = vgmInfo.GetOutputChannelBufferSample(ch, i + 0).Right * waveScale;
			FLOAT32 y1 = vgmInfo.GetOutputChannelBufferSample(ch, i + stepX).Right * waveScale;
			videoDevice.DrawLine(Vector2(i, y0), skin.rightColor, Vector2(i + stepX, y1), skin.rightColor);
		}
		/*
		for (UINT32 i = startX; i < (endX - stepX) ; i += stepX)
		{
			FLOAT32 y0 = vgmInfo.GetOutputChannelBufferSample(ch, i + 0).Left * waveScale / 2 + 16384;
			FLOAT32 y1 = vgmInfo.GetOutputChannelBufferSample(ch, i + stepX).Left * waveScale / 2 + 16384;
			videoDevice.DrawLine(Vector2(i, y0), skin.leftColor, Vector2(i + stepX, y1), skin.leftColor);
		}

		for (UINT32 i = startX; i < endX - stepX; i += stepX)
		{
			FLOAT32 y0 = vgmInfo.GetOutputChannelBufferSample(ch, i + 0).Right * waveScale / 2 - 16384;
			FLOAT32 y1 = vgmInfo.GetOutputChannelBufferSample(ch, i + stepX).Right * waveScale / 2 - 16384;
			videoDevice.DrawLine(Vector2(i, y0), skin.rightColor, Vector2(i + stepX, y1), skin.rightColor);
		}
		*/
	}

	//////////////////////////////////////////////////////////////////////////////
	// Draw Name
	videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
	videoDevice.LoadIdentity();
	videoDevice.Ortho2D(0, region.w, 0, region.h);
	videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);
	SetViewport(0, 0, 1, 1);

	videoDevice.SetFont(font);
	videoDevice.SetFontScale(1.0);
	videoDevice.SetFontColor(Color::BrightCyan);
	videoDevice.DrawText(name.c_str(), 0, region.h - font->GetSize());

	for (int ch = 0; ch < vgmInfo.GetOutputChannelBufferCount(); ch++)
	{
		float channelViewportWidth = 1.0f / xCount;
		float channelViewportHeight = 1.0f / yCount;
		int x = ch % xCount;
		int y = ch / xCount;
		
		videoDevice.SetFont(font);
		videoDevice.SetFontScale(1.0);
		videoDevice.SetFontColor(Color::BrightCyan);

		char buf[10];
		sprintf(buf, "ch%02d", ch);
		videoDevice.DrawText(buf, x * channelViewportWidth * region.w, y * channelViewportHeight * region.h);
	}
}
