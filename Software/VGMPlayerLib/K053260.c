#include "K053260.h"

/* 2004-02-28: Fixed ppcm decoding. Games sound much better now.*/

#define BASE_SHIFT	16
#define MAXOUT 0x7fff
#define MINOUT -0x8000

static u32 nUpdateStep;

typedef struct{
	u32		rate;
	u32		size;
	u32		start;
	u32		bank;
	u32		volume;
	s32		Play;
	u32		pan;
	u32		pos;
	s32		loop;
	s32		ppcm; /* packed PCM ( 4 bit signed ) */
	s32		ppcm_data;
}Channel;

typedef struct {
	s32				mode;
	s32				regs[0x30];
	u32				*delta_table;
	Channel			channels[4];
	ROM*			rom;
	/*
	u8				*rom;
	s32				rom_size;
	*/
	u32				channel_enabled;
	u32				channel_count;
}K053260;

#define K053260_CHIPS_COUNT 2
K053260 k053260Chips[K053260_CHIPS_COUNT];

s32 limit(s32 val, s32 max, s32 min) {
	if (val > max)
		val = max;
	else if (val < min)
		val = min;

	return val;
}

static void InitDeltaTable(u8 chipID, s32 rate, s32 clock) {
	s32		i;
	f64	base = (f64)rate;
	f64	max = (f64)(clock); /* Hz */
	u32 val;
	K053260* ic = &k053260Chips[chipID];

	for (i = 0; i < 0x1000; i++) {
		f64 v = (f64)(0x1000 - i);
		f64 target = (max) / v;
		f64 fixed = (f64)(1 << BASE_SHIFT);

		if (target && base) {
			target = fixed / (base / target);
			val = (u32)target;
			if (val == 0)
				val = 1;
		}
		else
			val = 1;

		ic->delta_table[i] = val;
	}
}

void check_bounds(u8 chipID, s32 channel)
{
	K053260* ic = &k053260Chips[chipID];

	u32 channel_start = (ic->channels[channel].bank << 16) + ic->channels[channel].start;
	u32 channel_end = channel_start + ic->channels[channel].size - 1;

	if (channel_start > ROM_getTotalSize(ic->rom))
	{
		ic->channels[channel].Play = 0;

		return;
	}

	if (channel_end > ROM_getTotalSize(ic->rom))
	{
		ic->channels[channel].size = ROM_getTotalSize(ic->rom) - channel_start;
	}
}

s32 K053260_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	K053260* ic = &k053260Chips[chipID];
	memset(ic, 0, sizeof(K053260));

	s32 rate = clock / 32;
	s32 i;

	nUpdateStep = (s32)(((f32)rate / sampleRate) * 32768);

	ic->mode = 0;
	ic->rom = 0;

	K053260_Reset(chipID);

	for (i = 0; i < 0x30; i++)
		ic->regs[i] = 0;

	ic->delta_table = (u32*)malloc(0x1000 * sizeof(u32));

	InitDeltaTable(chipID, rate, clock);

	/* setup SH1 timer if necessary */
	//	if ( ic->intf->irq )
	//		timer_pulse( attotime_mul(ATTOTIME_IN_HZ(clock), 32), NULL, 0, ic->intf->irq );

	return -1;
}

void K053260_Shutdown(u8 chipID)
{
	K053260* ic = &k053260Chips[chipID];

	if (ic->delta_table)
	{
		free(ic->delta_table);
		ic->delta_table = 0;
	}

	/*
	if (ic->rom)
	{
		free(ic->rom);
		ic->rom = 0;
	}
	*/

	nUpdateStep = 0;
}

void K053260_Reset(u8 chipID)
{
	K053260* ic = &k053260Chips[chipID];

	for (s32 i = 0; i < 4; i++)
	{
		ic->channels[i].rate = 0;
		ic->channels[i].size = 0;
		ic->channels[i].start = 0;
		ic->channels[i].bank = 0;
		ic->channels[i].volume = 0;
		ic->channels[i].Play = 0;
		ic->channels[i].pan = 0;
		ic->channels[i].pos = 0;
		ic->channels[i].loop = 0;
		ic->channels[i].ppcm = 0;
		ic->channels[i].ppcm_data = 0;
	}

	ic->channel_enabled = 0xffffffff;
	ic->channel_count = 4;
}

