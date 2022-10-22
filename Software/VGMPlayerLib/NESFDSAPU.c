#include "NESFDSAPU.h"

typedef struct
{
	u8	reg[0x80];
	u8	volenv_mode;		// Volume Envelope
	u8	volenv_gain;
	u8	volenv_decay;
	f32	volenv_phaseacc;

	u8	swpenv_mode;		// Sweep Envelope
	u8	swpenv_gain;
	u8	swpenv_decay;
	f32	swpenv_phaseacc;

	// For envelope unit
	u8	envelope_enable;	// $4083 bit6
	u8	envelope_speed;		// $408A
								// For $4089
	u8	wave_setup;			// bit7
	s32	master_volume;		// bit1-0
								// For Main unit

	s32	main_wavetable[64];
	u8	main_enable;
	s32	main_frequency;
	s32	main_addr;

	// For Effector(LFO) unit
	u8	lfo_wavetable[64];
	u8	lfo_enable;			// 0:Enable 1:Wavetable setup
	s32	lfo_frequency;
	s32	lfo_addr;
	f32	lfo_phaseacc;

	// For Sweep unit
	s32	sweep_bias;

	// Misc
	s32	volume;
	s32	freq;

	u32	samplingRate;

	u32 channel_output_enabled;
	u32	channel_count;
}NESFDSAPU;

#define NESFDSAPU_CHIPS_COUNT 2
NESFDSAPU nesfdsapuChips[NESFDSAPU_CHIPS_COUNT];

s32 NESFDSAPU_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	int a = sizeof(NESFDSAPU);

	memset(ic, 0, sizeof(NESFDSAPU));
	ic->samplingRate = sampleRate;
	ic->channel_output_enabled = 0xffffffff;
	ic->channel_count = 1;

	return -1;
}

void NESFDSAPU_Shutdown(u8 chipID)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	memset(ic, 0, sizeof(NESFDSAPU));
}

void NESFDSAPU_Reset(u8 chipID)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	u32 sampleRate = ic->samplingRate;
	memset(ic, 0, sizeof(NESFDSAPU));
	ic->samplingRate = sampleRate;

	ic->channel_output_enabled = 0xffffffff;
	ic->channel_count = 1;
}

