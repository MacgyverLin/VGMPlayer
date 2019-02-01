#include "segapcm.h"

#define MAX_CHIPS		2

typedef struct
{
	u8 ram[0x800];
	u8 low[16];
	s32 bankshift;
	s32 bankmask;
	s32 UpdateStep;
	ROM* rom;
}SEGAPCM;

static SEGAPCM segapcm[MAX_CHIPS];

s32 SEGAPCM_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	SEGAPCM* ic = &segapcm[chipID];

	memset(ic, 0, sizeof(SEGAPCM));
	memset(ic->ram, 0xff, 0x800);

	f32 Rate = (f32)clock / 128 / 44100;
	ic->UpdateStep = (INT32)(Rate * 0x10000);

	return 0;
}

void SEGAPCM_Shutdown(u8 chipID)
{
	SEGAPCM* ic = &segapcm[chipID];
}

void SEGAPCM_Reset(u8 chipID)
{
}

void SEGAPCM_WriteRegister(u8 chipID, u32 address, u32 data)
{
	SEGAPCM* ic = &segapcm[chipID];

	ic->ram[address & 0x07ff] = data;
}

u8 SEGAPCM_ReadRegister(u8 chipID, u32 address)
{
	SEGAPCM* ic = &segapcm[chipID];

	return ic->ram[address & 0x07ff];
}

#if 0
void SEGAPCM_Update(u8 chipID, s32** buffer, u32 length)
{
	INT32 Channel;
	SEGAPCM* ic = &segapcm[chipID];

	//u32 bank = BANK_256;
	//u32 bank = BANK_512;
	u32 bank = BANK_12M;
	//u32 bank = BANK_12M;
	ic->bankshift = bank;
	s32 Mask;
	s32 RomMask;
	Mask = bank >> 16;
	if (!Mask)
		Mask = BANK_MASK7 >> 16;

	// todo
	for (RomMask = 1; RomMask < ic->rom->length; RomMask *= 2)
	{
	}
	RomMask--;
	ic->bankmask = Mask & (RomMask >> ic->bankshift);

	s32 *lBuf = buffer[0];
	s32 *rBuf = buffer[1];

	INT32 i;
	for (i = 0; i < length; i++)
	{
		s32 l = 0;
		s32 r = 0;
		for (Channel = 0; Channel < 16; Channel++)
		{
			UINT8 *Regs = ic->ram + 8 * Channel;
			if (!(Regs[0x86] & 1))
			{
				const UINT8 *Rom = ic->rom->buffer + ((Regs[0x86] & ic->bankmask) << ic->bankshift);
				UINT32 Addr = (Regs[0x85] << 16) | (Regs[0x84] << 8) | ic->low[Channel];
				UINT32 Loop = (Regs[0x05] << 16) | (Regs[0x04] << 8);
				UINT8 End = Regs[6] + 1;

				INT8 v = 0;

				if ((Addr >> 16) == End)
				{
					if (Regs[0x86] & 2)
					{
						Regs[0x86] |= 1;
						break;
					}
					else
					{
						Addr = Loop;
					}
				}

				v = Rom[Addr >> 8] - 0x80;

				l += v * Regs[2];
				r += v * Regs[3];
				Addr = (Addr + ((Regs[7] * ic->UpdateStep) >> 16)) & 0xffffff;

				Regs[0x84] = Addr >> 8;
				Regs[0x85] = Addr >> 16;
				ic->low[Channel] = Regs[0x86] & 1 ? 0 : Addr;
			}
		}
		lBuf[i] += l;
		rBuf[i] += r;
	}
}
#else
void SEGAPCM_Update(u8 chipID, s32** buffer, u32 length)
{
	INT32 Channel;
	SEGAPCM* ic = &segapcm[chipID];

	u32 bank = BANK_12M;
	ic->bankshift = bank;
	s32 Mask;
	s32 RomMask;
	Mask = bank >> 16;
	if (!Mask)
		Mask = BANK_MASK7 >> 16;

	// todo
	for (RomMask = 1; RomMask < ic->rom->length; RomMask *= 2)
	{
	}
	RomMask--;
	ic->bankmask = Mask & (RomMask >> ic->bankshift);

	s32 *lBuf = buffer[0];
	s32 *rBuf = buffer[1];

	for (Channel = 0; Channel < 16; Channel++)
	{
		UINT8 *Regs = ic->ram + 8 * Channel;
		if (!(Regs[0x86] & 1))
		{
			const UINT8 *Rom = ic->rom->buffer + ((Regs[0x86] & ic->bankmask) << ic->bankshift);
			UINT32 Addr = (Regs[0x85] << 16) | (Regs[0x84] << 8) | ic->low[Channel];
			UINT32 Loop = (Regs[0x05] << 16) | (Regs[0x04] << 8);
			UINT8 End = Regs[6] + 1;
			INT32 i;

			for (i = 0; i < length; i++)
			{
				INT8 v = 0;

				if ((Addr >> 16) == End)
				{
					if (Regs[0x86] & 2)
					{
						Regs[0x86] |= 1;
						break;
					}
					else
					{
						Addr = Loop;
					}
				}

				v = Rom[Addr >> 8] - 0x80;

				lBuf[i] += v * Regs[2] * 2;
				rBuf[i] += v * Regs[3] * 2;
				Addr = (Addr + ((Regs[7] * ic->UpdateStep) >> 16)) & 0xffffff;
			}

			Regs[0x84] = Addr >> 8;
			Regs[0x85] = Addr >> 16;
			ic->low[Channel] = Regs[0x86] & 1 ? 0 : Addr;
		}
	}
}
#endif

