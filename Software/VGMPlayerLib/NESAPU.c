#include "NESAPU.h"
#include "nes_defs.h"

#define N2A03_DEFAULTCLOCK		 1789772 	/* 1.789772 MHz */		/* pal 1.662607 MHz */

/* GLOBAL CONSTANTS */
#define  SYNCS_MAX1     0x20
#define  SYNCS_MAX2     0x80

/* GLOBAL VARIABLES */
typedef struct
{
	apu_t	APU;							/* Actual APUs */
	f32		apu_incsize;					/* Adjustment increment */
	//u16	samps_per_sync;					/* Number of samples per vsync */
	//u16	buffer_size;					/* Actual buffer size in bytes */
	//u16	real_rate;						/* Actual playback rate */
	u8		noise_lut[NOISE_LONG];			/* Noise sample lookup table */
	u16		vbl_times[0x20];				/* VBL durations in samples */
	u32		sync_times1[SYNCS_MAX1];		/* Samples per sync table */
	u32		sync_times2[SYNCS_MAX2];		/* Samples per sync table */
	s32		channel;
	u32		channel_output_enabled;
	u32		channel_count;
}NESAPU;

#define NESAPU_CHIPS_COUNT 2
NESAPU nesapuChips[NESAPU_CHIPS_COUNT];

#include <math.h>

/* INTERNAL FUNCTIONS */
/* INITIALIZE NOISE LOOKUP TABLE */
void create_noise(u8 *noise_lut, const s32 bits, s32 size)
{
	/*
	s32 i;
	for (i = 0; i < size; i++)
	{
		float ph = ((float)i) / size * 3.14f * 2 * 300;

		noise_lut[i] = 127 * sin(ph) + 127;
	}
	*/
	
	//static s32 m = 0x0011;
	s32 m = 0x0011;
	s32 xor_val, i;
	for (i = 0; i < size; i++)
	{
		xor_val = m & 1;
		m >>= 1;
		xor_val ^= (m & 1);
		m |= xor_val << (bits - 1);

		noise_lut[i] = m;
	}
}

/* INITIALIZE WAVE TIMES RELATIVE TO SAMPLE RATE */
void create_vbltimes(u16 *vbl_times, const u8 *vbl, u32 rate)
{
	s32 i;
	for (i = 0; i < 0x20; i++)
		vbl_times[i] = vbl[i] * rate;
}

/* INITIALIZE SAMPLE TIMES IN TERMS OF VSYNCS */
void create_syncs(u32* sync_times1, u32* sync_times2, u64 sps)
{
	s32 i;
	u64 val = sps;

	for (i = 0; i < SYNCS_MAX1; i++)
	{
		sync_times1[i] = (u32)val;
		val += sps;
	}

	val = 0;
	for (i = 0; i < SYNCS_MAX2; i++)
	{
		sync_times2[i] = (u32)val;
		sync_times2[i] >>= 2;
		val += sps;
	}
}

/* TODO: sound channels should *ALL* have DC volume decay */
/* OUTPUT SQUARE WAVE SAMPLE (VALUES FROM -16 to +15) */
s8 apu_square(u8 chipID, square_t *chan)
{
	s32 env_delay;
	s32 sweep_delay;
	s8 output;
	NESAPU *ic = &nesapuChips[chipID];

	/* reg0: 0-3=volume, 4=envelope, 5=hold, 6-7=duty cycle
	** reg1: 0-2=sweep shifts, 3=sweep inc/dec, 4-6=sweep length, 7=sweep on
	** reg2: 8 bits of freq
	** reg3: 0-2=high freq, 7-4=vbl length counter
	*/

	if (FALSE == chan->enabled)
		return 0;

	/* enveloping */
	env_delay = ic->sync_times1[chan->regs[0] & 0x0F];

	/* decay is at a rate of (env_regs + 1) / 240 secs */
	chan->env_phase -= 4;
	while (chan->env_phase < 0)
	{
		chan->env_phase += env_delay;
		if (chan->regs[0] & 0x20)
			chan->env_vol = (chan->env_vol + 1) & 15;
		else if (chan->env_vol < 15)
			chan->env_vol++;
	}

	/* vbl length counter */
	if (chan->vbl_length > 0 && 0 == (chan->regs[0] & 0x20))
		chan->vbl_length--;

	if (0 == chan->vbl_length)
		return 0;

	/* freqsweeps */
	if ((chan->regs[1] & 0x80) && (chan->regs[1] & 7))
	{
		sweep_delay = ic->sync_times1[(chan->regs[1] >> 4) & 7];
		chan->sweep_phase -= 2;
		while (chan->sweep_phase < 0)
		{
			chan->sweep_phase += sweep_delay;
			if (chan->regs[1] & 8)
				chan->freq -= chan->freq >> (chan->regs[1] & 7);
			else
				chan->freq += chan->freq >> (chan->regs[1] & 7);
		}
	}

	if ((0 == (chan->regs[1] & 8) && (chan->freq >> 16) > freq_limit[chan->regs[1] & 7])
		|| (chan->freq >> 16) < 4)
		return 0;

	chan->phaseacc -= ic->apu_incsize; /* # of cycles per sample */

	while (chan->phaseacc < 0)
	{
		chan->phaseacc += (chan->freq >> 16);
		chan->adder = (chan->adder + 1) & 0x0F;
	}

	if (chan->regs[0] & 0x10) /* fixed volume */
		output = chan->regs[0] & 0x0F;
	else
		output = 0x0F - chan->env_vol;

	if (chan->adder < (duty_lut[chan->regs[0] >> 6]))
		output = -output;

	return (s8)output;
}

