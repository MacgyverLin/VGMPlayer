#ifndef _SoundDevice_h_
#define _SoundDevice_h_

#include "vgmdef.h"

class AudioDeviceImpl;

class AudioDevice
{
public:
	AudioDevice();
	~AudioDevice();

	boolean Open(INT32 channels, INT32 bitsPerSample, INT32 sampleRate, INT32 bufferCount);
	void Close();

	INT32 Play();
	INT32 Stop();
	INT32 Update();
	INT32 Queue(void* data_, int dataSize_);
	INT32 GetQueued();
	void SetVolume(FLOAT32 volume_);
	FLOAT32 GetVolume();
	void SetPlayRate(FLOAT32 playRate_);
	FLOAT32 GetPlayRate();

	INT32 GetDeviceState();
protected:
private:
	AudioDeviceImpl*	impl;
};

#endif