void SEGAPCM_SetROM(u8 chipID, ROM* rom)
{
	SEGAPCM* ic = &segapcm[chipID];

	ic->rom = rom;
}

#if 0

static INT32 *Left[MAX_CHIPS] = { NULL, };
static INT32 *Right[MAX_CHIPS] = { NULL, };

static INT32 nNumChips = 0;

static void SegaPCMUpdateOne(INT32 nChip, INT32 nLength)
{
	INT32 Channel;

	memset(Left[nChip], 0, nLength * sizeof(INT32));
	memset(Right[nChip], 0, nLength * sizeof(INT32));

	for (Channel = 0; Channel < 16; Channel++) {
		UINT8 *Regs = Chip[nChip]->ram + 8 * Channel;
		if (!(Regs[0x86] & 1)) {
			const UINT8 *Rom = Chip[nChip]->rom + ((Regs[0x86] & Chip[nChip]->bankmask) << Chip[nChip]->bankshift);
			UINT32 Addr = (Regs[0x85] << 16) | (Regs[0x84] << 8) | Chip[nChip]->low[Channel];
			UINT32 Loop = (Regs[0x05] << 16) | (Regs[0x04] << 8);
			UINT8 End = Regs[6] + 1;
			INT32 i;

			for (i = 0; i < nLength; i++) {
				INT8 v = 0;

				if ((Addr >> 16) == End) {
					if (Regs[0x86] & 2) {
						Regs[0x86] |= 1;
						break;
					}
					else {
						Addr = Loop;
					}
				}

				v = Rom[Addr >> 8] - 0x80;

				Left[nChip][i] += v * Regs[2];
				Right[nChip][i] += v * Regs[3];
				Addr = (Addr + ((Regs[7] * Chip[nChip]->UpdateStep) >> 16)) & 0xffffff;
			}

			Regs[0x84] = Addr >> 8;
			Regs[0x85] = Addr >> 16;
			Chip[nChip]->low[Channel] = Regs[0x86] & 1 ? 0 : Addr;
		}
	}
}