/* OUTPUT TRIANGLE WAVE SAMPLE (VALUES FROM -16 to +15) */
s8 apu_triangle(u8 chipID, triangle_t *chan)
{
	s8 output;
	NESAPU *ic = &nesapuChips[chipID];
	/* reg0: 7=holdnote, 6-0=linear length counter
	** reg2: low 8 bits of frequency
	** reg3: 7-3=length counter, 2-0=high 3 bits of frequency
	*/

	if (FALSE == chan->enabled)
		return 0;

	if (FALSE == chan->counter_started && 0 == (chan->regs[0] & 0x80))
	{
		if (chan->write_latency)
			chan->write_latency--;
		if (0 == chan->write_latency)
			chan->counter_started = TRUE;
	}

	if (chan->counter_started)
	{
		if (chan->linear_length > 0)
			chan->linear_length--;
		if (chan->vbl_length && 0 == (chan->regs[0] & 0x80))
			chan->vbl_length--;

		if (0 == chan->vbl_length)
			return 0;
	}

	if (0 == chan->linear_length)
		return 0;

	s32 freq = (((chan->regs[3] & 7) << 8) + chan->regs[2]) + 1;
	if (freq < 4) /* inaudible */
		return 0;

	chan->phaseacc -= ic->apu_incsize; /* # of cycles per sample */
	while (chan->phaseacc < 0)
	{
		chan->phaseacc += freq;
		chan->adder = (chan->adder + 1) & 0x1F;

		output = (chan->adder & 7) << 1;
		if (chan->adder & 8)
			output = 0x10 - output;
		if (chan->adder & 0x10)
			output = -output;

		chan->output_vol = output;
	}

	return (s8)chan->output_vol;
}

/* OUTPUT NOISE WAVE SAMPLE (VALUES FROM -16 to +15) */
s8 apu_noise(u8 chipID, noise_t *chan)
{
	s32 env_delay;
	u8 outvol;
	u8 output;
	NESAPU *ic = &nesapuChips[chipID];

	/* reg0: 0-3=volume, 4=envelope, 5=hold
	** reg2: 7=small(93 byte) sample,3-0=freq lookup
	** reg3: 7-4=vbl length counter
	*/

	if (FALSE == chan->enabled)
		return 0;

	/* enveloping */
	env_delay = ic->sync_times1[chan->regs[0] & 0x0F];

	/* decay is at a rate of (env_regs + 1) / 240 secs */
	chan->env_phase -= 4;
	while (chan->env_phase < 0)
	{
		chan->env_phase += env_delay;
		if (chan->regs[0] & 0x20)
			chan->env_vol = (chan->env_vol + 1) & 15;
		else if (chan->env_vol < 15)
			chan->env_vol++;
	}

	/* length counter */
	if (0 == (chan->regs[0] & 0x20))
	{
		if (chan->vbl_length > 0)
			chan->vbl_length--;
	}

	if (0 == chan->vbl_length)
		return 0;

	s32 freq = noise_freq[chan->regs[2] & 0x0F];
	chan->phaseacc -= ic->apu_incsize; /* # of cycles per sample */
	while (chan->phaseacc < 0)
	{
		chan->phaseacc += freq;

		chan->cur_pos++;
		if (NOISE_SHORT == chan->cur_pos && (chan->regs[2] & 0x80))
			chan->cur_pos = 0;
		else if (NOISE_LONG == chan->cur_pos)
			chan->cur_pos = 0;
	}

	if (chan->regs[0] & 0x10) /* fixed volume */
		outvol = chan->regs[0] & 0x0F;
	else
		outvol = 0x0F - chan->env_vol;

	output = ic->noise_lut[chan->cur_pos];
	if (output > outvol)
		output = outvol;

	if (ic->noise_lut[chan->cur_pos] & 0x80) /* make it negative */
		output = -output;

	return (s8)output;
}

