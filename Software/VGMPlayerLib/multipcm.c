/*
 * Sega System 32 Multi/Model 1/Model 2 custom PCM chip (315-5560) emulation.
 *
 * by Miguel Angel Horna (ElSemi) for Model 2 Emulator and MAME.
 * Information by R.Belmont and the YMF278B (OPL4) manual.
 *
 * voice registers:
 * 0: Pan
 * 1: Index of sample
 * 2: LSB of pitch (low 2 bits seem unused so)
 * 3: MSB of pitch (ooooppppppppppxx) (o=octave (4 bit signed), p=pitch (10 bits), x=unused?
 * 4: voice control: top bit = 1 for key on, 0 for key off
 * 5: bit 0: 0: interpolate volume changes, 1: direct set volume,
	  bits 1-7 = volume attenuate (0=max, 7f=min)
 * 6: LFO frequency + Phase LFO depth
 * 7: Amplitude LFO size
 *
 * The first sample ROM contains a variable length table with 12
 * bytes per instrument/sample. This is very similar to the YMF278B.
 *
 * The first 3 bytes are the offset into the file (big endian).
 * The next 2 are the loop start offset into the file (big endian)
 * The next 2 are the 2's complement of the total sample size (big endian)
 * The next byte is LFO freq + depth (copied to reg 6 ?)
 * The next 3 are envelope params (Attack, Decay1 and 2, sustain level, release, Key Rate Scaling)
 * The next byte is Amplitude LFO size (copied to reg 7 ?)
 *
 * TODO
 * - The YM278B manual states that the chip supports 512 instruments. The MultiPCM probably supports them
 * too but the high bit position is unknown (probably reg 2 low bit). Any game use more than 256?
 *
 */

#include <math.h>
#include "vgmdef.h"
#include "multipcm.h"


#define MULTIPCM_CLIP(A) ((A) < -0x8000 ? -0x8000 : (A) > 0x7fff ? 0x7fff : (A))

typedef struct
{
	void* Data;
	s32 nLen;
	const char* szName;
}ScanMemoryArea;
s32(*BurnAcb) (ScanMemoryArea* pba);

static void ScanVar(void* pv, s32 nSize, const char* szName)
{
	ScanMemoryArea ba;
	memset(&ba, 0, sizeof(ba));
	ba.Data = pv;
	ba.nLen = nSize;
	ba.szName = szName;
	BurnAcb(&ba);
}
#define SCAN_VAR(x) ScanVar(&x, sizeof(x), #x)

//????
#define MULTIPCM_CLOCKDIV       (180.0)

static signed int LPANTABLE[0x800], RPANTABLE[0x800];

#define FIX(v)  ((u32) ((float) (1<<SHIFT)*(v)))

static const int val2chan[] =
{
	0, 1, 2, 3, 4, 5, 6 , -1,
	7, 8, 9, 10,11,12,13, -1,
	14,15,16,17,18,19,20, -1,
	21,22,23,24,25,26,27, -1,
};


#define SHIFT       12


#define MULTIPCM_RATE   44100.0

typedef struct
{
	unsigned int Start;
	unsigned int Loop;
	unsigned int End;
	unsigned char AR, DR1, DR2, DL, RR;
	unsigned char KRS;
	unsigned char LFOVIB;
	unsigned char AM;
}Sample_t;

typedef enum
{
	ATTACK, DECAY1, DECAY2, RELEASE
}STATE;

typedef struct
{
	int volume; //
	STATE state;
	int step;
	//step vals
	int AR;     //Attack
	int D1R;    //Decay1
	int D2R;    //Decay2
	int RR;     //Release
	int DL;     //Decay level
}EG_t;

typedef struct
{
	unsigned short phase;
	u32 phase_step;
	int* table;
	int* scale;
}LFO_t;

typedef struct
{
	unsigned char Num;
	unsigned char Regs[8];
	int Playing;
	Sample_t* Sample;
	unsigned int Base;
	unsigned int offset;
	unsigned int step;
	unsigned int Pan, TL;
	unsigned int DstTL;
	int TLStep;
	signed int Prev;
	EG_t EG;
	LFO_t PLFO; //Phase lfo
	LFO_t ALFO; //AM lfo
}SLOT;

