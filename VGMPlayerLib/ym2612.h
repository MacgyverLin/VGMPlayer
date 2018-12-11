#ifndef _YM2612_H_
#define _YM2612_H_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  MAX_UPDATE_LENGHT   2000

#define  OUTPUT_BITS         16

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define ATTACK    0
#define DECAY     1
#define SUBSTAIN  2
#define RELEASE   3

#define SIN_HBITS      12								// Sin phase counter integer
#define SIN_LBITS      (26 - SIN_HBITS)					// Sin phase counter decimal part (best setting)

#if(SIN_LBITS > 16)
#define SIN_LBITS      16								// Can't be greater than 16 bits
#endif

#define ENV_HBITS      12								// Env phase counter integer
#define ENV_LBITS      (28 - ENV_HBITS)					// Env phase counter decimal part (best setting)

#define LFO_HBITS      10								// LFO phase counter integer
#define LFO_LBITS      (28 - LFO_HBITS)					// LFO phase counter decimal part (best setting)

#define SIN_LENGHT     (1 << SIN_HBITS)
#define ENV_LENGHT     (1 << ENV_HBITS)
#define LFO_LENGHT     (1 << LFO_HBITS)

#define TL_LENGHT      (ENV_LENGHT * 3)					// Env + TL scaling + LFO

#define SIN_MASK       (SIN_LENGHT - 1)
#define ENV_MASK       (ENV_LENGHT - 1)
#define LFO_MASK       (LFO_LENGHT - 1)

#define ENV_STEP       (96.0 / ENV_LENGHT)				// ENV_MAX = 96 dB

#define ENV_ATTACK     ((ENV_LENGHT * 0) << ENV_LBITS)
#define ENV_DECAY      ((ENV_LENGHT * 1) << ENV_LBITS)
#define ENV_END        ((ENV_LENGHT * 2) << ENV_LBITS)

#define MAX_OUT_BITS   (SIN_HBITS + SIN_LBITS + 2)		// Modulation = -4 <--> +4
#define MAX_OUT        ((1 << MAX_OUT_BITS) - 1)

#define OUT_BITS       (OUTPUT_BITS - 2)
#define OUT_SHIFT      (MAX_OUT_BITS - OUT_BITS)
#define LIMIT_CH_OUT   ((int) (((1 << OUT_BITS) * 1.5) - 1))

#define PG_CUT_OFF     ((int) (78.0 / ENV_STEP))
#define ENV_CUT_OFF    ((int) (68.0 / ENV_STEP))

#define AR_RATE        399128
#define DR_RATE        5514396

#define LFO_FMS_LBITS  9								// FIXED (LFO_FMS_BASE gives somethink as 1)
#define LFO_FMS_BASE   ((int) (0.05946309436 * 0.0338 * (double) (1 << LFO_FMS_LBITS)))

#define S0             0
#define S1             2
#define S2             1
#define S3             3


typedef struct slot__ {
  int *DT;  // paramètre detune
  int MUL;  // paramètre "multiple de fréquence"
  int TL;    // Total Level = volume lorsque l'enveloppe est au plus haut
  int TLL;  // Total Level ajusted
  int SLL;  // Sustin Level (ajusted) = volume o?l'enveloppe termine sa première phase de régression
  int KSR_S;  // Key Scale Rate Shift = facteur de prise en compte du KSL dans la variations de l'enveloppe
  int KSR;  // Key Scale Rate = cette valeur est calculée par rapport ?la fréquence actuelle, elle va influer
        // sur les différents paramètres de l'enveloppe comme l'attaque, le decay ...  comme dans la réalit?!
  int SEG;  // Type enveloppe SSG
  int *AR;  // Attack Rate (table pointeur) = Taux d'attaque (AR[KSR])
  int *DR;  // Decay Rate (table pointeur) = Taux pour la régression (DR[KSR])
  int *SR;  // Sustin Rate (table pointeur) = Taux pour le maintien (SR[KSR])
  int *RR;  // Release Rate (table pointeur) = Taux pour le relâchement (RR[KSR])
  int Fcnt;  // Frequency Count = compteur-fréquence pour déterminer l'amplitude actuelle (SIN[Finc >> 16])
  int Finc;  // frequency step = pas d'incrémentation du compteur-fréquence
        // plus le pas est grand, plus la fréquence est aïgu (ou haute)
  int Ecurp;  // Envelope current phase = cette variable permet de savoir dans quelle phase
        // de l'enveloppe on se trouve, par exemple phase d'attaque ou phase de maintenue ...
        // en fonction de la valeur de cette variable, on va appeler une fonction permettant
        // de mettre ?jour l'enveloppe courante.
  int Ecnt;  // Envelope counter = le compteur-enveloppe permet de savoir o?l'on se trouve dans l'enveloppe
  int Einc;  // Envelope step courant
  int Ecmp;  // Envelope counter limite pour la prochaine phase
  int EincA;  // Envelope step for Attack = pas d'incrémentation du compteur durant la phase d'attaque
        // cette valeur est égal ?AR[KSR]
  int EincD;  // Envelope step for Decay = pas d'incrémentation du compteur durant la phase de regression
        // cette valeur est égal ?DR[KSR]
  int EincS;  // Envelope step for Sustain = pas d'incrémentation du compteur durant la phase de maintenue
        // cette valeur est égal ?SR[KSR]
  int EincR;  // Envelope step for Release = pas d'incrémentation du compteur durant la phase de relâchement
        // cette valeur est égal ?RR[KSR]
  int *OUTp;  // pointeur of SLOT output = pointeur permettant de connecter la sortie de ce slot ?l'entrée
        // d'un autre ou carrement ?la sortie de la voie
  int INd;  // input data of the slot = données en entrée du slot
  int ChgEnM;  // Change envelop mask.
  int AMS;  // AMS depth level of this SLOT = degr?de modulation de l'amplitude par le LFO
  int AMSon;  // AMS enable flag = drapeau d'activation de l'AMS
} slot_;

