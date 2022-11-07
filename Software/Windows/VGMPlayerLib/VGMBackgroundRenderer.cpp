#include "VGMBackgroundRenderer.h"

VGMBackgroundRenderer::VGMBackgroundRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, const VGMBackgroundRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, skin(skin_)
{
}

VGMBackgroundRenderer::~VGMBackgroundRenderer()
{
}

void VGMBackgroundRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMBackgroundRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();

	texture.Load(info.texturePath.c_str());
}

void VGMBackgroundRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();
}

void VGMBackgroundRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMBackgroundRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMBackgroundRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMBackgroundRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMBackgroundRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		static float t = 0;

		float startX = -1 + t;
		float endX = 1 + t;

		float startY = -1 + t;
		float endY = 1 + t;
		
		t += 0.0001 * 5;
		if (t > 1)
			t = 0;

		/////////////////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, 1, 0, 1);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		/////////////////////////////////////////////////////////////////////////////////
		SetViewport(0, 0, 1, 1);

		videoDevice.Disable(VideoDeviceEnum::BLEND);

		float tileX = ((float)GetRegion().w) / (texture.GetWidth());
		float tileY = ((float)GetRegion().h) / (texture.GetHeight());

		texture.SetWrapS(VideoDeviceEnum::REPEAT);
		texture.SetWrapR(VideoDeviceEnum::REPEAT);
		texture.SetWrapT(VideoDeviceEnum::REPEAT);
		videoDevice.DrawTexSolidRectangle
		(
			texture,
			Vector2(startX, startY), skin.bgColor, Vector2(    0, 0),
			Vector2(  endX, startY), skin.bgColor, Vector2(tileX, 0),
			Vector2(  endX,   endY), skin.bgColor, Vector2(tileX, tileY),
			Vector2(startX,   endY), skin.bgColor, Vector2(    0, tileY)
		);
	}
}