typedef struct {
	Sample_t Samples[0x200];        //Max 512 samples
	SLOT Slots[28];
	unsigned int CurSlot;
	unsigned int Address;
	unsigned int BankR, BankL;
	float Rate;
	unsigned int ARStep[0x40], DRStep[0x40]; //Envelope step table
	unsigned int FNS_Table[0x400];      //Frequency step table
	int mono;

	ROM* m_ROM;
	// s16* mixer_buffer_left;
	// s16* mixer_buffer_right;
	// s32 samples_from; // "re"sampler
	s32 add_to_stream;
	double volume;

	s8				sampleReady;
	u32				channel_output_enabled;
	u32				channel_count;
}MultiPCM;

// dink was here! feb 2020
static MultiPCM multiPCMs[2];

u8 MultiPCMRead(MultiPCM* ic);
void MultiPCMWrite(MultiPCM* ic, s32 offset, u8 data);
void MultiPCMSetBank(MultiPCM* ic, u32 leftoffs, u32 rightoffs);
void MultiPCMSetVolume(MultiPCM* ic, double vol);
void MultiPCMInit(MultiPCM* ic, s32 clock, s32 bAdd);
void MultiPCMSetROM(MultiPCM* ic, ROM* rom);
void MultiPCMExit(MultiPCM* ic);
void MultiPCMReset(MultiPCM* ic);
void MultiPCMSetMono(MultiPCM* ic, s32 ismono);
void MultiPCMUpdate(MultiPCM* ic, s32** buffer, s32 samples);
void MultiPCMScan(MultiPCM* ic, s32 nAction, s32* pnMin);



/*******************************
		ENVELOPE SECTION
*******************************/

//Times are based on a 44100Hz timebase. It's adjusted to the actual sampling rate on startup

static const double BaseTimes[64] = { 0,0,0,0,6222.95,4978.37,4148.66,3556.01,3111.47,2489.21,2074.33,1778.00,1555.74,1244.63,1037.19,889.02,
777.87,622.31,518.59,444.54,388.93,311.16,259.32,222.27,194.47,155.60,129.66,111.16,97.23,77.82,64.85,55.60,
48.62,38.91,32.43,27.80,24.31,19.46,16.24,13.92,12.15,9.75,8.12,6.98,6.08,4.90,4.08,3.49,
3.04,2.49,2.13,1.90,1.72,1.41,1.18,1.04,0.91,0.73,0.59,0.50,0.45,0.45,0.45,0.45 };
#define AR2DR   14.32833
static signed int lin2expvol[0x400];
static int TLSteps[2];

#define EG_SHIFT    16

static int EG_Update(SLOT* slot)
{
	switch (slot->EG.state)
	{
	case ATTACK:
		slot->EG.volume += slot->EG.AR;
		if (slot->EG.volume >= (0x3ff << EG_SHIFT))
		{
			slot->EG.state = DECAY1;
			if (slot->EG.D1R >= (0x400 << EG_SHIFT)) //Skip DECAY1, go directly to DECAY2
				slot->EG.state = DECAY2;
			slot->EG.volume = 0x3ff << EG_SHIFT;
		}
		break;
	case DECAY1:
		slot->EG.volume -= slot->EG.D1R;
		if (slot->EG.volume <= 0)
			slot->EG.volume = 0;
		if (slot->EG.volume >> EG_SHIFT <= (slot->EG.DL << (10 - 4)))
			slot->EG.state = DECAY2;
		break;
	case DECAY2:
		slot->EG.volume -= slot->EG.D2R;
		if (slot->EG.volume <= 0)
			slot->EG.volume = 0;
		break;
	case RELEASE:
		slot->EG.volume -= slot->EG.RR;
		if (slot->EG.volume <= 0)
		{
			slot->EG.volume = 0;
			slot->Playing = 0;
		}
		break;
	default:
		return 1 << SHIFT;
	}
	return lin2expvol[slot->EG.volume >> EG_SHIFT];
}

