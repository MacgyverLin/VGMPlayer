#include "QSound.h"
#include "vgmdef.h"

typedef struct 
{
	u8 bKey;						// 1 if channel is playing
	s8 nBank;						// Bank we are currently playing a sample from

	s32 nPlayStart;				// Start of being played
	s32 nStart;					// Start of sample 16.12
	s32 nEnd;						// End of sample   16.12
	s32 nLoop;					// Loop offset from end
	s32 nPos;						// Current position within the bank 16.12
	s32 nAdvance;					// Sample size

	s32 nMasterVolume;			// Master volume for the channel
	s32 nVolume[2];				// Left & right side volumes (panning)

	s32 nPitch;					// Playback frequency

	s8 nEndBuffer[8];				// Buffer to enable correct cubic interpolation

	u32 PlayBank;					// Pointer to current bank
}QSoundChannel;

typedef struct
{
	s32 clock;
	s32 clockDivider;
	s32 sampleRate;
	s32 Mmatrix;
	s32 interpolation;
	QSoundChannel channels[16];
	s32 panningVolumes[33];
	ROM* rom;

	u32 channel_enabled;
	u32	channel_count;
}QSound;

#define MAX_QSOUND 2
QSound qSound[MAX_QSOUND];

#define INTERPOLATE4PS_CUSTOM(fp, sN, s0, s1, s2, v) (((s32)((sN) * Precalc[(s32)(fp) * 4 + 0]) + (s32)((s0) * Precalc[(s32)(fp) * 4 + 1]) + (s32)((s1) * Precalc[(s32)(fp) * 4 + 2]) + (s32)((s2) * Precalc[(s32)(fp) * 4 + 3])) / (s32)(v))
//#define INTERPOLATE4PS_CUSTOM(fp, sN, s0, s1, s2, v) (((s32)((sN) * Precalc[(s32)(fp<<2) + 0]) + (s32)((s0) * Precalc[(s32)(fp<<2) + 1]) + (s32)((s1) * Precalc[(s32)(fp<<2) + 2]) + (s32)((s2) * Precalc[(s32)(fp<<2) + 3])) >> 8)
s16 Precalc[4096 * 4];

s8 QSound_GetSample(u8 chipID, u32 address)
{
	QSound* ic = &qSound[chipID];

	return *(s8*)ROM_getPtr(ic->rom, address);
}

// Routine used to precalculate the table used for interpolation
s32 cmc_4p_Precalc()
{
	s32 a, x, x2, x3;

	for (a = 0; a < 4096; a++) {
		x = a * 4;			// x = 0..16384
		x2 = x * x / 16384;	// pow(x, 2);
		x3 = x2 * x / 16384;	// pow(x, 3);

		Precalc[a * 4 + 0] = (s16)(-x / 3 + x2 / 2 - x3 / 6);
		Precalc[a * 4 + 1] = (s16)(-x / 2 - x2 + x3 / 2 + 16384);
		Precalc[a * 4 + 2] = (s16)(x + x2 / 2 - x3 / 2);
		Precalc[a * 4 + 3] = (s16)(-x / 6 + x3 / 6);
	}

	return 0;
}

void MapBank(u8 chipID, QSoundChannel* pc)
{
	QSound* ic = &qSound[chipID];
	u32 nBank = (pc->nBank & 0x7F) << 16;	// Banks are 0x10000 samples long

	// Confirm whole bank is in range:
	// If bank is out of range use bank 0 instead
	//if ((nBank + 0x10000) > ic->romLength)
	{
		//nBank = 0;
	}
	pc->PlayBank = nBank;
}

void UpdateEndBuffer(u8 chipID, QSoundChannel* pc)
{
	QSound* ic = &qSound[chipID];

	if (pc->bKey) 
	{
		// prepare a buffer to correctly interpolate the last 4 samples
		if(ic->interpolation >= 3)
		{
			for (s32 i = 0; i < 4; i++) 
			{
				pc->nEndBuffer[i] = QSound_GetSample(chipID, pc->PlayBank + (pc->nEnd >> 12) - 4 + i);
			}

			if (pc->nLoop) 
			{
				for (s32 i = 0, j = 0; i < 4; i++, j++) 
				{
					if (j >= (pc->nLoop >> 12)) 
					{
						j = 0;
					}
					pc->nEndBuffer[i + 4] = QSound_GetSample(chipID, pc->PlayBank + ((pc->nEnd - pc->nLoop) >> 12) + j);
				}
			}
			else 
			{
				for (s32 i = 0; i < 4; i++) 
				{
					pc->nEndBuffer[i + 4] = pc->nEndBuffer[3];
				}
			}
		}
	}
}

void CalcAdvance(u8 chipID, QSoundChannel* pc)
{
	QSound* ic = &qSound[chipID];

	if (ic->sampleRate) 
	{
		pc->nAdvance = (INT64)pc->nPitch * ic->clock / ic->clockDivider / ic->sampleRate;
	}
}

