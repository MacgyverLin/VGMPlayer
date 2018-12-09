#include "SoundDevice.h"

#ifdef SOUND_DEVICE_USE_OPENAL

#define COPY_LOCK

int createOpenAL(SoundDevice** soundDevice, int channels, int bitsPerSample, int sampleRate, int bufferSegmentSize, int bufferSegmentCount)
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
	(*soundDevice)->bufferSegmentSize = bufferSegmentSize;
	(*soundDevice)->bufferSegmentCount = bufferSegmentCount;
	(*soundDevice)->playRate = 1.0;
	(*soundDevice)->volume = 1.0;

	(*soundDevice)->device = alcOpenDevice(NULL);
	if ((*soundDevice)->device)
	{
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

		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			printf("error initOpenAL %x\n", error);
			return 0;
		}

		return -1;
	}
	else
	{
		return 0;
	}
}

void releaseOpenAL(SoundDevice* soundDevice)
{
	if (soundDevice)
	{
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

int playSound(SoundDevice* soundDevice)
{
	int error;

	alSourcePlay(soundDevice->outSource);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error playSound %x\n", error);
		return 0;
	}

	return -1;
}

int stopSound(SoundDevice* soundDevice)
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

int updataQueueBuffer(SoundDevice* soundDevice)
{
	//播放状态字段
	ALint sourceState = 0;

	//获取处理队列，得出已经播放过的缓冲器的数量
	//获取缓存队列，缓存的队列数量
	//获取播放状态，是不是正在播放
	alGetSourcei(soundDevice->outSource, AL_BUFFERS_PROCESSED, &soundDevice->processed);
	alGetSourcei(soundDevice->outSource, AL_BUFFERS_QUEUED, &soundDevice->queued);
	printf("processed=%d, queued=%d\n", soundDevice->processed, soundDevice->queued);

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

	//将已经播放过的的数据删除掉
	while (soundDevice->processed--)
	{
		ALuint buff;

		// 更新缓存buffer中的数据到source中
		alSourceUnqueueBuffers(soundDevice->outSource, 1, &buff);

		// 删除缓存buff中的数据
		alDeleteBuffers(1, &buff);

		// 得到已经播放的音频队列多少块
		soundDevice->processedBuffer++;
	}

	return -1;
}

int addAudioToQueue(SoundDevice* soundDevice, char* data_, int dataSize_)
{
	//样本数openal的表示方法
	ALenum format = 0;
	//buffer id 负责缓存,要用局部变量每次数据都是新的地址
	ALuint bufferID = 0;
	ALuint error = 0;

	//创建一个buffer
	alGenBuffers(1, &bufferID);
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
	alBufferData(bufferID, format, data_, dataSize_, soundDevice->sampleRate);
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
	alSourceQueueBuffers(soundDevice->outSource, 1, &bufferID);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		printf("error alSourceQueueBuffers %x\n", error);

		return 0;
	}

	return -1;
}

void setVolume(SoundDevice* soundDevice, float volume_)
{
	soundDevice->volume = volume_;

	alSourcef(soundDevice->outSource, AL_GAIN, soundDevice->volume);
}

float getVolume(SoundDevice* soundDevice)
{
	return soundDevice->volume;
}

void setPlayRate(SoundDevice* soundDevice, float playRate_)
{
	soundDevice->playRate = playRate_;

	alSourcef(soundDevice->outSource, AL_PITCH, soundDevice->playRate);
}

float getPlayRate(SoundDevice* soundDevice)
{
	return soundDevice->playRate;
}

//////////////////////////////////////////////////////////////////////////////


int DirectSound_Create(void *ptr1, SoundDevice** soundDevice, void *ptr2)
{
	//if (!createOpenAL(soundDevice))
		return 0;
}

int DirectSound_SetCooperativeLevel(SoundDevice* soundDevice)
{
	return -1;
}

void DirectSound_Release(SoundDevice* soundDevice)
{
	stopSound(soundDevice);

	releaseOpenAL(soundDevice);
}

int DirectSoundSoundBuffer_Create(SoundDevice* soundDevice,
	int primary,
	unsigned int channels, unsigned int sampleRate, unsigned int bitsPerSample, unsigned int bytesPerUnit,
	unsigned int bufferSize, SoundBuffer** soundBuffer)
{
	return -1;
}

void DirectSoundSoundBuffer_Release(SoundBuffer* soundBuffer)
{
}

