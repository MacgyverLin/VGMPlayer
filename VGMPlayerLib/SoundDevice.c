#include "SoundDevice.h"

int SoundDevice_Create(SoundDevice** soundDevice, int channels, int bitsPerSample, int sampleRate, int bufferSize, int bufferCount)
{
	ALuint error = 0;

	*soundDevice = (SoundDevice*)malloc(sizeof(SoundDevice));
	if (!(*soundDevice))
		return 0;

	(*soundDevice)->context = 0;
	(*soundDevice)->device = 0;
	(*soundDevice)->outSource = 0;
	(*soundDevice)->processedBuffer = 0;
	(*soundDevice)->queuedBuffer = 0;
	(*soundDevice)->channels = channels;
	(*soundDevice)->bitsPerSample = bitsPerSample;
	(*soundDevice)->sampleRate = sampleRate;
	(*soundDevice)->bufferSize = bufferSize;
	(*soundDevice)->bufferCount = bufferCount;
	(*soundDevice)->playRate = 1.0;
	(*soundDevice)->volume = 1.0;

	// device
	(*soundDevice)->device = alcOpenDevice(NULL);
	if (!(*soundDevice)->device)
		return 0;

	// context
	(*soundDevice)->context = alcCreateContext((*soundDevice)->device, NULL);
	alcMakeContextCurrent((*soundDevice)->context);

	// sources
	alGenSources(1, &((*soundDevice)->outSource));
	alSpeedOfSound(1.0);
	alDopplerVelocity(1.0);
	alDopplerFactor(1.0);
	alSourcei((*soundDevice)->outSource, AL_LOOPING, AL_FALSE);
	alSourcef((*soundDevice)->outSource, AL_PITCH, (*soundDevice)->playRate);
	alSourcef((*soundDevice)->outSource, AL_GAIN, (*soundDevice)->volume);

	// create 32 buffers
	for(int i=0; i< (*soundDevice)->bufferCount; i++)
		(*soundDevice)->sndBuffer[i] = 0;
	alGenBuffers((*soundDevice)->bufferCount, (*soundDevice)->sndBuffer);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error alGenBuffers %x \n", error);
		// printf("error alGenBuffers %x : %s\n", ret,alutGetErrorString (ret));
		//AL_ILLEGAL_ENUM
		//AL_INVALID_VALUE
		//#define AL_ILLEGAL_COMMAND                        0xA004
		//#define AL_INVALID_OPERATION                      0xA004
		return 0;
	}

	return -1;
}

void SoundDevice_Release(SoundDevice* soundDevice)
{
	if (soundDevice)
	{
		if (soundDevice->sndBuffer)
		{
			alDeleteBuffers(soundDevice->bufferCount, soundDevice->sndBuffer);
		}

		if (soundDevice->outSource)
		{
			alDeleteSources(1, &(soundDevice->outSource));
			soundDevice->outSource = 0;
		}

		if (soundDevice->device)
		{
			alcCloseDevice(soundDevice->device);
			soundDevice->device = NULL;
		}

		if (soundDevice->context)
		{
			alcMakeContextCurrent(NULL);

			alcDestroyContext(soundDevice->context);
			soundDevice->context = NULL;
		}

		free(soundDevice);
		soundDevice = NULL;
	}
}

int SoundDevice_PlaySound(SoundDevice* soundDevice)
{
	ALuint error;
	alSourcePlay(soundDevice->outSource);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error playSound %x\n", error);
		return 0;
	}

	return -1;
}

int SoundDevice_StopSound(SoundDevice* soundDevice)
{
	int error;

	alSourceStop(soundDevice->outSource);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error stopSound %x\n", error);
		return 0;
	}

	return -1;
}

int GetDeviceState(SoundDevice* soundDevice)
{
	int sourceState;
	alGetSourcei(soundDevice->outSource, AL_SOURCE_STATE, &sourceState);

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

int SoundDevice_UpdataQueueBuffer(SoundDevice* soundDevice)
{
	int processed;
	alGetSourcei(soundDevice->outSource, AL_BUFFERS_PROCESSED, &processed);
	alGetSourcei(soundDevice->outSource, AL_BUFFERS_QUEUED, &soundDevice->queuedBuffer);
	soundDevice->processedBuffer += processed;
	
	//printf("processedBuffer: %d, processed: %d, queuedBuffer: %d\n", soundDevice->processedBuffer, processed, soundDevice->queuedBuffer);

	while (processed--)
	{
		ALuint buff;

		// unqueue process buffer
		alSourceUnqueueBuffers(soundDevice->outSource, 1, &buff);

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

int SoundDevice_AddAudioToQueue(SoundDevice* soundDevice, int WP, char* data_, int dataSize_)
{
	ALenum format = 0;
	ALuint error = 0;

	if (soundDevice->bitsPerSample == 8)
	{
		if (soundDevice->channels == 1)
		{
			format = AL_FORMAT_MONO8;
		}
		else if (soundDevice->channels == 2)
		{
			format = AL_FORMAT_STEREO8;
		}
		else
		{
			printf("Error: Channel must be 1 or 2\n");
			return 0;
		}
	}
	else if (soundDevice->bitsPerSample == 16)
	{
		if (soundDevice->channels == 1)
		{
			format = AL_FORMAT_MONO16;
		}
		else if (soundDevice->channels == 2)
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
	ALuint buffer = soundDevice->sndBuffer[WP];
	alBufferData(buffer, format, data_, dataSize_, soundDevice->sampleRate);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		int p, q;
		alGetSourcei(soundDevice->outSource, AL_BUFFERS_PROCESSED, &p);
		alGetSourcei(soundDevice->outSource, AL_BUFFERS_QUEUED, &q);		
		printf("alGetError %x: WP=%d, processed: %d, queued: %d, alBufferData(%x, %x, %p, %d, %d)\n", 
			error, WP, p, q, buffer, format, data_, dataSize_, soundDevice->sampleRate);
		//AL_ILLEGAL_ENUM
		//AL_INVALID_VALUE
		//#define AL_ILLEGAL_COMMAND                        0xA004
		//#define AL_INVALID_OPERATION                      0xA004
		return 0;
	}

	// queue a buffer
	alSourceQueueBuffers(soundDevice->outSource, 1, &buffer);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error alSourceQueueBuffers %x\n", error);

		return 0;
	}

	alGetSourcei(soundDevice->outSource, AL_BUFFERS_QUEUED, &soundDevice->queuedBuffer);

	return -1;
}

int SoundDevice_GetQueuedAudioCount(SoundDevice* soundDevice)
{
	return soundDevice->queuedBuffer;
}

void SoundDevice_SetVolume(SoundDevice* soundDevice, float volume_)
{
	soundDevice->volume = volume_;

	alSourcef(soundDevice->outSource, AL_GAIN, soundDevice->volume);
}

float SoundDevice_GetVolume(SoundDevice* soundDevice)
{
	return soundDevice->volume;
}

void SoundDevice_SetPlayRate(SoundDevice* soundDevice, float playRate_)
{
	soundDevice->playRate = playRate_;

	alSourcef(soundDevice->outSource, AL_PITCH, soundDevice->playRate);
}

float SoundDevice_GetPlayRate(SoundDevice* soundDevice)
{
	return soundDevice->playRate;
}