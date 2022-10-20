#include "AudioDevice.h"
#ifdef STM32
#include <Array.h>
#include <stdlib.h>
#include <al.h>
#include <alc.h>
#else
#include <Array.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#endif
using namespace std;

class AudioDeviceImpl
{
public:
	ALCcontext*		context;
	ALCdevice*		device;
	ALuint			outSource;
	ALint			processedBuffer;
	ALint			queuedBuffer;

	Vector<ALuint>	sndBuffers;

	s32			channels;
	s32			bitsPerSample;
	s32			sampleRate;

	f32			volume;
	f32			playRate;

	s32			WP;
};

AudioDevice::AudioDevice()
{
	impl = new AudioDeviceImpl();
}

AudioDevice::~AudioDevice()
{
	if (impl)
	{
		delete impl;
		impl = 0;
	}
}

ALuint test[10];

boolean AudioDevice::open(s32 channels_, s32 bitsPerSample_, s32 sampleRate_, s32 bufferCount_)
{
	ALuint error = 0;

	impl->context = 0;
	impl->device = 0;
	impl->outSource = 0;
	impl->processedBuffer = 0;
	impl->queuedBuffer = 0;
	impl->channels = channels_;
	impl->bitsPerSample = bitsPerSample_;
	impl->sampleRate = sampleRate_;
	impl->playRate = 1.0;
	impl->volume = 1.0;

	impl->WP = 0;

	// device
	impl->device = alcOpenDevice(NULL);
	if (!impl->device)
		return false;

	// context
	impl->context = alcCreateContext(impl->device, NULL);
	alcMakeContextCurrent(impl->context);

	/*
	ALuint test[10];
	alGenSources(1, &test[0]);
	alGenSources(2, &test[1]);
	alDeleteSources(2, &test[1]);
	alDeleteSources(1, &test[0]);
	//alDeleteSources(2, &test[1]);
	//alDeleteSources(1, &test[1]);
	alGenSources(2, &test[1]);
	alDeleteSources(1, &test[2]);
	alDeleteSources(1, &test[1]);
	*/
	
	alSpeedOfSound(1.0);
	alDopplerVelocity(1.0);
	alDopplerFactor(1.0);

	// sources
	alGenSources(1, &impl->outSource);
	alSourcei(impl->outSource, AL_LOOPING, AL_FALSE);
	alSourcef(impl->outSource, AL_PITCH, impl->playRate);
	alSourcef(impl->outSource, AL_GAIN, impl->volume);

	// create 32 buffers
	impl->sndBuffers.resize(bufferCount_);
	alGenBuffers(impl->sndBuffers.size(), &impl->sndBuffers[0]);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//printf("error alGenBuffers %x \n", error);
		return FALSE;
	}

	return TRUE;
}

void AudioDevice::close()
{
	if (impl->sndBuffers.size() != 0)
	{
		alDeleteBuffers(impl->sndBuffers.size(), &impl->sndBuffers[0]);
	}

	if(impl->outSource)
	{
		alDeleteSources(1, &impl->outSource);
		impl->outSource = 0;
	}

	if(impl->device)
	{
		alcCloseDevice(impl->device);
		impl->device = NULL;
	}

	if(impl->context)
	{
		alcMakeContextCurrent(NULL);

		alcDestroyContext(impl->context);
		impl->context = NULL;
	}
}

s32 AudioDevice::play()
{
	alSourcePlay(impl->outSource);

	ALuint error;
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//printf("error playSound %x\n", error);
		return 0;
	}

	return -1;
}

s32 AudioDevice::stop()
{
	alSourceStop(impl->outSource);

	ALuint error;
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//printf("error stopSound %x\n", error);
		return 0;
	}

	return -1;
}

s32 AudioDevice::getDeviceState()
{
	int sourceState;
	alGetSourcei(impl->outSource, AL_SOURCE_STATE, &sourceState);

	switch (sourceState)
	{
	case AL_INITIAL:
		return 0;
		break;
	case AL_PAUSED:
		return 1;
		break;
	case AL_STOPPED:
		return 2;
		break;
	default:
	case AL_PLAYING:
		return 3;
		break;
	};
}

