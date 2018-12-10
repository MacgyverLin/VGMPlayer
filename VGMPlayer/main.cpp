#include <stdio.h>
#include <iostream>
#include "VGMPlayerLib.h"
#include <windows.h>

#include "SoundDevice.h"
#include <vector>
using namespace std;

#if 0
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
#endif

int main(int argc, const char** argv)
{
	VGMData *vgmData = VGMData_Create("1.vgm");

	VGMPlayer *vgmPlayer = VGMPlayer_Create(vgmData, 44100, -1);

	VGMPlayer_Update(vgmPlayer);

	VGMPlayer_Release(vgmPlayer);
	VGMData_Release(vgmData);

	return -1;

	Sound_Enable = 1;
	Sound_Stereo = 1;
	Sound_Initialised = 0;
	Sound_Is_Playing = 0;
	Sound_Rate = 11025;
	Sound_Rate = 22050;
	//Sound_Rate = 44100;
	Sound_Segs = 8;
	WAV_Dumping = 0;
	GYM_Dumping = 0;

	const char *gymFile = argv[1];
	Start_Play_GYM(gymFile);

	while (true)
	{
		if (!Play_GYM())
			break;
	};

	End_Sound();

	return -1;
}