static unsigned int Get_RATE(unsigned int* Steps, unsigned int rate, unsigned int val)
{
	int r = 4 * val + rate;
	if (val == 0)
		return Steps[0];
	if (val == 0xf)
		return Steps[0x3f];
	if (r > 0x3f)
		r = 0x3f;
	return Steps[r];
}

static void EG_Calc(MultiPCM* ic, SLOT* slot)
{
	int octave = ((slot->Regs[3] >> 4) - 1) & 0xf;
	int rate;
	if (octave & 8) octave = octave - 16;
	if (slot->Sample->KRS != 0xf)
		rate = (octave + slot->Sample->KRS) * 2 + ((slot->Regs[3] >> 3) & 1);
	else
		rate = 0;

	slot->EG.AR = Get_RATE(ic->ARStep, rate, slot->Sample->AR);
	slot->EG.D1R = Get_RATE(ic->DRStep, rate, slot->Sample->DR1);
	slot->EG.D2R = Get_RATE(ic->DRStep, rate, slot->Sample->DR2);
	slot->EG.RR = Get_RATE(ic->DRStep, rate, slot->Sample->RR);
	slot->EG.DL = 0xf - slot->Sample->DL;

}

/*****************************
		LFO  SECTION
*****************************/

#define LFO_SHIFT   8


#define LFIX(v) ((unsigned int) ((float) (1<<LFO_SHIFT)*(v)))

//Convert DB to multiply amplitude
#define DB(v)   LFIX(pow(10.0,v/20.0))

//Convert cents to step increment
#define CENTS(v) LFIX(pow(2.0,v/1200.0))

static int PLFO_TRI[256];
static int ALFO_TRI[256];

static const float LFOFreq[8] = { 0.168f,2.019f,3.196f,4.206f,5.215f,5.888f,6.224f,7.066f };    //Hz;
static const float PSCALE[8] = { 0.0f,3.378f,5.065f,6.750f,10.114f,20.170f,40.180f,79.307f };   //cents
static const float ASCALE[8] = { 0.0f,0.4f,0.8f,1.5f,3.0f,6.0f,12.0f,24.0f };                   //DB
static int PSCALES[8][256];
static int ASCALES[8][256];

static void LFO_Init(void)
{
	int i, s;
	for (i = 0; i < 256; ++i)
	{
		int a;  //amplitude
		int p;  //phase

		//Tri
		if (i < 128)
			a = 255 - (i * 2);
		else
			a = (i * 2) - 256;
		if (i < 64)
			p = i * 2;
		else if (i < 128)
			p = 255 - i * 2;
		else if (i < 192)
			p = 256 - i * 2;
		else
			p = i * 2 - 511;
		ALFO_TRI[i] = a;
		PLFO_TRI[i] = p;
	}

	for (s = 0; s < 8; ++s)
	{
		float limit = PSCALE[s];
		for (i = -128; i < 128; ++i)
		{
			PSCALES[s][i + 128] = CENTS(((limit * (float)i) / 128.0));
		}
		limit = -ASCALE[s];
		for (i = 0; i < 256; ++i)
		{
			ASCALES[s][i] = DB(((limit * (float)i) / 256.0));
		}
	}
}

#define INLINE static inline

INLINE signed int PLFO_Step(LFO_t* LFO)
{
	int p;
	LFO->phase += LFO->phase_step;
	p = LFO->table[(LFO->phase >> LFO_SHIFT) & 0xff];
	p = LFO->scale[p + 128];
	return p << (SHIFT - LFO_SHIFT);
}

INLINE signed int ALFO_Step(LFO_t* LFO)
{
	int p;
	LFO->phase += LFO->phase_step;
	p = LFO->table[(LFO->phase >> LFO_SHIFT) & 0xff];
	p = LFO->scale[p];
	return p << (SHIFT - LFO_SHIFT);
}

static void LFO_ComputeStep(MultiPCM* ic, LFO_t* LFO, u32 LFOF, u32 LFOS, int ALFO)
{
	float step = (float)LFOFreq[LFOF] * 256.0 / (float)ic->Rate;
	LFO->phase_step = (unsigned int)((float)(1 << LFO_SHIFT) * step);
	if (ALFO)
	{
		LFO->table = ALFO_TRI;
		LFO->scale = ASCALES[LFOS];
	}
	else
	{
		LFO->table = PLFO_TRI;
		LFO->scale = PSCALES[LFOS];
	}
}