/* RESET DPCM PARAMETERS */
void apu_dpcmreset(dpcm_t *chan)
{
	chan->address = 0xC000 + (u16)(chan->regs[2] << 6);
	chan->length = (u16)(chan->regs[3] << 4) + 1;
	chan->bits_left = chan->length << 3;
	chan->irq_occurred = FALSE;
	chan->enabled = TRUE; /* Fixed * Proper DPCM channel ENABLE/DISABLE flag behaviour*/
	chan->vol = 0; /* Fixed * DPCM DAC resets itself when restarted */
}

/* OUTPUT DPCM WAVE SAMPLE (VALUES FROM -64 to +63) */
/* TODO: centerline naughtiness */
s8 apu_dpcm(u8 chipID, dpcm_t *chan)
{
	s32 bit_pos;
	NESAPU *ic = &nesapuChips[chipID];

	/* reg0: 7=irq gen, 6=looping, 3-0=pointer to clock table
	** reg1: output dc level, 7 bits unsigned
	** reg2: 8 bits of 64-byte aligned address offset : $C000 + (value * 64)
	** reg3: length, (value * 16) + 1
	*/
	return 0;

	s32 freq = 0;
	if (chan->enabled)
	{
		freq = dpcm_clocks[chan->regs[0] & 0x0F];
		chan->phaseacc -= ic->apu_incsize; /* # of cycles per sample */

		while (chan->phaseacc < 0)
		{
			chan->phaseacc += freq;

			if (0 == chan->length)
			{
				chan->enabled = FALSE; /* Fixed * Proper DPCM channel ENABLE/DISABLE flag behaviour*/
				chan->vol = 0; /* Fixed * DPCM DAC resets itself when restarted */
				if (chan->regs[0] & 0x40)
					apu_dpcmreset(chan);
				else
				{
					if (chan->regs[0] & 0x80) /* IRQ Generator */
					{
						chan->irq_occurred = TRUE;
						//n2a03_irq();
					}
					break;
				}
			}


			chan->bits_left--;
			bit_pos = 7 - (chan->bits_left & 7);
			if (7 == bit_pos)
			{
				chan->cur_byte = 0;//chan->cpu_mem[chan->address];
				chan->address++;
				chan->length--;
			}

			if (chan->cur_byte & (1 << bit_pos))
				//            chan->regs[1]++;
				chan->vol += 2; /* FIXED * DPCM channel only uses the upper 6 bits of the DAC */
			else
				//            chan->regs[1]--;
				chan->vol -= 2;
		}
	}

	if (chan->vol > 63)
		chan->vol = 63;
	else if (chan->vol < -64)
		chan->vol = -64;

	return (s8)(chan->vol);
}

