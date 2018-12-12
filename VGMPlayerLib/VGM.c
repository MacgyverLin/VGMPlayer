#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VGM.h"
#include "SN76489.h"
#include "YM2612.h"
#include "YM2151.h"
#include "K053260.h"
#include "NESAPU.h"

enum VGMCommand
{
	GAME_GEAR_PSG_PORT6_WRITE = 0x4F,	// dd
	SN76489_WRITE = 0x50,				// dd
	YM2413_WRITE = 0x51,				// aa dd
	YM2612_PORT0_WRITE = 0x52,			// aa dd
	YM2612_PORT1_WRITE = 0x53,			// aa dd

	YM2151_WRITE = 0x54,				// aa dd	
	YM2203_WRITE = 0x55,				// aa dd	
	YM2608_PORT0_WRITE = 0x56,			// aa dd	
	YM2608_PORT1_WRITE = 0x57,			// aa dd	
	YM2610_PORT0_WRITE = 0x58,			// aa dd	
	YM2610_PORT1_WRITE = 0x59,			// aa dd	
	YM3812_WRITE = 0x5A,				// aa dd	
	YM3526_WRITE = 0x5B,				// aa dd	
	Y8950_WRITE = 0x5C,					// aa dd	
	YMZ280B_WRITE = 0x5D,				// aa dd	
	YMF262_PORT0_WRITE = 0x5E,			// aa dd	
	YMF262_PORT1_WRITE = 0x5F,			// aa dd	

	WAIT_NNNN_SAMPLES = 0x61,			// nn nn
	WAIT_735_SAMPLES = 0x62,
	WAIT_882_SAMPLES = 0x63,
	END_OF_SOUND = 0x66,
	DATA_BLOCKS = 0x67,

	WAIT_1_SAMPLES = 0x70,
	WAIT_2_SAMPLES = 0x71,
	WAIT_3_SAMPLES = 0x72,
	WAIT_4_SAMPLES = 0x73,
	WAIT_5_SAMPLES = 0x74,
	WAIT_6_SAMPLES = 0x75,
	WAIT_7_SAMPLES = 0x76,
	WAIT_8_SAMPLES = 0x77,
	WAIT_9_SAMPLES = 0x78,
	WAIT_10_SAMPLES = 0x79,
	WAIT_11_SAMPLES = 0x7A,
	WAIT_12_SAMPLES = 0x7B,
	WAIT_13_SAMPLES = 0x7C,
	WAIT_14_SAMPLES = 0x7D,
	WAIT_15_SAMPLES = 0x7E,
	WAIT_16_SAMPLES = 0x7F,

	K053260_WRITE = 0xBA,				// aa dd
	NES_APU_WRITE = 0xB4, 				// aa dd
};

typedef struct
{
	unsigned char command;
	void(*handler)(unsigned char command);
}VGMCommandFunc;

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)

gzFile VGM_GZOpen(const char* path)
{
	return gzopen(path, "r");
}

int VGM_GZClose(gzFile file)
{
	return gzclose(file);
}

int VGM_GZRead(gzFile file, void *buffer, unsigned int size)
{
	return gzread(file, buffer, size);
}

int VGM_GZSeekSet(gzFile file, int offset)
{
	return gzseek(file, offset, SEEK_SET);
}

int VGM_GZSeekCur(gzFile file, int offset)
{
	return gzseek(file, offset, SEEK_CUR);
}


/*
void *VGM_FOpen(const char* path)
{
	return fopen(path, "rb");
}

int VGM_FClose(void *file)
{
	return fclose((FILE*)(file));
}

int VGM_FRead(void *file, void * buffer, unsigned int size)
{
	//int tell = ftell((FILE*)(file));
	//printf("tell: 0x%04x\n", tell);

	return fread(buffer, 1, size, (FILE*)(file));
}

int VGM_FSeekSet(void *file, int offset)
{
	return fseek((FILE*)(file), offset, SEEK_SET);
}

int VGM_FSeekCur(void *file, int offset)
{
	return fseek((FILE*)(file), offset, SEEK_CUR);
}
*/

