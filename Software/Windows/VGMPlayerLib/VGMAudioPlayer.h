#ifndef _VGMAudioPlayer_h_
#define _VGMAudioPlayer_h_

#include "VGMData.h"
#include "VGMObverser.h"
#include "AudioDevice.h"

class VGMAudioPlayer : public VGMObverser
{
public:
	VGMAudioPlayer();
	virtual ~VGMAudioPlayer();

	UINT32 GetQueued();
protected:
	virtual void OnNotifySomething(Obserable& vgmData);
	virtual void OnNotifyOpen(Obserable& vgmData);
	virtual void OnNotifyClose(Obserable& vgmData);
	virtual void OnNotifyPlay(Obserable& vgmData);
	virtual void OnNotifyStop(Obserable& vgmData);
	virtual void OnNotifyPause(Obserable& vgmData);
	virtual void OnNotifyResume(Obserable& vgmData);
	virtual void OnNotifyUpdate(Obserable& vgmData, bool needUpdateSample);
private:
public:
protected:
private:
	AudioDevice outputDevice;
};

#endif