void NESFDSAPU_WriteRegister(u8 chipID, u32 addr, u32 data, s32* channel, f32* freq)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	if (addr >= 0x20 && addr <= 0x3e)
		addr += 0x4060;
	if (addr == 0x3f)
		addr = 0x4023;
	if (addr >= 0x40 && addr <= 0x7f)
		addr += 0x4000;

	if (addr < 0x4040 || addr > 0x40BF)
		return;

	ic->reg[addr - 0x4040] = data;
	if (addr >= 0x4040 && addr <= 0x407F)
	{
		if (ic->wave_setup)
		{
			ic->main_wavetable[addr - 0x4040] = 0x20 - ((s32)data & 0x3F);
		}
	}
	else
	{
		switch (addr)
		{
		case 0x4080:	// Volume Envelope
			ic->volenv_mode = data >> 6;
			if (data & 0x80)
			{
				ic->volenv_gain = data & 0x3F;
				if (!ic->main_addr)
				{
					ic->volume = (ic->volenv_gain < 0x21) ? ic->volenv_gain : 0x20;
				}
			}
			ic->volenv_decay = data & 0x3F;
			ic->volenv_phaseacc = ic->envelope_speed * (ic->volenv_decay + 1) * ic->samplingRate / (232.0f * 960.0f);
			break;

		case	0x4082:	// Main Frequency(Low)
			ic->main_frequency = (ic->main_frequency & ~0x00FF) | (s32)data;
			break;

		case	0x4083:	// Main Frequency(High)
			ic->main_enable = (~data) & (1 << 7);
			ic->envelope_enable = (~data) & (1 << 6);
			if (!ic->main_enable)
			{
				ic->main_addr = 0;
				ic->volume = (ic->volenv_gain < 0x21) ? ic->volenv_gain : 0x20;
			}
			ic->main_frequency = (ic->main_frequency & 0x00FF) | (((s32)data & 0x0F) << 8);
			break;

		case	0x4084:	// Sweep Envelope
			ic->swpenv_mode = data >> 6;
			if (data & 0x80) {
				ic->swpenv_gain = data & 0x3F;
			}
			ic->swpenv_decay = data & 0x3F;
			ic->swpenv_phaseacc = ic->envelope_speed * (ic->swpenv_decay + 1) * ic->samplingRate / (232.0f * 960.0f);			break;

		case	0x4085:	// Sweep Bias
			if (data & 0x40)
				ic->sweep_bias = (data & 0x3f) - 0x40;
			else
				ic->sweep_bias = data & 0x3f;
			ic->lfo_addr = 0;
			break;

		case	0x4086:	// Effector(LFO) Frequency(Low)
			ic->lfo_frequency = (ic->lfo_frequency & (~0x00FF)) | (s32)data;
			break;

		case	0x4087:	// Effector(LFO) Frequency(High)
			ic->lfo_enable = (~data & 0x80);
			ic->lfo_frequency = (ic->lfo_frequency & 0x00FF) | (((s32)data & 0x0F) << 8);
			break;

		case	0x4088:	// Effector(LFO) wavetable
			if (!ic->lfo_enable)
			{
				// FIFO?
				for (s32 i = 0; i < 31; i++) {
					ic->lfo_wavetable[i * 2 + 0] = ic->lfo_wavetable[(i + 1) * 2 + 0];
					ic->lfo_wavetable[i * 2 + 1] = ic->lfo_wavetable[(i + 1) * 2 + 1];
				}
				ic->lfo_wavetable[31 * 2 + 0] = data & 0x07;
				ic->lfo_wavetable[31 * 2 + 1] = data & 0x07;
			}
			break;

		case	0x4089:	// Sound control
		{
			s32	tbl[] = { 30, 20, 15, 12 };
			ic->master_volume = tbl[data & 3];
			ic->wave_setup = data & 0x80;
		}
		break;

		case	0x408A:	// Sound control 2
			ic->envelope_speed = data;
			break;

		default:
			break;
		}
	}
}

u8 NESFDSAPU_ReadRegister(u8 chipID, u32 address)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	u8 data = address >> 8;

	if (address >= 0x4040 && address <= 0x407F)
	{
		data = ic->main_wavetable[address & 0x3F] | 0x40;
	}
	else if (address == 0x4090)
	{
		data = (ic->volenv_gain & 0x3F) | 0x40;
	}
	else if (address == 0x4092)
	{
		data = (ic->swpenv_gain & 0x3F) | 0x40;
	}

	return data;
}

