#include "HuC6280.h"
#include <math.h>

typedef struct {
	u16 frequency;
	u8 control;
	u8 balance;
	u8 waveform[32];
	u8 index;
	s16 dda;
	u8 noise_control;
	u32 noise_counter;
	u32 counter;
} Channel;

typedef struct {
	u8 select;
	u8 balance;
	u8 lfo_frequency;
	u8 lfo_control;
	Channel channel[8];

	s16 volume_table[32];
	u32 noise_freq_tab[32];
	u32 wave_freq_tab[4096];

	u32 channel_enabled;
	u32 channel_count;
}HUC6280;

#define HUC6280_CHIPS_COUNT 2
static HUC6280 chips[HUC6280_CHIPS_COUNT];

static void c6280_stream_update(u8 chipID, s32 **buffer, u32 length)
{
	HUC6280 *ic = &chips[chipID];

	static const s32 scale_tab[] = {
		0x00, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F,
		0x10, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F
	};
	u32 ch;
	u32 sample;

	s32 lmal = (ic->balance >> 4) & 0x0F;
	s32 rmal = (ic->balance >> 0) & 0x0F;
	s32 vll, vlr;

	lmal = scale_tab[lmal];
	rmal = scale_tab[rmal];

	/* Clear buffer */
	for(ch = 0; ch < ic->channel_count; ch++)
	{
		if ((ic->channel_enabled & (1 << (ch))) == 0)
		{
			for (sample = 0; sample < length; sample++)
			{
				buffer[((ch) << 1) + 0][sample] = 0;
				buffer[((ch) << 1) + 1][sample] = 0;
			}
		}
		else
		{
			/* Only look at enabled channels */
			if (ic->channel[ch].control & 0x80)
			{
				s32 lal = (ic->channel[ch].balance >> 4) & 0x0F;
				s32 ral = (ic->channel[ch].balance >> 0) & 0x0F;
				s32 al = ic->channel[ch].control & 0x1F;

				lal = scale_tab[lal];
				ral = scale_tab[ral];

				/* Calculate volume just as the patent says */
				vll = (0x1F - lal) + (0x1F - al) + (0x1F - lmal);
				if (vll > 0x1F)
					vll = 0x1F;

				vlr = (0x1F - ral) + (0x1F - al) + (0x1F - rmal);
				if (vlr > 0x1F)
					vlr = 0x1F;

				vll = ic->volume_table[vll] << 2;
				vlr = ic->volume_table[vlr] << 2;

				/* Check channel mode */
				if ((ch >= 4) && (ic->channel[ch].noise_control & 0x80))
				{
					/* Noise mode */
					u32 step = ic->noise_freq_tab[(ic->channel[ch].noise_control & 0x1F) ^ 0x1F];
					for (sample = 0; sample < length; sample++)
					{
						static s32 data = 0;
						ic->channel[ch].noise_counter += step;
						if (ic->channel[ch].noise_counter >= 0x800)
						{
							data = (rand() & 1) ? 0x1F : 0;
						}

						ic->channel[ch].noise_counter &= 0x7FF;

						buffer[((ch) << 1) + 0][sample] = (vll * (data - 16)) >> 2;
						buffer[((ch) << 1) + 1][sample] = (vlr * (data - 16)) >> 2;
					}
				}
				else if (ic->channel[ch].control & 0x40)
				{
					/* DDA mode */
					for (sample = 0; sample < length; sample++)
					{
						buffer[((ch) << 1) + 0][sample] = (vll * (ic->channel[ch].dda - 16)) >> 2;
						buffer[((ch) << 1) + 1][sample] = (vlr * (ic->channel[ch].dda - 16)) >> 2;
					}
				}
				else
				{
					/* Waveform mode */
					u32 step = ic->wave_freq_tab[ic->channel[ch].frequency];
					for (sample = 0; sample < length; sample++)
					{
						s32 offset = (ic->channel[ch].counter >> 12) & 0x1F;
						ic->channel[ch].counter += step;
						ic->channel[ch].counter &= 0x1FFFF;
						s16 data = ic->channel[ch].waveform[offset];

						buffer[((ch) << 1) + 0][sample] = (vll * (data - 16)) >> 2;
						buffer[((ch) << 1) + 1][sample] = (vlr * (data - 16)) >> 2;
					}
				}
			}
		}
	}
}

