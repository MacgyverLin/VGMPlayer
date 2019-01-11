#ifndef _SoundDevice_h_
#define _SoundDevice_h_

#include "vgmdef.h"

class AudioDeviceImpl;

class AudioDevice
{
public:
	AudioDevice();
	~AudioDevice();

	boolean open(s32 channels, s32 bitsPerSample, s32 sampleRate, s32 bufferCount);
	void close();

	s32 play();
	s32 stop();
	s32 update();
	s32 queue(void* data_, int dataSize_);
	s32 getQueued();
	void setVolume(f32 volume_);
	f32 getVolume();
	void setPlayRate(f32 playRate_);
	f32 getPlayRate();

	s32 getDeviceState();
protected:
private:
	AudioDeviceImpl*	impl;
};

#endif