#ifndef _VGMAudioPlayer_h_
#define _VGMAudioPlayer_h_

#include "VGMData.h"
#include "VGMDataObverser.h"
#include "AudioDevice.h"

class VGMAudioPlayer : public VGMDataObverser
{
public:
	VGMAudioPlayer();
	virtual ~VGMAudioPlayer();

	u32 GetQueued();
protected:
	virtual void OnNotifySomething(Obserable& vgmData);
	virtual void OnNotifyOpen(Obserable& vgmData);
	virtual void OnNotifyClose(Obserable& vgmData);
	virtual void OnNotifyPlay(Obserable& vgmData);
	virtual void OnNotifyStop(Obserable& vgmData);
	virtual void OnNotifyPause(Obserable& vgmData);
	virtual void OnNotifyResume(Obserable& vgmData);
	virtual void OnNotifyUpdate(Obserable& vgmData);
private:
public:
protected:
private:
	AudioDevice outputDevice;
};

#endif
