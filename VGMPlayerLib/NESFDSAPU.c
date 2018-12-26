#include "NESFDSAPU.h"

typedef struct
{
	UINT8	reg[0x80];
	UINT8	volenv_mode;		// Volume Envelope
	UINT8	volenv_gain;
	UINT8	volenv_decay;
	FLOAT32	volenv_phaseacc;

	UINT8	swpenv_mode;		// Sweep Envelope
	UINT8	swpenv_gain;
	UINT8	swpenv_decay;
	FLOAT32	swpenv_phaseacc;

	// For envelope unit
	UINT8	envelope_enable;	// $4083 bit6
	UINT8	envelope_speed;		// $408A
								// For $4089
	UINT8	wave_setup;			// bit7
	INT32	master_volume;		// bit1-0
								// For Main unit

	INT32	main_wavetable[64];
	UINT8	main_enable;
	INT32	main_frequency;
	INT32	main_addr;
	
	// For Effector(LFO) unit
	UINT8	lfo_wavetable[64];
	UINT8	lfo_enable;			// 0:Enable 1:Wavetable setup
	INT32	lfo_frequency;
	INT32	lfo_addr;
	FLOAT32	lfo_phaseacc;
	
	// For Sweep unit
	INT32	sweep_bias;
	
	// Misc
	INT32	volume;
	INT32	freq;

	UINT32	samplingRate;
}NESFDSAPU;

#define NESFDSAPU_CHIPS_COUNT 2
NESFDSAPU nesfdsapuChips[NESFDSAPU_CHIPS_COUNT];

INT32 NESFDSAPU_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	int a = sizeof(NESFDSAPU);

	memset(ic, 0, sizeof(NESFDSAPU));
	ic->samplingRate = sampleRate;

	return -1;
}

void NESFDSAPU_Shutdown(UINT8 chipID)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	memset(ic, 0, sizeof(NESFDSAPU));
}

void NESFDSAPU_Reset(UINT8 chipID)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	UINT32 sampleRate = ic->samplingRate;
	memset(ic, 0, sizeof(NESFDSAPU));
	ic->samplingRate = sampleRate;
}

void NESFDSAPU_WriteRegister(UINT8 chipID, UINT32 addr, UINT8 data)
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
			ic->main_wavetable[addr - 0x4040] = 0x20 - ((INT32)data & 0x3F);
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
			ic->main_frequency = (ic->main_frequency & ~0x00FF) | (INT32)data;
			break;

		case	0x4083:	// Main Frequency(High)
			ic->main_enable = (~data)&(1 << 7);
			ic->envelope_enable = (~data)&(1 << 6);
			if (!ic->main_enable)
			{
				ic->main_addr = 0;
				ic->volume = (ic->volenv_gain < 0x21) ? ic->volenv_gain : 0x20;
			}
			ic->main_frequency = (ic->main_frequency & 0x00FF) | (((INT32)data & 0x0F) << 8);
			break;

		case	0x4084:	// Sweep Envelope
			ic->swpenv_mode = data >> 6;
			if (data & 0x80) {
				ic->swpenv_gain = data & 0x3F;
			}
			ic->swpenv_decay = data & 0x3F;
			ic->swpenv_phaseacc = ic->envelope_speed * (ic->swpenv_decay + 1) * ic->samplingRate / (232.0f*960.0f);			break;

		case	0x4085:	// Sweep Bias
			if (data & 0x40)
				ic->sweep_bias = (data & 0x3f) - 0x40;
			else
				ic->sweep_bias = data & 0x3f;
			ic->lfo_addr = 0;
			break;

		case	0x4086:	// Effector(LFO) Frequency(Low)
			ic->lfo_frequency = (ic->lfo_frequency & (~0x00FF)) | (INT32)data;
			break;

		case	0x4087:	// Effector(LFO) Frequency(High)
			ic->lfo_enable = (~data & 0x80);
			ic->lfo_frequency = (ic->lfo_frequency & 0x00FF) | (((INT32)data & 0x0F) << 8);
			break;

		case	0x4088:	// Effector(LFO) wavetable
			if (!ic->lfo_enable)
			{
				// FIFO?
				for (INT32 i = 0; i < 31; i++) {
					ic->lfo_wavetable[i * 2 + 0] = ic->lfo_wavetable[(i + 1) * 2 + 0];
					ic->lfo_wavetable[i * 2 + 1] = ic->lfo_wavetable[(i + 1) * 2 + 1];
				}
				ic->lfo_wavetable[31 * 2 + 0] = data & 0x07;
				ic->lfo_wavetable[31 * 2 + 1] = data & 0x07;
			}
			break;

		case	0x4089:	// Sound control
		{
			INT32	tbl[] = { 30, 20, 15, 12 };
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

UINT8 NESFDSAPU_ReadRegister(UINT8 chipID, UINT32 address)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	UINT8 data = address >> 8;

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

void NESFDSAPU_Update(UINT8 chipID, INT32** buffer, UINT32 length)
{
	NESFDSAPU* ic = &nesfdsapuChips[chipID];

	for(UINT32 i = 0; i < length; i++)
	{
		// Envelope unit
		if (ic->envelope_enable && ic->envelope_speed)
		{
			// Volume envelope
			if (ic->volenv_mode < 2)
			{
				float decay = (ic->envelope_speed * (ic->volenv_decay + 1) * ic->samplingRate) / (232.0f * 960.0f);
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
				float decay = (ic->envelope_speed * (ic->swpenv_decay + 1) * ic->samplingRate) / (232.0f * 960.0f);
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
		INT32 sub_freq = 0;
		if (ic->lfo_enable && ic->envelope_speed && ic->lfo_frequency)
		{
			static INT32 tbl[8] = { 0, 1, 2, 4, 0, -4, -2, -1 };
			ic->lfo_phaseacc -= (1789772.5f*(FLOAT32)(ic->lfo_frequency)) / 65536.0f;
			while (ic->lfo_phaseacc < 0.0)
			{
				ic->lfo_phaseacc += (FLOAT32)ic->samplingRate;
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
			INT32 sub_multi = ic->sweep_bias * ic->swpenv_gain;
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

		// Main unit
		INT32 output = 0;
		if (ic->main_enable && ic->main_frequency && !ic->wave_setup)
		{
			INT32 freq;
			INT32 main_addr_old = ic->main_addr;
			freq = (INT32)((ic->main_frequency + sub_freq) * 1789772.5f / 65536.0f);
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

		buffer[0][i] += output;
		buffer[1][i] += output;
	}
}