/* WRITE REGISTER VALUE */
void apu_regwrite(u8 chipID, u32 address, u8 value, s32* channel, f32* freq)
{
	NESAPU *ic = &nesapuChips[chipID];
	s32 chan = (address & 4) ? 1 : 0;

	// float squarefreq[2] = { 0, 0 };
	// float trifreq = 0;
	// float noisefreq = 0;
	// float dpcmfreq = 0;

	switch (address)
	{
	/* squares */
	case APU_WRA0:
	case APU_WRB0:
		ic->APU.squ[chan].regs[0] = value;
		break;

	case APU_WRA1:
	case APU_WRB1:
		ic->APU.squ[chan].regs[1] = value;
		break;

	case APU_WRA2:
	case APU_WRB2:
		ic->APU.squ[chan].regs[2] = value;
		if (ic->APU.squ[chan].enabled)
		{
			ic->APU.squ[chan].freq = ((((ic->APU.squ[chan].regs[3] & 7) << 8) + value) + 1) << 16;
			
			// ic->APU.squ[chan].freq = (144*4 << 16);
			// 576  ->  G
			// 288  -> 391.96
			// 144  -> 783.91
			// 72   -> 1567.83
			
			if ((ic->channel_output_enabled & (1 << (chan))))
			{
				*channel = chan;
				int invF = (ic->APU.squ[chan].freq >> 16);
				*freq = (invF != 0) ? (144.0f / invF) * 783.91f : 0;
			}
		}
		break;

	case APU_WRA3:
	case APU_WRB3:
		ic->APU.squ[chan].regs[3] = value;

		if (ic->APU.squ[chan].enabled)
		{
			ic->APU.squ[chan].vbl_length = ic->vbl_times[value >> 3];
			ic->APU.squ[chan].env_vol = 0;
			ic->APU.squ[chan].freq = ((((value & 7) << 8) + ic->APU.squ[chan].regs[2]) + 1) << 16;

			if ((ic->channel_output_enabled & (1 << (chan))))
			{
				*channel = chan;
				int invF = (ic->APU.squ[chan].freq >> 16);
				*freq = (invF != 0) ? (144.0f / invF) * 783.91f : 0;
			}
		}

		break;

		/* triangle */
	case APU_WRC0:
		ic->APU.tri.regs[0] = value;

		if (ic->APU.tri.enabled)
		{                                          /* ??? */
			if (FALSE == ic->APU.tri.counter_started)
				ic->APU.tri.linear_length = ic->sync_times2[value & 0x7F];
		}

		break;

	case 0x4009:
		/* unused */
		ic->APU.tri.regs[1] = value;
		break;

	case APU_WRC2:
		ic->APU.tri.regs[2] = value;
		break;

	case APU_WRC3:
		ic->APU.tri.regs[3] = value;

		/* this is somewhat of a hack.  there is some latency on the Real
		** Thing between when trireg0 is written to and when the linear
		** length counter actually begins its countdown.  we want to prevent
		** the case where the program writes to the freq regs first, then
		** to reg 0, and the counter accidentally starts running because of
		** the sound queue's timestamp processing.
		**
		** set to a few NES sample -- should be sufficient
		**
		**     3 * (1789772.727 / 44100) = ~122 cycles, just around one scanline
		**
		** should be plenty of time for the 6502 code to do a couple of table
		** dereferences and load up the other triregs
		*/

		ic->APU.tri.write_latency = 3;

		if (ic->APU.tri.enabled)
		{
			ic->APU.tri.counter_started = FALSE;
			ic->APU.tri.vbl_length = ic->vbl_times[value >> 3];
			ic->APU.tri.linear_length = ic->sync_times2[ic->APU.tri.regs[0] & 0x7F];

			// 540			106.9
			// 270			220.0
			// 135			440.0
			if ((ic->channel_output_enabled & (1 << (2))))
			{
				*channel = 2;
				int invF = ((((ic->APU.tri.regs[3] & 7) << 8) + ic->APU.tri.regs[2]) + 1);
				*freq = (invF != 0) ? (270.0f / invF) * 220.0f : 0.0f;
			}
		}

		break;

		/* noise */
	case APU_WRD0:
		ic->APU.noi.regs[0] = value;
		break;

	case 0x0D:
		/* unused */
		ic->APU.noi.regs[1] = value;
		break;

	case APU_WRD2:
		ic->APU.noi.regs[2] = value;
		break;

	case APU_WRD3:
		ic->APU.noi.regs[3] = value;

		if (ic->APU.noi.enabled)
		{
			ic->APU.noi.vbl_length = ic->vbl_times[value >> 3];
			ic->APU.noi.env_vol = 0; /* reset envelope */

			if ((ic->channel_output_enabled & (1 << (3))))
			{
				*channel = 3;
				*freq = noise_freq[ic->APU.noi.regs[2] & 0x0F] / 32.0f * 783.91;
			}
		}
		break;

		/* DMC */
	case APU_WRE0:
		ic->APU.dpcm.regs[0] = value;
		if (0 == (value & 0x80))
			ic->APU.dpcm.irq_occurred = FALSE;
		break;

	case APU_WRE1: /* 7-bit DAC */
	   //cur->dpcm.regs[1] = value - 0x40;
		ic->APU.dpcm.regs[1] = value & 0x7F;
		ic->APU.dpcm.vol = (ic->APU.dpcm.regs[1] - 64);
		break;

	case APU_WRE2:
		ic->APU.dpcm.regs[2] = value;
		//apu_dpcmreset(cur->dpcm);
	
		if (ic->APU.dpcm.enabled)
		{
			if ((ic->channel_output_enabled & (1 << (4))))
			{
				*channel = 4;
				*freq = dpcm_clocks[ic->APU.dpcm.regs[0] & 0x0F];
			}
		}
		break;

	case APU_WRE3:
		ic->APU.dpcm.regs[3] = value;
		//apu_dpcmreset(cur->dpcm);

		if (ic->APU.dpcm.enabled)
		{
			*channel = 4;
			*freq = dpcm_clocks[ic->APU.dpcm.regs[0] & 0x0F];
		}
		break;

	case APU_SMASK:
		if (value & 0x01)
		{
			ic->APU.squ[0].enabled = TRUE;
		}
		else
		{
			ic->APU.squ[0].enabled = FALSE;
			ic->APU.squ[0].vbl_length = 0;
		}

		if (value & 0x02)
		{
			ic->APU.squ[1].enabled = TRUE;
		}
		else
		{
			ic->APU.squ[1].enabled = FALSE;
			ic->APU.squ[1].vbl_length = 0;
		}

		if (value & 0x04)
		{
			ic->APU.tri.enabled = TRUE;
		}
		else
		{
			ic->APU.tri.enabled = FALSE;
			ic->APU.tri.vbl_length = 0;
			ic->APU.tri.linear_length = 0;
			ic->APU.tri.counter_started = FALSE;
			ic->APU.tri.write_latency = 0;
		}

		if (value & 0x08)
		{
			ic->APU.noi.enabled = TRUE;
		}
		else
		{
			ic->APU.noi.enabled = FALSE;
			ic->APU.noi.vbl_length = 0;
		}

		if (value & 0x10)
		{
			/* only reset dpcm values if DMA is finished */
			if (FALSE == ic->APU.dpcm.enabled)
			{
				ic->APU.dpcm.enabled = TRUE;
				apu_dpcmreset(&ic->APU.dpcm);
			}
		}
		else
		{
			ic->APU.dpcm.enabled = FALSE;
		}
		ic->APU.dpcm.irq_occurred = FALSE;

		break;
	default:
		break;
	}
}