//////////////////////////////////////////////////////////////////////////////
VGMData *VGMData_Create(const char* path)
{
	VGMData *vgmData = (VGMData *)malloc(sizeof(VGMData));
	if (!vgmData)
		return 0;

	vgmData->data = VGM_GZOpen(path);
	if (!vgmData->data)
	{
		VGMData_Release(vgmData);
		vgmData = 0;

		return 0;
	}

	if (VGMData_Read(vgmData, &vgmData->header, 256) != 256)
	{
		VGMData_Release(vgmData);
		vgmData = 0;

		return 0;
	}

	if (vgmData->header.ID[0] != 'V' ||
		vgmData->header.ID[1] != 'g' ||
		vgmData->header.ID[2] != 'm' ||
		vgmData->header.ID[3] != ' ')
	{
		fprintf(stderr, "Data is not a .vgm format");

		VGMData_Release(vgmData);
		vgmData = 0;

		return 0;
	}

	// seek to first data
	if (vgmData->header.version < 0x150)
	{
		assert(vgmData->header.VGMDataOffset == 0);
	}
	else if (vgmData->header.version >= 0x150)
	{
		if (vgmData->header.VGMDataOffset == 0x0c)
		{
			VGMData_FSeekSet(vgmData, 0x40);		// point to 0x40
		}
		else
		{
			VGMData_FSeekSet(vgmData, 0x34 + vgmData->header.VGMDataOffset);		// point to 0x40
		}
	}

	return vgmData;
}

void VGMData_Release(VGMData *vgmData)
{
	if (vgmData)
	{
		if (vgmData->data)
		{
			VGM_GZClose(vgmData->data);
			vgmData->data = 0;
		}

		free(vgmData);
		vgmData = 0;
	}
}

int VGMData_Read(VGMData* vgmData, void *buffer, unsigned int size)
{
	return VGM_GZRead(vgmData->data, buffer, size);
}

int VGMData_FSeekSet(VGMData* vgmData, unsigned int size)
{
	return VGM_GZSeekSet(vgmData->data, size);
}

int VGMData_FSeekCur(VGMData* vgmData, unsigned int size)
{
	return VGM_GZSeekCur(vgmData->data, size);
}


///////////////////////////////////////////////////////////////////////////
VGMPlayer *VGMPlayer_Create(VGMData* vgmData, int sampleRate, int interpolation)
{
	VGMPlayer *vgmPlayer = (VGMPlayer *)malloc(sizeof(VGMPlayer));
	if (!vgmPlayer)
		return 0;

	vgmPlayer->paused = 0;
	vgmPlayer->playing = 0;

	vgmPlayer->channels = 2;
	vgmPlayer->bitPerSamples = 16;
	vgmPlayer->sampleRate = sampleRate;

	vgmPlayer->sampleIdx = 0;
	if (vgmData->header.Rate == 0)
		vgmData->header.Rate = 60;

	vgmPlayer->sampleCount = vgmPlayer->sampleRate / vgmData->header.Rate;

	vgmPlayer->bufferIdx = 0;
	vgmPlayer->bufferCount = 32;
	vgmPlayer->bufferSize = vgmPlayer->channels * (vgmPlayer->bitPerSamples / 8) * vgmPlayer->sampleCount;
	memset(&vgmPlayer->bufferLeft[0], 0, 882);
	memset(&vgmPlayer->bufferRight[0], 0, 882);

	vgmPlayer->vgmData = vgmData;
	if (!SoundDevice_Create(&vgmPlayer->outputDevice,
		vgmPlayer->channels, vgmPlayer->bitPerSamples, vgmPlayer->sampleRate,
		vgmPlayer->bufferSize, vgmPlayer->bufferCount))
	{
		VGMPlayer_Release(vgmPlayer);
		vgmPlayer = 0;

		return 0;
	}

	if (vgmData->header.YM2612Clock)
	{
		YM2612_Initialize(1, vgmData->header.YM2612Clock, sampleRate);
	}

	if (vgmData->header.SN76489Clock)
	{
		SN76489_Initialize(1, vgmData->header.SN76489Clock, sampleRate);
	}

	if (vgmData->header.YM2151Clock)
	{
		YM2151_Initialize(1, vgmData->header.YM2151Clock, sampleRate);
	}
	if (vgmData->header.K053260Clock)
	{
		K053260_Initialize(0, vgmData->header.K053260Clock, sampleRate);
	}

	if (vgmData->header.NESAPUClock)
	{
		if (vgmData->header.NESAPUClock & 0x80000000)
		{
			NESAPU_Initialize(0, vgmData->header.NESAPUClock & 0x7fffffff, sampleRate);
		}
		else
		{
			NESAPU_Initialize(0, vgmData->header.NESAPUClock & 0x7fffffff, sampleRate);
		}
	}

	return vgmPlayer;
}