static void c6280_write_internal(u8 chipID, s32 address, s32 data)
{
	HUC6280 *ic = &chips[chipID];
	Channel *q = &ic->channel[ic->select];
		
	switch(address & 0x0F)
	{
		case 0x00: /* Channel select */
			ic->select = data & 0x07;
			break;

		case 0x01: /* Global balance */
			ic->balance  = data;
			break;

		case 0x02: /* Channel frequency (LSB) */
			q->frequency = (q->frequency & 0x0F00) | data;
			q->frequency &= 0x0FFF;
			break;

		case 0x03: /* Channel frequency (MSB) */
			q->frequency = (q->frequency & 0x00FF) | (data << 8);
			q->frequency &= 0x0FFF;
			break;

		case 0x04: /* Channel control (key-on, DDA mode, volume) */

			/* 1-to-0 transition of DDA bit resets waveform index */
			if((q->control & 0x40) && ((data & 0x40) == 0))
			{
				q->index = 0;
			}
			q->control = data;
			break;

		case 0x05: /* Channel balance */
			q->balance = data;
			break;

		case 0x06: /* Channel waveform data */

			switch(q->control & 0xC0)
			{
				case 0x00:
					q->waveform[q->index & 0x1F] = data & 0x1F;
					q->index = (q->index + 1) & 0x1F;
					break;

				case 0x40:
					break;

				case 0x80:
					q->waveform[q->index & 0x1F] = data & 0x1F;
					q->index = (q->index + 1) & 0x1F;
					break;

				case 0xC0:
					q->dda = data & 0x1F;
					break;
			}

			break;

		case 0x07: /* Noise control (enable, frequency) */
			q->noise_control = data;
			break;

		case 0x08: /* LFO frequency */
			ic->lfo_frequency = data;
			break;

		case 0x09: /* LFO control (enable, mode) */
			ic->lfo_control = data;
			break;

		default:
			break;
	}
}

#define nBurnSoundRate sampleRate
s32 HUC6280_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	s32 i;
	f32 step;
	HUC6280 *ic = &chips[chipID];

	/* Loudest volume level for table */
	f32 level = 65535.0 / 6.0 / 32.0;

	/* Clear context */
	memset(ic, 0, sizeof(HUC6280));

	/* Make waveform frequency table */
	for (i = 0; i < 4096; i += 1)
	{
		step = ((clock / (nBurnSoundRate * 1.0000f)) * 4096) / (i + 1);
		ic->wave_freq_tab[(1 + i) & 0xFFF] = (u32)step;
	}

	/* Make noise frequency table */
	for (i = 0; i < 32; i += 1)
	{
		step = ((clock / (nBurnSoundRate * 1.0000f)) * 32) / (i + 1);
		ic->noise_freq_tab[i] = (u32)step;
	}

	/* Make volume table */
	/* PSG has 48dB volume range spread over 32 steps */
	step = 48.0f / 32.0f;
	for (i = 0; i < 31; i++)
	{
		ic->volume_table[i] = (s16)level;
		level /= pow(10.0f, step / 20.0f);
	}
	ic->volume_table[31] = 0;

	ic->channel_enabled = 0xffffffff;
	ic->channel_count = 6;

	return -1;
}

void HUC6280_Shutdown(u8 chipID)
{
	HUC6280 *ic = &chips[chipID];
}

void HUC6280_Reset(u8 chipID)
{
	HUC6280 *ic = &chips[chipID];

	ic->select = 0;
	ic->balance = 0;
	ic->lfo_frequency = 0;
	ic->lfo_control = 0;
	memset(ic->channel, 0, 8 * sizeof(Channel));

	ic->channel_enabled = 0xffffffff;
	ic->channel_count = 6;
}

void HUC6280_Update(u8 chipID, s32 **buffer, u32 length)
{
	HUC6280 *ic = &chips[chipID];
	c6280_stream_update(chipID, buffer, length);
}

u8 HUC6280_ReadRegister(u8 chipID, u32 address)
{
	return 0;//h6280io_get_buffer();
}

void HUC6280_WriteRegister(u8 chipID, u32 address, u8 data)
{
	// h6280io_set_buffer(data);

	c6280_write_internal(chipID, address, data);
}

void HUC6280_SetChannelEnable(u8 chipID, u8 channel, u8 enable)
{
	HUC6280* ic = &chips[chipID];

	if (enable)
		ic->channel_enabled |= (1 << channel);
	else
		ic->channel_enabled &= (~(1 << channel));
}

u8 HUC6280_GetChannelEnable(u8 chipID, u8 channel)
{
	HUC6280* ic = &chips[chipID];

	return (ic->channel_enabled & (1 << channel)) != 0;
}

u32 HUC6280_GetChannelCount(u8 chipID)
{
	HUC6280* ic = &chips[chipID];

	return ic->channel_count;
}