s32 QSound_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	QSound* ic = &qSound[chipID];

	ic->clock = 4000000;
	ic->clockDivider = 166;
	ic->sampleRate = sampleRate;
	ic->Mmatrix = 0;
	ic->interpolation = 3;
	ic->rom = 0;
	for(s32 i = 0; i < 33; i++)
	{
		ic->panningVolumes[i] = (s32)((256.0f / sqrt(32.0f)) * sqrt((double)i));
	}
	
	cmc_4p_Precalc();

	QSound_Reset(chipID);

	return 0;
}

void QSound_Reset(u8 chipID)
{
	QSound* ic = &qSound[chipID];

	memset(ic->channels, 0, sizeof(ic->channels));

	// Point all to bank 0
	for (s32 i = 0; i < 16; i++)
	{
		ic->channels[i].PlayBank = 0;
	}

	ic->channel_enabled = 0xffffffff;
	ic->channel_count = 16;
}

void QSound_Shutdown(u8 chipID)
{
	QSound* ic = &qSound[chipID];
	ic->sampleRate = 0;
}

void QSound_WriteRegister(u8 chipID, u32 address, u32 data, s32* channel, f32* freq)
{
	QSound* ic = &qSound[chipID];
	QSoundChannel* pc;
	s32 nChanNum, r;

	// unknown
	if (address >= 0x90)
	{
		// bprintf(PRINT_NORMAL, "QSound: reg 0x%02X -> 0x%02X.\n", address, data);
		return;
	}

	//QscSyncQsnd();
	if(address >= 0x80)
	{									// Set panning for channel
		s32 nPan;

		nChanNum = address & 15;

		pc = ic->channels + nChanNum;			// Find channel
		nPan = (data - 0x10) & 0x3F;	// nPan = 0x00 to 0x20 now
		if (nPan > 0x20) 
		{
			nPan = 0x20;
		}

		// bprintf(PRINT_NORMAL, _T("QSound: ch#%i pan -> 0x%X\n"), nChanNum, nPan);
		if(ic->Mmatrix && nPan == 0x00)
		{
			nPan = 0x10; // Fixes all sfx that are hard-panned to the right. (Mars Matrix only)
		}

		pc->nVolume[0] = ic->panningVolumes[0x20 - nPan];
		pc->nVolume[1] = ic->panningVolumes[0x00 + nPan];
		return;
	}

	// Get channel and register number
	nChanNum = (address >> 3) & 15;
	r = address & 7;

	// Pointer to channel info
	pc = ic->channels + nChanNum;
	switch (r) 
	{
	case 0: 
	{										// Set bank
		// Strange but true
		pc = ic->channels + ((nChanNum + 1) & 15);
		pc->nBank = data;
		MapBank(chipID, pc);
		UpdateEndBuffer(chipID, pc);
		break;
	}
	case 1: 
	{										// Set sample start offset
		pc->nStart = data << 12;
		break;
	}
	case 2: 
	{
		pc->nPitch = data;
		CalcAdvance(chipID, pc);

		if (data == 0) {					// Key off; stop playing
			pc->bKey = 0;
		}

		break;
	}
#if 0
	case 3: 
	{
		break;
	}
#endif
	case 4: 
	{										// Set sample loop offset
		pc->nLoop = data << 12;
		UpdateEndBuffer(chipID, pc);
		break;
	}
	case 5: 
	{										// Set sample end offset
		pc->nEnd = data << 12;
		UpdateEndBuffer(chipID, pc);
		break;
	}
	case 6: 
	{										// Set volume
		pc->nMasterVolume = data;

		if(data == 0) 
		{
			pc->bKey = 0;
		}
		else 
		{
			if (pc->bKey == 0) 
			{					// Key on; play sample
				pc->nPlayStart = pc->nStart;

				pc->nPos = 0;
				pc->bKey = 3;
				UpdateEndBuffer(chipID, pc);
			}
		}
		break;
	}
#if 0
	case 7: 
	{
		break;
	}
#endif
	}
}

u8 QSound_ReadRegister(u8 chipID, u32 address)
{
	return 0;
}

