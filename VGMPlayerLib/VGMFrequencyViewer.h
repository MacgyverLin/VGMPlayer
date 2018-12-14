#ifndef _VGMFrequencyViewer_h_
#define _VGMFrequencyViewer_h_

#include "VGMData.h"
#include "VGMDataObverser.h"
#include "VideoDevice.h"

class VGMFrequencyViewer : public VGMDataObverser
{
public:
	VGMFrequencyViewer(const string& name_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_, const Color& bg_);
	virtual ~VGMFrequencyViewer();
protected:
	virtual void onNotifySomething(Obserable& vgmData);
	virtual void onNotifyOpen(Obserable& vgmData);
	virtual void onNotifyClose(Obserable& vgmData);
	virtual void onNotifyPlay(Obserable& vgmData);
	virtual void onNotifyStop(Obserable& vgmData);
	virtual void onNotifyPause(Obserable& vgmData);
	virtual void onNotifyResume(Obserable& vgmData);
	virtual void onNotifyUpdate(Obserable& vgmData);
private:
public:
protected:
private:
	VideoDevice videoDevice;

	string name;
	UINT32 x;
	UINT32 y;
	UINT32 width;
	UINT32 height;
	Color bg;
};

#endif