void NESFDSAPU_Update(u8 chipID, s32** buffer, u32 length)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	for (u32 sample = 0; sample < length; sample++)
	{
		// Envelope unit
		if (ic->envelope_enable && ic->envelope_speed)
		{
			// Volume envelope
			if (ic->volenv_mode < 2)
			{
				f32 decay = (ic->envelope_speed * (ic->volenv_decay + 1) * ic->samplingRate) / (232.0f * 960.0f);
				ic->volenv_phaseacc -= 1.0;
				while (ic->volenv_phaseacc < 0.0)
				{
					ic->volenv_phaseacc += decay;
					if (ic->volenv_mode == 0)
					{
						if (ic->volenv_gain)
							ic->volenv_gain--;
					}
					else if (ic->volenv_mode == 1)
					{
						if (ic->volenv_gain < 0x20)
							ic->volenv_gain++;
					}
				}
			}

			// Sweep envelope
			if (ic->swpenv_mode < 2)
			{
				f32 decay = (ic->envelope_speed * (ic->swpenv_decay + 1) * ic->samplingRate) / (232.0f * 960.0f);
				ic->swpenv_phaseacc -= 1.0;
				while (ic->swpenv_phaseacc < 0.0)
				{
					ic->swpenv_phaseacc += decay;
					if (ic->swpenv_mode == 0)
					{
						if (ic->swpenv_gain)
							ic->swpenv_gain--;
					}
					else if (ic->swpenv_mode == 1)
					{
						if (ic->swpenv_gain < 0x20)
							ic->swpenv_gain++;
					}
				}
			}
		}

		// Effector(LFO) unit
		s32 sub_freq = 0;
		if (ic->lfo_enable && ic->envelope_speed && ic->lfo_frequency)
		{
			static s32 tbl[8] = { 0, 1, 2, 4, 0, -4, -2, -1 };
			ic->lfo_phaseacc -= (1789772.5f * (f32)(ic->lfo_frequency)) / 65536.0f;
			while (ic->lfo_phaseacc < 0.0)
			{
				ic->lfo_phaseacc += (f32)ic->samplingRate;
				if (ic->lfo_wavetable[ic->lfo_addr] == 4)
					ic->sweep_bias = 0;
				else
					ic->sweep_bias += tbl[ic->lfo_wavetable[ic->lfo_addr]];
				ic->lfo_addr = (ic->lfo_addr + 1) & 63;
			}
			if (ic->sweep_bias > 63)
				ic->sweep_bias -= 128;
			else if (ic->sweep_bias < -64)
				ic->sweep_bias += 128;
			s32 sub_multi = ic->sweep_bias * ic->swpenv_gain;
			if (sub_multi & 0x0F)
			{
				sub_multi = (sub_multi / 16);
				if (ic->sweep_bias >= 0)
					sub_multi += 2;
				else
					sub_multi -= 1;
			}
			else
			{
				sub_multi = (sub_multi / 16);
			}
			if (sub_multi > 193)
				sub_multi -= 258;
			if (sub_multi < -64)
				sub_multi += 256;
			sub_freq = (ic->main_frequency) * sub_multi / 64;
		}

		if ((ic->channel_output_enabled & (1 << (0))) == 0)
		{
			buffer[((0) << 1) + 0][sample] = 0;
			buffer[((0) << 1) + 1][sample] = 0;
		}
		else
		{
			// Main unit
			s32 output = 0;
			if (ic->main_enable && ic->main_frequency && !ic->wave_setup)
			{
				s32 freq;
				s32 main_addr_old = ic->main_addr;
				freq = (s32)((ic->main_frequency + sub_freq) * 1789772.5f / 65536.0f);
				ic->main_addr = (ic->main_addr + freq + 64 * ic->samplingRate) % (64 * ic->samplingRate);

				if (main_addr_old > ic->main_addr)
					ic->volume = (ic->volenv_gain < 0x21) ? ic->volenv_gain : 0x20;

				output = ic->main_wavetable[(ic->main_addr / ic->samplingRate) & 0x3f] * 8 * ic->volume * ic->master_volume / 30;
				if (ic->volume)
					ic->freq = freq * 4;
				else
					ic->freq = 0;
			}
			else
			{
				ic->freq = 0;
				output = 0;
			}

			buffer[((0) << 1) + 0][sample] = output;
			buffer[((0) << 1) + 1][sample] = output;
		}
	}
}

void NESFDSAPU_SetChannelEnable(u8 chipID, u8 channel, u8 enable)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	if (enable)
		ic->channel_output_enabled |= (1 << channel);
	else
		ic->channel_output_enabled &= (~(1 << channel));
}

u8 NESFDSAPU_GetChannelEnable(u8 chipID, u8 channel)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	return (ic->channel_output_enabled & (1 << channel)) != 0;
}

u32 NESFDSAPU_GetChannelCount(u8 chipID)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	return ic->channel_count;
}