void K053260_WriteRegister(u8 chipID, u32 address, u8 data)
{
	s32 i, t;
	s32 r = address;
	s32 v = data;
	K053260* ic = &k053260Chips[chipID];

	if (r > 0x2f) {
		return;
	}

	/* before we update the regs, we need to check for a latched reg */
	if (r == 0x28) {
		t = ic->regs[r] ^ v;

		for (i = 0; i < 4; i++) {
			if (t & (1 << i)) {
				if (v & (1 << i)) {
					ic->channels[i].Play = 1;
					ic->channels[i].pos = 0;
					ic->channels[i].ppcm_data = 0;
					check_bounds(chipID, i);
				}
				else
					ic->channels[i].Play = 0;
			}
		}

		ic->regs[r] = v;
		return;
	}

	/* update regs */
	ic->regs[r] = v;

	/* communication registers */
	if (r < 8)
		return;

	/* channel setup */
	if (r < 0x28) {
		s32 channel = (r - 8) / 8;

		switch ((r - 8) & 0x07) {
		case 0: /* sample rate low */
			ic->channels[channel].rate &= 0x0f00;
			ic->channels[channel].rate |= v;
			break;

		case 1: /* sample rate high */
			ic->channels[channel].rate &= 0x00ff;
			ic->channels[channel].rate |= (v & 0x0f) << 8;
			break;

		case 2: /* size low */
			ic->channels[channel].size &= 0xff00;
			ic->channels[channel].size |= v;
			break;

		case 3: /* size high */
			ic->channels[channel].size &= 0x00ff;
			ic->channels[channel].size |= v << 8;
			break;

		case 4: /* start low */
			ic->channels[channel].start &= 0xff00;
			ic->channels[channel].start |= v;
			break;

		case 5: /* start high */
			ic->channels[channel].start &= 0x00ff;
			ic->channels[channel].start |= v << 8;
			break;

		case 6: /* bank */
			ic->channels[channel].bank = v & 0xff;
			break;

		case 7: /* volume is 7 bits. Convert to 8 bits now. */
			ic->channels[channel].volume = ((v & 0x7f) << 1) | (v & 1);
			break;
		}

		return;
	}

	switch (r) {
	case 0x2a: /* loop, ppcm */
		for (i = 0; i < 4; i++)
			ic->channels[i].loop = (v & (1 << i)) != 0;

		for (i = 4; i < 8; i++)
			ic->channels[i - 4].ppcm = (v & (1 << i)) != 0;
		break;

	case 0x2c: /* pan */
		ic->channels[0].pan = v & 7;
		ic->channels[1].pan = (v >> 3) & 7;
		break;

	case 0x2d: /* more pan */
		ic->channels[2].pan = v & 7;
		ic->channels[3].pan = (v >> 3) & 7;
		break;

	case 0x2f: /* control */
		ic->mode = v & 7;
		/* bit 0 = read ROM */
		/* bit 1 = enable sound output */
		/* bit 2 = unknown */
		break;
	}
}

inline s8 K053260_GetSample(u8 chipID, u32 address)
{
	K053260* ic = &k053260Chips[chipID];

	return *(s8*)ROM_getPtr(ic->rom, address);
}

u8 K053260_ReadRegister(u8 chipID, u32 address)
{
	K053260* ic = &k053260Chips[chipID];

	switch (address) {
	case 0x29: /* channel status */
	{
		s32 i, status = 0;

		for (i = 0; i < 4; i++)
			status |= ic->channels[i].Play << i;

		return status;
	}
	break;

	case 0x2e: /* read rom */
		if (ic->mode & 1) {
			u32 offs = ic->channels[0].start + (ic->channels[0].pos >> BASE_SHIFT) + (ic->channels[0].bank << 16);

			ic->channels[0].pos += (1 << 16);

			if (offs > ROM_getTotalSize(ic->rom)) {

				return 0;
			}

			return K053260_GetSample(chipID, offs);
		}
		break;
	}

	return ic->regs[address];
}

