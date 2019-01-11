#include "NESAPU.h"
#include "nes_defs.h"

#define N2A03_DEFAULTCLOCK		 1789772 	/* 1.789772 MHz */		/* pal 1.662607 MHz */

/* GLOBAL CONSTANTS */
#define  SYNCS_MAX1     0x20
#define  SYNCS_MAX2     0x80

/* GLOBAL VARIABLES */
typedef struct
{
	apu_t		APU;							/* Actual APUs */
	f32		apu_incsize;					/* Adjustment increment */
	//u16		samps_per_sync;					/* Number of samples per vsync */
	//u16		buffer_size;					/* Actual buffer size in bytes */
	//u16		real_rate;						/* Actual playback rate */
	u8		noise_lut[NOISE_LONG];			/* Noise sample lookup table */
	u16		vbl_times[0x20];				/* VBL durations in samples */
	u32		sync_times1[SYNCS_MAX1];		/* Samples per sync table */
	u32		sync_times2[SYNCS_MAX2];		/* Samples per sync table */
	s32		channel;
}NESAPU;

#define NESAPU_CHIPS_COUNT 2
NESAPU nesapuChips[NESAPU_CHIPS_COUNT];

/* INTERNAL FUNCTIONS */
/* INITIALIZE NOISE LOOKUP TABLE */
void create_noise(u8 *noise_lut, const s32 bits, s32 size)
{
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
	NESAPU *apu = &nesapuChips[chipID];

	/* reg0: 0-3=volume, 4=envelope, 5=hold, 6-7=duty cycle
	** reg1: 0-2=sweep shifts, 3=sweep inc/dec, 4-6=sweep length, 7=sweep on
	** reg2: 8 bits of freq
	** reg3: 0-2=high freq, 7-4=vbl length counter
	*/

	if (FALSE == chan->enabled)
		return 0;

	/* enveloping */
	env_delay = apu->sync_times1[chan->regs[0] & 0x0F];

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
		sweep_delay = apu->sync_times1[(chan->regs[1] >> 4) & 7];
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

	chan->phaseacc -= apu->apu_incsize; /* # of cycles per sample */

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
	s32 freq;
	s8 output;
	NESAPU *apu = &nesapuChips[chipID];
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

	freq = (((chan->regs[3] & 7) << 8) + chan->regs[2]) + 1;

	if (freq < 4) /* inaudible */
		return 0;

	chan->phaseacc -= apu->apu_incsize; /* # of cycles per sample */
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
	s32 freq, env_delay;
	u8 outvol;
	u8 output;
	NESAPU *apu = &nesapuChips[chipID];

	/* reg0: 0-3=volume, 4=envelope, 5=hold
	** reg2: 7=small(93 byte) sample,3-0=freq lookup
	** reg3: 7-4=vbl length counter
	*/

	if (FALSE == chan->enabled)
		return 0;

	/* enveloping */
	env_delay = apu->sync_times1[chan->regs[0] & 0x0F];

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

	freq = noise_freq[chan->regs[2] & 0x0F];
	chan->phaseacc -= apu->apu_incsize; /* # of cycles per sample */
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

	output = apu->noise_lut[chan->cur_pos];
	if (output > outvol)
		output = outvol;

	if (apu->noise_lut[chan->cur_pos] & 0x80) /* make it negative */
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
	s32 freq, bit_pos;
	NESAPU *apu = &nesapuChips[chipID];

	/* reg0: 7=irq gen, 6=looping, 3-0=pointer to clock table
	** reg1: output dc level, 7 bits unsigned
	** reg2: 8 bits of 64-byte aligned address offset : $C000 + (value * 64)
	** reg3: length, (value * 16) + 1
	*/
	return 0;
	if (chan->enabled)
	{
		freq = dpcm_clocks[chan->regs[0] & 0x0F];
		chan->phaseacc -= apu->apu_incsize; /* # of cycles per sample */

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
void apu_regwrite(u8 chipID, u32 address, u8 value)
{
	NESAPU *apu = &nesapuChips[chipID];
	s32 chan = (address & 4) ? 1 : 0;

	switch (address)
	{
		/* squares */
	case APU_WRA0:
	case APU_WRB0:
		apu->APU.squ[chan].regs[0] = value;
		break;

	case APU_WRA1:
	case APU_WRB1:
		apu->APU.squ[chan].regs[1] = value;
		break;

	case APU_WRA2:
	case APU_WRB2:
		apu->APU.squ[chan].regs[2] = value;
		if (apu->APU.squ[chan].enabled)
			apu->APU.squ[chan].freq = ((((apu->APU.squ[chan].regs[3] & 7) << 8) + value) + 1) << 16;
		break;

	case APU_WRA3:
	case APU_WRB3:
		apu->APU.squ[chan].regs[3] = value;

		if (apu->APU.squ[chan].enabled)
		{
			apu->APU.squ[chan].vbl_length = apu->vbl_times[value >> 3];
			apu->APU.squ[chan].env_vol = 0;
			apu->APU.squ[chan].freq = ((((value & 7) << 8) + apu->APU.squ[chan].regs[2]) + 1) << 16;
		}

		break;

		/* triangle */
	case APU_WRC0:
		apu->APU.tri.regs[0] = value;

		if (apu->APU.tri.enabled)
		{                                          /* ??? */
			if (FALSE == apu->APU.tri.counter_started)
				apu->APU.tri.linear_length = apu->sync_times2[value & 0x7F];
		}

		break;

	case 0x4009:
		/* unused */
		apu->APU.tri.regs[1] = value;
		break;

	case APU_WRC2:
		apu->APU.tri.regs[2] = value;
		break;

	case APU_WRC3:
		apu->APU.tri.regs[3] = value;

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

		apu->APU.tri.write_latency = 3;

		if (apu->APU.tri.enabled)
		{
			apu->APU.tri.counter_started = FALSE;
			apu->APU.tri.vbl_length = apu->vbl_times[value >> 3];
			apu->APU.tri.linear_length = apu->sync_times2[apu->APU.tri.regs[0] & 0x7F];
		}

		break;

		/* noise */
	case APU_WRD0:
		apu->APU.noi.regs[0] = value;
		break;

	case 0x0D:
		/* unused */
		apu->APU.noi.regs[1] = value;
		break;

	case APU_WRD2:
		apu->APU.noi.regs[2] = value;
		break;

	case APU_WRD3:
		apu->APU.noi.regs[3] = value;

		if (apu->APU.noi.enabled)
		{
			apu->APU.noi.vbl_length = apu->vbl_times[value >> 3];
			apu->APU.noi.env_vol = 0; /* reset envelope */
		}
		break;

		/* DMC */
	case APU_WRE0:
		apu->APU.dpcm.regs[0] = value;
		if (0 == (value & 0x80))
			apu->APU.dpcm.irq_occurred = FALSE;
		break;

	case APU_WRE1: /* 7-bit DAC */
	   //cur->dpcm.regs[1] = value - 0x40;
		apu->APU.dpcm.regs[1] = value & 0x7F;
		apu->APU.dpcm.vol = (apu->APU.dpcm.regs[1] - 64);
		break;

	case APU_WRE2:
		apu->APU.dpcm.regs[2] = value;
		//apu_dpcmreset(cur->dpcm);
		break;

	case APU_WRE3:
		apu->APU.dpcm.regs[3] = value;
		//apu_dpcmreset(cur->dpcm);
		break;

	case APU_SMASK:
		if (value & 0x01)
			apu->APU.squ[0].enabled = TRUE;
		else
		{
			apu->APU.squ[0].enabled = FALSE;
			apu->APU.squ[0].vbl_length = 0;
		}

		if (value & 0x02)
			apu->APU.squ[1].enabled = TRUE;
		else
		{
			apu->APU.squ[1].enabled = FALSE;
			apu->APU.squ[1].vbl_length = 0;
		}

		if (value & 0x04)
			apu->APU.tri.enabled = TRUE;
		else
		{
			apu->APU.tri.enabled = FALSE;
			apu->APU.tri.vbl_length = 0;
			apu->APU.tri.linear_length = 0;
			apu->APU.tri.counter_started = FALSE;
			apu->APU.tri.write_latency = 0;
		}

		if (value & 0x08)
			apu->APU.noi.enabled = TRUE;
		else
		{
			apu->APU.noi.enabled = FALSE;
			apu->APU.noi.vbl_length = 0;
		}

		if (value & 0x10)
		{
			/* only reset dpcm values if DMA is finished */
			if (FALSE == apu->APU.dpcm.enabled)
			{
				apu->APU.dpcm.enabled = TRUE;
				apu_dpcmreset(&apu->APU.dpcm);
			}
		}
		else
			apu->APU.dpcm.enabled = FALSE;

		apu->APU.dpcm.irq_occurred = FALSE;

		break;
	default:
		break;
	}
}

/* UPDATE SOUND BUFFER USING CURRENT DATA */
void apu_update(u8 chipID, s32** buffer, u32 samples)
{
	NESAPU *apu = &nesapuChips[chipID];

	s32 accum;
	for (u32 i = 0; i < samples; i++)
	{
		accum = apu_square(chipID, &apu->APU.squ[0]);
		accum += apu_square(chipID, &apu->APU.squ[1]);
		accum += apu_triangle(chipID, &apu->APU.tri);
		accum += apu_noise(chipID, &apu->APU.noi);
		accum += apu_dpcm(chipID, &apu->APU.dpcm);

#ifdef NO_CLAMP
		accum <<= 8;

		buffer[0][i] = accum;
		buffer[1][i] = accum;
#else
		accum <<= 8;
		if (accum > 32767)
			accum = 32767;
		else if (accum < -32768)
			accum = -32768;

		buffer[0][i] = accum;
		buffer[1][i] = accum;
#endif
	}
}

/* READ VALUES FROM REGISTERS */
u8 apu_read(u8 chipID, u32 address)
{
	NESAPU *apu = &nesapuChips[chipID];

	if (address == 0x0f) /*FIXED* Address $4015 has different behaviour*/
	{
		s32 readval = 0;
		if (apu->APU.dpcm.enabled == TRUE)
		{
			readval |= 0x10;
		}

		if (apu->APU.dpcm.irq_occurred == TRUE)
		{
			readval |= 0x80;
		}
		return readval;
	}
	else
	{
		return apu->APU.regs[address];
	}
}

/* WRITE VALUE TO TEMP REGISTRY AND QUEUE EVENT */
void apu_write(u8 chipID, u32 address, u8 value)
{
	if (address >= 0x17)
		return;

	NESAPU *apu = &nesapuChips[chipID];

	apu->APU.regs[address] = value;
	apu_regwrite(chipID, address, value);
}

s32 NESAPU_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	int a = sizeof(NESAPU);

	NESAPU* apu = &nesapuChips[chipID];
	memset(apu, 0, sizeof(NESAPU));

	/* Initialize global variables */
	int fps = 60;
	u16 samps_per_sync = sampleRate / fps;
	//apu->buffer_size = apu->samps_per_sync;
	u16 real_rate = samps_per_sync * fps;
	apu->apu_incsize = (f32)(clock / (f32)real_rate);

	apu->APU.squ[0].enabled = TRUE;
	apu->APU.squ[1].enabled = TRUE;
	apu->APU.tri.enabled = TRUE;
	apu->APU.noi.enabled = TRUE;
	apu->APU.dpcm.enabled = TRUE;

	/* Use initializer calls */
	create_noise(apu->noise_lut, 13, NOISE_LONG);
	create_vbltimes(apu->vbl_times, vbl_length, samps_per_sync);
	create_syncs(apu->sync_times1, apu->sync_times2, samps_per_sync);

	/* Adjust buffer size if 16 bits */
	//apu->buffer_size += apu->samps_per_sync;

	/* Initialize individual nesapuChips */
	/* Check for buffer allocation failure and bail out if necessary */
	//apu->APU.buffer = malloc(apu->buffer_size);
	//if (!apu->APU.buffer)
	//{
		//free(apu->APU.buffer);
		//return 0;
	//}

	// set dpcm memory
	// (apu->APU.dpcm).cpu_mem = memory_region(intf->region[i]);

	return -1;
}

void NESAPU_Shutdown(u8 chipID)
{
	NESAPU* apu = &nesapuChips[chipID];

	//if (apu->APU.buffer)
	//{
		//free(apu->APU.buffer);
		//apu->APU.buffer = 0;
	//}

	memset(apu, 0, sizeof(NESAPU));
}

void NESAPU_Reset(u8 chipID)
{
}

void NESAPU_Update(u8 chipID, s32** buffer, u32 length)
{
	NESAPU* apu = &nesapuChips[chipID];

	//if (apu->real_rate == 0)
		//return;

	apu_update(chipID, buffer, length);
}

void NESAPU_WriteRegister(u8 chipID, u32 address, u8 data)
{
	apu_write(chipID, address, data);
}

u8 NESAPU_ReadRegister(u8 chipID, u32 address)
{
	return apu_read(chipID, address);
}