int DirectSound_Update(SoundDevice* soundDevice, float dt)
{
	/*
	int bytesToCopy = soundDevice->primary->bytesPerSec * dt;

	while (bytesToCopy > 0)
	{
		int pBytesToCopy;
		int pOverFlow = soundDevice->primary->wp + bytesToCopy - soundDevice->primary->bufferSize;
		if (pOverFlow > 0) // buffer over
		{
			pBytesToCopy = bytesToCopy - pOverFlow;
		}
		else
		{
			pBytesToCopy = bytesToCopy;
		}

		int sBytesToCopy;
		int sOverFlow = soundDevice->sources->rp + bytesToCopy - soundDevice->sources->bufferSize;
		if (sOverFlow > 0) // buffer over
		{
			sBytesToCopy = bytesToCopy - sOverFlow;
		}
		else
		{
			sBytesToCopy = bytesToCopy;
		}

		int writeLen = min(pBytesToCopy, sBytesToCopy);

		char *dst = &soundDevice->primary->buffer[soundDevice->primary->wp];
		soundDevice->primary->wp += writeLen;
		if (soundDevice->primary->wp >= soundDevice->primary->bufferSize)
		{
			soundDevice->primary->wp = 0;
		}

		char *src = &soundDevice->sources->buffer[soundDevice->sources->rp];
		soundDevice->sources->rp += writeLen;
		if (soundDevice->sources->rp >= soundDevice->sources->bufferSize)
		{
			soundDevice->sources->rp = 0;
		}

		memcpy(dst, src, writeLen);

		bytesToCopy -= writeLen;
	}
	*/

	return -1;
}

void DirectSoundSoundBuffer_GetCurrentPosition(SoundBuffer* soundBuffer, unsigned long* R)
{
	if (soundBuffer->primary)
	{
		*R = soundBuffer->wp;
	}
	else
	{
		*R = soundBuffer->rp;
	}
}

int DirectSoundSoundBuffer_Play(SoundBuffer* soundBuffer, int a, int b, int loop)
{
	return -1;
}

int DirectSoundSoundBuffer_Stop(SoundBuffer* soundBuffer)
{
	return -1;
}

int DirectSoundSoundBuffer_Lock(SoundBuffer* soundBuffer,
	unsigned long offset, unsigned long length,
	void** audioPtr, int* audioBytes)
{
	if (soundBuffer->lockedBuffer)
		return 0;

	if (length == 0 || (offset + length) > soundBuffer->bufferSize)
		return 0;

#ifdef COPY_LOCK
	soundBuffer->lockedBuffer = malloc(length);
	memcpy(soundBuffer->lockedBuffer, &soundBuffer->buffer[offset], length);
#else
	soundBuffer->lockedBuffer = &soundBuffer->buffer[offset];
#endif
	if (!soundBuffer->lockedBuffer)
		return 0;

	soundBuffer->lockedOffset = offset;
	soundBuffer->lockedLength = length;

	*audioPtr = soundBuffer->lockedBuffer;
	*audioBytes = length;

	return -1;
}

int DirectSoundSoundBuffer_Unlock(SoundBuffer* soundBuffer, void* audioPtr, int audioBytes)
{
	if (!soundBuffer->lockedBuffer)
		return 0;

	if (audioBytes > soundBuffer->lockedLength)
		return 0;

#ifdef COPY_LOCK
	memcpy(&soundBuffer->buffer[soundBuffer->lockedOffset], audioPtr, audioBytes);
	free(soundBuffer->lockedBuffer);
#else
#endif

	soundBuffer->lockedBuffer = 0;
	soundBuffer->lockedOffset = 0;
	soundBuffer->lockedLength = 0;

	return -1;
}

#else 


int DirectSound_Create(void *ptr1, SoundDevice** soundDevice, void *ptr2)
{
	*soundDevice = (SoundDevice*)malloc(sizeof(SoundDevice));

	return -1;
}

int DirectSound_SetCooperativeLevel(SoundDevice* soundDevice)
{
	return -1;
}

void DirectSound_Release(SoundDevice* soundDevice)
{
	if (soundDevice)
	{
		free(soundDevice);
		soundDevice = 0;
	}
}

int DirectSoundSoundBuffer_Create(
	SoundDevice* soundDevice,
	int primary,
	unsigned int channels,
	unsigned int sampleRate,
	unsigned int bitsPerSample,
	unsigned int bytesPerUnit,
	unsigned int bufferSize,
	SoundBuffer** soundBuffer)
{
	if (!soundDevice)
		return 0;

	*soundBuffer = (SoundBuffer*)malloc(sizeof(SoundBuffer));
	if (!(*soundBuffer))
		return 0;

	memset(*soundBuffer, 0, sizeof(SoundBuffer));
	(*soundBuffer)->primary = primary;
	(*soundBuffer)->channels = channels;
	(*soundBuffer)->sampleRate = sampleRate;
	(*soundBuffer)->bitsPerSample = bitsPerSample;
	(*soundBuffer)->bytesPerUnit = bytesPerUnit;
	(*soundBuffer)->bytesPerSec = sampleRate * bytesPerUnit;
	(*soundBuffer)->bufferSize = bufferSize;
	(*soundBuffer)->WP = 0;
	(*soundBuffer)->RP = 0;

	(*soundBuffer)->buffer = malloc(bufferSize);
	if (!((*soundBuffer)->buffer))
	{
		if (*soundBuffer)
		{
			free(*soundBuffer);
		}
		return 0;
	}

	memset((*soundBuffer)->buffer, 0x5a, bufferSize);
	(*soundBuffer)->lockedBuffer = 0;
	(*soundBuffer)->lockedOffset = 0;
	(*soundBuffer)->lockedLength = 0;

	if (primary)
	{
		soundDevice->primary = (*soundBuffer);
	}
	else
	{
		soundDevice->sources = (*soundBuffer);
	}

	return -1;
}

