#include "NESFDSAPU.h"

NESFDSAPU nesfdsapu[2];

INT32 NESFDSAPU_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate)
{
	memset(&nesfdsapu[chipID], 0, sizeof(nesfdsapu[chipID]));
	nesfdsapu[chipID].sampling_rate = sampleRate;

	nesfdsapu[chipID].channelEnabled = -1;

	return -1;
}

void NESFDSAPU_Shutdown()
{
}

void NESFDSAPU_Reset(UINT8 chipID)
{
	UINT32 sampleRate = nesfdsapu[chipID].sampling_rate;
	memset(&nesfdsapu[chipID], 0, sizeof(nesfdsapu[chipID]));
	nesfdsapu[chipID].sampling_rate = sampleRate;
}

void NESFDSAPU_WriteRegister(UINT8 chipID, UINT32 addr, UINT8 data)
{
	NESFDSAPU* apu = &nesfdsapu[chipID];

	NESFDSAPU_WriteSub(chipID, addr, data, apu->sampling_rate);
}

void NESFDSAPU_WriteSub(UINT8 chipID, UINT32 addr, UINT8 data, UINT32 rate)
{
	NESFDSAPU* apu = &nesfdsapu[chipID];

	if(addr>=0x20 && addr<=0x3e)
		addr += 0x4060;
	if (addr == 0x3f)
		addr = 0x4023;
	if (addr >= 0x40 && addr <= 0x7f)
		addr += 0x4000;

	if (addr < 0x4040 || addr > 0x40BF)
		return;

	apu->reg[addr - 0x4040] = data;
	if (addr >= 0x4040 && addr <= 0x407F)
	{
		if (apu->wave_setup)
		{
			apu->main_wavetable[addr - 0x4040] = 0x20 - ((INT32)data & 0x3F);
		}
	}
	else
	{
		switch (addr)
		{
		case 0x4080:	// Volume Envelope
			apu->volenv_mode = data >> 6;
			if (data & 0x80)
			{
				apu->volenv_gain = data & 0x3F;
				if (!apu->main_addr)
				{
					apu->now_volume = (apu->volenv_gain < 0x21) ? apu->volenv_gain : 0x20;
				}
			}
			apu->volenv_decay = data & 0x3F;
			apu->volenv_phaseacc = apu->envelope_speed * (apu->volenv_decay + 1) * rate / (232.0 * 960.0);
			break;

		case	0x4082:	// Main Frequency(Low)
			apu->main_frequency = (apu->main_frequency & ~0x00FF) | (INT32)data;
			break;

		case	0x4083:	// Main Frequency(High)
			apu->main_enable = (~data)&(1 << 7);
			apu->envelope_enable = (~data)&(1 << 6);
			if (!apu->main_enable)
			{
				apu->main_addr = 0;
				apu->now_volume = (apu->volenv_gain < 0x21) ? apu->volenv_gain : 0x20;
			}
			apu->main_frequency = (apu->main_frequency & 0x00FF) | (((INT32)data & 0x0F) << 8);
			break;

		case	0x4084:	// Sweep Envelope
			apu->swpenv_mode = data >> 6;
			if (data & 0x80) {
				apu->swpenv_gain = data & 0x3F;
			}
			apu->swpenv_decay = data & 0x3F;
			apu->swpenv_phaseacc = apu->envelope_speed * (apu->swpenv_decay + 1) * rate / (232.0*960.0);
			break;

		case	0x4085:	// Sweep Bias
			if (data & 0x40)
				apu->sweep_bias = (data & 0x3f) - 0x40;
			else
				apu->sweep_bias = data & 0x3f;
			apu->lfo_addr = 0;
			break;

		case	0x4086:	// Effector(LFO) Frequency(Low)
			apu->lfo_frequency = (apu->lfo_frequency & (~0x00FF)) | (INT32)data;
			break;

		case	0x4087:	// Effector(LFO) Frequency(High)
			apu->lfo_enable = (~data & 0x80);
			apu->lfo_frequency = (apu->lfo_frequency & 0x00FF) | (((INT32)data & 0x0F) << 8);
			break;

		case	0x4088:	// Effector(LFO) wavetable
			if (!apu->lfo_enable)
			{
				// FIFO?
				for (INT32 i = 0; i < 31; i++) {
					apu->lfo_wavetable[i * 2 + 0] = apu->lfo_wavetable[(i + 1) * 2 + 0];
					apu->lfo_wavetable[i * 2 + 1] = apu->lfo_wavetable[(i + 1) * 2 + 1];
				}
				apu->lfo_wavetable[31 * 2 + 0] = data & 0x07;
				apu->lfo_wavetable[31 * 2 + 1] = data & 0x07;
			}
			break;

		case	0x4089:	// Sound control
		{
			INT32	tbl[] = { 30, 20, 15, 12 };
			apu->master_volume = tbl[data & 3];
			apu->wave_setup = data & 0x80;
		}
		break;

		case	0x408A:	// Sound control 2
			apu->envelope_speed = data;
			break;

		default:
			break;
		}
	}
}

