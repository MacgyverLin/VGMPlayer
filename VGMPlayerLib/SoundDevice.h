#ifndef _SoundDevice_h_
#define _SoundDevice_h_

#include "vgmdef.h"
#include <stdio.h>
#include <stdlib.h>
#include <AL/al.h>
#include <AL/alc.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct
	{
		ALCcontext*		context;
		ALCdevice*		device;
		ALuint			outSource;
		ALint			processedBuffer;
		ALint			queuedBuffer;

		ALuint			sndBuffer[VGM_OUTPUT_BUFFER_COUNT];
		int				channels;
		int				bitsPerSample;
		int				sampleRate;
		int				bufferCount;

		float			volume;
		float			playRate;

		int				WP;
	}SoundDevice;

	extern int SoundDevice_Create(SoundDevice** soundDevice, int channels, int bitsPerSample, int sampleRate, int bufferCount);
	extern void SoundDevice_Release(SoundDevice* soundDevice);
	extern int SoundDevice_PlaySound(SoundDevice* soundDevice);
	extern int SoundDevice_StopSound(SoundDevice* soundDevice);
	extern int SoundDevice_UpdataQueueBuffer(SoundDevice* soundDevice);
	extern int SoundDevice_AddAudioToQueue(SoundDevice* soundDevice, void* data_, int dataSize_);
	extern int SoundDevice_GetQueuedAudioCount(SoundDevice* soundDevice);
	extern void SoundDevice_SetVolume(SoundDevice* soundDevice, float volume_);
	extern float SoundDevice_GetVolume(SoundDevice* soundDevice);
	extern void SoundDevice_SetPlayRate(SoundDevice* soundDevice, float playRate_);
	extern float SoundDevice_GetPlayRate(SoundDevice* soundDevice);

	extern int SoundDevice_GetDeviceState(SoundDevice* soundDevice);
	extern int SoundDevice_GetQueued(SoundDevice* soundDevice);
#ifdef __cplusplus 
};
#endif

#endif