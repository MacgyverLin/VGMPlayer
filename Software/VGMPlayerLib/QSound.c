#include "QSound.h"

#ifdef STM32
#include <VGMBoard.h>

s32 QSound_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	return -1;
}

void QSound_Shutdown(u8 chipID)
{
}

void QSound_Reset(u8 chipID)
{
}

void QSound_WriteRegister(u8 chipID, u32 address, u8 data)
{
}

u8 QSound_ReadRegister(u8 chipID, u32 address)
{
	return -1;
}

void QSound_Update(u8 chipID, s32** buffer, u32 length)
{
}

#else

#include "vgmdef.h"

struct QChan_s
{
	UINT8 bKey;						// 1 if channel is playing
	INT8 nBank;						// Bank we are currently playing a sample from

	INT32 nPlayStart;				// Start of being played
	INT32 nStart;					// Start of sample 16.12
	INT32 nEnd;						// End of sample   16.12
	INT32 nLoop;					// Loop offset from end
	INT32 nPos;						// Current position within the bank 16.12
	INT32 nAdvance;					// Sample size

	INT32 nMasterVolume;			// Master volume for the channel
	INT32 nVolume[2];				// Left & right side volumes (panning)

	INT32 nPitch;					// Playback frequency

	INT8 nEndBuffer[8];				// Buffer to enable correct cubic interpolation

	INT8* PlayBank;					// Pointer to current bank
};

static const INT32 nQscClock = 4000000;
static const INT32 nQscClockDivider = 166;
static INT32 nQscRate = 0;
static INT32 Mmatrix;
static INT32 nInterpolation = 3;
static INT32 nBurnSoundLen = VGM_SAMPLE_COUNT;
static struct QChan_s QChan[16];
static INT32 PanningVolumes[33];
INT8 *CpsQSam = 0; 
UINT32 nCpsQSamLen = 0;

#define INTERPOLATE4PS_CUSTOM(fp, sN, s0, s1, s2, v) (((INT32)((sN) * Precalc[(INT32)(fp) * 4 + 0]) + (INT32)((s0) * Precalc[(INT32)(fp) * 4 + 1]) + (INT32)((s1) * Precalc[(INT32)(fp) * 4 + 2]) + (INT32)((s2) * Precalc[(INT32)(fp) * 4 + 3])) / (INT32)(v))
INT16 Precalc[4096 * 4];

// Routine used to precalculate the table used for interpolation
INT32 cmc_4p_Precalc()
{
	INT32 a, x, x2, x3;

	for (a = 0; a < 4096; a++) {
		x = a * 4;			// x = 0..16384
		x2 = x * x / 16384;	// pow(x, 2);
		x3 = x2 * x / 16384;	// pow(x, 3);

		Precalc[a * 4 + 0] = (INT16)(-x / 3 + x2 / 2 - x3 / 6);
		Precalc[a * 4 + 1] = (INT16)(-x / 2 - x2 + x3 / 2 + 16384);
		Precalc[a * 4 + 2] = (INT16)(x + x2 / 2 - x3 / 2);
		Precalc[a * 4 + 3] = (INT16)(-x / 6 + x3 / 6);
	}

	return 0;
}

static void MapBank(struct QChan_s* pc)
{
	UINT32 nBank;

	nBank = (pc->nBank & 0x7F) << 16;	// Banks are 0x10000 samples long

	// Confirm whole bank is in range:
	// If bank is out of range use bank 0 instead
	if ((nBank + 0x10000) > nCpsQSamLen)
	{
		nBank = 0;
	}
	pc->PlayBank = (INT8*)CpsQSam + nBank;
}

static void UpdateEndBuffer(struct QChan_s* pc)
{
	if (pc->bKey) 
	{
		// prepare a buffer to correctly interpolate the last 4 samples
		if (nInterpolation >= 3) 
		{
			for (INT32 i = 0; i < 4; i++) 
			{
				pc->nEndBuffer[i] = pc->PlayBank[(pc->nEnd >> 12) - 4 + i];
			}

			if (pc->nLoop) 
			{
				for (INT32 i = 0, j = 0; i < 4; i++, j++) 
				{
					if (j >= (pc->nLoop >> 12)) 
					{
						j = 0;
					}
					pc->nEndBuffer[i + 4] = pc->PlayBank[((pc->nEnd - pc->nLoop) >> 12) + j];
				}
			}
			else 
			{
				for (INT32 i = 0; i < 4; i++) 
				{
					pc->nEndBuffer[i + 4] = pc->nEndBuffer[3];
				}
			}
		}
	}
}

static void CalcAdvance(struct QChan_s* pc)
{
	if (nQscRate) {
		pc->nAdvance = (INT64)pc->nPitch * nQscClock / nQscClockDivider / nQscRate;
	}
}

s32 QSound_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	nQscRate = sampleRate;
	for (INT32 i = 0; i < 33; i++)
	{
		PanningVolumes[i] = (INT32)((256.0f / sqrt(32.0f)) * sqrt((double)i));
	}

	//nCpsQSamLen = 0x10000 * 8;
	//CpsQSam = (INT8*)malloc(nCpsQSamLen);
	
	cmc_4p_Precalc();

	QSound_Reset(chipID);

	return 0;
}

void QSound_Reset(u8 chipID)
{
	memset(QChan, 0, sizeof(QChan));

	// Point all to bank 0
	for (INT32 i = 0; i < 16; i++)
	{
		QChan[i].PlayBank = (INT8*)CpsQSam;
	}
}

void QSound_Shutdown(u8 chipID)
{
	nQscRate = 0;

	if(CpsQSam)
	{
		free(CpsQSam);
		CpsQSam = 0;
		nCpsQSamLen = 0;
	}
}