/* UPDATE SOUND BUFFER USING CURRENT DATA */
void apu_update(u8 chipID, s32** bufs, u32 length)
{
	NESAPU *ic = &nesapuChips[chipID];

	for (u32 i = 0; i < length; i++)
	{
		if((ic->channel_output_enabled & (1<<(0)))!=0)
		{
			bufs[((0) << 1) + 0][i] = apu_square(chipID, &ic->APU.squ[0]) << 8;
			bufs[((0) << 1) + 1][i] = bufs[((0) << 1) + 0][i];

		}
		else
		{
			bufs[((0) << 1) + 0][i] = 0;
			bufs[((0) << 1) + 1][i] = 0;
		}
		
		if ((ic->channel_output_enabled & (1 << (1))) != 0)
		{
			bufs[((1) << 1) + 0][i] = apu_square(chipID, &ic->APU.squ[1]) << 8;
			bufs[((1) << 1) + 1][i] = bufs[((1) << 1) + 0][i];
		}
		else
		{
			bufs[((1) << 1) + 0][i] = 0;
			bufs[((1) << 1) + 1][i] = 0;
		}

		if ((ic->channel_output_enabled & (1 << (2))) != 0)
		{
			bufs[((2) << 1) + 0][i] = apu_triangle(chipID, &ic->APU.tri) << 8;
			bufs[((2) << 1) + 1][i] = bufs[((2) << 1) + 0][i];
		}
		else
		{
			bufs[((2) << 1) + 0][i] = 0;
			bufs[((2) << 1) + 1][i] = 0;
		}

		if ((ic->channel_output_enabled & (1 << (3))) != 0)
		{
			bufs[((3) << 1) + 0][i] = apu_noise(chipID, &ic->APU.noi) << 8;
			bufs[((3) << 1) + 1][i] = bufs[((3) << 1) + 0][i];
		}
		else
		{
			bufs[((3) << 1) + 0][i] = 0;
			bufs[((3) << 1) + 1][i] = 0;
		}
		
		if ((ic->channel_output_enabled & (1 << (4))) != 0)
		{
			bufs[((4) << 1) + 0][i] = apu_dpcm(chipID, &ic->APU.dpcm) << 8;
			bufs[((4) << 1) + 1][i] = bufs[((4) << 1) + 0][i];
		}
		else
		{
			bufs[((4) << 1) + 0][i] = 0;
			bufs[((4) << 1) + 1][i] = 0;
		}
	}
}