void SegaPCMUpdate(INT16* pSoundBuf, INT32 nLength)
{
#if defined FBA_DEBUG
	if (!DebugSnd_SegaPCMInitted) bprintf(PRINT_ERROR, _T("SegaPCMUpdate called without init\n"));
#endif

	for (INT32 i = 0; i < nNumChips + 1; i++) {
		SegaPCMUpdateOne(i, nLength);
	}

	for (INT32 i = 0; i < nLength; i++) {
		INT32 nLeftSample = 0;
		INT32 nRightSample = 0;

		if ((Chip[0]->OutputDir[BURN_SND_SEGAPCM_ROUTE_1] & BURN_SND_ROUTE_LEFT) == BURN_SND_ROUTE_LEFT) {
			nLeftSample += (INT32)(Left[0][i] * Chip[0]->Volume[BURN_SND_SEGAPCM_ROUTE_1]);
		}
		if ((Chip[0]->OutputDir[BURN_SND_SEGAPCM_ROUTE_1] & BURN_SND_ROUTE_RIGHT) == BURN_SND_ROUTE_RIGHT) {
			nRightSample += (INT32)(Left[0][i] * Chip[0]->Volume[BURN_SND_SEGAPCM_ROUTE_1]);
		}

		if ((Chip[0]->OutputDir[BURN_SND_SEGAPCM_ROUTE_2] & BURN_SND_ROUTE_LEFT) == BURN_SND_ROUTE_LEFT) {
			nLeftSample += (INT32)(Right[0][i] * Chip[0]->Volume[BURN_SND_SEGAPCM_ROUTE_2]);
		}
		if ((Chip[0]->OutputDir[BURN_SND_SEGAPCM_ROUTE_2] & BURN_SND_ROUTE_RIGHT) == BURN_SND_ROUTE_RIGHT) {
			nRightSample += (INT32)(Right[0][i] * Chip[0]->Volume[BURN_SND_SEGAPCM_ROUTE_2]);
		}

		nLeftSample = BURN_SND_CLIP(nLeftSample);
		nRightSample = BURN_SND_CLIP(nRightSample);

		if (nNumChips >= 1) {
			if ((Chip[1]->OutputDir[BURN_SND_SEGAPCM_ROUTE_1] & BURN_SND_ROUTE_LEFT) == BURN_SND_ROUTE_LEFT) {
				nLeftSample += (INT32)(Left[1][i] * Chip[1]->Volume[BURN_SND_SEGAPCM_ROUTE_1]);
			}
			if ((Chip[1]->OutputDir[BURN_SND_SEGAPCM_ROUTE_1] & BURN_SND_ROUTE_RIGHT) == BURN_SND_ROUTE_RIGHT) {
				nRightSample += (INT32)(Left[1][i] * Chip[1]->Volume[BURN_SND_SEGAPCM_ROUTE_1]);
			}

			if ((Chip[1]->OutputDir[BURN_SND_SEGAPCM_ROUTE_2] & BURN_SND_ROUTE_LEFT) == BURN_SND_ROUTE_LEFT) {
				nLeftSample += (INT32)(Right[1][i] * Chip[1]->Volume[BURN_SND_SEGAPCM_ROUTE_2]);
			}
			if ((Chip[1]->OutputDir[BURN_SND_SEGAPCM_ROUTE_2] & BURN_SND_ROUTE_RIGHT) == BURN_SND_ROUTE_RIGHT) {
				nRightSample += (INT32)(Right[1][i] * Chip[1]->Volume[BURN_SND_SEGAPCM_ROUTE_2]);
			}

			nLeftSample = BURN_SND_CLIP(nLeftSample);
			nRightSample = BURN_SND_CLIP(nRightSample);
		}

		pSoundBuf[0] = BURN_SND_CLIP(pSoundBuf[0] + nLeftSample);
		pSoundBuf[1] = BURN_SND_CLIP(pSoundBuf[1] + nRightSample);
		pSoundBuf += 2;
	}
}

