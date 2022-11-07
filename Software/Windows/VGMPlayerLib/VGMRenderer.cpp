#include "VGMRenderer.h"

VGMRenderer::VGMRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_)
	: videoDevice(videoDevice_)
	, name(name_)
	, region(region_)
{
}

VGMRenderer::~VGMRenderer()
{
}

void VGMRenderer::SetRegion(Rect region_)
{
	region = region_;
}

Rect VGMRenderer::GetRegion() const
{
	return region;
}

void VGMRenderer::SetName(const char* name_)
{
	name = name_;
}

const char* VGMRenderer::GetName() const
{
	return name.c_str();
}

//////////////////////////////////////////////////////////////////////
void VGMRenderer::SetViewport(float x_, float y_, float width_, float height_)
{
	videoDevice.Viewport(x_ * region.w + region.x, y_ * region.h + region.y, width_ * region.w, height_ * region.h);
}

void VGMRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}