UINT8 NESFDSAPU_ReadRegister(UINT8 chipID, UINT32 addr)
{
	NESFDSAPU* apu = &nesfdsapu[chipID];
	UINT8 data = addr >> 8;

	if (addr >= 0x4040 && addr <= 0x407F)
	{
		data = apu->main_wavetable[addr & 0x3F] | 0x40;
	}
	else if (addr == 0x4090)
	{
		data = (apu->volenv_gain & 0x3F) | 0x40;
	}
	else if (addr == 0x4092)
	{
		data = (apu->swpenv_gain & 0x3F) | 0x40;
	}
	return data;
}

INT32 NESFDSAPU_Update(UINT8 chipID, INT32 **buffers, UINT32 length)
{
	NESFDSAPU* apu = &nesfdsapu[chipID];

	for (int i = 0; i < length; i++)
	{
		// Envelope unit
		if (apu->envelope_enable && apu->envelope_speed)
		{
			// Volume envelope
			if (apu->volenv_mode < 2)
			{
				double decay = (apu->envelope_speed * (apu->volenv_decay + 1) * apu->sampling_rate) / (232.0*960.0);
				apu->volenv_phaseacc -= 1.0;
				while (apu->volenv_phaseacc < 0.0)
				{
					apu->volenv_phaseacc += decay;
					if (apu->volenv_mode == 0)
					{
						if (apu->volenv_gain)
							apu->volenv_gain--;
					}
					else if (apu->volenv_mode == 1)
					{
						if (apu->volenv_gain < 0x20)
							apu->volenv_gain++;
					}
				}
			}

			// Sweep envelope
			if (apu->swpenv_mode < 2)
			{
				double decay = (apu->envelope_speed * (apu->swpenv_decay + 1) * apu->sampling_rate) / (232.0*960.0);
				apu->swpenv_phaseacc -= 1.0;
				while (apu->swpenv_phaseacc < 0.0)
				{
					apu->swpenv_phaseacc += decay;
					if (apu->swpenv_mode == 0)
					{
						if (apu->swpenv_gain)
							apu->swpenv_gain--;
					}
					else if (apu->swpenv_mode == 1)
					{
						if (apu->swpenv_gain < 0x20)
							apu->swpenv_gain++;
					}
				}
			}
		}

		// Effector(LFO) unit
		INT32 sub_freq = 0;
		if (apu->lfo_enable && apu->envelope_speed && apu->lfo_frequency)
		{
			static INT32 tbl[8] = { 0, 1, 2, 4, 0, -4, -2, -1 };
			apu->lfo_phaseacc -= (1789772.5*(FLOAT32)(apu->lfo_frequency)) / 65536.0;
			while (apu->lfo_phaseacc < 0.0)
			{
				apu->lfo_phaseacc += (FLOAT32)apu->sampling_rate;
				if (apu->lfo_wavetable[apu->lfo_addr] == 4)
					apu->sweep_bias = 0;
				else
					apu->sweep_bias += tbl[apu->lfo_wavetable[apu->lfo_addr]];
				apu->lfo_addr = (apu->lfo_addr + 1) & 63;
			}
			if (apu->sweep_bias > 63)
				apu->sweep_bias -= 128;
			else if (apu->sweep_bias < -64)
				apu->sweep_bias += 128;
			INT32 sub_multi = apu->sweep_bias * apu->swpenv_gain;
			if (sub_multi & 0x0F)
			{
				sub_multi = (sub_multi / 16);
				if (apu->sweep_bias >= 0)
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
			sub_freq = (apu->main_frequency) * sub_multi / 64;
		}

		// Main unit
		INT32 output = 0;
		if (apu->main_enable && apu->main_frequency && !apu->wave_setup)
		{
			INT32 freq;
			INT32 main_addr_old = apu->main_addr;
			freq = (apu->main_frequency + sub_freq) * 1789772.5 / 65536.0;
			apu->main_addr = (apu->main_addr + freq + 64 * apu->sampling_rate) % (64 * apu->sampling_rate);

			if (main_addr_old > apu->main_addr)
				apu->now_volume = (apu->volenv_gain < 0x21) ? apu->volenv_gain : 0x20;

			output = apu->main_wavetable[(apu->main_addr / apu->sampling_rate) & 0x3f] * 8 * apu->now_volume * apu->master_volume / 30;
			if (apu->now_volume)
				apu->now_freq = freq * 4;
			else
				apu->now_freq = 0;
		}
		else
		{
			apu->now_freq = 0;
			output = 0;
		}

		// LPF
#if 0
#if	0
		output = (apu->output_buf[0] * 2 + output) / 3;
		apu->output_buf[0] = output;
#else
		output = (apu->output_buf[0] + apu->output_buf[1] + output) / 3;
		apu->output_buf[0] = apu->output_buf[1];
		apu->output_buf[1] = output;
#endif
#endif
		if(apu->channelEnabled)
		{
			buffers[0][i] += output;
			buffers[1][i] += output;
		}
	}

	return apu->output;
}

INT32 NESFDSAPU_GetFreq(UINT8 chipID)
{
	NESFDSAPU* apu = &nesfdsapu[chipID];

	return apu->now_freq;
}

VOID NESFDSAPU_SetChannelEnabled(UINT8 chipID, INT8 enabled)
{
	NESFDSAPU* apu = &nesfdsapu[chipID];

	apu->channelEnabled = enabled;
}

INT8 NESFDSAPU_IsEnabledChannel(UINT8 chipID)
{
	NESFDSAPU* apu = &nesfdsapu[chipID];

	return apu->channelEnabled;
}