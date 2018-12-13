#ifndef _VGMAudioPlayer_h_
#define _VGMAudioPlayer_h_

#include "VGMData.h"
#include "VGMDataObverser.h"
#include "SoundDevice.h"

class VGMAudioPlayer : public VGMDataObverser
{
public:
	VGMAudioPlayer(VGMData& vgmData);
	virtual ~VGMAudioPlayer();

	UINT32 getQueued();
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
	SoundDevice outputDevice;
};

#endif