void QSound_Update(u8 chipID, s32** buffer, u32 length)
{
	QSound* ic = &qSound[chipID];

	// Go through all channels
	for (s32 ch = 0; ch < ic->channel_count; ch++)
	{
		if ((ic->channel_enabled & (1 << (ch))) == 0)
		{
			for (int sample = 0; sample < length; sample++)
			{
				buffer[((ch) << 1) + 0][sample] = 0;
				buffer[((ch) << 1) + 1][sample] = 0;
			}
		}
		else
		{
			// If the channel is playing, add the samples to the buffer
			if (ic->channels[ch].bKey)
			{
				s32 VolL = (ic->channels[ch].nMasterVolume * ic->channels[ch].nVolume[0]) >> 11;
				s32 VolR = (ic->channels[ch].nMasterVolume * ic->channels[ch].nVolume[1]) >> 11;
				//s32 *lBuf = buffer[0];
				//s32 *rBuf = buffer[1];
				s32 i = length;
				int idx = 0;

				// handle 1st sample
				if (ic->channels[ch].bKey & 2)
				{
					while (ic->channels[ch].nPos < 0x1000 && i)
					{
						s32 p = ic->channels[ch].nPlayStart >> 12;
						s32 s = INTERPOLATE4PS_CUSTOM(
							ic->channels[ch].nPos,
							0,
							QSound_GetSample(chipID, ic->channels[ch].PlayBank + p + 0),
							QSound_GetSample(chipID, ic->channels[ch].PlayBank + p + 1),
							QSound_GetSample(chipID, ic->channels[ch].PlayBank + p + 2),
							256);

						int sample = length - i;
						buffer[((ch) << 1) + 0][sample] = ((s * VolL) >> 6);
						buffer[((ch) << 1) + 1][sample] = ((s * VolR) >> 6);
						//*(lBuf++) += ((s * VolL) >> 8);
						//*(rBuf++) += ((s * VolR) >> 8);

						ic->channels[ch].nPos += ic->channels[ch].nAdvance;				// increment sample position based on pitch
						i--;
					}
					if (i > 0)
					{
						ic->channels[ch].bKey &= ~2;
						ic->channels[ch].nPos = (ic->channels[ch].nPos & 0x0FFF) + ic->channels[ch].nPlayStart;
					}
				}

				while (i > 0)
				{
					s32 s, p;

					// Check for end of sample
					if (ic->channels[ch].nPos >= (ic->channels[ch].nEnd - 0x3000))
					{
						if (ic->channels[ch].nPos < ic->channels[ch].nEnd)
						{
							s32 nIndex = 4 - ((ic->channels[ch].nEnd - ic->channels[ch].nPos) >> 12);
							s = INTERPOLATE4PS_CUSTOM(
								(ic->channels[ch].nPos) & ((1 << 12) - 1),
								ic->channels[ch].nEndBuffer[nIndex + 0],
								ic->channels[ch].nEndBuffer[nIndex + 1],
								ic->channels[ch].nEndBuffer[nIndex + 2],
								ic->channels[ch].nEndBuffer[nIndex + 3],
								256);
						}
						else
						{
							if (ic->channels[ch].nLoop)
							{					// Loop sample
								if (ic->channels[ch].nLoop <= 0x1000)
								{		// Don't play, but leave bKey on
									ic->channels[ch].nPos = ic->channels[ch].nEnd - 0x1000;
									break;
								}
								ic->channels[ch].nPos -= ic->channels[ch].nLoop;
								continue;
							}
							else
							{
								ic->channels[ch].bKey = 0;					// Stop playing
								break;
							}
						}
					}
					else
					{
						p = (ic->channels[ch].nPos >> 12) & 0xFFFF;
						s = INTERPOLATE4PS_CUSTOM(
							(ic->channels[ch].nPos) & ((1 << 12) - 1),
							QSound_GetSample(chipID, ic->channels[ch].PlayBank + p + 0),
							QSound_GetSample(chipID, ic->channels[ch].PlayBank + p + 1),
							QSound_GetSample(chipID, ic->channels[ch].PlayBank + p + 2),
							QSound_GetSample(chipID, ic->channels[ch].PlayBank + p + 3),
							256);
					}

					// Add to the sound currently in the buffer
					int sample = length - i;
					buffer[((ch) << 1) + 0][sample] = ((s * VolL) >> 6);
					buffer[((ch) << 1) + 1][sample] = ((s * VolR) >> 6);
					//*(lBuf++) += ((s * VolL) >> 8);
					//*(rBuf++) += ((s * VolR) >> 8);

					ic->channels[ch].nPos += ic->channels[ch].nAdvance;				// increment sample position based on pitch
					i--;
				}
			}
		}
	}
}

/*
void QSound_SetROM(u8 chipID, u32 totalROMSize, u32 startAddress, u8 *rom, u32 nLen)
{
	QSound* ic = &qSound[chipID];

	if(ic->romLength != totalROMSize)
	{
		if(ic->rom)
		{
			u8* newRom = malloc(totalROMSize);

			memcpy(newRom, ic->rom, totalROMSize);
			free(ic->rom);

			ic->rom = newRom;
		}
		else
		{
			ic->rom = malloc(totalROMSize);
		}

		ic->romLength = totalROMSize;
	}

	memcpy(&ic->rom[startAddress], rom, nLen);
}
*/

void QSound_SetROM(u8 chipID, ROM* rom)
{
	QSound* ic = &qSound[chipID];

	ic->rom = rom;
}

void QSound_SetChannelEnable(u8 chipID, u8 channel, u8 enable)
{
	QSound* ic = &qSound[chipID];

	if (enable)
		ic->channel_enabled |= (1 << channel);
	else
		ic->channel_enabled &= (~(1 << channel));
}

u8 QSound_GetChannelEnable(u8 chipID, u8 channel)
{
	QSound* ic = &qSound[chipID];

	return (ic->channel_enabled & (1 << channel)) != 0;
}

u32 QSound_GetChannelCount(u8 chipID)
{
	QSound* ic = &qSound[chipID];

	return ic->channel_count;
}