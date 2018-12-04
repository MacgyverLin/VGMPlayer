#include <stdio.h>
#include <stdlib.h>
#include "def.h"

int CPU_Mode = 0;
int VDP_Current_Line = 0;
int VDP_Num_Lines = 0;
int VDP_Status = 0;


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


SoundDevice* lpDS;
SoundBuffer* lpDSPrimary;
SoundBuffer* lpDSBuffer;

int Seg_L[882], Seg_R[882];
int Seg_Lenght, SBuffer_Lenght;
int Sound_Rate = 22050, Sound_Segs = 8;
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

#define COPY_LOCK

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

		/*
		printf("-----------------------------------------------------\n");
		printf("Left: \n");
		for (int i = 0; i < writeLen; i += 4*20)
		{
			unsigned short *left = (unsigned short *)(&dst[i]);
			unsigned short *right = (unsigned short *)(&dst[i + 2]);
			printf("%u,", *left);
		}
		printf("\n");
		printf("Right: \n");
		for (int i = 0; i < writeLen; i += 4 * 20)
		{
			unsigned short *left = (unsigned short *)(&dst[i]);
			unsigned short *right = (unsigned short *)(&dst[i + 2]);

			printf("%u,", *right);
		}
		printf("\n");
		*/

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

void Put_Info(const char *msg, int value)
{
	printf("%d: %s", value, msg);
}

void End_Sound()
{
	if (Sound_Initialised)
	{
		Clear_Sound_Buffer();

		if (lpDSPrimary)
		{
			DirectSoundSoundBuffer_Release(lpDSPrimary);
			lpDSPrimary = 0;
		}

		if (lpDSBuffer)
		{
			DirectSoundSoundBuffer_Stop(lpDSBuffer);
			Sound_Is_Playing = 0;

			DirectSoundSoundBuffer_Release(lpDSBuffer);
			lpDSBuffer = 0;
		}

		if (lpDS)
		{
			DirectSound_Release(lpDS);
			lpDS = 0;
		}

		Sound_Initialised = 0;
	}
}

void Update_Sound(float time)
{
	DirectSound_Update(lpDS, time);
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

	rval = DirectSound_Create(0, &lpDS, 0);
	if (!rval)
		return 0;

	rval = DirectSound_SetCooperativeLevel(lpDS/* DSSCL_PRIORITY*/);
	if (!rval)
	{
		DirectSound_Release(lpDS);
		lpDS = 0;
		return 0;
	}

	/*
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	*/
	/*
	memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	if (Sound_Stereo)
		wfx.nChannels = 2;
	else
		wfx.nChannels = 1;
	wfx.nSamplesPerSec = Sound_Rate;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = Bytes_Per_Unit = (wfx.wBitsPerSample / 8) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * Bytes_Per_Unit;
	rval = lpDSPrimary->SetFormat(&wfx);
	*/
	int nBitsPerSample = 16;
	int nChannels = 0;
	if (Sound_Stereo)
		nChannels = 2;
	else
		nChannels = 1;
	Bytes_Per_Unit = nBitsPerSample / 8 * nChannels;
	SBuffer_Lenght = Seg_Lenght * Sound_Segs * Bytes_Per_Unit;

	rval = DirectSoundSoundBuffer_Create(lpDS, -1, nChannels, Sound_Rate, nBitsPerSample, Bytes_Per_Unit, SBuffer_Lenght, &lpDSPrimary/*, NULL*/);
	if (!rval)
	{
		DirectSound_Release(lpDS);
		lpDS = 0;
		return 0;
	}

	rval = DirectSoundSoundBuffer_Create(lpDS, 0, nChannels, Sound_Rate, nBitsPerSample, Bytes_Per_Unit, SBuffer_Lenght, &lpDSBuffer/*, NULL*/);
	if (!rval)
	{
		DirectSound_Release(lpDS);
		lpDS = 0;
		return 0;
	}

	Sound_Initialised = -1;

	return Sound_Initialised;
}

int Get_Current_Seg(void)
{
	unsigned long R;

	DirectSoundSoundBuffer_GetCurrentPosition(lpDSBuffer, &R);

	return R / (Seg_Lenght * Bytes_Per_Unit);
}


int Check_Sound_Timing(void)
{
	unsigned long R;

	DirectSoundSoundBuffer_GetCurrentPosition(lpDSBuffer, &R);

	RP = R / (Seg_Lenght * Bytes_Per_Unit);

	if (RP == ((WP + 1) & (Sound_Segs - 1)))
		return 2;

	if (RP != WP)
		return 1;

	return 0;
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
		DirectSoundSoundBuffer_Lock(lpDSBuffer, WP * Seg_Lenght * Bytes_Per_Unit, Seg_Lenght * Bytes_Per_Unit, &lpvPtr1, &dwBytes1, NULL, NULL, 0);
		if (Sound_Stereo)
		{
			Write_Sound_Stereo((short *)lpvPtr1, Seg_Lenght);
		}
		else
		{
			Write_Sound_Mono((short *)lpvPtr1, Seg_Lenght);
		}

		DirectSoundSoundBuffer_Unlock(lpDSBuffer, lpvPtr1, dwBytes1, NULL, NULL);
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

	rval = DirectSoundSoundBuffer_Lock(lpDSBuffer, 0, Seg_Lenght * Sound_Segs * Bytes_Per_Unit, &lpvPtr1, &dwBytes1, NULL, NULL, 0);
	if (rval)
	{
		signed short *w = (signed short *)lpvPtr1;

		for (i = 0; i < Seg_Lenght * Sound_Segs * Bytes_Per_Unit; i += 2)
			*w++ = (signed short)0;

		rval = DirectSoundSoundBuffer_Unlock(lpDSBuffer, lpvPtr1, dwBytes1, NULL, NULL);
		if (rval)
			return 1;
	}

	return 0;
}

int Play_Sound(void)
{
	int rval;

	if (Sound_Is_Playing)
		return 1;

	rval = DirectSoundSoundBuffer_Play(lpDSBuffer, 0, 0, -1/*DSBPLAY_LOOPING*/);
	Clear_Sound_Buffer();

	if (!rval)
		return 0;

	Sound_Is_Playing = -1;

	return Sound_Is_Playing;
}

int Stop_Sound(void)
{
	int rval;

	rval = DirectSoundSoundBuffer_Stop(lpDSBuffer);
	if (!rval)
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

	if (waveWriteFile(lenght, &Buf_Tmp[0]))
	{
		Put_Info("Error in WAV dumping", 1000);
		return 0;
	}

	return 1;
}


int Stop_WAV_Dump(void)
{
	if (!WAV_Dumping)
	{
		Put_Info("Already stopped", 1000);
		return 0;
	}

	if (waveCloseWriteFile())
		return 0;

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

int Play_GYM(float dt)
{
	if (!GYM_Next())
	{
		Stop_Play_GYM();
		return 0;
	}

	while (WP == Get_Current_Seg())
	{
		Update_Sound(dt);
	}

	RP = Get_Current_Seg();
	while (WP != RP)
	{
		Update_WAV_Dump();
		Write_Sound_Buffer(NULL);
		WP = (WP + 1) & (Sound_Segs - 1);

		if (WP != RP)
		{
			if (!GYM_Next())
			{
				Stop_Play_GYM();
				return 0;
			}
		}
	}

	return 1;
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