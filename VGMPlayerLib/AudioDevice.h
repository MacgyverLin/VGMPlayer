#ifndef _SoundDevice_h_
#define _SoundDevice_h_

#include "vgmdef.h"

class AudioDeviceImpl;

class AudioDevice
{
public:
	AudioDevice();
	~AudioDevice();

	BOOL open(INT32 channels, INT32 bitsPerSample, INT32 sampleRate, INT32 bufferCount);
	VOID close();

	INT32 play();
	INT32 stop();
	INT32 update();
	INT32 queue(void* data_, int dataSize_);
	INT32 getQueued();
	VOID setVolume(FLOAT32 volume_);
	FLOAT32 getVolume();
	VOID setPlayRate(FLOAT32 playRate_);
	FLOAT32 getPlayRate();

	INT32 getDeviceState();
protected:
private:
	AudioDeviceImpl*	impl;
};

#endif