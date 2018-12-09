#ifndef _SoundDevice_h_
#define _SoundDevice_h_

#include <stdio.h>
#include <stdlib.h>
#include "def.h"

#ifdef __cplusplus 
extern "C" {
#endif

#define SOUND_DEVICE_USE_OPENAL

#include <AL/al.h>
#include <AL/alc.h>

#ifdef SOUND_DEVICE_USE_OPENAL
	typedef struct
	{
		char			primary;
		int				bufferSize;
		int				sampleRate;
		int				bitsPerSample;
		int				bytesPerUnit;
		int				bytesPerSec;
		unsigned char	*buffer;

		unsigned int	wp;
		unsigned int	rp;

		unsigned char	*lockedBuffer;
		unsigned long	lockedOffset;
		unsigned long	lockedLength;
	}SoundBuffer;

	typedef struct
	{
		SoundBuffer *primary;
		SoundBuffer *sources;

		ALCcontext*		context;
		ALCdevice*		device;
		ALuint			outSource;
		ALint			processed;
		ALint			queued;
		int				processedBuffer;

		int				channels;
		int				bitsPerSample;
		int				sampleRate;
		int				bufferSegmentSize;
		int				bufferSegmentCount;

		float			volume;
		float			playRate;
	}SoundDevice;

	extern int createOpenAL(SoundDevice** soundDevice, int channels, int bitsPerSample, int sampleRate, int bufferSegmentSize, int bufferSegmentCount);
	extern void releaseOpenAL(SoundDevice* soundDevice);
	extern int playSound(SoundDevice* soundDevice);
	extern int stopSound(SoundDevice* soundDevice);
	extern int updataQueueBuffer(SoundDevice* soundDevice);
	extern int addAudioToQueue(SoundDevice* soundDevice, char* data_, int dataSize_);
	extern void setVolume(SoundDevice* soundDevice, float volume_);
	extern float getVolume(SoundDevice* soundDevice);
	extern void setPlayRate(SoundDevice* soundDevice, float playRate_);
	extern float getPlayRate(SoundDevice* soundDevice);
#else
	typedef struct
	{
		char primary;
		unsigned int channels;
		unsigned int sampleRate;
		unsigned int bitsPerSample;
		unsigned int bytesPerUnit;
		unsigned int bytesPerSec;
		unsigned int bufferSize;
		unsigned char *buffer;
		unsigned int WP;
		unsigned int RP;

		unsigned char *lockedBuffer;
		unsigned long lockedOffset;
		unsigned long lockedLength;
	}SoundBuffer;

	typedef struct
	{
		SoundBuffer *primary;
		SoundBuffer *sources;
	}SoundDevice;
#endif

#ifdef __cplusplus 
};
#endif

int DirectSound_Create(void *ptr1, SoundDevice** soundDevice, void *ptr2);
int DirectSound_SetCooperativeLevel(SoundDevice* soundDevice);
void DirectSound_Release(SoundDevice* soundDevice);
int DirectSoundSoundBuffer_Create(SoundDevice* soundDevice, int primary, unsigned int channels, unsigned int sampleRate,
	unsigned int bitsPerSample, unsigned int bytesPerUnit, unsigned int bufferSize, SoundBuffer** soundBuffer);
void DirectSoundSoundBuffer_Release(SoundBuffer* soundBuffer);
int DirectSound_Update(SoundDevice* soundDevice, float dt);
void DirectSoundSoundBuffer_GetCurrentPosition(SoundBuffer* soundBuffer, unsigned long* R);
int DirectSoundSoundBuffer_Play(SoundBuffer* soundBuffer, int a, int b, int loop);
int DirectSoundSoundBuffer_Stop(SoundBuffer* soundBuffer);
int DirectSoundSoundBuffer_Lock(SoundBuffer* soundBuffer, unsigned long offset, unsigned long length, void** audioPtr, int* audioBytes);
int DirectSoundSoundBuffer_Unlock(SoundBuffer* soundBuffer, void* audioPtr, int audioBytes);

#endif