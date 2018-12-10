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
	(*soundDevice)->processed = 0;
	(*soundDevice)->queued = 0;
	(*soundDevice)->processedBuffer = 0;

	(*soundDevice)->channels = channels;
	(*soundDevice)->bitsPerSample = bitsPerSample;
	(*soundDevice)->sampleRate = sampleRate;
	(*soundDevice)->bufferSize = bufferSize;
	(*soundDevice)->bufferCount = bufferCount;
	(*soundDevice)->playRate = 1.0;
	(*soundDevice)->volume = 1.0;

	(*soundDevice)->device = alcOpenDevice(NULL);
	if (!(*soundDevice)->device)
		return 0;

	//建立声音文本描述
	(*soundDevice)->context = alcCreateContext((*soundDevice)->device, NULL);

	//设置行为文本描述
	alcMakeContextCurrent((*soundDevice)->context);

	//创建一个source并设置一些属性
	alGenSources(1, &((*soundDevice)->outSource));
	alSpeedOfSound(1.0);
	alDopplerVelocity(1.0);
	alDopplerFactor(1.0);
	alSourcei((*soundDevice)->outSource, AL_LOOPING, AL_FALSE);
	alSourcef((*soundDevice)->outSource, AL_PITCH, (*soundDevice)->playRate);
	alSourcef((*soundDevice)->outSource, AL_GAIN, (*soundDevice)->volume);

#if 1
	//创建一个buffer
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
#endif
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error initOpenAL %x\n", error);
		return 0;
	}

	return -1;
}

void SoundDevice_Release(SoundDevice* soundDevice)
{
	if (soundDevice)
	{
#if 1
		if (soundDevice->sndBuffer)
		{
			alDeleteBuffers(soundDevice->bufferCount, soundDevice->sndBuffer);
		}
#endif
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
	int error;
	ALuint sourceState;

	alSourcePlay(soundDevice->outSource);

	alGetSourcei(soundDevice->outSource, AL_SOURCE_STATE, &sourceState);
	switch (sourceState) {
	case AL_INITIAL:
		printf("AL_INITIAL\n");
		break;
	case AL_PAUSED:
		printf("AL_PAUSED\n");
		break;
	case AL_STOPPED:
		printf("AL_STOPPED\n");
		break;
	case AL_PLAYING:
		printf("AL_PLAYING\n");
		break;
	};

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
	//播放状态字段
	ALint sourceState = 0;
	ALint offset = 0;

	//获取处理队列，得出已经播放过的缓冲器的数量
	//获取缓存队列，缓存的队列数量
	//获取播放状态，是不是正在播放
	alGetSourcei(soundDevice->outSource, AL_BUFFERS_PROCESSED, &soundDevice->processed);
	alGetSourcei(soundDevice->outSource, AL_BUFFERS_QUEUED, &soundDevice->queued);
	alGetSourcei(soundDevice->outSource, AL_SOURCE_STATE, &sourceState);
	printf("processed: %d, queued:%d\n", soundDevice->processed, soundDevice->queued);
#if 1
	alGetSourcei(soundDevice->outSource, AL_SOURCE_STATE, &sourceState);
	if (sourceState == AL_STOPPED ||
		sourceState == AL_PAUSED ||
		sourceState == AL_INITIAL)
	{
		//如果没有数据,或数据播放完了
		if (soundDevice->queued < soundDevice->processed ||
			soundDevice->queued == 0 ||
			(soundDevice->queued == 1 && soundDevice->processed == 1))
		{
			//停止播放
			printf("...Audio Stop\n");

			// stopSound();
			// cleanUpOpenAL();

			return 0;
		}
	}
#else
#endif

	switch (sourceState) {
	case AL_INITIAL:
		printf("AL_INITIAL\n");
		break;
	case AL_PAUSED:
		printf("AL_PAUSED\n");
		break;
	case AL_STOPPED:
		printf("AL_STOPPED\n");
		break;
	case AL_PLAYING:
		printf("AL_PLAYING\n");
		break;
	};

	//将已经播放过的的数据删除掉
	while (soundDevice->processed--)
	{
		ALuint buff;

		// 更新缓存buffer中的数据到source中
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
#if 1
#else
		// 删除缓存buff中的数据
		alDeleteBuffers(1, &buff);
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			printf("error alDeleteBuffers %x\n", error);
			//AL_ILLEGAL_ENUM
			//AL_INVALID_VALUE
			//#define AL_ILLEGAL_COMMAND                        0xA004
			//#define AL_INVALID_OPERATION                      0xA004
			return 0;
		}
#endif
		// 得到已经播放的音频队列多少块
		soundDevice->processedBuffer++;
		soundDevice->RP++;
		if(soundDevice->RP >= soundDevice->bufferCount)
			soundDevice->RP = 0;
	}

	return -1;
}

int SoundDevice_AddAudioToQueue(SoundDevice* soundDevice, int WP, char* data_, int dataSize_)
{
	//样本数openal的表示方法
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

	//指定要将数据复制到缓冲区中的数据
	ALuint buffer = soundDevice->sndBuffer[WP];
	alBufferData(buffer, format, data_, dataSize_, soundDevice->sampleRate);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error alBufferData %x\n", error);
		//AL_ILLEGAL_ENUM
		//AL_INVALID_VALUE
		//#define AL_ILLEGAL_COMMAND                        0xA004
		//#define AL_INVALID_OPERATION                      0xA004
		return 0;
	}

	//附加一个或一组buffer到一个source上
	alSourceQueueBuffers(soundDevice->outSource, 1, &buffer);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error alSourceQueueBuffers %x\n", error);

		return 0;
	}

	return -1;
}

int SoundDevice_GetQueuedAudioCount(SoundDevice* soundDevice)
{
	alGetSourcei(soundDevice->outSource, AL_BUFFERS_QUEUED, &soundDevice->queued);

	return soundDevice->queued;
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