s32 AudioDevice::update()
{
	int queuedBuffer;
	int processed;
	alGetSourcei(impl->outSource, AL_BUFFERS_PROCESSED, &processed);
	alGetSourcei(impl->outSource, AL_BUFFERS_QUEUED, &queuedBuffer);
	impl->processedBuffer += processed;
	impl->queuedBuffer = queuedBuffer;
	//printf("processedBuffer: %d, processed: %d, queuedBuffer: %d\n", processedBuffer, processed, queuedBuffer);

	while (processed--)
	{
		ALuint buff;

		// unqueue process buffer
		alSourceUnqueueBuffers(impl->outSource, 1, &buff);

		ALuint error;
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			//printf("error alSourceUnqueueBuffers %x\n", error);
			//AL_ILLEGAL_ENUM
			//AL_INVALID_VALUE
			//#define AL_ILLEGAL_COMMAND                        0xA004
			//#define AL_INVALID_OPERATION                      0xA004
			return 0;
		}
	}

	return -1;
}

u32 entered = 0;

s32 AudioDevice::queue(void* data_, int dataSize_)
{
	ALenum format = 0;
	ALuint error = 0;

	if (impl->bitsPerSample == 8)
	{
		if (impl->channels == 1)
		{
			format = AL_FORMAT_MONO8;
		}
		else if (impl->channels == 2)
		{
			format = AL_FORMAT_STEREO8;
		}
		else
		{
			//printf("Error: Channel must be 1 or 2\n");
			return 0;
		}
	}
	else if (impl->bitsPerSample == 16)
	{
		if (impl->channels == 1)
		{
			format = AL_FORMAT_MONO16;
		}
		else if (impl->channels == 2)
		{
			format = AL_FORMAT_STEREO16;
		}
		else
		{
			//printf("Error: Channel must be 1 or 2\n");
			return 0;
		}
	}
	else
	{
		//printf("Error: bit per sample must be 8 or 16\n");
		return 0;
	}
	
	entered++;
	if(entered==3)
	{
		entered = 0;
	}

	// fill data
	// printf("soundDevice->WP %d\n", WP);
	ALuint buffer = impl->sndBuffers[impl->WP];
	alBufferData(buffer, format, data_, dataSize_, impl->sampleRate);
	error = alGetError();
	int p=0, q = 0;
	if (error != AL_NO_ERROR)
	{
		alGetSourcei(impl->outSource, AL_BUFFERS_PROCESSED, &p);
		alGetSourcei(impl->outSource, AL_BUFFERS_QUEUED, &q);
		//printf("alGetError %x: WP=%d, processed: %d, queued: %d, alBufferData(%x, %x, %p, %d, %d)\n", error, impl->WP, p, q, buffer, format, data_, dataSize_, impl->sampleRate);
		//AL_ILLEGAL_ENUM
		//AL_INVALID_VALUE
		//#define AL_ILLEGAL_COMMAND                        0xA004
		//#define AL_INVALID_OPERATION                      0xA004
		return 0;
	}

	// queue a buffer
	alSourceQueueBuffers(impl->outSource, 1, &buffer);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		// printf("error alSourceQueueBuffers %x\n", error);
		return 0;
	}

	impl->WP = impl->WP + 1;
	if(impl->WP>= impl->sndBuffers.size())
	{
		impl->WP = 0;
	}
	//WP = (WP + 1) % sndBuffers.size();

	return -1;
}

s32 AudioDevice::getQueued()
{
	return impl->queuedBuffer;
}

void AudioDevice::setVolume(f32 volume_)
{
	impl->volume = volume_;

	alSourcef(impl->outSource, AL_GAIN, impl->volume);
}

f32 AudioDevice::getVolume()
{
	return impl->volume;
}

void AudioDevice::setPlayRate(f32 playRate_)
{
	impl->playRate = playRate_;

	alSourcef(impl->outSource, AL_PITCH, impl->playRate);
}

f32 AudioDevice::getPlayRate()
{
	return impl->playRate;
}
