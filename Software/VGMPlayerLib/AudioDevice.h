#ifndef _SoundDevice_h_
#define _SoundDevice_h_

#include "vgmdef.h"

class AudioDeviceImpl;

class AudioDevice
{
public:
	AudioDevice();
	~AudioDevice();

	boolean Open(s32 channels, s32 bitsPerSample, s32 sampleRate, s32 bufferCount);
	void Close();

	s32 Play();
	s32 Stop();
	s32 Update();
	s32 Queue(void* data_, int dataSize_);
	s32 GetQueued();
	void SetVolume(f32 volume_);
	f32 GetVolume();
	void SetPlayRate(f32 playRate_);
	f32 GetPlayRate();

	s32 GetDeviceState();
protected:
private:
	AudioDeviceImpl*	impl;
};

#endif