void DirectSoundSoundBuffer_Release(SoundBuffer* soundBuffer)
{
	if (soundBuffer)
	{
#ifdef COPY_LOCK
		if (soundBuffer->lockedBuffer)
		{
			free(soundBuffer->lockedBuffer);
			soundBuffer->lockedBuffer = 0;
		}
#endif

		if (soundBuffer->buffer)
		{
			free(soundBuffer->buffer);
			soundBuffer->buffer = 0;
		}

		free(soundBuffer);
		soundBuffer = 0;
	}
}

int DirectSound_Update(SoundDevice* soundDevice, float dt)
{
	int bytesToCopy = soundDevice->primary->bytesPerSec * dt;

	while (bytesToCopy > 0)
	{
		int pBytesToCopy;
		int pOverFlow = soundDevice->primary->WP + bytesToCopy - soundDevice->primary->bufferSize;
		if (pOverFlow > 0) // buffer over
		{
			pBytesToCopy = bytesToCopy - pOverFlow;
		}
		else
		{
			pBytesToCopy = bytesToCopy;
		}

		int sBytesToCopy;
		int sOverFlow = soundDevice->sources->RP + bytesToCopy - soundDevice->sources->bufferSize;
		if (sOverFlow > 0) // buffer over
		{
			sBytesToCopy = bytesToCopy - sOverFlow;
		}
		else
		{
			sBytesToCopy = bytesToCopy;
		}

		int writeLen = min(pBytesToCopy, sBytesToCopy);

		char *dst = &soundDevice->primary->buffer[soundDevice->primary->WP];
		soundDevice->primary->WP += writeLen;
		if (soundDevice->primary->WP >= soundDevice->primary->bufferSize)
		{
			soundDevice->primary->WP = 0;
		}

		char *src = &soundDevice->sources->buffer[soundDevice->sources->RP];
		soundDevice->sources->RP += writeLen;
		if (soundDevice->sources->RP >= soundDevice->sources->bufferSize)
		{
			soundDevice->sources->RP = 0;
		}

		memcpy(dst, src, writeLen);

		bytesToCopy -= writeLen;
	}

	return -1;
}

void DirectSoundSoundBuffer_GetCurrentPosition(SoundBuffer* soundBuffer, unsigned long* R)
{
	if (soundBuffer->primary)
	{
		*R = soundBuffer->WP;
	}
	else
	{
		*R = soundBuffer->RP;
	}
}

int DirectSoundSoundBuffer_Play(SoundBuffer* soundBuffer, int a, int b, int loop)
{
	return -1;
}

int DirectSoundSoundBuffer_Stop(SoundBuffer* soundBuffer)
{
	return -1;
}

int DirectSoundSoundBuffer_Lock(SoundBuffer* soundBuffer,
	unsigned long offset, unsigned long length,
	void** audioPtr, int* audioBytes)
{
	if (soundBuffer->lockedBuffer)
		return 0;

	if (length == 0 || (offset + length) > soundBuffer->bufferSize)
		return 0;

#ifdef COPY_LOCK
	soundBuffer->lockedBuffer = malloc(length);
	memcpy(soundBuffer->lockedBuffer, &soundBuffer->buffer[offset], length);
#else
	soundBuffer->lockedBuffer = &soundBuffer->buffer[offset];
#endif
	if (!soundBuffer->lockedBuffer)
		return 0;

	soundBuffer->lockedOffset = offset;
	soundBuffer->lockedLength = length;

	*audioPtr = soundBuffer->lockedBuffer;
	*audioBytes = length;

	return -1;
}

int DirectSoundSoundBuffer_Unlock(SoundBuffer* soundBuffer, void* audioPtr, int audioBytes)
{
	if (!soundBuffer->lockedBuffer)
		return 0;

	if (audioBytes > soundBuffer->lockedLength)
		return 0;

#ifdef COPY_LOCK
	memcpy(&soundBuffer->buffer[soundBuffer->lockedOffset], audioPtr, audioBytes);
	free(soundBuffer->lockedBuffer);
#else
#endif

	soundBuffer->lockedBuffer = 0;
	soundBuffer->lockedOffset = 0;
	soundBuffer->lockedLength = 0;

	return -1;
}

#endif