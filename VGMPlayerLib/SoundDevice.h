#ifndef _SoundDevice_h_
#define _SoundDevice_h_

#include "vgmdef.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
using namespace std;

class SoundDevice
{
public:
	SoundDevice();
	~SoundDevice();

	BOOL open(int channels, int bitsPerSample, int sampleRate, int bufferCount);
	VOID close();

	INT32 play();
	INT32 stop();
	INT32 update();
	INT32 queue(void* data_, int dataSize_);
	INT32 getQueued();
	void setVolume(float volume_);
	float getVolume();
	void setPlayRate(float playRate_);
	float getPlayRate();

	INT32 getDeviceState();
protected:
private:
	ALCcontext*		context;
	ALCdevice*		device;
	ALuint			outSource;
	ALint			processedBuffer;
	ALint			queuedBuffer;

	vector<ALuint>	sndBuffers;
	INT32			channels;
	INT32			bitsPerSample;
	INT32			sampleRate;

	FLOAT32			volume;
	FLOAT32			playRate;

	INT32			WP;
};

#endif