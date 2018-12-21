#include "HuC6280.h"
#include <math.h>

typedef struct {
	UINT16 frequency;
	UINT8 control;
	UINT8 balance;
	UINT8 waveform[32];
	UINT8 index;
	INT16 dda;
	UINT8 noise_control;
	UINT32 noise_counter;
	UINT32 counter;
} Channel;

typedef struct {
	UINT8 select;
	UINT8 balance;
	UINT8 lfo_frequency;
	UINT8 lfo_control;
	Channel channel[8];

	INT16 volume_table[32];
	UINT32 noise_freq_tab[32];
	UINT32 wave_freq_tab[4096];
}HUC6280;

#define HUC6280_CHIPS_COUNT 2
static HUC6280 chips[HUC6280_CHIPS_COUNT];

static void c6280_stream_update(UINT8 chipID, INT32 **buffer, UINT32 length)
{
	HUC6280 *ic = &chips[chipID];

	static const INT32 scale_tab[] = {
		0x00, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F,
		0x10, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F
	};
	UINT32 ch;
	UINT32 i;

	INT32 lmal = (ic->balance >> 4) & 0x0F;
	INT32 rmal = (ic->balance >> 0) & 0x0F;
	INT32 vll, vlr;

	lmal = scale_tab[lmal];
	rmal = scale_tab[rmal];

	INT32* lBuf = buffer[0];
	INT32* rBuf = buffer[1];

	/* Clear buffer */
	for(ch = 0; ch < 6; ch++)
	{
		/* Only look at enabled channels */
		if(ic->channel[ch].control & 0x80)
		{
			INT32 lal = (ic->channel[ch].balance >> 4) & 0x0F;
			INT32 ral = (ic->channel[ch].balance >> 0) & 0x0F;
			INT32 al  = ic->channel[ch].control & 0x1F;

			lal = scale_tab[lal];
			ral = scale_tab[ral];

			/* Calculate volume just as the patent says */
			vll = (0x1F - lal) + (0x1F - al) + (0x1F - lmal);
			if(vll > 0x1F)
				vll = 0x1F;

			vlr = (0x1F - ral) + (0x1F - al) + (0x1F - rmal);
			if(vlr > 0x1F)
				vlr = 0x1F;

			vll = ic->volume_table[vll];
			vlr = ic->volume_table[vlr];
			
			/* Check channel mode */
			if((ch >= 4) && (ic->channel[ch].noise_control & 0x80))
			{
				/* Noise mode */
				UINT32 step = ic->noise_freq_tab[(ic->channel[ch].noise_control & 0x1F) ^ 0x1F];
				for(i=0; i<length; i++)
				{
					static INT32 data = 0;
					ic->channel[ch].noise_counter += step;
					if(ic->channel[ch].noise_counter >= 0x800)
					{
						data = (rand() & 1) ? 0x1F : 0;
					}

					ic->channel[ch].noise_counter &= 0x7FF;

					lBuf[i] += (vll * (data - 16));
					rBuf[i] += (vlr * (data - 16));
				}
			}
			else if(ic->channel[ch].control & 0x40)
			{
				/* DDA mode */
				for(i = 0; i < length; i++)
				{
					lBuf[i] += (vll * (ic->channel[ch].dda - 16));
					rBuf[i] += (vlr * (ic->channel[ch].dda - 16));
				}
			}
			else
			{
				/* Waveform mode */
				UINT32 step = ic->wave_freq_tab[ic->channel[ch].frequency];
				for(i = 0; i < length; i++)
				{
					INT32 offset = (ic->channel[ch].counter >> 12) & 0x1F;
					ic->channel[ch].counter += step;
					ic->channel[ch].counter &= 0x1FFFF;
					INT16 data = ic->channel[ch].waveform[offset];
					
					lBuf[i] += (vll * (data - 16));
					rBuf[i] += (vlr * (data - 16));
				}
			}
		}
	}
}

static void c6280_write_internal(UINT8 chipID, INT32 address, INT32 data)
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
INT32 HUC6280_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate)
{
	INT32 i;
	FLOAT32 step;
	HUC6280 *ic = &chips[chipID];

	/* Loudest volume level for table */
	FLOAT32 level = 65535.0 / 6.0 / 32.0;

	/* Clear context */
	memset(ic, 0, sizeof(HUC6280));

	/* Make waveform frequency table */
	for (i = 0; i < 4096; i += 1)
	{
		step = ((clock / (nBurnSoundRate * 1.0000f)) * 4096) / (i + 1);
		ic->wave_freq_tab[(1 + i) & 0xFFF] = (UINT32)step;
	}

	/* Make noise frequency table */
	for (i = 0; i < 32; i += 1)
	{
		step = ((clock / (nBurnSoundRate * 1.0000f)) * 32) / (i + 1);
		ic->noise_freq_tab[i] = (UINT32)step;
	}

	/* Make volume table */
	/* PSG has 48dB volume range spread over 32 steps */
	step = 48.0f / 32.0f;
	for (i = 0; i < 31; i++)
	{
		ic->volume_table[i] = (INT16)level;
		level /= pow(10.0f, step / 20.0f);
	}
	ic->volume_table[31] = 0;

	return -1;
}

void HUC6280_Shutdown(UINT8 chipID)
{
	HUC6280 *ic = &chips[chipID];
}

void HUC6280_Reset(UINT8 chipID)
{
	HUC6280 *ic = &chips[chipID];

	ic->select = 0;
	ic->balance = 0;
	ic->lfo_frequency = 0;
	ic->lfo_control = 0;
	memset(ic->channel, 0, 8 * sizeof(Channel));
}

void HUC6280_Update(UINT8 chipID, INT32 **buffer, UINT32 length)
{
	HUC6280 *ic = &chips[chipID];
	c6280_stream_update(chipID, buffer, length);
}

UINT8 HUC6280_ReadRegister(UINT8 chipID, UINT32 address)
{
	return 0;//h6280io_get_buffer();
}

void HUC6280_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data)
{
	// h6280io_set_buffer(data);

	c6280_write_internal(chipID, address, data);
}