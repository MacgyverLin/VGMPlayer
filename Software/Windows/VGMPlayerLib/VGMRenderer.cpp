#include "VGMRenderer.h"

VGMRenderer::VGMRenderer(const char* name_, float viewportX_, float viewportY_, float viewportWidth_, float viewportHeight_)
	: name(name_)
	, viewportX(viewportX_)
	, viewportY(viewportY_)
	, viewportWidth(viewportWidth_)
	, viewportHeight(viewportHeight_)
{
}

VGMRenderer::~VGMRenderer()
{
}

void VGMRenderer::SetSize(float viewportX_, float viewportY_, float viewportWidth_, float viewportHeight_)
{
	viewportX = viewportX_;
	viewportY = viewportY_;
	viewportWidth = viewportWidth_;
	viewportHeight = viewportHeight_;
}

void VGMRenderer::GetSize(float& viewportX_, float& viewportY_, float& viewportWidth_, float& viewportHeight_)
{
	viewportX_ = viewportX;
	viewportY_ = viewportY;
	viewportWidth_ = viewportWidth;
	viewportHeight_ = viewportHeight;
}

//////////////////////////////////////////////////////////////////////
void VGMRenderer::SetViewport(float x_, float y_, float width_, float height_)
{
	videoDevice.Viewport(x_ * viewportWidth + viewportX, y_ * viewportHeight + viewportY, width_ * viewportWidth, height_ * viewportHeight);
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
