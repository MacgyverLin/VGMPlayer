#ifndef _SoundDevice_h_
#define _SoundDevice_h_

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
		ALint			processed;
		ALint			queued;
		int				processedBuffer;
		int				RP;

		ALuint			sndBuffer[32];
		int				channels;
		int				bitsPerSample;
		int				sampleRate;
		int				bufferSize;
		int				bufferCount;

		float			volume;
		float			playRate;
	}SoundDevice;

	extern int SoundDevice_Create(SoundDevice** soundDevice, int channels, int bitsPerSample, int sampleRate, int bufferSize, int bufferCount);
	extern void SoundDevice_Release(SoundDevice* soundDevice);
	extern int SoundDevice_PlaySound(SoundDevice* soundDevice);
	extern int SoundDevice_StopSound(SoundDevice* soundDevice);
	extern int GetDeviceState(SoundDevice* soundDevice);
	extern int SoundDevice_UpdataQueueBuffer(SoundDevice* soundDevice);
	extern int SoundDevice_AddAudioToQueue(SoundDevice* soundDevice, int WP, char* data_, int dataSize_);
	extern int SoundDevice_GetQueuedAudioCount(SoundDevice* soundDevice);
	extern void SoundDevice_SetVolume(SoundDevice* soundDevice, float volume_);
	extern float SoundDevice_GetVolume(SoundDevice* soundDevice);
	extern void SoundDevice_SetPlayRate(SoundDevice* soundDevice, float playRate_);
	extern float SoundDevice_GetPlayRate(SoundDevice* soundDevice);
#ifdef __cplusplus 
};
#endif

#endif