static void WriteSlot(MultiPCM* ic, SLOT* slot, int reg, unsigned char data)
{
	slot->Regs[reg] = data;

	switch (reg)
	{
	case 0: //PANPOT
		slot->Pan = (data >> 4) & 0xf;
		break;
	case 1: //Sample
		//according to YMF278 sample write causes some base params written to the regs (envelope+lfos)
		//the game should never change the sample while playing.  "sure about that?" -dink
	{
		Sample_t* Sample = ic->Samples + slot->Regs[1];

		slot->Sample = ic->Samples + slot->Regs[1];
		slot->Base = slot->Sample->Start;
		slot->offset = 0;
		slot->Prev = 0;
		slot->TL = slot->DstTL << SHIFT;

		EG_Calc(ic, slot);
		slot->EG.state = ATTACK;
		slot->EG.volume = 0;

		if (slot->Base >= 0x100000)
		{
			if (slot->Pan & 8)
				slot->Base = (slot->Base & 0xfffff) | (ic->BankL);
			else
				slot->Base = (slot->Base & 0xfffff) | (ic->BankR);
		}

		WriteSlot(ic, slot, 6, Sample->LFOVIB);
		WriteSlot(ic, slot, 7, Sample->AM);
	}
	break;
	case 2: //Pitch
	case 3:
	{
		unsigned int oct = ((slot->Regs[3] >> 4) - 1) & 0xf;
		unsigned int pitch = ((slot->Regs[3] & 0xf) << 6) | (slot->Regs[2] >> 2);
		pitch = ic->FNS_Table[pitch];
		if (oct & 0x8)
			pitch >>= (16 - oct);
		else
			pitch <<= oct;
		slot->step = pitch / ic->Rate;
	}
	break;
	case 4:     //KeyOn/Off (and more?)
	{
		if (data & 0x80)       //KeyOn
		{
			slot->Playing = 1;
		}
		else
		{
			if (slot->Playing)
			{
				if (slot->Sample->RR != 0xf)
					slot->EG.state = RELEASE;
				else
					slot->Playing = 0;
			}
		}
	}
	break;
	case 5: //TL+Interpolation
	{
		slot->DstTL = (data >> 1) & 0x7f;
		if (!(data & 1))   //Interpolate TL
		{
			if ((slot->TL >> SHIFT) > slot->DstTL)
				slot->TLStep = TLSteps[0];        //decrease
			else
				slot->TLStep = TLSteps[1];        //increase
		}
		else
			slot->TL = slot->DstTL << SHIFT;
	}
	break;
	case 6: //LFO freq+PLFO
	{
		if (data)
		{
			LFO_ComputeStep(ic, &(slot->PLFO), (slot->Regs[6] >> 3) & 7, slot->Regs[6] & 7, 0);
			LFO_ComputeStep(ic, &(slot->ALFO), (slot->Regs[6] >> 3) & 7, slot->Regs[7] & 7, 1);
		}
	}
	break;
	case 7: //ALFO
	{
		if (data)
		{
			LFO_ComputeStep(ic, &(slot->PLFO), (slot->Regs[6] >> 3) & 7, slot->Regs[6] & 7, 0);
			LFO_ComputeStep(ic, &(slot->ALFO), (slot->Regs[6] >> 3) & 7, slot->Regs[7] & 7, 1);
		}
	}
	break;
	}
}

void MultiPCMSetUpSample(MultiPCM* ic)
{
	if (!ic->sampleReady)
	{
		for (int i = 0; i < 512; ++i)
		{
			u8 ptSample[12];

			for (int j = 0; j < 12; j++)
			{
				ptSample[j] = ROM_getU8(ic->m_ROM, ((i * 12) + j) & 0x3fffff);;
			}

			ic->Samples[i].Start = (ptSample[0] << 16) | (ptSample[1] << 8) | (ptSample[2] << 0);
			ic->Samples[i].Loop = (ptSample[3] << 8) | (ptSample[4] << 0);
			ic->Samples[i].End = 0xffff - ((ptSample[5] << 8) | (ptSample[6] << 0));
			ic->Samples[i].LFOVIB = ptSample[7];
			ic->Samples[i].DR1 = ptSample[8] & 0xf;
			ic->Samples[i].AR = (ptSample[8] >> 4) & 0xf;
			ic->Samples[i].DR2 = ptSample[9] & 0xf;
			ic->Samples[i].DL = (ptSample[9] >> 4) & 0xf;
			ic->Samples[i].RR = ptSample[10] & 0xf;
			ic->Samples[i].KRS = (ptSample[10] >> 4) & 0xf;
			ic->Samples[i].AM = ptSample[11];
		}

		ic->sampleReady = -1;
	}
}

