#ifndef _VGMRenderer_h_
#define _VGMRenderer_h_

#include "VGMData.h"
#include "VideoDevice.h"

class VGMRenderer
{
public:
	VGMRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_ = Rect(0, 0, 320, 240));
	virtual ~VGMRenderer();

public:
	void SetRegion(Rect region_);
	Rect GetRegion() const;
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
	VideoDevice& videoDevice;

	std::string name;
	Rect region;
};

#endif