void VGMPlayer_Release(VGMPlayer *vgmPlayer)
{
	if (vgmPlayer)
	{
		NESAPU_Shutdown();
		K053260_Shutdown();
		YM2151_Shutdown();
		SN76489_Shutdown();
		YM2612_Shutdown();

		SoundDevice_StopSound(vgmPlayer->outputDevice);
		SoundDevice_Release(vgmPlayer->outputDevice);

		free(vgmPlayer);
		vgmPlayer = 0;
	}
}

int VGMPlayer_Play(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	vgmPlayer->playing = -1;

	return -1;
}

int VGMPlayer_Stop(VGMPlayer *vgmPlayer)
{
	vgmPlayer->playing = 0;

	return -1;
}

int VGMPlayer_Paused(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	vgmPlayer->paused = -1;

	return -1;
}

int VGMPlayer_Resume(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	vgmPlayer->paused = 0;

	return -1;
}

int VGMPlayer_isPlaying(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	return vgmPlayer->playing;
}

int VGMPlayer_isPaused(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	return vgmPlayer->paused;
}

void VGMPlayer_ConvertAudioData(VGMPlayer *vgmPlayer, short *Dest)
{
	int i, out_L, out_R;
	short *dest = Dest;

	for (i = 0; i < vgmPlayer->sampleCount; i++)
	{
		out_L = vgmPlayer->bufferLeft[i];
		vgmPlayer->bufferLeft[i] = 0;

		if (out_L < -0x7FFF)
			*dest++ = -0x7FFF;
		else if (out_L > 0x7FFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = (short)out_L;

		out_R = vgmPlayer->bufferRight[i];
		vgmPlayer->bufferRight[i] = 0;

		if (out_R < -0x7FFF)
			*dest++ = -0x7FFF;
		else if (out_R > 0x7FFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = (short)out_R;
	}
}

void VGMPlayer_Wait_NNNN_Sample(VGMPlayer *vgmPlayer, unsigned short NNNN)
{
	int* buf[2];

	int remainedSample = NNNN * vgmPlayer->sampleRate / 44100;
	while (remainedSample > 0)
	{
		buf[0] = &(vgmPlayer->bufferLeft[vgmPlayer->sampleIdx]);
		buf[1] = &(vgmPlayer->bufferRight[vgmPlayer->sampleIdx]);

		int updateSampleCount = VGMPlayer_MIN((vgmPlayer->sampleCount - vgmPlayer->sampleIdx), remainedSample);

		if (vgmPlayer->vgmData->header.YM2612Clock)
			YM2612_Update(0, buf, updateSampleCount);
		if (vgmPlayer->vgmData->header.SN76489Clock)
			SN76489_Update(0, buf, updateSampleCount);
		if (vgmPlayer->vgmData->header.YM2151Clock)
			YM2151_Update(0, buf, updateSampleCount);
		if (vgmPlayer->vgmData->header.K053260Clock)
			K053260_Update(0, buf, updateSampleCount);
		if (vgmPlayer->vgmData->header.NESAPUClock)
			NESAPU_Update(0, buf, updateSampleCount);

		remainedSample -= updateSampleCount;
		vgmPlayer->sampleIdx = vgmPlayer->sampleIdx + updateSampleCount;
		assert(vgmPlayer->sampleIdx <= vgmPlayer->sampleCount);
		if (vgmPlayer->sampleIdx == vgmPlayer->sampleCount)
		{
			vgmPlayer->sampleIdx = 0;

			char* data = malloc(vgmPlayer->bufferSize);

			VGMPlayer_ConvertAudioData(vgmPlayer, (short *)data);

			if (!SoundDevice_AddAudioToQueue(vgmPlayer->outputDevice,
				vgmPlayer->bufferIdx, data, vgmPlayer->bufferSize))
				break;

			free(data);

			vgmPlayer->bufferIdx = (vgmPlayer->bufferIdx + 1) % vgmPlayer->bufferCount;

			//while (SoundDevice_GetQueuedAudioCount(vgmPlayer->outputDevice) == vgmPlayer->bufferCount)
				//SoundDevice_UpdataQueueBuffer(vgmPlayer->outputDevice);
		}
	}
}

void VGMPlayer_K053260SetROM(VGMPlayer *vgmPlayer, INT32 skipByte0x66, INT32 blockType, INT32 blockSize)
{
	unsigned int entireRomSize;
	VGMData_Read(vgmPlayer->vgmData, &entireRomSize, sizeof(entireRomSize));

	unsigned int startAddress;
	VGMData_Read(vgmPlayer->vgmData, &startAddress, sizeof(startAddress));

	char* romData = malloc(blockSize - 8);
	VGMData_Read(vgmPlayer->vgmData, romData, blockSize - 8);

	// k053260_write_rom(0, entireRomSize, startAddress, blockSize - 8, romData);
	K053260_SetROM(0, entireRomSize, startAddress, romData, blockSize - 8);

	free(romData);
}

void VGMPlayer_Data_Blocks(VGMPlayer *vgmPlayer)
{
	unsigned char skipByte0x66;
	VGMData_Read(vgmPlayer->vgmData, &skipByte0x66, sizeof(skipByte0x66));

	unsigned char blockType;
	VGMData_Read(vgmPlayer->vgmData, &blockType, sizeof(blockType));

	unsigned int blockSize;
	VGMData_Read(vgmPlayer->vgmData, &blockSize, sizeof(blockSize));

	if (blockType == 0x8e)
	{
		VGMPlayer_K053260SetROM(vgmPlayer, skipByte0x66, blockType, blockSize);
	}
	else
	{
		VGMData_FSeekCur(vgmPlayer->vgmData, blockSize);
	}
}

int VGMPlayer_Update(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	while (SoundDevice_GetQueuedAudioCount(vgmPlayer->outputDevice) < vgmPlayer->bufferCount / 4)
	{
		unsigned char command;
		VGMData_Read(vgmPlayer->vgmData, &command, sizeof(command));

		unsigned char aa;
		unsigned char dd;
		unsigned short NNNN;
		switch (command)
		{
		case YM2612_PORT0_WRITE:
			VGMData_Read(vgmPlayer->vgmData, &aa, sizeof(aa));
			YM2612_WriteRegister(0, 0, aa);

			VGMData_Read(vgmPlayer->vgmData, &dd, sizeof(dd));
			YM2612_WriteRegister(0, 1, dd);

			//printf("YM2612WriteReg(0, 0, 0x%02x)", aa);
			//printf("YM2612WriteReg(0, 1, 0x%02x)", dd);
			break;

		case YM2612_PORT1_WRITE:
			VGMData_Read(vgmPlayer->vgmData, &aa, sizeof(aa));
			YM2612_WriteRegister(0, 2, aa);

			VGMData_Read(vgmPlayer->vgmData, &dd, sizeof(dd));
			YM2612_WriteRegister(0, 3, dd);

			//printf("YM2612WriteReg(0, 2, 0x%02x)", aa);
			//printf("YM2612WriteReg(0, 3, 0x%02x)", dd);

			break;

		case YM2151_WRITE:
			VGMData_Read(vgmPlayer->vgmData, &aa, sizeof(aa));
			VGMData_Read(vgmPlayer->vgmData, &dd, sizeof(dd));
			//printf("YM2151_WRITE(0x%02x, 0x%02x);\n", aa, dd);
			YM2151_WriteRegister(0, aa, dd);

			break;

		case GAME_GEAR_PSG_PORT6_WRITE:
			VGMData_Read(vgmPlayer->vgmData, &dd, sizeof(dd));
			//printf("Game Gear PSG stereo: GAME_GEAR_PSG_PORT6_WRITE(0x%02x);\n", dd);
			SN76489_WriteRegister(0, -1, dd);

			break;

		case SN76489_WRITE:
			VGMData_Read(vgmPlayer->vgmData, &dd, sizeof(dd));
			//printf("SN76489_WRITE(0x%02x);\n", dd);
			SN76489_WriteRegister(0, -1, dd);
			break;

		case WAIT_NNNN_SAMPLES:
			VGMData_Read(vgmPlayer->vgmData, &NNNN, sizeof(NNNN));
			//printf("VGMPlayer_Wait_NNNN_Sample(vgmPlayer, 0x%04x);\n", NNNN);
			VGMPlayer_Wait_NNNN_Sample(vgmPlayer, NNNN);
			break;

		case WAIT_735_SAMPLES:
			//printf("VGMPlayer_Wait_NNNN_Sample(vgmPlayer, 735);\n");
			VGMPlayer_Wait_NNNN_Sample(vgmPlayer, 735);
			break;

		case WAIT_882_SAMPLES:
			//printf("VGMPlayer_Wait_NNNN_Sample(vgmPlayer, 882);\n");
			VGMPlayer_Wait_NNNN_Sample(vgmPlayer, 882);
			break;

		case WAIT_1_SAMPLES:
		case WAIT_2_SAMPLES:
		case WAIT_3_SAMPLES:
		case WAIT_4_SAMPLES:
		case WAIT_5_SAMPLES:
		case WAIT_6_SAMPLES:
		case WAIT_7_SAMPLES:
		case WAIT_8_SAMPLES:
		case WAIT_9_SAMPLES:
		case WAIT_10_SAMPLES:
		case WAIT_11_SAMPLES:
		case WAIT_12_SAMPLES:
		case WAIT_13_SAMPLES:
		case WAIT_14_SAMPLES:
		case WAIT_15_SAMPLES:
		case WAIT_16_SAMPLES:
			//printf("VGMPlayer_Wait_NNNN_Sample(vgmPlayer, 0x%02x);\n", (command & 0x0f) + 1);
			VGMPlayer_Wait_NNNN_Sample(vgmPlayer, (command & 0x0f) + 1);
			break;

		case END_OF_SOUND:
			printf("END_OF_SOUND();\n");
			return 0;
			break;

		case DATA_BLOCKS:
			printf("VGMPlayer_Data_Blocks(vgmPlayer);\n");
			VGMPlayer_Data_Blocks(vgmPlayer);
			break;

		case K053260_WRITE:
			VGMData_Read(vgmPlayer->vgmData, &aa, sizeof(aa));
			VGMData_Read(vgmPlayer->vgmData, &dd, sizeof(dd));

			K053260_WriteRegister(0, aa, dd);

			//printf("k053260_w(0x%02x, 0x%02x);\n", aa, dd);
			break;

		case NES_APU_WRITE:
			VGMData_Read(vgmPlayer->vgmData, &aa, sizeof(aa));
			VGMData_Read(vgmPlayer->vgmData, &dd, sizeof(dd));

			NESAPU_WriteRegister(0, aa, dd);
			break;

		default:
			printf("UnHandled Command: 0x%02x\n", command);
		}
	};

	if (GetDeviceState(vgmPlayer->outputDevice) != 3)
	{
		SoundDevice_PlaySound(vgmPlayer->outputDevice);

		//SoundDevice_SetVolume(vgmPlayer->outputDevice, 1.0);
		//SoundDevice_SetPlayRate(vgmPlayer->outputDevice, 1.0);
	}

	if (!SoundDevice_UpdataQueueBuffer(vgmPlayer->outputDevice))
		return 0;

	return -1;
};