/* READ VALUES FROM REGISTERS */
u8 apu_read(u8 chipID, u32 address)
{
	NESAPU *ic = &nesapuChips[chipID];

	if (address == 0x0f) /*FIXED* Address $4015 has different behaviour*/
	{
		s32 readval = 0;
		if (ic->APU.dpcm.enabled == TRUE)
		{
			readval |= 0x10;
		}

		if (ic->APU.dpcm.irq_occurred == TRUE)
		{
			readval |= 0x80;
		}
		return readval;
	}
	else
	{
		return ic->APU.regs[address];
	}
}

/* WRITE VALUE TO TEMP REGISTRY AND QUEUE EVENT */
void apu_write(u8 chipID, u32 address, u32 data, s32* channel, f32* freq)
{
	if (address >= 0x17)
		return;

	NESAPU *ic = &nesapuChips[chipID];

	ic->APU.regs[address] = data;
	apu_regwrite(chipID, address, data, channel, freq);
}

s32 NESAPU_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	int a = sizeof(NESAPU);

	NESAPU* ic = &nesapuChips[chipID];
	memset(ic, 0, sizeof(NESAPU));

	/* Initialize global variables */
	int fps = 60;
	u16 samps_per_sync = sampleRate / fps;
	//ic->buffer_size = ic->samps_per_sync;
	u16 real_rate = samps_per_sync * fps;
	ic->apu_incsize = (f32)(clock / (f32)real_rate);

	ic->APU.squ[0].enabled = TRUE;
	ic->APU.squ[1].enabled = TRUE;
	ic->APU.tri.enabled = TRUE;
	ic->APU.noi.enabled = TRUE;
	ic->APU.dpcm.enabled = TRUE;

	NESAPU_Reset(chipID);

	/* Use initializer calls */
	create_noise(ic->noise_lut, 13, NOISE_LONG);
	create_vbltimes(ic->vbl_times, vbl_length, samps_per_sync);
	create_syncs(ic->sync_times1, ic->sync_times2, samps_per_sync);

	/* Adjust buffer size if 16 bits */
	//ic->buffer_size += ic->samps_per_sync;

	/* Initialize individual nesapuChips */
	/* Check for buffer allocation failure and bail out if necessary */
	//ic->APU.buffer = malloc(ic->buffer_size);
	//if (!ic->APU.buffer)
	//{
		//free(ic->APU.buffer);
		//return 0;
	//}

	// set dpcm memory
	// (ic->APU.dpcm).cpu_mem = memory_region(intf->region[i]);

	return -1;
}

void NESAPU_Shutdown(u8 chipID)
{
	NESAPU* ic = &nesapuChips[chipID];

	//if (ic->APU.buffer)
	//{
		//free(ic->APU.buffer);
		//ic->APU.buffer = 0;
	//}

	memset(ic, 0, sizeof(NESAPU));
}

void NESAPU_Reset(u8 chipID)
{
	NESAPU* ic = &nesapuChips[chipID];

	ic->channel_output_enabled = 0xffffffff;
	ic->channel_count = 5;
}

void NESAPU_Update(u8 chipID, s32** bufs, u32 length)
{
	NESAPU* ic = &nesapuChips[chipID];

	//if (ic->real_rate == 0)
		//return;

	apu_update(chipID, bufs, length);
}

void NESAPU_WriteRegister(u8 chipID, u32 address, u32 data, s32* channel, f32* freq)
{
	apu_write(chipID, address, data, channel, freq);
}

u8 NESAPU_ReadRegister(u8 chipID, u32 address)
{
	return apu_read(chipID, address);
}

void NESAPU_SetChannelEnable(u8 chipID, u8 channel, u8 enable)
{
	NESAPU* ic = &nesapuChips[chipID];

	if (enable)
		ic->channel_output_enabled |= (1 << channel);
	else
		ic->channel_output_enabled &= (~(1 << channel));
}

u8 NESAPU_GetChannelEnable(u8 chipID, u8 channel)
{
	NESAPU* ic = &nesapuChips[chipID];

	return (ic->channel_output_enabled & (1 << channel)) !=0;
}

u32 NESAPU_GetChannelCount(u8 chipID)
{
	NESAPU* ic = &nesapuChips[chipID];

	return ic->channel_count;
}