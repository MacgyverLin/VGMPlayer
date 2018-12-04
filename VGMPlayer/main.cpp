#include <stdio.h>
#include <iostream>
#include "VGMPlayerLib.h"
#include <windows.h>

int Update_Emulation(float dt)
{
	if (Sound_Enable)
	{
		WP = (WP + 1) & (Sound_Segs - 1);

		int test = Get_Current_Seg();
		if (WP == test)
			WP = (WP + Sound_Segs - 1) & (Sound_Segs - 1);

		Write_Sound_Buffer(NULL);

		Update_Sound(dt);
	}

	return -1;
}

DWORD last;
void InitTime()
{
	last = timeGetTime();
}

float GetTime()
{
	DWORD current = timeGetTime();
	float dt = (current - last) / 1000.0f;
	last = current;

	return dt;
}

int main(int argc, const char** argv)
{
	Sound_Enable = 1;
	Sound_Segs = 8;
	Sound_Stereo = 1;
	Sound_Initialised = 0;
	Sound_Is_Playing = 0;
	WAV_Dumping = 0;
	GYM_Dumping = 0;

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

	InitTime();

	const char *gymFile = argv[1];
	Start_Play_GYM(gymFile);

	const char *wavFile = argv[2];
	Start_WAV_Dump(wavFile);

	while (true)
	{
		Sleep(16);
		float dt = GetTime();

		if (!Play_GYM(dt))
			break;
		// Update_Emulation(dt);

		//if (WAV_Dumping) 
			//Update_WAV_Dump();
		//if (GYM_Dumping) 
			//Update_GYM_Dump((unsigned char)0, (unsigned char)0, (unsigned char)0);
	};

	Stop_WAV_Dump();
	
	End_Sound();

	return -1;
}