u8 MultiPCMRead(MultiPCM* ic)
{
	return 0;
}

void MultiPCMWrite(MultiPCM* ic, s32 offset, u8 data)
{
	MultiPCMSetUpSample(ic);

	switch (offset)
	{
	case 0:     //Data write
		WriteSlot(ic, ic->Slots + ic->CurSlot, ic->Address, data);
		break;

	case 1:
		ic->CurSlot = val2chan[data & 0x1f];
		break;

	case 2:
		ic->Address = (data > 7) ? 7 : data;
		break;
	}
}

/* MAME/M1 access functions */

void MultiPCMSetBank(MultiPCM* ic, u32 leftoffs, u32 rightoffs)
{
	ic->BankL = leftoffs;
	ic->BankR = rightoffs;
}

//	AM_RANGE(0x000000, 0x3fffff) AM_ROM

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void MultiPCMExit(MultiPCM* ic)
{
	//if (ic->mixer_buffer_left) {
		//free(ic->mixer_buffer_left);
		//ic->mixer_buffer_left = NULL;
		//ic->mixer_buffer_right = NULL;
	//}
	ic->m_ROM = NULL;
}

void MultiPCMScan(MultiPCM* ic, s32 nAction, s32* pnMin)
{
	SCAN_VAR(ic);
}

void MultiPCMReset(MultiPCM* ic)
{
	for (s32 i = 0; i < 28; i++) {
		ic->Slots[i].Num = i;
		ic->Slots[i].Playing = 0;
	}

	ic->channel_output_enabled = 0xffffffff;
}

void MultiPCMSetVolume(MultiPCM* ic, double vol)
{
	ic->volume = vol;
}