void QSound_WriteRegister(u8 chipID, u32 address, u32 data)
{
	struct QChan_s* pc;
	INT32 nChanNum, r;

	// unknown
	if (address >= 0x90)
	{
		// bprintf(PRINT_NORMAL, "QSound: reg 0x%02X -> 0x%02X.\n", address, data);
		return;
	}

	//QscSyncQsnd();
	if(address >= 0x80)
	{									// Set panning for channel
		INT32 nPan;

		nChanNum = address & 15;

		pc = QChan + nChanNum;			// Find channel
		nPan = (data - 0x10) & 0x3F;	// nPan = 0x00 to 0x20 now
		if (nPan > 0x20) 
		{
			nPan = 0x20;
		}

		// bprintf(PRINT_NORMAL, _T("QSound: ch#%i pan -> 0x%X\n"), nChanNum, nPan);
		if (Mmatrix && nPan == 0x00) 
		{
			nPan = 0x10; // Fixes all sfx that are hard-panned to the right. (Mars Matrix only)
		}

		pc->nVolume[0] = PanningVolumes[0x20 - nPan];
		pc->nVolume[1] = PanningVolumes[0x00 + nPan];
		return;
	}

	// Get channel and register number
	nChanNum = (address >> 3) & 15;
	r = address & 7;

	// Pointer to channel info
	pc = QChan + nChanNum;

	switch (r) 
	{
	case 0: 
	{										// Set bank
		// Strange but true
		pc = QChan + ((nChanNum + 1) & 15);
		pc->nBank = data;
		MapBank(pc);
		UpdateEndBuffer(pc);
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
		CalcAdvance(pc);

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
		UpdateEndBuffer(pc);
		break;
	}
	case 5: 
	{										// Set sample end offset
		pc->nEnd = data << 12;
		UpdateEndBuffer(pc);
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
				UpdateEndBuffer(pc);
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

void QSound_Update(u8 chipID, s32** buffer, u32 length)
{
	// Go through all channels
	for (INT32 c = 0; c < 16; c++)
	{
		// If the channel is playing, add the samples to the buffer
		if (QChan[c].bKey)
		{
			INT32 VolL = (QChan[c].nMasterVolume * QChan[c].nVolume[0]) >> 11;
			INT32 VolR = (QChan[c].nMasterVolume * QChan[c].nVolume[1]) >> 11;
			s32 *lBuf = buffer[0];
			s32 *rBuf = buffer[1];
			INT32 i = length;

			// handle 1st sample
			if (QChan[c].bKey & 2)
			{
				while (QChan[c].nPos < 0x1000 && i)
				{
					INT32 p = QChan[c].nPlayStart >> 12;
					INT32 s = INTERPOLATE4PS_CUSTOM(QChan[c].nPos, 0, QChan[c].PlayBank[p + 0], QChan[c].PlayBank[p + 1], QChan[c].PlayBank[p + 2], 256);

					*(lBuf++) += ((s * VolL) >> 8);
					*(rBuf++) += ((s * VolR) >> 8);

					QChan[c].nPos += QChan[c].nAdvance;				// increment sample position based on pitch
					i--;
				}
				if (i > 0)
				{
					QChan[c].bKey &= ~2;
					QChan[c].nPos = (QChan[c].nPos & 0x0FFF) + QChan[c].nPlayStart;
				}
			}

			while (i > 0)
			{
				INT32 s, p;

				// Check for end of sample
				if (QChan[c].nPos >= (QChan[c].nEnd - 0x3000))
				{
					if (QChan[c].nPos < QChan[c].nEnd)
					{
						INT32 nIndex = 4 - ((QChan[c].nEnd - QChan[c].nPos) >> 12);
						s = INTERPOLATE4PS_CUSTOM((QChan[c].nPos) & ((1 << 12) - 1),
							QChan[c].nEndBuffer[nIndex + 0],
							QChan[c].nEndBuffer[nIndex + 1],
							QChan[c].nEndBuffer[nIndex + 2],
							QChan[c].nEndBuffer[nIndex + 3],
							256);
					}
					else
					{
						if (QChan[c].nLoop)
						{					// Loop sample
							if (QChan[c].nLoop <= 0x1000)
							{		// Don't play, but leave bKey on
								QChan[c].nPos = QChan[c].nEnd - 0x1000;
								break;
							}
							QChan[c].nPos -= QChan[c].nLoop;
							continue;
						}
						else
						{
							QChan[c].bKey = 0;					// Stop playing
							break;
						}
					}
				}
				else
				{
					p = (QChan[c].nPos >> 12) & 0xFFFF;
					s = INTERPOLATE4PS_CUSTOM((QChan[c].nPos) & ((1 << 12) - 1), 
						QChan[c].PlayBank[p + 0],
						QChan[c].PlayBank[p + 1],
						QChan[c].PlayBank[p + 2],
						QChan[c].PlayBank[p + 3],
						256);
				}

				// Add to the sound currently in the buffer
				*(lBuf++) += ((s * VolL) >> 8);
				*(rBuf++) += ((s * VolR) >> 8);

				QChan[c].nPos += QChan[c].nAdvance;				// increment sample position based on pitch
				i--;
			}
		}
	}
}

void QSound_SetROM(u8 chipID, u32 totalROMSize, u32 startAddress, u8 *rom, u32 nLen)
{
	if(nCpsQSamLen != totalROMSize)
	{
		if (CpsQSam)
		{
			u8* newRom = malloc(totalROMSize);

			memcpy(newRom, CpsQSam, totalROMSize);
			free(CpsQSam);

			CpsQSam = newRom;
		}
		else
		{
			CpsQSam = malloc(totalROMSize);
		}

		nCpsQSamLen = totalROMSize;
	}

	memcpy(&CpsQSam[startAddress], rom, nLen);
}

#endif
