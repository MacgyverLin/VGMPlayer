#include "AudioDevice.h"
#include "assert.h"

AudioDevice::AudioDevice()
{
}

AudioDevice::~AudioDevice()
{
}

BOOL AudioDevice::open(int channels_, int bitsPerSample_, int sampleRate_, int bufferCount_)
{
	ALuint error = 0;

	context = 0;
	device = 0;
	outSource = 0;
	processedBuffer = 0;
	queuedBuffer = 0;
	channels = channels_;
	bitsPerSample = bitsPerSample_;
	sampleRate = sampleRate_;
	playRate = 1.0;
	volume = 1.0;

	WP = 0;

	// device
	device = alcOpenDevice(NULL);
	if (!device)
		return false;

	// context
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	// sources
	alGenSources(1, &outSource);
	alSpeedOfSound(1.0);
	alDopplerVelocity(1.0);
	alDopplerFactor(1.0);
	alSourcei(outSource, AL_LOOPING, AL_FALSE);
	alSourcef(outSource, AL_PITCH, playRate);
	alSourcef(outSource, AL_GAIN, volume);

	// create 32 buffers
	sndBuffers.resize(bufferCount_);
	alGenBuffers(sndBuffers.size(), &sndBuffers[0]);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error alGenBuffers %x \n", error);
		// printf("error alGenBuffers %x : %s\n", ret,alutGetErrorString (ret));
		//AL_ILLEGAL_ENUM
		//AL_INVALID_VALUE
		//#define AL_ILLEGAL_COMMAND                        0xA004
		//#define AL_INVALID_OPERATION                      0xA004
		return false;
	}

	return true;
}

VOID AudioDevice::close()
{
	if (sndBuffers.size() != 0)
	{
		alDeleteBuffers(sndBuffers.size(), &sndBuffers[0]);
	}

	if (outSource)
	{
		alDeleteSources(1, &outSource);
		outSource = 0;
	}

	if (device)
	{
		alcCloseDevice(device);
		device = NULL;
	}

	if (context)
	{
		alcMakeContextCurrent(NULL);

		alcDestroyContext(context);
		context = NULL;
	}
}

INT32 AudioDevice::play()
{
	alSourcePlay(outSource);

	ALuint error;
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error playSound %x\n", error);
		return 0;
	}

	return -1;
}

INT32 AudioDevice::stop()
{
	alSourceStop(outSource);

	ALuint error;
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error stopSound %x\n", error);
		return 0;
	}

	return -1;
}

INT32 AudioDevice::getDeviceState()
{
	int sourceState;
	alGetSourcei(outSource, AL_SOURCE_STATE, &sourceState);

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

INT32 AudioDevice::update()
{
	int processed;
	alGetSourcei(outSource, AL_BUFFERS_PROCESSED, &processed);
	alGetSourcei(outSource, AL_BUFFERS_QUEUED, &queuedBuffer);
	processedBuffer += processed;
	//printf("processedBuffer: %d, processed: %d, queuedBuffer: %d\n", processedBuffer, processed, queuedBuffer);

	while (processed--)
	{
		ALuint buff;

		// unqueue process buffer
		alSourceUnqueueBuffers(outSource, 1, &buff);

		ALuint error;
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			printf("error alSourceUnqueueBuffers %x\n", error);
			//AL_ILLEGAL_ENUM
			//AL_INVALID_VALUE
			//#define AL_ILLEGAL_COMMAND                        0xA004
			//#define AL_INVALID_OPERATION                      0xA004
			return 0;
		}
	}

	return -1;
}

INT32 AudioDevice::queue(void* data_, int dataSize_)
{
	ALenum format = 0;
	ALuint error = 0;

	if (bitsPerSample == 8)
	{
		if (channels == 1)
		{
			format = AL_FORMAT_MONO8;
		}
		else if (channels == 2)
		{
			format = AL_FORMAT_STEREO8;
		}
		else
		{
			printf("Error: Channel must be 1 or 2\n");
			return 0;
		}
	}
	else if (bitsPerSample == 16)
	{
		if (channels == 1)
		{
			format = AL_FORMAT_MONO16;
		}
		else if (channels == 2)
		{
			format = AL_FORMAT_STEREO16;
		}
		else
		{
			printf("Error: Channel must be 1 or 2\n");
			return 0;
		}
	}
	else
	{
		printf("Error: bit per sample must be 8 or 16\n");
		return 0;
	}

	// fill data
	// printf("soundDevice->WP %d\n", WP);
	ALuint buffer = sndBuffers[WP];
	alBufferData(buffer, format, data_, dataSize_, sampleRate);
	error = alGetError();
	int p=0, q = 0;
	if (error != AL_NO_ERROR)
	{
		alGetSourcei(outSource, AL_BUFFERS_PROCESSED, &p);
		alGetSourcei(outSource, AL_BUFFERS_QUEUED, &q);
		printf("alGetError %x: WP=%d, processed: %d, queued: %d, alBufferData(%x, %x, %p, %d, %d)\n",
			error, WP, p, q, buffer, format, data_, dataSize_, sampleRate);
		//AL_ILLEGAL_ENUM
		//AL_INVALID_VALUE
		//#define AL_ILLEGAL_COMMAND                        0xA004
		//#define AL_INVALID_OPERATION                      0xA004
		return 0;
	}

	// queue a buffer
	alSourceQueueBuffers(outSource, 1, &buffer);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error alSourceQueueBuffers %x\n", error);

		return 0;
	}

	WP = WP + 1;
	if(WP>=sndBuffers.size())
	{
		WP = 0;
		if (q == 16)
		{
			return -1;
		}

	}
	//WP = (WP + 1) % sndBuffers.size();

	return -1;
}

INT32 AudioDevice::getQueued()
{
	return queuedBuffer;
}

VOID AudioDevice::setVolume(FLOAT32 volume_)
{
	volume = volume_;

	alSourcef(outSource, AL_GAIN, volume);
}

FLOAT32 AudioDevice::getVolume()
{
	return volume;
}

VOID AudioDevice::setPlayRate(FLOAT32 playRate_)
{
	playRate = playRate_;

	alSourcef(outSource, AL_PITCH, playRate);
}

FLOAT32 AudioDevice::getPlayRate()
{
	return playRate;
}