#include "VGM.h"
#include <assert.h>

void *VGMPlayer_FOpen(const char* path)
{
	return fopen(path, "rb");
}

void VGMPlayer_FClose(void *file)
{
	return fclose((FILE*)(file));
}

int VGMPlayer_FRead(void *file, void * buffer, unsigned int size)
{
	return fread(buffer, 1, size, (FILE*)(file));
}

void VGMPlayer_FSeekSet(void *file, int offset)
{
	return fseek((FILE*)(file), offset, SEEK_SET);
}

//////////////////////////////////////////////////////////////////////////////
VGMPlayer *VGMPlayer_Create(const char* path)
{
	VGMPlayer *vgmPlayer = malloc(sizeof(VGMPlayer));
	if (!vgmPlayer)
		return 0;

	vgmPlayer->file = VGMPlayer_FOpen(path);
	if (!vgmPlayer->file)
	{
		VGMPlayer_Release(vgmPlayer);
		vgmPlayer = 0;

		return 0;
	}

	if (VGMPlayer_FRead(vgmPlayer->file, &vgmPlayer->header, sizeof(vgmPlayer->header)) !=
		sizeof(vgmPlayer->header))
	{
		VGMPlayer_Release(vgmPlayer);
		vgmPlayer = 0;

		return 0;
	}

	if (vgmPlayer->header.ID[0] != 'V' ||
		vgmPlayer->header.ID[1] != 'g' ||
		vgmPlayer->header.ID[2] != 'm' ||
		vgmPlayer->header.ID[3] != ' ')
	{
		fprintf(stderr, "File is not a .vgm format");

		VGMPlayer_Release(vgmPlayer);
		vgmPlayer = 0;

		return 0;
	}

	vgmPlayer->currentMusic = 0;
	vgmPlayer->musicCount = 0;
	vgmPlayer->paused = 0;
	vgmPlayer->playing = 0;

	// seek to first data
	if (vgmPlayer->header.version < 0x150)
	{
		assert(vgmPlayer->header.VGMDataOffset != 0);

		VGMPlayer_FSeekSet(vgmPlayer->file, 0x40);
	}
	else if (vgmPlayer->header.version == 0x150)
	{
		if (vgmPlayer->header.VGMDataOffset == 0x0c) // force data at 0x40
		{
			VGMPlayer_FSeekSet(vgmPlayer->file, 0x40);
		}
		else
		{
			VGMPlayer_FSeekSet(vgmPlayer->file, vgmPlayer->header.VGMDataOffset);
		}
	}

	return vgmPlayer;
}

void VGMPlayer_Release(VGMPlayer *vgmPlayer)
{
	if (vgmPlayer)
	{
		if (vgmPlayer->file)
		{
			VGMPlayer_FClose(vgmPlayer->file);
			vgmPlayer->file = 0;
		}

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

int VGMPlayer_Next(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	vgmPlayer->currentMusic++;
	if (vgmPlayer->currentMusic >= vgmPlayer->musicCount)
	{
		int loop = 0;
		if (loop)
		{
			vgmPlayer->currentMusic = 0;
			VGMPlayer_Play(vgmPlayer);

			return -1;
		}
		else
		{
			vgmPlayer->currentMusic = vgmPlayer->musicCount - 1;
			VGMPlayer_Stop(vgmPlayer);

			return 0;
		}
	}
	else
	{
		VGMPlayer_Play(vgmPlayer);

		return -1;
	}
}

int VGMPlayer_Prev(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	vgmPlayer->currentMusic--;
	if (vgmPlayer->currentMusic <= 0)
	{
		int loop = 0;
		if (loop)
		{
			vgmPlayer->currentMusic = vgmPlayer->musicCount - 1;
			VGMPlayer_Play(vgmPlayer);

			return -1;
		}
		else
		{
			vgmPlayer->currentMusic = 0;
			VGMPlayer_Stop(vgmPlayer);

			return 0;
		}
	}
	else
	{
		VGMPlayer_Play(vgmPlayer);

		return -1;
	}
}

int VGMPlayer_Stop(VGMPlayer *vgmPlayer)
{
	vgmPlayer->playing = 0;

	return -1;
}

int VGMPlayer_isPlaying(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	return vgmPlayer->playing;
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

int VGMPlayer_isPaused(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	return vgmPlayer->paused;
}

enum VGMCommand
{
	GAME_GEAR_PSG_PORT6_WRITE = 0x4F,	// dd
	SN76489_WRITE = 0x50,				// dd
	YM2612_PORT0_WRITE = 0x52,			// aa dd
	YM2612_PORT1_WRITE = 0x53,			// aa dd
	WAIT_NNNN_SAMPLES = 0x61,			// nn nn
	END_OF_SOUND = 0x66,
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
};

typedef struct
{
	unsigned char command;
	int parametersByteCount;
	void(*handler)(unsigned char *data);
}VGMCommandFunc;

int VGMPlayer_Update(VGMPlayer *vgmPlayer)
{
	if (!vgmPlayer)
		return 0;

	unsigned char command;
	VGMPlayer_FRead(vgmPlayer->file, &command, sizeof(command));

	unsigned char address;
	unsigned char data;
	unsigned short waitNSamples;
	switch (command)
	{
	case YM2612_PORT0_WRITE:
		VGMPlayer_FRead(vgmPlayer->file, &address, sizeof(address));
		VGMPlayer_FRead(vgmPlayer->file, &data, sizeof(data));
		printf("YM2612_PORT0_WRITE(0x%02x, 0x%02x);\n", address, data);

		break;

	case YM2612_PORT1_WRITE:
		VGMPlayer_FRead(vgmPlayer->file, &address, sizeof(address));
		VGMPlayer_FRead(vgmPlayer->file, &data, sizeof(data));
		printf("YM2612_PORT1_WRITE(0x%02x, 0x%02x);\n", address, data);
		break;

	case GAME_GEAR_PSG_PORT6_WRITE:
		VGMPlayer_FRead(vgmPlayer->file, &data, sizeof(data));
		printf("Game Gear PSG stereo: GAME_GEAR_PSG_PORT6_WRITE(0x%02x);\n", data);
		break;

	case SN76489_WRITE:
		VGMPlayer_FRead(vgmPlayer->file, &data, sizeof(data));
		printf("SN76489_WRITE(0x%02x);\n", data);
		break;

	case WAIT_NNNN_SAMPLES:
		VGMPlayer_FRead(vgmPlayer->file, &waitNSamples, sizeof(waitNSamples));
		printf("WAIT_NNNN_SAMPLES(0x%04x);\n", waitNSamples);
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
		printf("WAIT_N1_SAMPLES(0x%02x);\n", command & 0x0f);
		break;

	case END_OF_SOUND:
		printf("END_OF_SOUND();\n");
		return -1;
		break;

	default:
		printf("UnHandled Command: 0x%02x", command);
	}

	return -1;
}