void MultiPCMInit(MultiPCM* ic, s32 clock, s32 bAdd)
{
	int i;

	memset(ic, 0, sizeof(MultiPCM)); // start fresh

	s32 fps = 60;
	s32 nBurnFPS = fps * 100;

	ic->Rate = (float)clock / MULTIPCM_CLOCKDIV;
	// ic->samples_from = (s32)((double)((ic->Rate * 100) / 60.0f) + 0.5);
	ic->add_to_stream = bAdd;

	// ic->mixer_buffer_left = (s16*)malloc(2 * sizeof(s16) * ic->Rate);
	// ic->mixer_buffer_right = ic->mixer_buffer_left + ((s32)ic->Rate);

	ic->volume = 1.00;

	//Volume+pan table
	for (i = 0; i < 0x800; ++i)
	{
		float SegaDB = 0;
		float TL;
		float LPAN, RPAN;

		unsigned char iTL = i & 0x7f;
		unsigned char iPAN = (i >> 7) & 0xf;

		SegaDB = (float)iTL * (-24.0) / (float)0x40;

		TL = pow(10.0, SegaDB / 20.0);


		if (iPAN == 0x8)
		{
			LPAN = RPAN = 0.0;
		}
		else if (iPAN == 0x0)
		{
			LPAN = RPAN = 1.0;
		}
		else if (iPAN & 0x8)
		{
			LPAN = 1.0;

			iPAN = 0x10 - iPAN;

			SegaDB = (float)iPAN * (-12.0) / (float)0x4;

			RPAN = pow(10.0, SegaDB / 20.0);

			if ((iPAN & 0x7) == 7)
				RPAN = 0.0;
		}
		else
		{
			RPAN = 1.0;

			SegaDB = (float)iPAN * (-12.0) / (float)0x4;

			LPAN = pow(10.0, SegaDB / 20.0);
			if ((iPAN & 0x7) == 7)
				LPAN = 0.0;
		}

		TL /= 4.0;

		LPANTABLE[i] = FIX((LPAN * TL));
		RPANTABLE[i] = FIX((RPAN * TL));
	}

	//Pitch steps
	for (i = 0; i < 0x400; ++i)
	{
		float fcent = ic->Rate * (1024.0 + (float)i) / 1024.0;
		ic->FNS_Table[i] = (unsigned int)((float)(1 << SHIFT) * fcent);
	}

	//Envelope steps
	for (i = 0; i < 0x40; ++i)
	{
		//Times are based on 44100 clock, adjust to real chip clock
		ic->ARStep[i] = (float)(0x400 << EG_SHIFT) / (BaseTimes[i] * 44100.0 / (1000.0));
		ic->DRStep[i] = (float)(0x400 << EG_SHIFT) / (BaseTimes[i] * AR2DR * 44100.0 / (1000.0));
	}
	ic->ARStep[0] = ic->ARStep[1] = ic->ARStep[2] = ic->ARStep[3] = 0;
	ic->ARStep[0x3f] = 0x400 << EG_SHIFT;
	ic->DRStep[0] = ic->DRStep[1] = ic->DRStep[2] = ic->DRStep[3] = 0;

	//TL Interpolation steps
	//lower
	TLSteps[0] = -(float)(0x80 << SHIFT) / (78.2 * 44100.0 / 1000.0);
	//raise
	TLSteps[1] = (float)(0x80 << SHIFT) / (78.2 * 2 * 44100.0 / 1000.0);

	//build the linear->exponential ramps
	for (i = 0; i < 0x400; ++i)
	{
		float db = -(96.0 - (96.0 * (float)i / (float)0x400));
		lin2expvol[i] = pow(10.0, db / 20.0) * (float)(1 << SHIFT);
	}

	ic->channel_count = 1;
	ic->channel_output_enabled = 0xffffffff;

	LFO_Init();

	MultiPCMReset(ic);
}

void MultiPCMSetMono(MultiPCM* ic, s32 ismono)
{
	ic->mono = (ismono) ? 1 : 0;
}

void MultiPCMSetROM(MultiPCM* ic, ROM* rom)
{
	ic->m_ROM = rom;
}

//-------------------------------------------------
//  sound_stream_update - handle a stream update
//-------------------------------------------------

void MultiPCMUpdate(MultiPCM* ic, s32** buffer, s32 samples_len)
{
	//s32 samples = (ic->samples_from * samples_len) / VGM_SAMPLE_BUFFER_SIZE;
	//s16* lmix = ic->mixer_buffer_left;
	//s16* rmix = ic->mixer_buffer_right;

	MultiPCMSetUpSample(ic);
	s32 samples = samples_len;
	s32* lmix = buffer[0];
	s32* rmix = buffer[1];

	for (s32 i = 0; i < samples; i++)
	{
		signed int smpl = 0;
		signed int smpr = 0;
		for (s32 sl = 0; sl < 28; sl++)
		{
			SLOT* slot = ic->Slots + sl;
			if (slot->Playing)
			{
				unsigned int vol = (slot->TL >> SHIFT) | (slot->Pan << 7);
				unsigned int adr = slot->offset >> SHIFT;
				signed int sample;
				unsigned int step = slot->step;
				//signed int csample = (signed short)(m_ROM[(slot->Base + adr) & 0x3fffff] << 8);
				signed int csample = ROM_getS16(ic->m_ROM, (slot->Base + adr) & 0x3fffff) << 8;

				signed int fpart = slot->offset & ((1 << SHIFT) - 1);
				sample = (csample * fpart + slot->Prev * ((1 << SHIFT) - fpart)) >> SHIFT;

				if (slot->Regs[6] & 7) //Vibrato enabled
				{
					step = step * PLFO_Step(&(slot->PLFO));
					step >>= SHIFT;
				}

				slot->offset += step;
				if (slot->offset >= (slot->Sample->End << SHIFT))
				{
					slot->offset = slot->Sample->Loop << SHIFT;
				}
				if (adr ^ (slot->offset >> SHIFT))
				{
					slot->Prev = csample;
				}

				if ((slot->TL >> SHIFT) != slot->DstTL)
					slot->TL += slot->TLStep;

				if (slot->Regs[7] & 7) //Tremolo enabled
				{
					sample = sample * ALFO_Step(&(slot->ALFO));
					sample >>= SHIFT;
				}

				sample = (sample * EG_Update(slot)) >> 10;

				smpl += (LPANTABLE[vol] * sample) >> SHIFT;
				smpr += (RPANTABLE[vol] * sample) >> SHIFT;
			}
		}

		if (ic->mono) {
			smpl = smpr;
		}

		smpl *= 0.01;
		smpr *= 0.01;

		buffer[0][i] = MULTIPCM_CLIP(smpl);
		//buffer[1][i] = MULTIPCM_CLIP(smpr);
		// *(lmix++) = MULTIPCM_CLIP(smpl); // why are these swapped??
		// *(rmix++) = MULTIPCM_CLIP(smpr);
	}

	/*
	// ghetto-tek resampler, #1 in the hood, g!
	lmix = ic->mixer_buffer_left;
	rmix = ic->mixer_buffer_right;

	for (s32 j = 0; j < samples_len; j++)
	{
		s32 k = (ic->samples_from * j) / VGM_SAMPLE_BUFFER_SIZE;

		s32 l = (ic->add_to_stream) ? (lmix[k] * ic->volume) + buffer[0] : (lmix[k] * ic->volume);
		s32 r = (ic->add_to_stream) ? (rmix[k] * ic->volume) + buffer[1] : (rmix[k] * ic->volume);
		
		buffer[0] = BURN_SND_CLIP(l);
		buffer[1] = BURN_SND_CLIP(r);
		buffer += 2;
	}
	*/
}


