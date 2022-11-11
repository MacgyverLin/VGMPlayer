#include "VGMBackgroundRenderer.h"

VGMBackgroundRenderer::VGMBackgroundRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, const VGMBackgroundRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, skin(skin_)
	, font(nullptr)
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
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	

	texture.Load(vgmInfo.texturePath.c_str());

	font = videoDevice.CreateFont("arial.ttf", 12);
}

void VGMBackgroundRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	

	videoDevice.DestroyFont(font);
	font = nullptr;
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
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	


	static float t1 = 0;
	static float t2 = 0;

	float startX = -1 + t1;
	float endX = 1 + t1;

	float startY = -1 + t2;
	float endY = 1 + t2;

	t1 += 0.0001 * 5;
	if (t1 > 1)
		t1 = 0;

	t2 += 0.0001 * 2;
	if (t2 > 1)
		t2 = 0;

	/////////////////////////////////////////////////////////////////////////////////
	videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
	videoDevice.LoadIdentity();
	videoDevice.Ortho2D(0, 1, 0, 1);
	videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

	/////////////////////////////////////////////////////////////////////////////////
	SetViewport(0, 0, 1, 1);

	videoDevice.Disable(VideoDeviceEnum::BLEND);

	float tileX = ((float)GetRegion().w) / (texture.GetWidth()) * 2;
	float tileY = ((float)GetRegion().h) / (texture.GetHeight()) * 2;

	texture.SetWrapS(VideoDeviceEnum::REPEAT);
	texture.SetWrapR(VideoDeviceEnum::REPEAT);
	texture.SetWrapT(VideoDeviceEnum::REPEAT);
	videoDevice.DrawTexSolidRectangle
	(
		texture,
		Vector2(startX, startY), skin.bgColor, Vector2(0, 0),
		Vector2(endX, startY), skin.bgColor, Vector2(tileX, 0),
		Vector2(endX, endY), skin.bgColor, Vector2(tileX, tileY),
		Vector2(startX, endY), skin.bgColor, Vector2(0, tileY)
	);


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
