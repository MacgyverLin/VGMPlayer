#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ym2612.h"

//////////////////////////////////////////////////////////////
static YM2612 *YM2612Chips = NULL;					// array of YM2612
static unsigned int YM2612NumChips;					// total # of YM2612 emulated
int YM2612_Enable_SSGEG = 0; // enable SSG-EG envelope (causes inacurate sound sometimes - rodrigo)

void Update_Chan_Algo0(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo1(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo2(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo3(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo4(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo5(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo6(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo7(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);

void Update_Chan_Algo0_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo1_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo2_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo3_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo4_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo5_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo6_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo7_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);

void Update_Chan_Algo0_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo1_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo2_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo3_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo4_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo5_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo6_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo7_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);

void Update_Chan_Algo0_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo1_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo2_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo3_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo4_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo5_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo6_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);
void Update_Chan_Algo7_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght);

void Env_Attack_Next(slot_ *SL);
void Env_Decay_Next(slot_ *SL);
void Env_Substain_Next(slot_ *SL);
void Env_Release_Next(slot_ *SL);
void Env_NULL_Next(slot_ *SL);

// Update Channel functions pointer table
const void(*UPDATE_CHAN[8 * 8])(YM2612 *ym2612, channel_ *CH, int **buf, int lenght) =
{
	Update_Chan_Algo0,
	Update_Chan_Algo1,
	Update_Chan_Algo2,
	Update_Chan_Algo3,
	Update_Chan_Algo4,
	Update_Chan_Algo5,
	Update_Chan_Algo6,
	Update_Chan_Algo7,

	Update_Chan_Algo0_LFO,
	Update_Chan_Algo1_LFO,
	Update_Chan_Algo2_LFO,
	Update_Chan_Algo3_LFO,
	Update_Chan_Algo4_LFO,
	Update_Chan_Algo5_LFO,
	Update_Chan_Algo6_LFO,
	Update_Chan_Algo7_LFO,

	Update_Chan_Algo0_Int,
	Update_Chan_Algo1_Int,
	Update_Chan_Algo2_Int,
	Update_Chan_Algo3_Int,
	Update_Chan_Algo4_Int,
	Update_Chan_Algo5_Int,
	Update_Chan_Algo6_Int,
	Update_Chan_Algo7_Int,

	Update_Chan_Algo0_LFO_Int,
	Update_Chan_Algo1_LFO_Int,
	Update_Chan_Algo2_LFO_Int,
	Update_Chan_Algo3_LFO_Int,
	Update_Chan_Algo4_LFO_Int,
	Update_Chan_Algo5_LFO_Int,
	Update_Chan_Algo6_LFO_Int,
	Update_Chan_Algo7_LFO_Int
};

// Next Enveloppe phase functions pointer table
const void(*ENV_NEXT_EVENT[8])(slot_ *SL) =
{
	Env_Attack_Next,
	Env_Decay_Next,
	Env_Substain_Next,
	Env_Release_Next,
	Env_NULL_Next,
	Env_NULL_Next,
	Env_NULL_Next,
	Env_NULL_Next
};

const unsigned int DT_DEF_TAB[4 * 32] =
{
	// FD = 0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	// FD = 1
	0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
	2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8, 8,

	// FD = 2
	1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5,
	5, 6, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 16, 16, 16, 16,

	// FD = 3
	2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7,
	8 , 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 20, 22, 22, 22, 22
};

const unsigned int FKEY_TAB[16] =
{
	0, 0, 0, 0,
	0, 0, 0, 1,
	2, 3, 3, 3,
	3, 3, 3, 3
};

const unsigned int LFO_AMS_TAB[4] =
{
	31, 4, 1, 0
};

const unsigned int LFO_FMS_TAB[8] =
{
	LFO_FMS_BASE * 0, LFO_FMS_BASE * 1,
	LFO_FMS_BASE * 2, LFO_FMS_BASE * 3,
	LFO_FMS_BASE * 4, LFO_FMS_BASE * 6,
	LFO_FMS_BASE * 12, LFO_FMS_BASE * 24
};

void CALC_FINC_SL(YM2612 *ym2612, slot_ *SL, int finc, int kc)
{
	int ksr;
	

	SL->Finc = (finc + SL->DT[kc]) * SL->MUL;

	ksr = kc >> SL->KSR_S;  // keycode atténuation

#if YM_DEBUG_LEVEL > 1
	fprintf(debug_file, "FINC = %d  SL->Finc = %d\n", finc, SL->Finc);
#endif

	if (SL->KSR != ksr)      // si le KSR a chang?alors
	{            // les différents taux pour l'enveloppe sont mis ?jour
		SL->KSR = ksr;

		SL->EincA = SL->AR[ksr];
		SL->EincD = SL->DR[ksr];
		SL->EincS = SL->SR[ksr];
		SL->EincR = SL->RR[ksr];

		if (SL->Ecurp == ATTACK) SL->Einc = SL->EincA;
		else if (SL->Ecurp == DECAY) SL->Einc = SL->EincD;
		else if (SL->Ecnt < ENV_END)
		{
			if (SL->Ecurp == SUBSTAIN) SL->Einc = SL->EincS;
			else if (SL->Ecurp == RELEASE) SL->Einc = SL->EincR;
		}
	}
}

void CALC_FINC_CH(YM2612 *ym2612, channel_ *CH)
{
	int finc, kc;
	

	finc = ym2612->FINC_TAB[CH->FNUM[0]] >> (7 - CH->FOCT[0]);
	kc = CH->KC[0];

	CALC_FINC_SL(ym2612, &CH->SLOT[0], finc, kc);
	CALC_FINC_SL(ym2612, &CH->SLOT[1], finc, kc);
	CALC_FINC_SL(ym2612, &CH->SLOT[2], finc, kc);
	CALC_FINC_SL(ym2612, &CH->SLOT[3], finc, kc);
}

void KEY_ON(YM2612 *ym2612, channel_ *CH, int nsl)
{
	slot_ *SL = &(CH->SLOT[nsl]);  // on recupère le bon pointeur de slot

	if (SL->Ecurp == RELEASE)    // la touche est-elle relâchée ?
	{
		SL->Fcnt = 0;

		// Fix Ecco 2 splash sound

		SL->Ecnt = (ym2612->DECAY_TO_ATTACK[ym2612->ENV_TAB[SL->Ecnt >> ENV_LBITS]] + ENV_ATTACK) & SL->ChgEnM;
		SL->ChgEnM = 0xFFFFFFFF;

		//    SL->Ecnt = DECAY_TO_ATTACK[ENV_TAB[SL->Ecnt >> ENV_LBITS]] + ENV_ATTACK;
		//    SL->Ecnt = 0;

		SL->Einc = SL->EincA;
		SL->Ecmp = ENV_DECAY;
		SL->Ecurp = ATTACK;
	}
}

void KEY_OFF(YM2612 *ym2612, channel_ *CH, int nsl)
{
	slot_ *SL = &(CH->SLOT[nsl]);  // on recupère le bon pointeur de slot

	if (SL->Ecurp != RELEASE)    // la touche est-elle appuyée ?
	{
		if (SL->Ecnt < ENV_DECAY)  // attack phase ?
		{
			SL->Ecnt = (ym2612->ENV_TAB[SL->Ecnt >> ENV_LBITS] << ENV_LBITS) + ENV_DECAY;
		}

		SL->Einc = SL->EincR;
		SL->Ecmp = ENV_END;
		SL->Ecurp = RELEASE;
	}
}

void CSM_Key_Control(YM2612 *ym2612)
{
	KEY_ON(ym2612, &ym2612->CHANNEL[2], 0);
	KEY_ON(ym2612, &ym2612->CHANNEL[2], 1);
	KEY_ON(ym2612, &ym2612->CHANNEL[2], 2);
	KEY_ON(ym2612, &ym2612->CHANNEL[2], 3);
}

int SLOT_SET(YM2612 *ym2612, int Adr, unsigned char data)
{
	channel_ *CH;
	slot_ *SL;
	int nch, nsl;

	if ((nch = Adr & 3) == 3)
		return 1;
	nsl = (Adr >> 2) & 3;

	if (Adr & 0x100)
		nch += 3;

	CH = &(ym2612->CHANNEL[nch]);
	SL = &(CH->SLOT[nsl]);

	switch (Adr & 0xF0)
	{
	case 0x30:
		if (SL->MUL = (data & 0x0F))
			SL->MUL <<= 1;
		else
			SL->MUL = 1;

		SL->DT = ym2612->DT_TAB[(data >> 4) & 7];

		CH->SLOT[0].Finc = -1;

		break;

	case 0x40:
		SL->TL = data & 0x7F;

		// SOR2 do a lot of TL adjustement and this fix R.Shinobi jump sound...
		// YM2612_Special_Update();

#if((ENV_HBITS - 7) < 0)
		SL->TLL = SL->TL >> (7 - ENV_HBITS);
#else
		SL->TLL = SL->TL << (ENV_HBITS - 7);
#endif
		break;

	case 0x50:
		SL->KSR_S = 3 - (data >> 6);

		CH->SLOT[0].Finc = -1;

		if (data &= 0x1F)
			SL->AR = &ym2612->AR_TAB[data << 1];
		else
			SL->AR = &ym2612->NULL_RATE[0];

		SL->EincA = SL->AR[SL->KSR];
		if (SL->Ecurp == ATTACK)
			SL->Einc = SL->EincA;

		break;

	case 0x60:
		if (SL->AMSon = (data & 0x80))
			SL->AMS = CH->AMS;
		else
			SL->AMS = 31;

		if (data &= 0x1F)
			SL->DR = &ym2612->DR_TAB[data << 1];
		else
			SL->DR = &ym2612->NULL_RATE[0];

		SL->EincD = SL->DR[SL->KSR];
		if (SL->Ecurp == DECAY)
			SL->Einc = SL->EincD;

		break;

	case 0x70:
		if (data &= 0x1F)
			SL->SR = &ym2612->DR_TAB[data << 1];
		else
			SL->SR = &ym2612->NULL_RATE[0];

		SL->EincS = SL->SR[SL->KSR];
		if ((SL->Ecurp == SUBSTAIN) && (SL->Ecnt < ENV_END))
			SL->Einc = SL->EincS;

		break;

	case 0x80:
		SL->SLL = ym2612->SL_TAB[data >> 4];

		SL->RR = &ym2612->DR_TAB[((data & 0xF) << 2) + 2];

		SL->EincR = SL->RR[SL->KSR];
		if ((SL->Ecurp == RELEASE) && (SL->Ecnt < ENV_END))
			SL->Einc = SL->EincR;

		break;

	case 0x90:
		// SSG-EG envelope shapes :
		//
		// E  At Al H
		//
		// 1  0  0  0  \\\\
        //
		// 1  0  0  1  \___
		//
		// 1  0  1  0  \/\/
		//              ___
		// 1  0  1  1  \
        //
		// 1  1  0  0  ////
		//              ___
		// 1  1  0  1  /
		//
		// 1  1  1  0  /\/\
        //
		// 1  1  1  1  /___
		//
		// E  = SSG-EG enable
		// At = Start negate
		// Al = Altern
		// H  = Hold
		if (YM2612_Enable_SSGEG)
		{
			if (data & 0x08)
				SL->SEG = data & 0x0F;
			else
				SL->SEG = 0;
		}
		break;
	}

	return 0;
}

int CHANNEL_SET(YM2612 *ym2612, int Adr, unsigned char data)
{
	channel_ *CH;
	int chnIdx;
	
	if ((chnIdx = Adr & 3) == 3) 
		return 1;

	switch (Adr & 0xFC)
	{
	case 0xA0:
		if (Adr & 0x100)
			chnIdx += 3;

		CH = &(ym2612->CHANNEL[chnIdx]);
		CH->FNUM[0] = (CH->FNUM[0] & 0x700) + data;
		CH->KC[0] = (CH->FOCT[0] << 2) | FKEY_TAB[CH->FNUM[0] >> 7];

		CH->SLOT[0].Finc = -1;
		break;

	case 0xA4:
		if (Adr & 0x100)
			chnIdx += 3;

		CH = &(ym2612->CHANNEL[chnIdx]);
		CH->FNUM[0] = (CH->FNUM[0] & 0x0FF) + ((int)(data & 0x07) << 8);
		CH->FOCT[0] = (data & 0x38) >> 3;
		CH->KC[0] = (CH->FOCT[0] << 2) | FKEY_TAB[CH->FNUM[0] >> 7];

		CH->SLOT[0].Finc = -1;
		break;

	case 0xA8:
		if (Adr < 0x100)
		{
			chnIdx++;

			ym2612->CHANNEL[2].FNUM[chnIdx] = (ym2612->CHANNEL[2].FNUM[chnIdx] & 0x700) + data;
			ym2612->CHANNEL[2].KC[chnIdx] = (ym2612->CHANNEL[2].FOCT[chnIdx] << 2) | FKEY_TAB[ym2612->CHANNEL[2].FNUM[chnIdx] >> 7];
			ym2612->CHANNEL[2].SLOT[0].Finc = -1;
		}
		break;

	case 0xAC:
		if (Adr < 0x100)
		{
			chnIdx++;

			ym2612->CHANNEL[2].FNUM[chnIdx] = (ym2612->CHANNEL[2].FNUM[chnIdx] & 0x0FF) + ((int)(data & 0x07) << 8);
			ym2612->CHANNEL[2].FOCT[chnIdx] = (data & 0x38) >> 3;
			ym2612->CHANNEL[2].KC[chnIdx] = (ym2612->CHANNEL[2].FOCT[chnIdx] << 2) | FKEY_TAB[ym2612->CHANNEL[2].FNUM[chnIdx] >> 7];
			ym2612->CHANNEL[2].SLOT[0].Finc = -1;
		}
		break;

	case 0xB0:
		if (Adr & 0x100)
			chnIdx += 3;

		CH = &(ym2612->CHANNEL[chnIdx]);
		if (CH->ALGO != (data & 7))
		{
			// Fix VectorMan 2 heli sound (level 1)
			// YM2612_Special_Update();

			CH->ALGO = data & 7;

			CH->SLOT[0].ChgEnM = 0;
			CH->SLOT[1].ChgEnM = 0;
			CH->SLOT[2].ChgEnM = 0;
			CH->SLOT[3].ChgEnM = 0;
		}

		CH->FB = 9 - ((data >> 3) & 7);                // Real thing ?
		break;

	case 0xB4:
		if (Adr & 0x100)
			chnIdx += 3;

		CH = &(ym2612->CHANNEL[chnIdx]);

		if (data & 0x80)
			CH->LEFT = 0xFFFFFFFF;
		else
			CH->LEFT = 0;

		if (data & 0x40)
			CH->RIGHT = 0xFFFFFFFF;
		else
			CH->RIGHT = 0;

		CH->AMS = LFO_AMS_TAB[(data >> 4) & 3];
		CH->FMS = LFO_FMS_TAB[data & 7];

		if (CH->SLOT[0].AMSon)
			CH->SLOT[0].AMS = CH->AMS;
		else
			CH->SLOT[0].AMS = 31;

		if (CH->SLOT[1].AMSon)
			CH->SLOT[1].AMS = CH->AMS;
		else
			CH->SLOT[1].AMS = 31;
		if (CH->SLOT[2].AMSon)
			CH->SLOT[2].AMS = CH->AMS;
		else
			CH->SLOT[2].AMS = 31;
		if (CH->SLOT[3].AMSon)
			CH->SLOT[3].AMS = CH->AMS;
		else
			CH->SLOT[3].AMS = 31;

		break;
	}

	return 0;
}

int YM_SET(YM2612 *ym2612, int Adr, unsigned char data)
{
	channel_ *CH;
	int nch;
	

	switch (Adr)
	{
	case 0x22:
		if (data & 8)
		{
			ym2612->LFOinc = ym2612->LFO_INC_TAB[data & 7];
		}
		else
		{
			ym2612->LFOinc = ym2612->LFOcnt = 0;
		}
		break;

	case 0x24:
		ym2612->TimerA = (ym2612->TimerA & 0x003) | (((int)data) << 2);

		if (ym2612->TimerAL != (1024 - ym2612->TimerA) << 12)
		{
			ym2612->TimerAcnt = ym2612->TimerAL = (1024 - ym2612->TimerA) << 12;
		}
		break;

	case 0x25:
		ym2612->TimerA = (ym2612->TimerA & 0x3fc) | (data & 3);

		if (ym2612->TimerAL != (1024 - ym2612->TimerA) << 12)
		{
			ym2612->TimerAcnt = ym2612->TimerAL = (1024 - ym2612->TimerA) << 12;
		}
		break;

	case 0x26:
		ym2612->TimerB = data;

		if (ym2612->TimerBL != (256 - ym2612->TimerB) << (4 + 12))
		{
			ym2612->TimerBcnt = ym2612->TimerBL = (256 - ym2612->TimerB) << (4 + 12);
		}
		break;

	case 0x27:
		// Paramètre divers
		// b7 = CSM MODE
		// b6 = 3 slot mode
		// b5 = reset b
		// b4 = reset a
		// b3 = timer enable b
		// b2 = timer enable a
		// b1 = load b
		// b0 = load a
		if ((data ^ ym2612->Mode) & 0x40)
		{
			// We changed the channel 2 mode, so recalculate phase step
			// This fix the punch sound in Street of Rage 2

			// YM2612_Special_Update();

			ym2612->CHANNEL[2].SLOT[0].Finc = -1;    // recalculate phase step
		}

		ym2612->Status &= (~data >> 4) & (data >> 2);  // Reset Status
		ym2612->Mode = data;
		break;

	case 0x28:
		if ((nch = data & 3) == 3)
			return 1;

		if (data & 4)
			nch += 3;
		CH = &(ym2612->CHANNEL[nch]);

		// YM2612_Special_Update();

		if (data & 0x10)
			KEY_ON(ym2612, CH, S0);			// On appuie sur la touche pour le slot 1
		else
			KEY_OFF(ym2612, CH, S0);		// On relâche la touche pour le slot 1

		if (data & 0x20)
			KEY_ON(ym2612, CH, S1);			// On appuie sur la touche pour le slot 2
		else
			KEY_OFF(ym2612, CH, S1);		// On relâche la touche pour le slot 2

		if (data & 0x40)
			KEY_ON(ym2612, CH, S2);			// On appuie sur la touche pour le slot 3
		else
			KEY_OFF(ym2612, CH, S2);		// On relâche la touche pour le slot 3

		if (data & 0x80)
			KEY_ON(ym2612, CH, S3);			// On appuie sur la touche pour le slot 4
		else
			KEY_OFF(ym2612, CH, S3);        // On relâche la touche pour le slot 4

		break;

	case 0x2A:
		ym2612->DACdata = ((int)data - 0x80) << 7;  // donnée du DAC
		break;

	case 0x2B:
		ym2612->DAC = data & 0x80;					// activation/désactivation du DAC
		break;
	}

	return 0;
}

void Env_NULL_Next(slot_ *SL)
{
}

void Env_Attack_Next(slot_ *SL)
{
	// Verified with Gynoug even in HQ (explode SFX)
	SL->Ecnt = ENV_DECAY;

	SL->Einc = SL->EincD;
	SL->Ecmp = SL->SLL;
	SL->Ecurp = DECAY;
}

void Env_Decay_Next(slot_ *SL)
{
	// Verified with Gynoug even in HQ (explode SFX)
	SL->Ecnt = SL->SLL;
	SL->Einc = SL->EincS;
	SL->Ecmp = ENV_END;
	SL->Ecurp = SUBSTAIN;
}

void Env_Substain_Next(slot_ *SL)
{
	if (YM2612_Enable_SSGEG)
	{
		if (SL->SEG & 8)  // SSG envelope type
		{
			if (SL->SEG & 1)
			{
				SL->Ecnt = ENV_END;
				SL->Einc = 0;
				SL->Ecmp = ENV_END + 1;
			}
			else
			{
				// re KEY ON

				// SL->Fcnt = 0;
				// SL->ChgEnM = 0xFFFFFFFF;

				SL->Ecnt = 0;
				SL->Einc = SL->EincA;
				SL->Ecmp = ENV_DECAY;
				SL->Ecurp = ATTACK;
			}

			SL->SEG ^= (SL->SEG & 2) << 1;
		}
		else
		{
			SL->Ecnt = ENV_END;
			SL->Einc = 0;
			SL->Ecmp = ENV_END + 1;
		}
	}
	else
	{
		SL->Ecnt = ENV_END;
		SL->Einc = 0;
		SL->Ecmp = ENV_END + 1;
	}
}

void Env_Release_Next(slot_ *SL)
{
	SL->Ecnt = ENV_END;
	SL->Einc = 0;
	SL->Ecmp = ENV_END + 1;
}

#define GET_CURRENT_PHASE(ym2612)  \
ym2612->in0 = CH->SLOT[S0].Fcnt;  \
ym2612->in1 = CH->SLOT[S1].Fcnt;  \
ym2612->in2 = CH->SLOT[S2].Fcnt;  \
ym2612->in3 = CH->SLOT[S3].Fcnt;

#define UPDATE_PHASE(ym2612)          \
CH->SLOT[S0].Fcnt += CH->SLOT[S0].Finc;  \
CH->SLOT[S1].Fcnt += CH->SLOT[S1].Finc;  \
CH->SLOT[S2].Fcnt += CH->SLOT[S2].Finc;  \
CH->SLOT[S3].Fcnt += CH->SLOT[S3].Finc;

#define UPDATE_PHASE_LFO(ym2612)                                    \
if(freq_LFO = (CH->FMS * ym2612->LFO_FREQ_UP[i]) >> (LFO_HBITS - 1))                  \
{                                                \
  CH->SLOT[S0].Fcnt += CH->SLOT[S0].Finc + ((CH->SLOT[S0].Finc * freq_LFO) >> LFO_FMS_LBITS);  \
  CH->SLOT[S1].Fcnt += CH->SLOT[S1].Finc + ((CH->SLOT[S1].Finc * freq_LFO) >> LFO_FMS_LBITS);  \
  CH->SLOT[S2].Fcnt += CH->SLOT[S2].Finc + ((CH->SLOT[S2].Finc * freq_LFO) >> LFO_FMS_LBITS);  \
  CH->SLOT[S3].Fcnt += CH->SLOT[S3].Finc + ((CH->SLOT[S3].Finc * freq_LFO) >> LFO_FMS_LBITS);  \
}                                                \
else                                              \
{                                                \
  CH->SLOT[S0].Fcnt += CH->SLOT[S0].Finc;                            \
  CH->SLOT[S1].Fcnt += CH->SLOT[S1].Finc;                            \
  CH->SLOT[S2].Fcnt += CH->SLOT[S2].Finc;                            \
  CH->SLOT[S3].Fcnt += CH->SLOT[S3].Finc;                            \
}

#define GET_CURRENT_ENV(ym2612) \
if(CH->SLOT[S0].SEG & 4) \
{                                                  \
	if((ym2612->en0 = ym2612->ENV_TAB[(CH->SLOT[S0].Ecnt >> ENV_LBITS)] + CH->SLOT[S0].TLL) > ENV_MASK) \
		ym2612->en0 = 0;  \
	else \
		ym2612->en0 ^= ENV_MASK; \
} \
else \
	ym2612->en0 = ym2612->ENV_TAB[(CH->SLOT[S0].Ecnt >> ENV_LBITS)] + CH->SLOT[S0].TLL; \
if(CH->SLOT[S1].SEG & 4) \
{ \
	if((ym2612->en1 = ym2612->ENV_TAB[(CH->SLOT[S1].Ecnt >> ENV_LBITS)] + CH->SLOT[S1].TLL) > ENV_MASK) \
		ym2612->en1 = 0;  \
	else \
		ym2612->en1 ^= ENV_MASK; \
} \
else \
	ym2612->en1 = ym2612->ENV_TAB[(CH->SLOT[S1].Ecnt >> ENV_LBITS)] + CH->SLOT[S1].TLL; \
if(CH->SLOT[S2].SEG & 4) \
{                                                  \
	if((ym2612->en2 = ym2612->ENV_TAB[(CH->SLOT[S2].Ecnt >> ENV_LBITS)] + CH->SLOT[S2].TLL) > ENV_MASK) \
		ym2612->en2 = 0;  \
	else \
		ym2612->en2 ^= ENV_MASK; \
} \
else \
	ym2612->en2 = ym2612->ENV_TAB[(CH->SLOT[S2].Ecnt >> ENV_LBITS)] + CH->SLOT[S2].TLL; \
if(CH->SLOT[S3].SEG & 4)                                      \
{ \
	if((ym2612->en3 = ym2612->ENV_TAB[(CH->SLOT[S3].Ecnt >> ENV_LBITS)] + CH->SLOT[S3].TLL) > ENV_MASK) \
		ym2612->en3 = 0; \
	else \
		ym2612->en3 ^= ENV_MASK; \
} \
else \
	ym2612->en3 = ym2612->ENV_TAB[(CH->SLOT[S3].Ecnt >> ENV_LBITS)] + CH->SLOT[S3].TLL;

#define GET_CURRENT_ENV_LFO(ym2612) \
env_LFO = ym2612->LFO_ENV_UP[i]; \
\
if(CH->SLOT[S0].SEG & 4) \
{ \
	if((ym2612->en0 = ym2612->ENV_TAB[(CH->SLOT[S0].Ecnt >> ENV_LBITS)] + CH->SLOT[S0].TLL) > ENV_MASK)\
		ym2612->en0 = 0; \
	else \
		ym2612->en0 = (ym2612->en0 ^ ENV_MASK) + (env_LFO >> CH->SLOT[S0].AMS); \
} \
else \
	ym2612->en0 = ym2612->ENV_TAB[(CH->SLOT[S0].Ecnt >> ENV_LBITS)] + CH->SLOT[S0].TLL + (env_LFO >> CH->SLOT[S0].AMS);  \
if(CH->SLOT[S1].SEG & 4) \
{ \
	if((ym2612->en1 = ym2612->ENV_TAB[(CH->SLOT[S1].Ecnt >> ENV_LBITS)] + CH->SLOT[S1].TLL) > ENV_MASK) \
		ym2612->en1 = 0; \
	else \
		ym2612->en1 = (ym2612->en1 ^ ENV_MASK) + (env_LFO >> CH->SLOT[S1].AMS); \
} \
else \
	ym2612->en1 = ym2612->ENV_TAB[(CH->SLOT[S1].Ecnt >> ENV_LBITS)] + CH->SLOT[S1].TLL + (env_LFO >> CH->SLOT[S1].AMS); \
if(CH->SLOT[S2].SEG & 4) \
{ \
	if((ym2612->en2 = ym2612->ENV_TAB[(CH->SLOT[S2].Ecnt >> ENV_LBITS)] + CH->SLOT[S2].TLL) > ENV_MASK) \
		ym2612->en2 = 0; \
	else \
		ym2612->en2 = (ym2612->en2 ^ ENV_MASK) + (env_LFO >> CH->SLOT[S2].AMS); \
} \
else \
	ym2612->en2 = ym2612->ENV_TAB[(CH->SLOT[S2].Ecnt >> ENV_LBITS)] + CH->SLOT[S2].TLL + (env_LFO >> CH->SLOT[S2].AMS); \
if(CH->SLOT[S3].SEG & 4) \
{ \
	if((ym2612->en3 = ym2612->ENV_TAB[(CH->SLOT[S3].Ecnt >> ENV_LBITS)] + CH->SLOT[S3].TLL) > ENV_MASK) \
		ym2612->en3 = 0; \
	else \
		ym2612->en3 = (ym2612->en3 ^ ENV_MASK) + (env_LFO >> CH->SLOT[S3].AMS); \
} \
else \
	ym2612->en3 = ym2612->ENV_TAB[(CH->SLOT[S3].Ecnt >> ENV_LBITS)] + CH->SLOT[S3].TLL + (env_LFO >> CH->SLOT[S3].AMS);

#define UPDATE_ENV(ym2612) \
	if((CH->SLOT[S0].Ecnt += CH->SLOT[S0].Einc) >= CH->SLOT[S0].Ecmp)    \
		ENV_NEXT_EVENT[CH->SLOT[S0].Ecurp](&(CH->SLOT[S0]));        \
	if((CH->SLOT[S1].Ecnt += CH->SLOT[S1].Einc) >= CH->SLOT[S1].Ecmp)    \
		ENV_NEXT_EVENT[CH->SLOT[S1].Ecurp](&(CH->SLOT[S1]));        \
	if((CH->SLOT[S2].Ecnt += CH->SLOT[S2].Einc) >= CH->SLOT[S2].Ecmp)    \
		ENV_NEXT_EVENT[CH->SLOT[S2].Ecurp](&(CH->SLOT[S2]));        \
	if((CH->SLOT[S3].Ecnt += CH->SLOT[S3].Einc) >= CH->SLOT[S3].Ecmp)    \
		ENV_NEXT_EVENT[CH->SLOT[S3].Ecurp](&(CH->SLOT[S3]));

#define DO_LIMIT(ym2612) \
if(CH->OUTd > LIMIT_CH_OUT) \
	CH->OUTd = LIMIT_CH_OUT;  \
else if(CH->OUTd < -LIMIT_CH_OUT) \
	CH->OUTd = -LIMIT_CH_OUT;

#define DO_FEEDBACK0(ym2612) \
ym2612->in0 += CH->S0_OUT[0] >> CH->FB; \
CH->S0_OUT[0] = ym2612->SIN_TAB[(ym2612->in0 >> SIN_LBITS) & SIN_MASK][ym2612->en0];

#define DO_FEEDBACK(ym2612) \
ym2612->in0 += (CH->S0_OUT[0] + CH->S0_OUT[1]) >> CH->FB;  \
CH->S0_OUT[1] = CH->S0_OUT[0]; \
CH->S0_OUT[0] = ym2612->SIN_TAB[(ym2612->in0 >> SIN_LBITS) & SIN_MASK][ym2612->en0];

#define DO_FEEDBACK2(ym2612) \
ym2612->in0 += (CH->S0_OUT[0] + (CH->S0_OUT[0] >> 2) + CH->S0_OUT[1]) >> CH->FB;  \
CH->S0_OUT[1] = CH->S0_OUT[0] >> 2;                      \
CH->S0_OUT[0] = ym2612->SIN_TAB[(ym2612->in0 >> SIN_LBITS) & SIN_MASK][ym2612->en0];

#define DO_FEEDBACK3(ym2612) \
ym2612->in0 += (CH->S0_OUT[0] + CH->S0_OUT[1] + CH->S0_OUT[2] + CH->S0_OUT[3]) >> CH->FB; \
CH->S0_OUT[3] = CH->S0_OUT[2] >> 1; \
CH->S0_OUT[2] = CH->S0_OUT[1] >> 1; \
CH->S0_OUT[1] = CH->S0_OUT[0] >> 1; \
CH->S0_OUT[0] = ym2612->SIN_TAB[(ym2612->in0 >> SIN_LBITS) & SIN_MASK][ym2612->en0];

#define DO_ALGO_0(ym2612) \
DO_FEEDBACK(ym2612) \
ym2612->in1 += CH->S0_OUT[1]; \
ym2612->in2 += ym2612->SIN_TAB[(ym2612->in1 >> SIN_LBITS) & SIN_MASK][ym2612->en1]; \
ym2612->in3 += ym2612->SIN_TAB[(ym2612->in2 >> SIN_LBITS) & SIN_MASK][ym2612->en2]; \
CH->OUTd = (ym2612->SIN_TAB[(ym2612->in3 >> SIN_LBITS) & SIN_MASK][ym2612->en3]) >> OUT_SHIFT;

#define DO_ALGO_1(ym2612) \
DO_FEEDBACK(ym2612) \
ym2612->in2 += CH->S0_OUT[1] + ym2612->SIN_TAB[(ym2612->in1 >> SIN_LBITS) & SIN_MASK][ym2612->en1]; \
ym2612->in3 += ym2612->SIN_TAB[(ym2612->in2 >> SIN_LBITS) & SIN_MASK][ym2612->en2];              \
CH->OUTd = (ym2612->SIN_TAB[(ym2612->in3 >> SIN_LBITS) & SIN_MASK][ym2612->en3]) >> OUT_SHIFT;

#define DO_ALGO_2(ym2612) \
DO_FEEDBACK(ym2612) \
ym2612->in2 += ym2612->SIN_TAB[(ym2612->in1 >> SIN_LBITS) & SIN_MASK][ym2612->en1];              \
ym2612->in3 += CH->S0_OUT[1] + ym2612->SIN_TAB[(ym2612->in2 >> SIN_LBITS) & SIN_MASK][ym2612->en2];      \
CH->OUTd = (ym2612->SIN_TAB[(ym2612->in3 >> SIN_LBITS) & SIN_MASK][ym2612->en3]) >> OUT_SHIFT;

#define DO_ALGO_3(ym2612) \
DO_FEEDBACK(ym2612) \
ym2612->in1 += CH->S0_OUT[1];                            \
ym2612->in3 += ym2612->SIN_TAB[(ym2612->in1 >> SIN_LBITS) & SIN_MASK][ym2612->en1] + ym2612->SIN_TAB[(ym2612->in2 >> SIN_LBITS) & SIN_MASK][ym2612->en2];  \
CH->OUTd = (ym2612->SIN_TAB[(ym2612->in3 >> SIN_LBITS) & SIN_MASK][ym2612->en3]) >> OUT_SHIFT;

#define DO_ALGO_4(ym2612)                              \
DO_FEEDBACK(ym2612)                                  \
ym2612->in1 += CH->S0_OUT[1];                            \
ym2612->in3 += ym2612->SIN_TAB[(ym2612->in2 >> SIN_LBITS) & SIN_MASK][ym2612->en2];              \
CH->OUTd = ((int)ym2612->SIN_TAB[(ym2612->in3 >> SIN_LBITS) & SIN_MASK][ym2612->en3] + (int)ym2612->SIN_TAB[(ym2612->in1 >> SIN_LBITS) & SIN_MASK][ym2612->en1]) >> OUT_SHIFT;  \
DO_LIMIT(ym2612)

#define DO_ALGO_5(ym2612)                              \
DO_FEEDBACK(ym2612)                                  \
ym2612->in1 += CH->S0_OUT[1];                            \
ym2612->in2 += CH->S0_OUT[1];                            \
ym2612->in3 += CH->S0_OUT[1];                            \
CH->OUTd = ((int)ym2612->SIN_TAB[(ym2612->in3 >> SIN_LBITS) & SIN_MASK][ym2612->en3] + (int)ym2612->SIN_TAB[(ym2612->in1 >> SIN_LBITS) & SIN_MASK][ym2612->en1] + (int)ym2612->SIN_TAB[(ym2612->in2 >> SIN_LBITS) & SIN_MASK][ym2612->en2]) >> OUT_SHIFT;  \
DO_LIMIT(ym2612)

#define DO_ALGO_6(ym2612)                              \
DO_FEEDBACK(ym2612)                                    \
ym2612->in1 += CH->S0_OUT[1];                            \
CH->OUTd = ((int)ym2612->SIN_TAB[(ym2612->in3 >> SIN_LBITS) & SIN_MASK][ym2612->en3] + (int)ym2612->SIN_TAB[(ym2612->in1 >> SIN_LBITS) & SIN_MASK][ym2612->en1] + (int)ym2612->SIN_TAB[(ym2612->in2 >> SIN_LBITS) & SIN_MASK][ym2612->en2]) >> OUT_SHIFT;  \
DO_LIMIT(ym2612)

#define DO_ALGO_7(ym2612)  \
DO_FEEDBACK(ym2612) \
CH->OUTd = ((int)ym2612->SIN_TAB[(ym2612->in3 >> SIN_LBITS) & SIN_MASK][ym2612->en3] + (int)ym2612->SIN_TAB[(ym2612->in1 >> SIN_LBITS) & SIN_MASK][ym2612->en1] + (int)ym2612->SIN_TAB[(ym2612->in2 >> SIN_LBITS) & SIN_MASK][ym2612->en2] + CH->S0_OUT[1]) >> OUT_SHIFT;  \
DO_LIMIT(ym2612)

#define DO_OUTPUT(ym2612)            \
buf[0][i] += CH->OUTd & CH->LEFT;    \
buf[1][i] += CH->OUTd & CH->RIGHT;

#define DO_OUTPUT_INT0(ym2612)              \
if((ym2612->int_cnt += ym2612->Inter_Step) & 0x04000)  \
{                        \
	ym2612->int_cnt &= 0x3FFF;              \
	buf[0][i] += CH->OUTd & CH->LEFT;      \
	buf[1][i] += CH->OUTd & CH->RIGHT;      \
}                        \
else \
	i--;

#define DO_OUTPUT_INT1(ym2612)              \
CH->Old_OUTd = (CH->OUTd + CH->Old_OUTd) >> 1;  \
if((ym2612->int_cnt += ym2612->Inter_Step) & 0x04000)  \
{                        \
	ym2612->int_cnt &= 0x3FFF;              \
	buf[0][i] += CH->Old_OUTd & CH->LEFT;    \
	buf[1][i] += CH->Old_OUTd & CH->RIGHT;    \
}                        \
else \
	i--;

#define DO_OUTPUT_INT2(ym2612)                \
if((ym2612->int_cnt += ym2612->Inter_Step) & 0x04000)    \
{                          \
	ym2612->int_cnt &= 0x3FFF;                \
	CH->Old_OUTd = (CH->OUTd + CH->Old_OUTd) >> 1;  \
	buf[0][i] += CH->Old_OUTd & CH->LEFT;      \
	buf[1][i] += CH->Old_OUTd & CH->RIGHT;      \
}                          \
else \
	i--;                      \
CH->Old_OUTd = CH->OUTd;

#define DO_OUTPUT_INT(ym2612)              \
if((ym2612->int_cnt += ym2612->Inter_Step) & 0x04000)  \
{                        \
  ym2612->int_cnt &= 0x3FFF;              \
  CH->Old_OUTd = (((ym2612->int_cnt ^ 0x3FFF) * CH->OUTd) + (ym2612->int_cnt * CH->Old_OUTd)) >> 14;  \
  buf[0][i] += CH->Old_OUTd & CH->LEFT;    \
  buf[1][i] += CH->Old_OUTd & CH->RIGHT;    \
}                        \
else \
	i--;                    \
CH->Old_OUTd = CH->OUTd;

void Update_Chan_Algo0(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	
	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
		UPDATE_PHASE(ym2612)
		GET_CURRENT_ENV(ym2612)
		UPDATE_ENV(ym2612)
		DO_ALGO_0(ym2612)
		DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo1(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_1(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo2(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_2(ym2612)
			DO_OUTPUT(ym2612)
	}
}


void Update_Chan_Algo3(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_3(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo4(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_4(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo5(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_5(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo6(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_6(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo7(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if ((CH->SLOT[S0].Ecnt == ENV_END) && (CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_7(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo0_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_0(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo1_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_1(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo2_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_2(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo3_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_3(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo4_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_4(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo5_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_5(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo6_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_6(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo7_LFO(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if ((CH->SLOT[S0].Ecnt == ENV_END) && (CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_7(ym2612)
			DO_OUTPUT(ym2612)
	}
}

void Update_Chan_Algo0_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_0(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}


void Update_Chan_Algo1_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_1(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}


void Update_Chan_Algo2_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_2(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}


void Update_Chan_Algo3_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;
	
	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_3(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}

void Update_Chan_Algo4_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_4(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}

void Update_Chan_Algo5_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_5(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}


void Update_Chan_Algo6_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_6(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}


void Update_Chan_Algo7_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i;
	

	if ((CH->SLOT[S0].Ecnt == ENV_END) && (CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE(ym2612)
			GET_CURRENT_ENV(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_7(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}


void Update_Chan_Algo0_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	
	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_0(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}


void Update_Chan_Algo1_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	
	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_1(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}

void Update_Chan_Algo2_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_2(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}

void Update_Chan_Algo3_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if (CH->SLOT[S3].Ecnt == ENV_END)
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_3(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}

void Update_Chan_Algo4_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	
	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_4(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}

void Update_Chan_Algo5_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	
	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_5(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}


void Update_Chan_Algo6_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	

	if ((CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_6(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}

void Update_Chan_Algo7_LFO_Int(YM2612 *ym2612, channel_ *CH, int **buf, int lenght)
{
	int i, env_LFO, freq_LFO;
	
	if ((CH->SLOT[S0].Ecnt == ENV_END) && (CH->SLOT[S1].Ecnt == ENV_END) && (CH->SLOT[S2].Ecnt == ENV_END) && (CH->SLOT[S3].Ecnt == ENV_END))
		return;

	ym2612->int_cnt = ym2612->Inter_Cnt;

	for (i = 0; i < lenght; i++)
	{
		GET_CURRENT_PHASE(ym2612)
			UPDATE_PHASE_LFO(ym2612)
			GET_CURRENT_ENV_LFO(ym2612)
			UPDATE_ENV(ym2612)
			DO_ALGO_7(ym2612)
			DO_OUTPUT_INT(ym2612)
	}
}

int YM2612Init(int chipCount, int clock, int rate)
{
	int i, j;
	double x;

	if ((rate == 0) || (clock == 0))
		return 0;

	YM2612Chips = (YM2612 *)malloc(sizeof(YM2612) * chipCount);
	if (YM2612Chips == NULL)
		return 0;

	YM2612NumChips = chipCount;
	memset(YM2612Chips, 0, sizeof(YM2612) * YM2612NumChips);

	for (int chipID = 0; chipID < YM2612NumChips; chipID++)
	{
		YM2612Chips[chipID].Clock = clock;
		YM2612Chips[chipID].Rate = rate;

		// 144 = 12 * (prescale * 2) = 12 * 6 * 2
		// prescale set to 6 by default

		YM2612Chips[chipID].Frequence = ((double)YM2612Chips[chipID].Clock / (double)YM2612Chips[chipID].Rate) / 144.0;
		YM2612Chips[chipID].TimerBase = (int)(YM2612Chips[chipID].Frequence * 4096.0);

		int Interpolation = -1;
		if ((Interpolation) && (YM2612Chips[chipID].Frequence > 1.0))
		{
			YM2612Chips[chipID].Inter_Step = (unsigned int)((1.0 / YM2612Chips[chipID].Frequence) * (double)(0x4000));
			YM2612Chips[chipID].Inter_Cnt = 0;

			// We recalculate rate and frequence after interpolation

			YM2612Chips[chipID].Rate = YM2612Chips[chipID].Clock / 144;
			YM2612Chips[chipID].Frequence = 1.0;
		}
		else
		{
			YM2612Chips[chipID].Inter_Step = 0x4000;
			YM2612Chips[chipID].Inter_Cnt = 0;
		}

		// Tableau TL :
		// [0     -  4095] = +output  [4095  - ...] = +output overflow (fill with 0)
		// [12288 - 16383] = -output  [16384 - ...] = -output overflow (fill with 0)
		for (i = 0; i < TL_LENGHT; i++)
		{
			if (i >= PG_CUT_OFF)  // YM2612 cut off sound after 78 dB (14 bits output ?)
			{
				YM2612Chips[chipID].TL_TAB[TL_LENGHT + i] = YM2612Chips[chipID].TL_TAB[i] = 0;
			}
			else
			{
				x = MAX_OUT;                // Max output
				x /= pow(10, (ENV_STEP * i) / 20);      // Decibel -> Voltage

				YM2612Chips[chipID].TL_TAB[i] = (int)x;
				YM2612Chips[chipID].TL_TAB[TL_LENGHT + i] = -YM2612Chips[chipID].TL_TAB[i];
			}
		}

		// Tableau SIN :
		// SIN_TAB[x][y] = sin(x) * y;
		// x = phase and y = volume

		YM2612Chips[chipID].SIN_TAB[0] = YM2612Chips[chipID].SIN_TAB[SIN_LENGHT / 2] = &YM2612Chips[chipID].TL_TAB[(int)PG_CUT_OFF];
		for (i = 1; i <= SIN_LENGHT / 4; i++)
		{
			x = sin(2.0 * PI * (double)(i) / (double)(SIN_LENGHT));  // Sinus
			x = 20 * log10(1 / x);                    // convert to dB

			j = (int)(x / ENV_STEP);            // Get TL range

			if (j > PG_CUT_OFF) j = (int)PG_CUT_OFF;

			YM2612Chips[chipID].SIN_TAB[i] = YM2612Chips[chipID].SIN_TAB[(SIN_LENGHT / 2) - i] = &YM2612Chips[chipID].TL_TAB[j];
			YM2612Chips[chipID].SIN_TAB[(SIN_LENGHT / 2) + i] = YM2612Chips[chipID].SIN_TAB[SIN_LENGHT - i] = &YM2612Chips[chipID].TL_TAB[TL_LENGHT + j];
		}

		// Tableau LFO (LFO wav) :
		for (i = 0; i < LFO_LENGHT; i++)
		{
			x = sin(2.0 * PI * (double)(i) / (double)(LFO_LENGHT));  // Sinus
			x += 1.0;
			x /= 2.0;          // positive only
			x *= 11.8 / ENV_STEP;    // ajusted to MAX enveloppe modulation

			YM2612Chips[chipID].LFO_ENV_TAB[i] = (int)x;

			x = sin(2.0 * PI * (double)(i) / (double)(LFO_LENGHT));  // Sinus
			x *= (double)((1 << (LFO_HBITS - 1)) - 1);

			YM2612Chips[chipID].LFO_FREQ_TAB[i] = (int)x;
		}

		// Tableau Enveloppe :
		// ENV_TAB[0] -> ENV_TAB[ENV_LENGHT - 1]        = attack curve
		// ENV_TAB[ENV_LENGHT] -> ENV_TAB[2 * ENV_LENGHT - 1]  = decay curve
		for (i = 0; i < ENV_LENGHT; i++)
		{
			// Attack curve (x^8 - music level 2 Vectorman 2)
			x = pow(((double)((ENV_LENGHT - 1) - i) / (double)(ENV_LENGHT)), 8);
			x *= ENV_LENGHT;

			YM2612Chips[chipID].ENV_TAB[i] = (int)x;

			// Decay curve (just linear)
			x = pow(((double)(i) / (double)(ENV_LENGHT)), 1);
			x *= ENV_LENGHT;

			YM2612Chips[chipID].ENV_TAB[ENV_LENGHT + i] = (int)x;
		}

		YM2612Chips[chipID].ENV_TAB[ENV_END >> ENV_LBITS] = ENV_LENGHT - 1;    // for the stopped state

		// Tableau pour la conversion Attack -> Decay and Decay -> Attack
		for (i = 0, j = ENV_LENGHT - 1; i < ENV_LENGHT; i++)
		{
			while (j && (YM2612Chips[chipID].ENV_TAB[j] < (unsigned)i)) j--;

			YM2612Chips[chipID].DECAY_TO_ATTACK[i] = j << ENV_LBITS;
		}

		// Tableau pour le Substain Level
		for (i = 0; i < 15; i++)
		{
			x = i * 3;          // 3 and not 6 (Mickey Mania first music for test)
			x /= ENV_STEP;

			j = (int)x;
			j <<= ENV_LBITS;

			YM2612Chips[chipID].SL_TAB[i] = j + ENV_DECAY;
		}

		j = ENV_LENGHT - 1;        // special case : volume off
		j <<= ENV_LBITS;
		YM2612Chips[chipID].SL_TAB[15] = j + ENV_DECAY;

		// Tableau Frequency Step
		for (i = 0; i < 2048; i++)
		{
			x = (double)(i)* YM2612Chips[chipID].Frequence;

#if((SIN_LBITS + SIN_HBITS - (21 - 7)) < 0)
			x /= (double)(1 << ((21 - 7) - SIN_LBITS - SIN_HBITS));
#else
			x *= (double)(1 << (SIN_LBITS + SIN_HBITS - (21 - 7)));
#endif
			x /= 2.0;  // because MUL = value * 2

			YM2612Chips[chipID].FINC_TAB[i] = (unsigned int)x;
		}

		// Tableaux Attack & Decay Rate

		for (i = 0; i < 4; i++)
		{
			YM2612Chips[chipID].AR_TAB[i] = 0;
			YM2612Chips[chipID].DR_TAB[i] = 0;
		}

		for (i = 0; i < 60; i++)
		{
			x = YM2612Chips[chipID].Frequence;

			x *= 1.0 + ((i & 3) * 0.25);          // bits 0-1 : x1.00, x1.25, x1.50, x1.75
			x *= (double)(1 << ((i >> 2)));        // bits 2-5 : shift bits (x2^0 - x2^15)
			x *= (double)(ENV_LENGHT << ENV_LBITS);    // on ajuste pour le tableau ENV_TAB

			YM2612Chips[chipID].AR_TAB[i + 4] = (unsigned int)(x / AR_RATE);
			YM2612Chips[chipID].DR_TAB[i + 4] = (unsigned int)(x / DR_RATE);
		}

		for (i = 64; i < 96; i++)
		{
			YM2612Chips[chipID].AR_TAB[i] = YM2612Chips[chipID].AR_TAB[63];
			YM2612Chips[chipID].DR_TAB[i] = YM2612Chips[chipID].DR_TAB[63];

			YM2612Chips[chipID].NULL_RATE[i - 64] = 0;
		}

		// Tableau Detune

		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 32; j++)
			{
#if((SIN_LBITS + SIN_HBITS - 21) < 0)
				x = (double)DT_DEF_TAB[(i << 5) + j] * YM2612Chips[chipID].Frequence / (double)(1 << (21 - SIN_LBITS - SIN_HBITS));
#else
				x = (double)DT_DEF_TAB[(i << 5) + j] * YM2612Chips[chipID].Frequence * (double)(1 << (SIN_LBITS + SIN_HBITS - 21));
#endif
				YM2612Chips[chipID].DT_TAB[i + 0][j] = (int)x;
				YM2612Chips[chipID].DT_TAB[i + 4][j] = (int)-x;
			}
		}

		// Tableau LFO

		j = (YM2612Chips[chipID].Rate * YM2612Chips[chipID].Inter_Step) / 0x4000;

		YM2612Chips[chipID].LFO_INC_TAB[0] = (unsigned int)(3.98 * (double)(1 << (LFO_HBITS + LFO_LBITS)) / j);
		YM2612Chips[chipID].LFO_INC_TAB[1] = (unsigned int)(5.56 * (double)(1 << (LFO_HBITS + LFO_LBITS)) / j);
		YM2612Chips[chipID].LFO_INC_TAB[2] = (unsigned int)(6.02 * (double)(1 << (LFO_HBITS + LFO_LBITS)) / j);
		YM2612Chips[chipID].LFO_INC_TAB[3] = (unsigned int)(6.37 * (double)(1 << (LFO_HBITS + LFO_LBITS)) / j);
		YM2612Chips[chipID].LFO_INC_TAB[4] = (unsigned int)(6.88 * (double)(1 << (LFO_HBITS + LFO_LBITS)) / j);
		YM2612Chips[chipID].LFO_INC_TAB[5] = (unsigned int)(9.63 * (double)(1 << (LFO_HBITS + LFO_LBITS)) / j);
		YM2612Chips[chipID].LFO_INC_TAB[6] = (unsigned int)(48.1 * (double)(1 << (LFO_HBITS + LFO_LBITS)) / j);
		YM2612Chips[chipID].LFO_INC_TAB[7] = (unsigned int)(72.2 * (double)(1 << (LFO_HBITS + LFO_LBITS)) / j);

		YM2612ResetChip(chipID);
	}

	return -1;
}

void YM2612Shutdown()
{
	if (!YM2612Chips)
		return;

	free(YM2612Chips);
	YM2612Chips = NULL;
	YM2612NumChips = 0;
}

void YM2612ResetChip(int chipID)
{
	int i, j;
	YM2612 *ym2612 = &YM2612Chips[chipID];

	ym2612->LFOcnt = 0;
	ym2612->TimerA = 0;
	ym2612->TimerAL = 0;
	ym2612->TimerAcnt = 0;
	ym2612->TimerB = 0;
	ym2612->TimerBL = 0;
	ym2612->TimerBcnt = 0;
	ym2612->DAC = 0;
	ym2612->DACdata = 0;

	ym2612->Status = 0;

	ym2612->OPNAadr = 0;
	ym2612->OPNBadr = 0;
	ym2612->Inter_Cnt = 0;

	for (i = 0; i < 6; i++)
	{
		ym2612->CHANNEL[i].Old_OUTd = 0;
		ym2612->CHANNEL[i].OUTd = 0;
		ym2612->CHANNEL[i].LEFT = 0xFFFFFFFF;
		ym2612->CHANNEL[i].RIGHT = 0xFFFFFFFF;
		ym2612->CHANNEL[i].ALGO = 0;;
		ym2612->CHANNEL[i].FB = 31;
		ym2612->CHANNEL[i].FMS = 0;
		ym2612->CHANNEL[i].AMS = 0;

		for (j = 0; j < 4; j++)
		{
			ym2612->CHANNEL[i].S0_OUT[j] = 0;
			ym2612->CHANNEL[i].FNUM[j] = 0;
			ym2612->CHANNEL[i].FOCT[j] = 0;
			ym2612->CHANNEL[i].KC[j] = 0;

			ym2612->CHANNEL[i].SLOT[j].Fcnt = 0;
			ym2612->CHANNEL[i].SLOT[j].Finc = 0;
			ym2612->CHANNEL[i].SLOT[j].Ecnt = ENV_END;    // Put it at the end of Decay phase...
			ym2612->CHANNEL[i].SLOT[j].Einc = 0;
			ym2612->CHANNEL[i].SLOT[j].Ecmp = 0;
			ym2612->CHANNEL[i].SLOT[j].Ecurp = RELEASE;

			ym2612->CHANNEL[i].SLOT[j].ChgEnM = 0;
		}
	}

	for (i = 0; i < 0x100; i++)
	{
		ym2612->REG[0][i] = -1;
		ym2612->REG[1][i] = -1;
	}

	for (i = 0xB6; i >= 0xB4; i--)
	{
		YM2612WriteReg(chipID, 0, (unsigned char)i);
		YM2612WriteReg(chipID, 2, (unsigned char)i);
		YM2612WriteReg(chipID, 1, 0xC0);
		YM2612WriteReg(chipID, 3, 0xC0);
	}

	for (i = 0xB2; i >= 0x22; i--)
	{
		YM2612WriteReg(chipID, 0, (unsigned char)i);
		YM2612WriteReg(chipID, 2, (unsigned char)i);
		YM2612WriteReg(chipID, 1, 0);
		YM2612WriteReg(chipID, 3, 0);
	}

	YM2612WriteReg(chipID, 0, 0x2A);
	YM2612WriteReg(chipID, 1, 0x80);
}

int YM2612ReadStatus(int chipID)
{
	YM2612 *ym2612 = &YM2612Chips[chipID];

	return ym2612->Status;
}

void YM2612WriteReg(int chipID, int adr, int data)
{
	int d;
	YM2612 *ym2612 = &YM2612Chips[chipID];

	data &= 0xFF;
	adr &= 0x03;

	switch (adr)
	{
	case 0:
		ym2612->OPNAadr = data;
		break;

	case 1:
		// Trivial optimisation
		if (ym2612->OPNAadr == 0x2A)
		{
			ym2612->DACdata = ((int)data - 0x80) << 7;
		}

		d = ym2612->OPNAadr & 0xF0;

		if (d >= 0x30)
		{
			//if (ym2612->REG[0][ym2612->OPNAadr] == data)
				//return 2;

			ym2612->REG[0][ym2612->OPNAadr] = data;

			if (d < 0xA0)    // SLOT
			{
				SLOT_SET(ym2612, ym2612->OPNAadr, data);
			}
			else        // CHANNEL
			{
				CHANNEL_SET(ym2612, ym2612->OPNAadr, data);
			}
		}
		else          // YM2612
		{
			ym2612->REG[0][ym2612->OPNAadr] = data;

			YM_SET(ym2612, ym2612->OPNAadr, data);
		}
		break;

	case 2:
		ym2612->OPNBadr = data;
		break;

	case 3:
		d = ym2612->OPNBadr & 0xF0;

		if (d >= 0x30)
		{
			//if (ym2612->REG[1][ym2612->OPNBadr] == data)
				//return 2;

			ym2612->REG[1][ym2612->OPNBadr] = data;

			if (d < 0xA0)    // SLOT
			{
				SLOT_SET(ym2612, ym2612->OPNBadr + 0x100, data);
			}
			else        // CHANNEL
			{
				CHANNEL_SET(ym2612, ym2612->OPNBadr + 0x100, data);
			}
		}
		else
		{
			//return 1;
		}
		break;
	}
}

void YM2612UpdateOne(int chipID, int **buffers, int length)
{
	int i, j, algo_type;
	YM2612 *ym2612 = &YM2612Chips[chipID];

	if (ym2612->CHANNEL[0].SLOT[0].Finc == -1)
		CALC_FINC_CH(ym2612, &ym2612->CHANNEL[0]);

	if (ym2612->CHANNEL[1].SLOT[0].Finc == -1)
		CALC_FINC_CH(ym2612, &ym2612->CHANNEL[1]);

	if (ym2612->CHANNEL[2].SLOT[0].Finc == -1)
	{
		if (ym2612->Mode & 0x40)
		{
			CALC_FINC_SL(ym2612, &(ym2612->CHANNEL[2].SLOT[S0]), ym2612->FINC_TAB[ym2612->CHANNEL[2].FNUM[2]] >> (7 - ym2612->CHANNEL[2].FOCT[2]), ym2612->CHANNEL[2].KC[2]);
			CALC_FINC_SL(ym2612, &(ym2612->CHANNEL[2].SLOT[S1]), ym2612->FINC_TAB[ym2612->CHANNEL[2].FNUM[3]] >> (7 - ym2612->CHANNEL[2].FOCT[3]), ym2612->CHANNEL[2].KC[3]);
			CALC_FINC_SL(ym2612, &(ym2612->CHANNEL[2].SLOT[S2]), ym2612->FINC_TAB[ym2612->CHANNEL[2].FNUM[1]] >> (7 - ym2612->CHANNEL[2].FOCT[1]), ym2612->CHANNEL[2].KC[1]);
			CALC_FINC_SL(ym2612, &(ym2612->CHANNEL[2].SLOT[S3]), ym2612->FINC_TAB[ym2612->CHANNEL[2].FNUM[0]] >> (7 - ym2612->CHANNEL[2].FOCT[0]), ym2612->CHANNEL[2].KC[0]);
		}
		else
		{
			CALC_FINC_CH(ym2612, &ym2612->CHANNEL[2]);
		}
	}

	if (ym2612->CHANNEL[3].SLOT[0].Finc == -1)
		CALC_FINC_CH(ym2612, &ym2612->CHANNEL[3]);

	if (ym2612->CHANNEL[4].SLOT[0].Finc == -1)
		CALC_FINC_CH(ym2612, &ym2612->CHANNEL[4]);

	if (ym2612->CHANNEL[5].SLOT[0].Finc == -1)
		CALC_FINC_CH(ym2612, &ym2612->CHANNEL[5]);

	if (ym2612->Inter_Step & 0x04000)
		algo_type = 0;
	else
		algo_type = 16;

	if (ym2612->LFOinc)
	{
		// Precalcul LFO wav

		for (i = 0; i < length; i++)
		{
			j = ((ym2612->LFOcnt += ym2612->LFOinc) >> LFO_LBITS) & LFO_MASK;

			ym2612->LFO_ENV_UP[i] = ym2612->LFO_ENV_TAB[j];
			ym2612->LFO_FREQ_UP[i] = ym2612->LFO_FREQ_TAB[j];
		}

		algo_type |= 8;
	}

	UPDATE_CHAN[ym2612->CHANNEL[0].ALGO + algo_type](ym2612, &(ym2612->CHANNEL[0]), buffers, length);

	UPDATE_CHAN[ym2612->CHANNEL[1].ALGO + algo_type](ym2612, &(ym2612->CHANNEL[1]), buffers, length);

	UPDATE_CHAN[ym2612->CHANNEL[2].ALGO + algo_type](ym2612, &(ym2612->CHANNEL[2]), buffers, length);

	UPDATE_CHAN[ym2612->CHANNEL[3].ALGO + algo_type](ym2612, &(ym2612->CHANNEL[3]), buffers, length);

	UPDATE_CHAN[ym2612->CHANNEL[4].ALGO + algo_type](ym2612, &(ym2612->CHANNEL[4]), buffers, length);

	if (!(ym2612->DAC))
		UPDATE_CHAN[ym2612->CHANNEL[5].ALGO + algo_type](ym2612, &(ym2612->CHANNEL[5]), buffers, length);

	ym2612->Inter_Cnt = ym2612->int_cnt;
}