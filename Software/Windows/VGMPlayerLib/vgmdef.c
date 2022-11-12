#include "vgmdef.h"
#include <stdio.h>

stream_sample_t* DUMMYBUF[0x02] = { NULL, NULL };
WAVE_32BS* DUMMY_CHANNEL_BUF[CHANNEL_BUFFER_COUNT] = { 0 };

void chip_reg_write(UINT8 ChipType, UINT8 ChipID, UINT8 Port, UINT8 Offset, UINT8 Data)
{
#ifndef DISABLE_HW_SUPPORT
	bool ModeFM;
	UINT8 CurChip;

	switch (PlayingMode)
	{
	case 0x00:
		ModeFM = false;
		break;
	case 0x01:
		ModeFM = true;
		break;
	case 0x02:
		ModeFM = false;
		for (CurChip = 0x00; CurChip < ChipCount; CurChip++)
		{
			if (ChipMap[CurChip].ChipType == ChipType &&
				ChipMap[CurChip].ChipID == ChipID)
			{
				ModeFM = true;
				break;
			}
		}
		break;
	}

	if (!ModeFM)
	{
#endif	// DISABLE_HW_SUPPORT
		switch (ChipType)
		{
		case 0x00:	// SN76496
			sn764xx_w(ChipID, Port, Data);
			break;
		case 0x01:	// YM2413
			ym2413_w(ChipID, 0x00, Offset);
			ym2413_w(ChipID, 0x01, Data);
			break;
		case 0x02:	// YM2612
			ym2612_w(ChipID, (Port << 1) | 0x00, Offset);
			ym2612_w(ChipID, (Port << 1) | 0x01, Data);
			break;
		case 0x03:	// YM2151
			ym2151_w(ChipID, 0x00, Offset);
			ym2151_w(ChipID, 0x01, Data);
			break;
		case 0x04:	// SegaPCM
			break;
		case 0x05:	// RF5C68
			rf5c68_w(ChipID, Offset, Data);
			break;
		case 0x06:	// YM2203
			ym2203_w(ChipID, 0x00, Offset);
			ym2203_w(ChipID, 0x01, Data);
			break;
		case 0x07:	// YM2608
			ym2608_w(ChipID, (Port << 1) | 0x00, Offset);
			ym2608_w(ChipID, (Port << 1) | 0x01, Data);
			break;
		case 0x08:	// YM2610/YM2610B
			ym2610_w(ChipID, (Port << 1) | 0x00, Offset);
			ym2610_w(ChipID, (Port << 1) | 0x01, Data);
			break;
		case 0x09:	// YM3812
			ym3812_w(ChipID, 0x00, Offset);
			ym3812_w(ChipID, 0x01, Data);
			break;
		case 0x0A:	// YM3526
			ym3526_w(ChipID, 0x00, Offset);
			ym3526_w(ChipID, 0x01, Data);
			break;
		case 0x0B:	// Y8950
			y8950_w(ChipID, 0x00, Offset);
			y8950_w(ChipID, 0x01, Data);
			break;
		case 0x0C:	// YMF262
			ymf262_w(ChipID, (Port << 1) | 0x00, Offset);
			ymf262_w(ChipID, (Port << 1) | 0x01, Data);
			break;
		case 0x0D:	// YMF278B
			ymf278b_w(ChipID, (Port << 1) | 0x00, Offset);
			ymf278b_w(ChipID, (Port << 1) | 0x01, Data);
			break;
		case 0x0E:	// YMF271
			ymf271_w(ChipID, (Port << 1) | 0x00, Offset);
			ymf271_w(ChipID, (Port << 1) | 0x01, Data);
			break;
		case 0x0F:	// YMZ280B
			ymz280b_w(ChipID, 0x00, Offset);
			ymz280b_w(ChipID, 0x01, Data);
			break;
		case 0x10:	// RF5C164
			rf5c164_w(ChipID, Offset, Data);
			break;
		case 0x11:	// PWM
			pwm_chn_w(ChipID, Port, (Offset << 8) | (Data << 0));
			break;
		case 0x12:	// AY8910
			ayxx_w(ChipID, 0x00, Offset);
			ayxx_w(ChipID, 0x01, Data);
			break;
		case 0x13:	// GameBoy
			gb_sound_w(ChipID, Offset, Data);
			break;
		case 0x14:	// NES APU
			nes_w(ChipID, Offset, Data);
			break;
		case 0x15:	// MultiPCM
			multipcm_w(ChipID, Offset, Data);
			break;
		case 0x16:	// UPD7759
			upd7759_write(ChipID, Offset, Data);
			break;
		case 0x17:	// OKIM6258
			okim6258_write(ChipID, Offset, Data);
			break;
		case 0x18:	// OKIM6295
			okim6295_w(ChipID, Offset, Data);
			break;
		case 0x19:	// K051649 / SCC1
			k051649_w(ChipID, (Port << 1) | 0x00, Offset);
			k051649_w(ChipID, (Port << 1) | 0x01, Data);
			break;
		case 0x1A:	// K054539
			k054539_w(ChipID, (Port << 8) | (Offset << 0), Data);
			break;
		case 0x1B:	// HuC6280
			c6280_w(ChipID, Offset, Data);
			break;
		case 0x1C:	// C140
			c140_w(ChipID, (Port << 8) | (Offset << 0), Data);
			break;
		case 0x1D:	// K053260
			k053260_w(ChipID, Offset, Data);
			break;
		case 0x1E:	// Pokey
			pokey_w(ChipID, Offset, Data);
			break;
		case 0x1F:	// QSound
			qsound_w(ChipID, 0x00, Port);	// Data MSB
			qsound_w(ChipID, 0x01, Offset);	// Data LSB
			qsound_w(ChipID, 0x02, Data);	// Register
			break;
		case 0x20:	// YMF292/SCSP
			scsp_w(ChipID, (Port << 8) | (Offset << 0), Data);
			break;
		case 0x21:	// WonderSwan
			ws_audio_port_write(ChipID, 0x80 | Offset, Data);
			break;
		case 0x22:	// VSU
			VSU_Write(ChipID, (Port << 8) | (Offset << 0), Data);
			break;
		case 0x23:	// SAA1099
			saa1099_control_w(ChipID, 0, Offset);
			saa1099_data_w(ChipID, 0, Data);
			break;
		case 0x24:	// ES5503
			es5503_w(ChipID, Offset, Data);
			break;
		case 0x25:	// ES5506
			if (Port & 0x80)
				es550x_w16(ChipID, Port & 0x7F, (Offset << 8) | (Data << 0));
			else
				es550x_w(ChipID, Port, Data);
			break;
		case 0x26:	// X1-010
			seta_sound_w(ChipID, (Port << 8) | (Offset << 0), Data);
			break;
		case 0x27:	// C352
			c352_w(ChipID, Port, (Offset << 8) | (Data << 0));
			break;
		case 0x28:	// GA20
			irem_ga20_w(ChipID, Offset, Data);
			break;
			//		case 0x##:	// OKIM6376
			//			break;
		}
#ifndef DISABLE_HW_SUPPORT
	}
	else
	{
		if (!OpenedFM)
			return;

		SelChip = ChipArr[(ChipType << 1) | (ChipID & 0x01)];
		switch (ChipType)
		{
		case 0x00:	// SN76496
			switch (Port)
			{
			case 0x00:
				sn76496_write_opl(ChipID, 0x00, Data);
				break;
			case 0x01:
				sn76496_stereo_opl(ChipID, 0x00, Data);
				break;
			}
			break;
		case 0x01:	// YM2413
			switch (YM2413_EMU_CORE)
			{
			case YM2413_EC_DEFAULT:
				ym2413_w_opl(ChipID, 0x00, Offset);
				ym2413_w_opl(ChipID, 0x01, Data);
				break;
			case YM2413_EC_MEKA:
				FM_OPL_Write(Offset, Data);
				break;
			}
			break;
		case 0x09:	// YM3812
			if ((Offset & 0xF0) == 0xC0 && (!FMOPL2Pan || !(Data & 0x30)))
				Data |= 0x30;
			else if ((Offset & 0xF0) == 0xE0)
				Data &= 0xF3;
			OPL_RegMapper((ChipID << 8) | Offset, Data);
			break;
		case 0x0A:	// YM3526
			if ((Offset & 0xF0) == 0xC0 && (!FMOPL2Pan || !(Data & 0x30)))
				Data |= 0x30;
			else if ((Offset & 0xF0) == 0xE0)
				Data &= 0xF0;
			OPL_RegMapper((ChipID << 8) | Offset, Data);
			break;
		case 0x0B:	// Y8950
			if (Offset == 0x07 || (Offset >= 0x09 && Offset <= 0x17))
				break;
			if ((Offset & 0xF0) == 0xC0 && (!FMOPL2Pan || !(Data & 0x30)))
				Data |= 0x30;
			else if ((Offset & 0xF0) == 0xE0)
				Data &= 0xF0;
			OPL_RegMapper((ChipID << 8) | Offset, Data);
			break;
		case 0x0C:	// YMF262
			OPL_RegMapper((Port << 8) | Offset, Data);
			break;
		case 0x12:	// AY8910
			ay8910_write_opl(ChipID, Offset, Data);
			break;
		}
	}
#endif	// DISABLE_HW_SUPPORT
	return;
}

