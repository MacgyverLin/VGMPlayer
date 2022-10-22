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

	u32 channel_enabled;
	u32	channel_count;
}SEGAPCM;

static SEGAPCM segapcm[MAX_CHIPS];

s32 SEGAPCM_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	SEGAPCM* ic = &segapcm[chipID];

	memset(ic, 0, sizeof(SEGAPCM));
	memset(ic->ram, 0xff, 0x800);

	f32 Rate = (f32)clock / 128 / 44100;
	ic->UpdateStep = (INT32)(Rate * 0x10000);

	SEGAPCM_Reset(chipID);

	return 0;
}

void SEGAPCM_Shutdown(u8 chipID)
{
	SEGAPCM* ic = &segapcm[chipID];
}

void SEGAPCM_Reset(u8 chipID)
{
	SEGAPCM* ic = &segapcm[chipID];

	ic->channel_enabled = 0xffffffff;
	ic->channel_count = 16;
}

void SEGAPCM_WriteRegister(u8 chipID, u32 address, u32 data, s32* channel, f32* freq)
{
	SEGAPCM* ic = &segapcm[chipID];

	ic->ram[address & 0x07ff] = data;
}

u8 SEGAPCM_ReadRegister(u8 chipID, u32 address)
{
	SEGAPCM* ic = &segapcm[chipID];

	return ic->ram[address & 0x07ff];
}

void SEGAPCM_Update(u8 chipID, s32** buffer, u32 length)
{
	INT32 ch;
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

	//s32 *lBuf = buffer[0];
	//s32 *rBuf = buffer[1];

	for (ch = 0; ch < ic->channel_count; ch++)
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
			UINT8* Regs = ic->ram + 8 * ch;
			if (!(Regs[0x86] & 1))
			{
				const UINT8* Rom = ic->rom->buffer + ((Regs[0x86] & ic->bankmask) << ic->bankshift);
				UINT32 Addr = (Regs[0x85] << 16) | (Regs[0x84] << 8) | ic->low[ch];
				UINT32 Loop = (Regs[0x05] << 16) | (Regs[0x04] << 8);
				UINT8 End = Regs[6] + 1;
				INT32 sample;

				for (sample = 0; sample < length; sample++)
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


					buffer[((ch) << 1) + 0][sample] = v * Regs[2] * 2;
					buffer[((ch) << 1) + 1][sample] = v * Regs[3] * 2;
					//lBuf[i] += v * Regs[2] * 2;
					//rBuf[i] += v * Regs[3] * 2;
					Addr = (Addr + ((Regs[7] * ic->UpdateStep) >> 16)) & 0xffffff;
				}

				Regs[0x84] = Addr >> 8;
				Regs[0x85] = Addr >> 16;
				ic->low[ch] = Regs[0x86] & 1 ? 0 : Addr;
			}
		}
	}
}

void SEGAPCM_SetROM(u8 chipID, ROM* rom)
{
	SEGAPCM* ic = &segapcm[chipID];

	ic->rom = rom;
}

void SEGAPCM_SetChannelEnable(u8 chipID, u8 channel, u8 enable)
{
	SEGAPCM* ic = &segapcm[chipID];

	if (enable)
		ic->channel_enabled |= (1 << channel);
	else
		ic->channel_enabled &= (~(1 << channel));
}

u8 SEGAPCM_GetChannelEnable(u8 chipID, u8 channel)
{
	SEGAPCM* ic = &segapcm[chipID];

	return (ic->channel_enabled & (1 << channel)) != 0;
}

u32 SEGAPCM_GetChannelCount(u8 chipID)
{
	SEGAPCM* ic = &segapcm[chipID];

	return ic->channel_count;
}