void SegaPCMInit(INT32 nChip, INT32 clock, INT32 bank, UINT8 *pPCMData, INT32 PCMDataSize)
{
	INT32 Mask, RomMask;

	Chip[nChip] = (struct segapcm*)BurnMalloc(sizeof(*Chip[nChip]));
	memset(Chip[nChip], 0, sizeof(*Chip[nChip]));

	Chip[nChip]->rom = pPCMData;

	memset(Chip[nChip]->ram, 0xff, 0x800);

	Left[nChip] = (INT32*)BurnMalloc(nBurnSoundLen * sizeof(INT32));
	Right[nChip] = (INT32*)BurnMalloc(nBurnSoundLen * sizeof(INT32));

	Chip[nChip]->bankshift = bank;
	Mask = bank >> 16;
	if (!Mask)
		Mask = BANK_MASK7 >> 16;

	for (RomMask = 1; RomMask < PCMDataSize; RomMask *= 2) {}
	RomMask--;

	Chip[nChip]->bankmask = Mask & (RomMask >> Chip[nChip]->bankshift);

	double Rate = (double)clock / 128 / nBurnSoundRate;
	Chip[nChip]->UpdateStep = (INT32)(Rate * 0x10000);

	Chip[nChip]->Volume[BURN_SND_SEGAPCM_ROUTE_1] = 1.00;
	Chip[nChip]->Volume[BURN_SND_SEGAPCM_ROUTE_2] = 1.00;
	Chip[nChip]->OutputDir[BURN_SND_SEGAPCM_ROUTE_1] = BURN_SND_ROUTE_LEFT;
	Chip[nChip]->OutputDir[BURN_SND_SEGAPCM_ROUTE_2] = BURN_SND_ROUTE_RIGHT;

	nNumChips = nChip;

	DebugSnd_SegaPCMInitted = 1;
}

void SegaPCMSetRoute(INT32 nChip, INT32 nIndex, double nVolume, INT32 nRouteDir)
{
#if defined FBA_DEBUG
	if (!DebugSnd_SegaPCMInitted) bprintf(PRINT_ERROR, _T("SegaPCMSetRoute called without init\n"));
	if (nIndex < 0 || nIndex > 1) bprintf(PRINT_ERROR, _T("SegaPCMSetRoute called with invalid index %i\n"), nIndex);
	if (nChip > nNumChips) bprintf(PRINT_ERROR, _T("SegaPCMSetRoute called with invalid chip %i\n"), nChip);
#endif

	Chip[nChip]->Volume[nIndex] = nVolume;
	Chip[nChip]->OutputDir[nIndex] = nRouteDir;
}

void SegaPCMExit()
{
#if defined FBA_DEBUG
	if (!DebugSnd_SegaPCMInitted) bprintf(PRINT_ERROR, _T("SegaPCMExit called without init\n"));
#endif

	if (!DebugSnd_SegaPCMInitted) return;

	for (INT32 i = 0; i < nNumChips + 1; i++) {
		BurnFree(Chip[i]);
		BurnFree(Left[i]);
		BurnFree(Right[i]);
	}

	nNumChips = 0;

	DebugSnd_SegaPCMInitted = 0;
}

void SegaPCMScan(INT32 nAction, INT32 *pnMin)
{
#if defined FBA_DEBUG
	if (!DebugSnd_SegaPCMInitted) bprintf(PRINT_ERROR, _T("SegaPCMScan called without init\n"));
#endif

	struct BurnArea ba;

	if (pnMin != NULL) {
		*pnMin = 0x029719;
	}

	if (nAction & ACB_DRIVER_DATA) {
		for (INT32 i = 0; i < nNumChips + 1; i++) {
			ScanVar(Chip[i]->low, sizeof(Chip[i]->low), "SegaPCMlow");

			memset(&ba, 0, sizeof(ba));
			ba.Data = Chip[i]->ram;
			ba.nLen = sizeof(Chip[i]->ram);
			ba.szName = "SegaPCMRAM";
			BurnAcb(&ba);
		}
	}
}

UINT8 SegaPCMRead(INT32 nChip, UINT32 Offset)
{
#if defined FBA_DEBUG
	if (!DebugSnd_SegaPCMInitted) bprintf(PRINT_ERROR, _T("SegaPCMRead called without init\n"));
	if (nChip > nNumChips) bprintf(PRINT_ERROR, _T("SegaPCMRead called with invalid chip %i\n"), nChip);
#endif

	return Chip[nChip]->ram[Offset & 0x07ff];
}

void SegaPCMWrite(INT32 nChip, UINT32 Offset, UINT8 Data)
{
#if defined FBA_DEBUG
	if (!DebugSnd_SegaPCMInitted) bprintf(PRINT_ERROR, _T("SegaPCMWrite called without init\n"));
	if (nChip > nNumChips) bprintf(PRINT_ERROR, _T("SegaPCMWrite called with invalid chip %i\n"), nChip);
#endif

	Chip[nChip]->ram[Offset & 0x07ff] = Data;
}
#endif