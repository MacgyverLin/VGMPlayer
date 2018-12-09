#include <stdio.h>
#include <stdlib.h>
#include "def.h"

int CPU_Mode = 0;
int VDP_Current_Line = 0;
int VDP_Num_Lines = 0;
int VDP_Status = 0;

#include "SoundDevice.h"
SoundDevice* soundDevice;

int Seg_L[882], Seg_R[882];
int Seg_Lenght, SBuffer_Lenght;
int Sound_Rate = 44100, Sound_Segs = 8;
int Bytes_Per_Unit;
int Sound_Enable;
int Sound_Stereo = 1;
int Sound_Is_Playing = 0;
int Sound_Initialised = 0;
int WAV_Dumping = 0;
int GYM_Dumping = 0;
FILE *GYM_File = 0;
int GYM_Playing = 0;
int WP = 0, RP = 0;

unsigned int Sound_Interpol[882];
unsigned int Sound_Extrapol[312][2];

char Dump_Dir[1024] = "";
char Dump_GYM_Dir[1024] = "";

void Put_Info(const char *msg, int value)
{
	printf("%d: %s", value, msg);
}

void End_Sound()
{
	if (Sound_Initialised)
	{
		Clear_Sound_Buffer();

		SoundDevice_StopSound(soundDevice);

		SoundDevice_Release(soundDevice);

		Sound_Initialised = 0;
	}
}

void Update_Sound(float time)
{
	SoundDevice_UpdataQueueBuffer(soundDevice);
}

///////////////////////////////////////////////////////////
int Init_Sound()
{
	int rval;
	int i;
	if (Sound_Initialised)
		return 0;

	End_Sound();

	switch (Sound_Rate)
	{
	case 11025:
		if (CPU_Mode)
			Seg_Lenght = 220;
		else
			Seg_Lenght = 184;
		break;

	case 22050:
		if (CPU_Mode)
			Seg_Lenght = 441;
		else
			Seg_Lenght = 368;
		break;

	case 44100:
		if (CPU_Mode)
			Seg_Lenght = 882;
		else
			Seg_Lenght = 735;
		break;
	}

	if (CPU_Mode)
	{
		for (i = 0; i < 312; i++)
		{
			Sound_Extrapol[i][0] = ((Seg_Lenght * i) / 312);
			Sound_Extrapol[i][1] = (((Seg_Lenght * (i + 1)) / 312) - Sound_Extrapol[i][0]);
		}

		for (i = 0; i < Seg_Lenght; i++)
			Sound_Interpol[i] = ((312 * i) / Seg_Lenght);
	}
	else
	{
		for (i = 0; i < 262; i++)
		{
			Sound_Extrapol[i][0] = ((Seg_Lenght * i) / 262);
			Sound_Extrapol[i][1] = (((Seg_Lenght * (i + 1)) / 262) - Sound_Extrapol[i][0]);
		}

		for (i = 0; i < Seg_Lenght; i++)
			Sound_Interpol[i] = ((262 * i) / Seg_Lenght);
	}

	memset(Seg_L, 0, Seg_Lenght << 2);
	memset(Seg_R, 0, Seg_Lenght << 2);

	WP = 0;
	RP = 0;
	
	int nBitsPerSample = 16;
	int nChannels = 0;
	if (Sound_Stereo)
		nChannels = 2;
	else
		nChannels = 1;
	Bytes_Per_Unit = nBitsPerSample / 8 * nChannels;
	SBuffer_Lenght = Seg_Lenght * Sound_Segs * Bytes_Per_Unit;

	SoundDevice_Create(&soundDevice, 
		nChannels, nBitsPerSample, Sound_Rate, Seg_Lenght * Bytes_Per_Unit, Sound_Segs);

	Sound_Initialised = -1;

	return Sound_Initialised;
}


