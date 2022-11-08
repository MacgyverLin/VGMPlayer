#include "AudioDevice.h"
#include "Array.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
using namespace std;

class AudioDeviceImpl
{
public:
	ALCcontext*		context;
	ALCdevice*		device;
	ALuint			outSource;

	Vector<ALuint>	sndBuffers;

	INT32			channels;
	INT32			bitsPerSample;
	INT32			sampleRate;

	FLOAT32			volume;
	FLOAT32			playRate;

	INT32			WP;
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

boolean AudioDevice::Open(INT32 channels_, INT32 bitsPerSample_, INT32 sampleRate_, INT32 bufferCount_)
{
	ALuint error = 0;

	impl->context = 0;
	impl->device = 0;
	impl->outSource = 0;
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

void AudioDevice::Close()
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

INT32 AudioDevice::Play()
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

INT32 AudioDevice::Stop()
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

INT32 AudioDevice::GetDeviceState()
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

INT32 AudioDevice::Update()
{
	int processed;
	alGetSourcei(impl->outSource, AL_BUFFERS_PROCESSED, &processed);

	while (processed--)
	{
		ALuint buff;

		alSourceUnqueueBuffers(impl->outSource, 1, &buff);
	}

	return -1;
}

INT32 AudioDevice::Queue(void* data_, int dataSize_)
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

INT32 AudioDevice::GetQueued()
{
	int queuedBuffer;
	alGetSourcei(impl->outSource, AL_BUFFERS_QUEUED, &queuedBuffer);
	return queuedBuffer;
}

void AudioDevice::SetVolume(FLOAT32 volume_)
{
	impl->volume = volume_;

	alSourcef(impl->outSource, AL_GAIN, impl->volume);
}

FLOAT32 AudioDevice::GetVolume()
{
	return impl->volume;
}

void AudioDevice::SetPlayRate(FLOAT32 playRate_)
{
	impl->playRate = playRate_;

	alSourcef(impl->outSource, AL_PITCH, impl->playRate);
}

FLOAT32 AudioDevice::GetPlayRate()
{
	return impl->playRate;
}
