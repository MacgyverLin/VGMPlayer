#include "AudioDevice.h"
#ifdef STM32
#else		
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#endif
using namespace std;

class AudioDeviceImpl
{
public:
#ifdef STM32
#else			
	ALCcontext*		context;
	ALCdevice*		device;
	ALuint			outSource;
	ALint			processedBuffer;
	ALint			queuedBuffer;

	vector<ALuint>	sndBuffers;
#endif
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

boolean AudioDevice::open(s32 channels_, s32 bitsPerSample_, s32 sampleRate_, s32 bufferCount_)
{
#ifdef STM32
	return TRUE;
#else		
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

	// sources
	alGenSources(1, &impl->outSource);
	alSpeedOfSound(1.0);
	alDopplerVelocity(1.0);
	alDopplerFactor(1.0);
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
		return false;
	}

	return true;
#endif
}

void AudioDevice::close()
{
#ifdef STM32
#else	
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
#endif
}

s32 AudioDevice::play()
{
#ifdef STM32
	return -1;
#else	
	alSourcePlay(impl->outSource);

	ALuint error;
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//printf("error playSound %x\n", error);
		return 0;
	}

	return -1;
#endif
}

s32 AudioDevice::stop()
{
#ifdef STM32
	return -1;
#else	
	alSourceStop(impl->outSource);

	ALuint error;
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		//printf("error stopSound %x\n", error);
		return 0;
	}

	return -1;
#endif	
}

s32 AudioDevice::getDeviceState()
{
#ifdef STM32
	return 3;
#else	
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
#endif
}

s32 AudioDevice::update()
{
#ifdef STM32
	return -1;
#else
	int processed;
	alGetSourcei(impl->outSource, AL_BUFFERS_PROCESSED, &processed);
	alGetSourcei(impl->outSource, AL_BUFFERS_QUEUED, &impl->queuedBuffer);
	impl->processedBuffer += processed;
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
#endif
}

s32 AudioDevice::queue(void* data_, int dataSize_)
{
#ifdef STM32
	return -1;
#else	
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
#endif
}

s32 AudioDevice::getQueued()
{
#ifdef STM32
	return 0;
#else	
	return impl->queuedBuffer;
#endif
}

void AudioDevice::setVolume(f32 volume_)
{
	impl->volume = volume_;
#ifdef STM32
#else	
	alSourcef(impl->outSource, AL_GAIN, impl->volume);
#endif
}

f32 AudioDevice::getVolume()
{
	return impl->volume;
}

void AudioDevice::setPlayRate(f32 playRate_)
{
	impl->playRate = playRate_;
#ifdef STM32
#else	
	alSourcef(impl->outSource, AL_PITCH, impl->playRate);
#endif
}

f32 AudioDevice::getPlayRate()
{
	return impl->playRate;
}