char* FindFile(const char* FileName)
{
	char* FullName;
	char** CurPath;
	UINT32 NameLen;
	UINT32 PathLen;
	UINT32 FullLen;
	FILE* hFile = NULL;

	NameLen = strlen(FileName);
	//printf("Find File: %s\n", FileName);

	// go to end of the list + get size of largest path
	// (The first entry has the lowest priority.)
	PathLen = 0;
	CurPath = "";
	while (*CurPath != NULL)
	{
		FullLen = strlen(*CurPath);
		if (FullLen > PathLen)
			PathLen = FullLen;
		CurPath++;
	}
	CurPath--;
	FullLen = PathLen + NameLen;
	FullName = (char*)malloc(FullLen + 1);

	hFile = NULL;
	//for (; CurPath >= AppPaths; CurPath--)
	//{
		strcpy(FullName, *CurPath);
		strcat(FullName, FileName);

		//printf("Trying path: %s\n", FullName);
		hFile = fopen(FullName, "r");
		if (hFile != NULL)
			return NULL;
	//}

	if (hFile != NULL)
	{
		fclose(hFile);
		//printf("Success!\n");
		return FullName;	// The caller has to free the string.
	}
	else
	{
		free(FullName);
		//printf("Fail!\n");
		return NULL;
	}
}