///////////////////////////////////////////////////////////////////////////////////////////////
s32 MultiPCM_Initialize(u8 chipID, u32 clock, u32 sampleRate)
{
	MultiPCM* ic = &multiPCMs[chipID];
	memset(ic, 0, sizeof(MultiPCM));

	MultiPCMInit(ic, clock, 0);

	return -1;
}

void MultiPCM_SetROM(u8 chipID, ROM* rom)
{
	MultiPCM* ic = &multiPCMs[chipID];

	MultiPCMSetROM(ic, rom);
}

void MultiPCM_Shutdown(u8 chipID)
{
	MultiPCM* ic = &multiPCMs[chipID];

	MultiPCMExit(ic);
}

void MultiPCM_Reset(u8 chipID)
{
	MultiPCM* ic = &multiPCMs[chipID];

	MultiPCMReset(ic);
}

void MultiPCM_WriteRegister(u8 chipID, u32 address, u32 data, s32* channel, f32* freq)
{
	MultiPCM* ic = &multiPCMs[chipID];

	MultiPCMWrite(ic, address, data);
}

u8 MultiPCM_ReadRegister(u8 chipID, u32 address)
{
	MultiPCM* ic = &multiPCMs[chipID];

	return MultiPCMRead(ic);
}

void MultiPCM_Update(u8 chipID, s32** buffer, u32 length)
{
	MultiPCM* ic = &multiPCMs[chipID];

	MultiPCMUpdate(ic, buffer, length);
}

void MultiPCM_SetChannelEnable(u8 chipID, u8 channel, u8 enable)
{
	MultiPCM* ic = &multiPCMs[chipID];

	if (enable)
		ic->channel_output_enabled |= (1 << channel);
	else
		ic->channel_output_enabled &= (~(1 << channel));
}

u8 MultiPCM_GetChannelEnable(u8 chipID, u8 channel)
{
	MultiPCM* ic = &multiPCMs[chipID];

	return (ic->channel_output_enabled & (1 << channel)) != 0;
}

u32 MultiPCM_GetChannelCount(u8 chipID)
{
	MultiPCM* ic = &multiPCMs[chipID];

	return ic->channel_count;
}

void MultiPCM_SetBank(u8 chipID, u32 leftoffs, u32 rightoffs)
{
	MultiPCM* ic = &multiPCMs[chipID];

	MultiPCMSetBank(ic, leftoffs, rightoffs);
}