typedef struct channel__ {
  int S0_OUT[4];      // anciennes sorties slot 0 (pour le feed back)
  int Old_OUTd;      // ancienne sortie de la voie (son brut)
  int OUTd;        // sortie de la voie (son brut)
  int LEFT;        // LEFT enable flag
  int RIGHT;        // RIGHT enable flag
  int ALGO;        // Algorythm = détermine les connections entre les opérateurs
  int FB;          // shift count of self feed back = degr?de "Feed-Back" du SLOT 1 (il est son unique entrée)
  int FMS;        // Fréquency Modulation Sensitivity of channel = degr?de modulation de la fréquence sur la voie par le LFO
  int AMS;        // Amplitude Modulation Sensitivity of channel = degr?de modulation de l'amplitude sur la voie par le LFO
  int FNUM[4];      // hauteur fréquence de la voie (+ 3 pour le mode spécial)
  int FOCT[4];      // octave de la voie (+ 3 pour le mode spécial)
  int KC[4];        // Key Code = valeur fonction de la fréquence (voir KSR pour les slots, KSR = KC >> KSR_S)
  struct slot__ SLOT[4];  // four slot.operators = les 4 slots de la voie
  int FFlag;        // Frequency step recalculation flag
} channel_;

typedef struct ym2612__ {
	int Clock;
	int Rate;
	int TimerBase;
	int Status;
	int OPNAadr;
	int OPNBadr;
	int LFOcnt;
	int LFOinc;
	
	int TimerA;
	int TimerAL;
	int TimerAcnt;
	int TimerB;
	int TimerBL;
	int TimerBcnt;
	int Mode;
	int DAC;
	int DACdata;
	double Frequence;
	unsigned int Inter_Cnt;
	unsigned int Inter_Step;
	struct channel__ CHANNEL[6];
	
	int *SIN_TAB[SIN_LENGHT];							// SINUS TABLE (pointer on TL TABLE)
	int TL_TAB[TL_LENGHT * 2];							// TOTAL LEVEL TABLE (positif and minus)
	unsigned int ENV_TAB[2 * ENV_LENGHT + 8];			// ENV CURVE TABLE (attack & decay)

	unsigned int DECAY_TO_ATTACK[ENV_LENGHT];			// Conversion from decay to attack phase
	unsigned int FINC_TAB[2048];						// Frequency step table

	unsigned int AR_TAB[128];							// Attack rate table
	unsigned int DR_TAB[96];							// Decay rate table
	unsigned int DT_TAB[8][32];							// Detune table
	unsigned int SL_TAB[16];							// Substain level table
	unsigned int NULL_RATE[32];							// Table for NULL rate

	int LFO_ENV_TAB[LFO_LENGHT];						// LFO AMS TABLE (adjusted for 11.8 dB)
	int LFO_FREQ_TAB[LFO_LENGHT];						// LFO FMS TABLE
	int LFO_ENV_UP[MAX_UPDATE_LENGHT];					// Temporary calculated LFO AMS (adjusted for 11.8 dB)
	int LFO_FREQ_UP[MAX_UPDATE_LENGHT];					// Temporary calculated LFO FMS

	int INTER_TAB[MAX_UPDATE_LENGHT];					// Interpolation table
	int LFO_INC_TAB[8];									// LFO step table

	int in0, in1, in2, in3;								// current phase calculation
	int en0, en1, en2, en3;								// current envelope calculation

	int int_cnt;                // Interpolation calculation

	int REG[2][0x100];
}YM2612;

int YM2612Init(int num, int clock, int rate);

/* shutdown the YM2151 emulators*/
void YM2612Shutdown();

/* reset all chip registers for YM2151 number 'num'*/
void YM2612ResetChip(int chipID);

/*
** Generate samples for one of the YM2151's
**
** 'num' is the number of virtual YM2151
** '**buffers' is table of pointers to the buffers: left and right
** 'length' is the number of samples that should be generated
*/
void YM2612UpdateOne(int chipID, int **buffers, int length);

/* write 'v' to register 'r' on YM2151 chip number 'n'*/
void YM2612WriteReg(int chipID, int r, int v);

/* read status register on YM2151 chip number 'n'*/
int YM2612ReadStatus(int chipID);


#ifdef __cplusplus
};
#endif

#endif
