#ifndef _VGMFile_h_
#define _VGMFile_h_

#include "VGMData.h"

class VGMFileImpl;

class VGMFile : public VGMData
{
public:
	VGMFile(const char* path_, const char* texturePath_, s32 channels_, s32 bitPerSample_, s32 sampleRate_);
	~VGMFile();
protected:
	virtual boolean onOpen();
	virtual void onClose();
	virtual void onPlay();
	virtual void onStop();
	virtual void onPause();
	virtual void onResume();
	virtual boolean onUpdate();
	virtual s32 onRead(void *buffer, u32 size);
	virtual s32 onSeekSet(u32 size);
	virtual s32 onSeekCur(u32 size);
private:

public:
protected:
private:
	VGMFileImpl* impl;
};

#endif
