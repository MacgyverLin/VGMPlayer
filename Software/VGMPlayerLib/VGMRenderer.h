#ifndef _VGMRenderer_h_
#define _VGMRenderer_h_

#include "VGMData.h"
#include "VideoDevice.h"

class VGMRenderer
{
public:
	VGMRenderer(const char* name_, float viewportX_ = 0, float viewportY_ = 0, float viewportWidth_ = 320, float viewportHeight_ = 240);
	virtual ~VGMRenderer();

public:
	void SetSize(float viewportX_, float viewportY_, float viewportWidth_, float viewportHeight_);
	void GetSize(float& viewportX_, float& viewportY_, float& viewportWidth_, float& viewportHeight_);
protected:
	//////////////////////////////////////////////////////////////////////
	void SetViewport(float x_, float y_, float width_, float height_);
protected:
	virtual void OnNotifySomething(Obserable& vgmData) = 0;
	virtual void OnNotifyOpen(Obserable& vgmData) = 0;
	virtual void OnNotifyClose(Obserable& vgmData) = 0;
	virtual void OnNotifyPlay(Obserable& vgmData) = 0;
	virtual void OnNotifyStop(Obserable& vgmData) = 0;
	virtual void OnNotifyPause(Obserable& vgmData) = 0;
	virtual void OnNotifyResume(Obserable& vgmData) = 0;
	virtual void OnNotifyUpdate(Obserable& vgmData) = 0;
private:
public:
protected:
	VideoDevice videoDevice;

	std::string name;
	float viewportX;
	float viewportY;
	float viewportWidth;
	float viewportHeight;
};

#endif
