#include <stdio.h>
#include <iostream>
#include "VGMPlayerLib.h"
#include <windows.h>

#include "SoundDevice.h"
#include <vector>
using namespace std;

SoundDevice* soundDevice;

bool test(const char *filename)
{
	int channels = 2;
	int bitPerSamples = 16;
	int sampleRate = 22050;
	int bufferSegmentCount = 8;
	int frameRate = 30;
	int bufferSegmentSize = channels * (bitPerSamples / 8) * sampleRate / frameRate; // 1 / bufferSegmentCount second
	if(!SoundDevice_Create(&soundDevice, channels, bitPerSamples, sampleRate, bufferSegmentSize, bufferSegmentCount))
		return false;

	FILE *fp = fopen("2.raw", "rb");
	if (!fp)
		return false;

	int batchSize = bufferSegmentSize;
	vector<char> rawData;
	rawData.resize(batchSize);
	memset(&rawData[0], 0, batchSize);

	int readSize = 0;
	int WP = 0;
	do
	{
		while(SoundDevice_GetQueuedAudioCount(soundDevice)<8)
		{
			readSize = fread(&rawData[0], 1, batchSize, fp);
			if(readSize==0)
				break;

			if(!SoundDevice_AddAudioToQueue(soundDevice, WP, &rawData[0], readSize))
				break;

			WP = (WP + 1) & (bufferSegmentCount - 1);
		};

		if (GetDeviceState(soundDevice) != 3)
		{
			SoundDevice_PlaySound(soundDevice);

			SoundDevice_SetVolume(soundDevice, 1.0);
			SoundDevice_SetPlayRate(soundDevice, 1.0);
		}

		if(!SoundDevice_UpdataQueueBuffer(soundDevice))
			break;
	}while(readSize!=0);

	SoundDevice_StopSound(soundDevice);

	SoundDevice_Release(soundDevice);

	fclose(fp);

	return true;
}

int main(int argc, const char** argv)
{
	Sound_Enable = 1;
	Sound_Stereo = 1;
	Sound_Initialised = 0;
	Sound_Is_Playing = 0;
	Sound_Rate = 11025;
	//Sound_Rate = 22050;
	//Sound_Rate = 44100;
	Sound_Segs = 8;
	WAV_Dumping = 0;
	GYM_Dumping = 0;

	 //test(argv[2]);
	 //return -1;

	if (CPU_Mode)
	{
		// CPL_Z80 = Round_Double((((double)CLOCK_PAL / 15.0) / 50.0) / 312.0);
		// CPL_M68K = Round_Double((((double)CLOCK_PAL / 7.0) / 50.0) / 312.0);
		// CPL_MSH2 = Round_Double(((((((double)CLOCK_PAL / 7.0) * 3.0) / 50.0) / 312.0) * (double)MSH2_Speed) / 100.0);
		// CPL_SSH2 = Round_Double(((((((double)CLOCK_PAL / 7.0) * 3.0) / 50.0) / 312.0) * (double)SSH2_Speed) / 100.0);

		VDP_Num_Lines = 312;
		VDP_Status |= 0x0001;

		YM2612_Init(CLOCK_PAL / 7, Sound_Rate, YM2612_Improv);
		PSG_Init(CLOCK_PAL / 15, Sound_Rate);
	}
	else
	{
		// CPL_Z80 = Round_Double((((double)CLOCK_NTSC / 15.0) / 60.0) / 262.0);
		// CPL_M68K = Round_Double((((double)CLOCK_NTSC / 7.0) / 60.0) / 262.0);
		// CPL_MSH2 = Round_Double(((((((double)CLOCK_NTSC / 7.0) * 3.0) / 60.0) / 262.0) * (double)MSH2_Speed) / 100.0);
		// CPL_SSH2 = Round_Double(((((((double)CLOCK_NTSC / 7.0) * 3.0) / 60.0) / 262.0) * (double)SSH2_Speed) / 100.0);

		VDP_Num_Lines = 262;
		VDP_Status &= 0xFFFE;

		YM2612_Init(CLOCK_NTSC / 7, Sound_Rate, YM2612_Improv);
		PSG_Init(CLOCK_NTSC / 15, Sound_Rate);
	}

	// if (Auto_Fix_CS) 
		//Fix_Checksum();
	/*
	if (Sound_Enable)
	{
		End_Sound();

		if (!Init_Sound())
			Sound_Enable = 0;
		else
			Play_Sound();
	}
	*/

	const char *gymFile = argv[1];
	Start_Play_GYM(gymFile);

	//const char *wavFile = argv[2];
	//Start_WAV_Dump(wavFile);

	while (true)
	{
		if (!Play_GYM())
			break;
	};

	//Stop_WAV_Dump();

	End_Sound();

	return -1;
}