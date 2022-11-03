#ifndef _VGMFile_h_
#define _VGMFile_h_

#include "VGMData.h"

class VGMFileImpl;

class VGMFile : public VGMData
{
public:
	VGMFile(const char* path_, const char* texturePath_, s32 channels_, s32 bitPerSample_, s32 sampleRate_);
	~VGMFile();

	const char* GetPath() const;
protected:
	virtual boolean OnOpen();
	virtual void OnClose();
	virtual void OnPlay();
	virtual void OnStop();
	virtual void OnPause();
	virtual void OnResume();
	virtual boolean OnUpdate();
	virtual s32 OnRead(void *buffer, u32 size);
	virtual s32 OnSeekSet(u32 size);
	virtual s32 OnSeekCur(u32 size);
private:

public:
protected:
private:
	VGMFileImpl* impl;
};

#endif