void K053260_Update(u8 chipID, s32** buffer, u32 length)
{
	static const s8 dpcmcnv[] = { 0,1,2,4,8,16,32,64, -128, -64, -32, -16, -8, -4, -2, -1 };

	s32 lvol[4], rvol[4], Play[4], loop[4], ppcm[4];
	u32 romStartAddr[4];
	u32 delta[4], end[4], pos[4];
	s8 ppcm_data[4];
	s8 d;
	K053260* ic = &k053260Chips[chipID];

	/* precache some values */
	for (int i = 0; i < ic->channel_count; i++)
	{
		u32 address = ic->channels[i].start + (ic->channels[i].bank << 16) + 1;
		
		romStartAddr[i] = address;

		delta[i] = (ic->delta_table[ic->channels[i].rate] * nUpdateStep) >> 15;
		lvol[i] = ic->channels[i].volume * ic->channels[i].pan;
		rvol[i] = ic->channels[i].volume * (8 - ic->channels[i].pan);
		end[i] = ic->channels[i].size - 1;
		pos[i] = ic->channels[i].pos;
		Play[i] = ic->channels[i].Play;
		loop[i] = ic->channels[i].loop;
		ppcm[i] = ic->channels[i].ppcm;
		ppcm_data[i] = ic->channels[i].ppcm_data;
		if (ppcm[i])
			delta[i] /= 2;
	}

	for (u32 sample = 0; sample < length; sample++) 
	{
		for (int ch = 0; ch < ic->channel_count; ch++)
		{
			if ((ic->channel_enabled & (1 << (ch))) == 0)
			{
				buffer[((ch) << 1) + 0][sample] = 0;
				buffer[((ch) << 1) + 1][sample] = 0;
			}
			else
			{
				/* see if the voice is on */
				if (Play[ch]) {
					/* see if we're done */
					if ((pos[ch] >> BASE_SHIFT) >= end[ch])
					{

						ppcm_data[ch] = 0;
						if (loop[ch])
							pos[ch] = 0;
						else {
							Play[ch] = 0;
							continue;
						}
					}

					if (ppcm[ch]) { /* Packed PCM */
								   /* we only update the signal if we're starting or a real sound sample has gone by */
								   /* this is all due to the dynamic sample rate convertion */
						if (pos[ch] == 0 || ((pos[ch] ^ (pos[ch] - delta[ch])) & 0x8000) == 0x8000)

						{
							s32 newdata;
							if (pos[ch] & 0x8000) {
								newdata = (K053260_GetSample(chipID, ((romStartAddr[ch] + (pos[ch] >> BASE_SHIFT)))) >> 4) & 0x0f;
							}
							else
							{
								newdata = (K053260_GetSample(chipID, ((romStartAddr[ch] + (pos[ch] >> BASE_SHIFT))))) & 0x0f; /*low nybble*/
							}

							ppcm_data[ch] += dpcmcnv[newdata];
						}

						d = ppcm_data[ch];

						pos[ch] += delta[ch];
					}
					else
					{ /* PCM */
						/* PCM */
						d = K053260_GetSample(chipID, romStartAddr[ch] + (pos[ch] >> BASE_SHIFT));
						pos[ch] += delta[ch];
					}

					buffer[((ch) << 1) + 0][sample] = 0;
					buffer[((ch) << 1) + 1][sample] = 0;
					if (ic->mode & 2)
					{
						buffer[((ch) << 1) + 0][sample] = ((d * lvol[ch]) >> 2);
						buffer[((ch) << 1) + 1][sample] = ((d * rvol[ch]) >> 2);
					}
				}
			}
		}
	}

	/* update the regs now */
	for (int i = 0; i < 4; i++) {
		ic->channels[i].pos = pos[i];
		ic->channels[i].Play = Play[i];
		ic->channels[i].ppcm_data = ppcm_data[i];
	}
}

/*
void K053260_SetROM(u8 chipID, u32 totalROMSize, u32 startAddress, u8 *rom, u32 nLen)
{
	K053260* ic = &k053260Chips[chipID];

	if (ic->rom_size != totalROMSize)
	{
		if (ic->rom)
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

		ic->rom_size = totalROMSize;
	}

	memcpy(&ic->rom[startAddress], rom, nLen);
}
*/

void K053260_SetROM(u8 chipID, ROM* rom)
{
	K053260* ic = &k053260Chips[chipID];

	ic->rom = rom;
}

void K053260_SetChannelEnable(u8 chipID, u8 channel, u8 enable)
{
	K053260* ic = &k053260Chips[chipID];

	if (enable)
		ic->channel_enabled |= (1 << channel);
	else
		ic->channel_enabled &= (~(1 << channel));
}

u8 K053260_GetChannelEnable(u8 chipID, u8 channel)
{
	K053260* ic = &k053260Chips[chipID];

	return (ic->channel_enabled & (1 << channel)) != 0;
}

u32 K053260_GetChannelCount(u8 chipID)
{
	K053260* ic = &k053260Chips[chipID];

	return ic->channel_count;
}