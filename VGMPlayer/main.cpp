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
	if (!SoundDevice_Create(&soundDevice, channels, bitPerSamples, sampleRate, bufferSegmentSize, bufferSegmentCount))
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
		while (SoundDevice_GetQueuedAudioCount(soundDevice) < 8)
		{
			readSize = fread(&rawData[0], 1, batchSize, fp);
			if (readSize == 0)
				break;

			if (!SoundDevice_AddAudioToQueue(soundDevice, WP, &rawData[0], readSize))
				break;

			WP = (WP + 1) & (bufferSegmentCount - 1);
		};

		if (GetDeviceState(soundDevice) != 3)
		{
			SoundDevice_PlaySound(soundDevice);

			SoundDevice_SetVolume(soundDevice, 1.0);
			SoundDevice_SetPlayRate(soundDevice, 1.0);
		}

		if (!SoundDevice_UpdataQueueBuffer(soundDevice))
			break;
	} while (readSize != 0);

	SoundDevice_StopSound(soundDevice);

	SoundDevice_Release(soundDevice);

	fclose(fp);

	return true;
}
#endif

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
# if 1
	int musicIdx = 13;
	const char* file1s[] =
	{
		"ThunderCross/01 The Thunder Fighters (Title).vgm",
		"ThunderCross/03 First Attack (1st.vgm",
		"ThunderCross/04 Skywalker (2st.vgm",
		"ThunderCross/05 Machine Graveyard (3st.vgm",
		"ThunderCross/06 Great Battleship (4st.vgm",
		"ThunderCross/07 Endless Labyrinth (5st.vgm",
		"ThunderCross/08 Fire Cavern (6st.vgm",
		"ThunderCross/09 Final Base (7st.vgm",
		"ThunderCross/10 Gloidential Mechanism (Boss BGM).vgm",
		"ThunderCross/11 Stage Clear.vgm",
		"ThunderCross/12 Ranking.vgm",
		"ThunderCross/13 Game Over.vgm",
		"ThunderCross/14 A Long Way (Ending).vgm",
		"ThunderCross/15 Start Demo - First Attack.vgm",
		"ThunderCross/16 Appear Demo - Great Battleship.vgm",
		"ThunderCross/17 Stage Clear (with Effect).vgm",
	};

	const char* file3s[] =
	{
		"ThunderForce3/01 - The Wind Blew All Day Long (Opening Theme).vgm",
		"ThunderForce3/02 - Beyond the Peace (Stage Select).vgm",
		"ThunderForce3/03 - Back to the Fire (Stage 1 - Hydra).vgm",
		"ThunderForce3/04 - Gargoyle (Stage 1 Boss).vgm",
		"ThunderForce3/05 - Venus Fire (Stage 2 - Gorgon).vgm",
		"ThunderForce3/06 - Twin Vulcan (Stage 2 Boss).vgm",
		"ThunderForce3/07 - The Grubby Dark Blue (Stage 3 - Seiren).vgm",
		"ThunderForce3/08 - King Fish (Stage 3 Boss).vgm",
		"ThunderForce3/09 - Truth (Stage 4 - Haides).vgm",
		"ThunderForce3/10 - G Lobster (Stage 4 Boss).vgm",
		"ThunderForce3/11 - Final Take a Chance (Stage 5 - Ellis).vgm",
		"ThunderForce3/12 - Mobile Fort (Stage 5 Boss).vgm",
		"ThunderForce3/13 - His Behavior Inspired Us With Distrust (Stage 6 - Cerberus).vgm",
		"ThunderForce3/14 - Hunger Made Them Desperate (Stage 7 - Orn Base).vgm",
		"ThunderForce3/15 - Off Luck (Stage 7 Boss).vgm",
		"ThunderForce3/16 - Final Moment (Stage 8 - Orn Core).vgm",
		"ThunderForce3/17 - Be Menaced by Orn (Stage 8 Boss).vgm",
		"ThunderForce3/18 - Stage Clear.vgm",
		"ThunderForce3/19 - A War Without the End (Ending).vgm",
		"ThunderForce3/20 - Present (Staff Roll).vgm",
		"ThunderForce3/21 - Continue.vgm",
		"ThunderForce3/22 - Game Over.vgm",
	};

	const char* file2s[] =
	{
		"ThunderCross2/01 Credit.vgm",
		"ThunderCross2/02 Rise in Arms (Opening Demo).vgm",
		"ThunderCross2/03 Air Battle = Thunder Cross II (Stage 1 BGM).vgm",
		"ThunderCross2/04 Approach the Boss (Boss Approach BGM).vgm",
		"ThunderCross2/05 He is not dead = Evil Eye (Stage 1, 5 Boss BGM).vgm",
		"ThunderCross2/06 Clear Demo (Stage Clear).vgm",
		"ThunderCross2/07 Kartus-Part 2 (Stage 2 BGM).vgm",
		"ThunderCross2/08 Theme of Ragamuffin (Stage 2 Boss BGM).vgm",
		"ThunderCross2/09 Heavy Metal Bomber (Stage 3 BGM).vgm",
		"ThunderCross2/10 Dark Force (Stage 3, 6 Boss BGM).vgm",
		"ThunderCross2/11 Dog Fight III (Stage 4 BGM).vgm",
		//"ThunderCross2/12 Theme of Dinosaur Jr. (Stage 4 Boss BGM).vgm"
		"ThunderCross2/13 Battles of Battleship (Stage 5 BGM).vgm",
		"ThunderCross2/14 Star Light (Stage 6 BGM).vgm",
		"ThunderCross2/15 At The Front (Stage 7 BGM).vgm",
		"ThunderCross2/16 An Ominous Silence (Boss Approach BGM).vgm",
		"ThunderCross2/17 Theme of Sprouts Layber (Stage 7 Boss BGM).vgm",
		"ThunderCross2/18 A Shooting Star (Ending).vgm",
		"ThunderCross2/19 Game Over.vgm",
		"ThunderCross2/20 A Victory March (Ranking).vgm"
	};


	//while (musicIdx < sizeof(file1s) / sizeof(file1s[0]))
	while (musicIdx < sizeof(file2s) / sizeof(file2s[0]))
	//while (musicIdx < sizeof(file3s) / sizeof(file3s[0]))
	{
		//VGMData *vgmData = VGMData_Create(file1s[musicIdx]);
		VGMData *vgmData = VGMData_Create(file2s[musicIdx]);
		//VGMData *vgmData = VGMData_Create(file3s[musicIdx]);

		VGMPlayer *vgmPlayer = VGMPlayer_Create(vgmData, 44100, -1);

		while (true)
		{
			if (!VGMPlayer_Update(vgmPlayer))
				break;
		}

		VGMPlayer_Release(vgmPlayer);
		VGMData_Release(vgmData);

		musicIdx++;
	}

	return -1;

#else

	const char *gymFile = argv[1];
	Start_Play_GYM(gymFile);

	while (true)
	{
		if (!Play_GYM())
			break;
	};

	End_Sound();

	return -1;

#endif
}