void Write_Sound_Stereo(short *Dest, int lenght)
{
	int i, out_L, out_R;
	short *dest = Dest;

	for (i = 0; i < Seg_Lenght; i++)
	{
		out_L = Seg_L[i];
		Seg_L[i] = 0;

		if (out_L < -0x7FFF)
			*dest++ = -0x7FFF;
		else if (out_L > 0x7FFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = (short)out_L;

		out_R = Seg_R[i];
		Seg_R[i] = 0;

		if (out_R < -0x7FFF)
			*dest++ = -0x7FFF;
		else if (out_R > 0x7FFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = (short)out_R;
	}
}

void Dump_Sound_Stereo(short *Dest, int lenght)
{
	int i, out_L, out_R;
	short *dest = Dest;

	for (i = 0; i < Seg_Lenght; i++)
	{
		out_L = Seg_L[i];

		if (out_L < -0x7FFF)
			*dest++ = -0x7FFF;
		else if (out_L > 0x7FFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = (short)out_L;

		out_R = Seg_R[i];

		if (out_R < -0x7FFF)
			*dest++ = -0x7FFF;
		else if (out_R > 0x7FFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = (short)out_R;
	}
}

void Write_Sound_Mono(short *Dest, int lenght)
{
	int i, out;
	short *dest = Dest;

	for (i = 0; i < Seg_Lenght; i++)
	{
		out = Seg_L[i] + Seg_R[i];
		Seg_L[i] = Seg_R[i] = 0;

		if (out < -0xFFFF)
			*dest++ = -0x7FFF;
		else if (out > 0xFFFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = (short)(out >> 1);
	}
}

void Dump_Sound_Mono(short *Dest, int lenght)
{
	int i, out;
	short *dest = Dest;

	for (i = 0; i < Seg_Lenght; i++)
	{
		out = Seg_L[i] + Seg_R[i];

		if (out < -0xFFFF)
			*dest++ = -0x7FFF;
		else if (out > 0xFFFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = (short)(out >> 1);
	}
}

int Write_Sound_Buffer(void *Dump_Buf)
{
	/*
	LPVOID lpvPtr1;
	DWORD dwBytes1;
	HRESULT rval;
	*/
	void* lpvPtr1;
	long dwBytes1;
	int rval;

	if (Dump_Buf)
	{
		if (Sound_Stereo)
			Dump_Sound_Stereo((short *)Dump_Buf, Seg_Lenght);
		else
			Dump_Sound_Mono((short *)Dump_Buf, Seg_Lenght);
	}
	else
	{
		char* data = malloc(Seg_Lenght * Bytes_Per_Unit);
		//assert(data);
		if (Sound_Stereo)
		{
			Write_Sound_Stereo((short *)data, Seg_Lenght);
		}
		else
		{
			Write_Sound_Mono((short *)data, Seg_Lenght);
		}

		SoundDevice_AddAudioToQueue(soundDevice, WP, data, Seg_Lenght * Bytes_Per_Unit);

		free(data);
	}

	return 1;
}

int Clear_Sound_Buffer(void)
{
	/*
	LPVOID lpvPtr1;
	DWORD dwBytes1;
	HRESULT rval;
	*/
	void* lpvPtr1;
	long dwBytes1;
	int rval;
	int i;

	if (!Sound_Initialised)
		return 0;

	return 0;
}

int Play_Sound(void)
{
	int rval;

	if (Sound_Is_Playing)
		return 1;

	if(!SoundDevice_PlaySound(soundDevice))
		return 0;

	Sound_Is_Playing = -1;

	return Sound_Is_Playing;
}

int Stop_Sound(void)
{
	if (!SoundDevice_StopSound(soundDevice))
		return 0;

	Sound_Is_Playing = 0;

	return -1;
}

int Start_WAV_Dump(const char *filename)
{
	char Name[1024] = "";

	if (!(Sound_Is_Playing)/* || !(Game)*/)
		return 0;

	if (WAV_Dumping)
	{
		Put_Info("WAV sound is already dumping", 1000);
		return(0);
	}

	strcpy(Name, Dump_Dir);
	strcat(Name, filename);

#if 0
	if (waveCreateWave(filename))
	{
		Put_Info("Error in WAV dumping", 1000);
		return(0);
	}

	if (waveStartDataWrite())
	{
		Put_Info("Error in WAV dumping", 1000);
		return(0);
	}
#endif

	Put_Info("Starting to dump WAV sound", 1000);
	WAV_Dumping = 1;


	return 1;
}

int Update_WAV_Dump(void)
{
	unsigned char Buf_Tmp[882 * 4 + 16];
	unsigned int lenght, Writted;

	if (!WAV_Dumping)
		return 0;

	Write_Sound_Buffer(Buf_Tmp);

	lenght = Seg_Lenght << 1;

	if (Sound_Stereo)
		lenght *= 2;
#if 0
	if (waveWriteFile(lenght, &Buf_Tmp[0]))
	{
		Put_Info("Error in WAV dumping", 1000);
		return 0;
	}
#endif

	return 1;
}


int Stop_WAV_Dump(void)
{

	if (!WAV_Dumping)
	{
		Put_Info("Already stopped", 1000);
		return 0;
	}

#if 0
	if (waveCloseWriteFile())
		return 0;
#endif
	Put_Info("WAV dump stopped", 1000);
	WAV_Dumping = 0;

	return 1;
}

int Start_GYM_Dump(const char *filename)
{
	char Name[1024], Name2[1024];
	char ext[12] = "_000.gym";
	unsigned char YM_Save[0x200], t_buf[4];
	int num = -1, i, j;
	unsigned long bwr;

	//SetCurrentDirectory(Gens_Path);
	//if (!Game) return 0;

	if (GYM_Dumping)
	{
		Put_Info("GYM sound is already dumping", 1000);
		return 0;
	}

	strcpy(Name, Dump_GYM_Dir);
	strcat(Name, filename);

	/*
	do
	{
		if (num++ > 99999)
		{
			Put_Info("Too much GYM files in your GYM directory", 1000);
			GYM_File = NULL;
			return(0);
		}

		ext[0] = '_';
		i = 1;

		j = num / 10000;
		if (j) ext[i++] = '0' + j;
		j = (num / 1000) % 10;
		if (j) ext[i++] = '0' + j;
		j = (num / 100) % 10;
		ext[i++] = '0' + j;
		j = (num / 10) % 10;
		ext[i++] = '0' + j;
		j = num % 10;
		ext[i++] = '0' + j;
		ext[i++] = '.';
		ext[i++] = 'g';
		ext[i++] = 'y';
		ext[i++] = 'm';
		ext[i] = 0;

		if ((strlen(Name) + strlen(ext)) > 1023) return(0);

		strcpy(Name2, Name);
		strcat(Name2, ext);
	} while ((GYM_File = CreateFile(Name2, GENERIC_WRITE, NULL,
		NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE);
	*/
	GYM_File = fopen(filename, "wb");
	if (!GYM_File)
		return 0;

	YM2612_Save(YM_Save);

	for (i = 0x30; i < 0x90; i++)
	{
		t_buf[0] = 1;
		t_buf[1] = i;
		t_buf[2] = YM_Save[i];
		bwr = fwrite(t_buf, 3, 1, GYM_File);
		// WriteFile(GYM_File, t_buf, 3, &bwr, NULL);

		t_buf[0] = 2;
		t_buf[1] = i;
		t_buf[2] = YM_Save[i + 0x100];
		bwr = fwrite(t_buf, 3, 1, GYM_File);
		// WriteFile(GYM_File, t_buf, 3, &bwr, NULL);
	}

	for (i = 0xA0; i < 0xB8; i++)
	{
		t_buf[0] = 1;
		t_buf[1] = i;
		t_buf[2] = YM_Save[i];
		bwr = fwrite(t_buf, 3, 1, GYM_File);
		// WriteFile(GYM_File, t_buf, 3, &bwr, NULL);

		t_buf[0] = 2;
		t_buf[1] = i;
		t_buf[2] = YM_Save[i + 0x100];
		bwr = fwrite(t_buf, 3, 1, GYM_File);
		// WriteFile(GYM_File, t_buf, 3, &bwr, NULL);
	}

	t_buf[0] = 1;
	t_buf[1] = 0x22;
	t_buf[2] = YM_Save[0x22];
	bwr = fwrite(t_buf, 3, 1, GYM_File);
	// WriteFile(GYM_File, t_buf, 3, &bwr, NULL);

	t_buf[0] = 1;
	t_buf[1] = 0x27;
	t_buf[2] = YM_Save[0x27];
	bwr = fwrite(t_buf, 3, 1, GYM_File);
	// WriteFile(GYM_File, t_buf, 3, &bwr, NULL);

	t_buf[0] = 1;
	t_buf[1] = 0x28;
	t_buf[2] = YM_Save[0x28];
	// WriteFile(GYM_File, t_buf, 3, &bwr, NULL);
	bwr = fwrite(t_buf, 3, 1, GYM_File);

	Put_Info("Starting to dump GYM sound", 1000);
	GYM_Dumping = 1;

	return 1;
}

int Stop_GYM_Dump(void)
{
	if (!GYM_Dumping)
	{
		Put_Info("Already stopped", 1000);
		return 0;
	}

	if (GYM_File)
	{
		fclose(GYM_File);
	}

	Clear_Sound_Buffer();

	Put_Info("GYM dump stopped", 1000);
	GYM_Dumping = 0;

	return 1;
}

int Start_Play_GYM(const char* filename)
{
	//char Name[1024];
	//OPENFILENAME ofn;

	if (/*Game ||*/ !(Sound_Enable))
		return(0);

	if (GYM_Playing)
	{
		Put_Info("Already playing GYM", 1000);
		return 0;
	}

	End_Sound();
	CPU_Mode = 0;

	if (!Init_Sound())
	{
		Sound_Enable = 0;
		Put_Info("Can't initialise DirectSound", 1000);
		return 0;
	}

	Play_Sound();

	/*
	memset(Name, 0, 1024);
	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = HWnd;
	ofn.hInstance = ghInstance;
	ofn.lpstrFile = Name;
	ofn.nMaxFile = 1023;
	ofn.lpstrFilter = "GYM files\0*.gym\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = Dump_GYM_Dir;
	ofn.lpstrDefExt = "gym";
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		GYM_File = CreateFile(Name, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (GYM_File == INVALID_HANDLE_VALUE) return 0;
	}
	else
		return 0;
	*/
	GYM_File = fopen(filename, "rb");
	if (!GYM_File)
		return 0;

	YM2612_Init(CLOCK_NTSC / 7, Sound_Rate, -1);
	PSG_Init(CLOCK_NTSC / 15, Sound_Rate);
	GYM_Playing = 1;

	Put_Info("Starting to play GYM", 1000);

	return 1;
}

int Stop_Play_GYM(void)
{
	if (!GYM_Playing)
	{
		Put_Info("Already stopped", 1000);
		return 0;
	}

	if (GYM_File)
	{
		fclose(GYM_File);
		GYM_File = 0;
	}
	Clear_Sound_Buffer();
	GYM_Playing = 0;

	Put_Info("Stop playing GYM", 1000);

	return 1;
}

int GYM_Next(void)
{
	unsigned char c, c2;
	unsigned long l;
	int *buf[2];

	buf[0] = Seg_L;
	buf[1] = Seg_R;

	if (!(GYM_Playing) || !(GYM_File))
		return 0;

	do
	{
		l = fread(&c, 1, 1, GYM_File);
		if (l == 0)
			return 0;

		switch (c)
		{
		case 0:
			PSG_Update(buf, Seg_Lenght);
			YM2612_Update(buf, Seg_Lenght);
			break;

		case 1:
			l = fread(&c2, 1, 1, GYM_File);
			//ReadFile(GYM_File, &c2, 1, &l, NULL);
			YM2612_Write(0, c2);

			l = fread(&c2, 1, 1, GYM_File);
			//ReadFile(GYM_File, &c2, 1, &l, NULL);
			YM2612_Write(1, c2);
			break;

		case 2:
			l = fread(&c2, 1, 1, GYM_File);
			//ReadFile(GYM_File, &c2, 1, &l, NULL);
			YM2612_Write(2, c2);

			l = fread(&c2, 1, 1, GYM_File);
			//ReadFile(GYM_File, &c2, 1, &l, NULL);
			YM2612_Write(3, c2);
			break;

		case 3:
			l = fread(&c2, 1, 1, GYM_File);
			//ReadFile(GYM_File, &c2, 1, &l, NULL);
			PSG_Write(c2);
			break;
		}

	} while (c);

	return 1;
}

int Play_GYM()
{
	while (SoundDevice_GetQueuedAudioCount(soundDevice) < soundDevice->bufferSegmentCount / 2)
	{
		if (!GYM_Next())
		{
			Stop_Play_GYM();
			return 0;
		}

		Write_Sound_Buffer(NULL);
		WP = (WP + 1) & (Sound_Segs - 1);
	};

	if (GetDeviceState(soundDevice) != 3)
	{
		SoundDevice_PlaySound(soundDevice);

		SoundDevice_SetVolume(soundDevice, 1.0);
		SoundDevice_SetPlayRate(soundDevice, 1.0);
	}

	if (!SoundDevice_UpdataQueueBuffer(soundDevice))
		return 0;

	return -1;
}

int Play_GYM_Bench(void)
{
	if (!GYM_Next())
	{
		Stop_Play_GYM();
		return 0;
	}

	Write_Sound_Buffer(NULL);
	WP = (WP + 1) & (Sound_Segs - 1);

	return 1;
}

int Update_GYM_Dump(int v0, int v1, unsigned char v2)
{
	int bResult;
	char buf_tmp[4];
	unsigned long l, l2;

	if (!GYM_Dumping)
		return 0;
	if (!GYM_File)
		return 0;

	v1 &= 0xFF;

	buf_tmp[0] = v0;
	l = 1;

	switch (v0)
	{
	case 1:
	case 2:
		buf_tmp[1] = (unsigned char)v1;
		buf_tmp[2] = v2;
		l = 3;
		break;

	case 3:
		buf_tmp[1] = (unsigned char)v1;
		l = 2;
		break;
	}

	fwrite(buf_tmp, 1, l, buf_tmp, GYM_File);

	return 1;
}