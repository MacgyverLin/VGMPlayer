#ifndef	__NESFDSAPU__
#define	__NESFDSAPU__

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct
	{
		UINT8	reg[0x80];
		UINT8	volenv_mode;	// Volume Envelope
		UINT8	volenv_gain;
		UINT8	volenv_decay;
		FLOAT32	volenv_phaseacc;
		UINT8	swpenv_mode;	// Sweep Envelope
		UINT8	swpenv_gain;
		UINT8	swpenv_decay;
		FLOAT32	swpenv_phaseacc;
		// For envelope unit
		UINT8	envelope_enable;	// $4083 bit6
		UINT8	envelope_speed;	// $408A
								// For $4089
		UINT8	wave_setup;	// bit7
		INT32	master_volume;	// bit1-0
							// For Main unit
		INT32	main_wavetable[64];
		UINT8	main_enable;
		INT32	main_frequency;
		INT32	main_addr;
		// For Effector(LFO) unit
		UINT8	lfo_wavetable[64];
		UINT8	lfo_enable;	// 0:Enable 1:Wavetable setup
		INT32	lfo_frequency;
		INT32	lfo_addr;
		FLOAT32	lfo_phaseacc;
		// For Sweep unit
		INT32	sweep_bias;
		// Misc
		INT32	now_volume;
		INT32	now_freq;
		INT32	output;

		INT32	sampling_rate;
		INT32	output_buf[8];

		INT8 channelEnabled;
	}NESFDSAPU;

	INT32	NESFDSAPU_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate);
	void	NESFDSAPU_Shutdown();
	void	NESFDSAPU_Reset(UINT8 chipID);
	void	NESFDSAPU_WriteRegister(UINT8 chipID, UINT32 addr, UINT8 data);
	UINT8	NESFDSAPU_ReadRegister(UINT8 chipID, UINT32 addr);
	INT32	NESFDSAPU_Update(UINT8 chipID, INT32 **buffers, UINT32 length);
	INT32	NESFDSAPU_GetFreq(UINT8 chipID);
	void	NESFDSAPU_WriteSub(UINT8 chipID, UINT32 addr, UINT8 data);

	VOID NESFDSAPU_SetChannelEnabled(UINT8 chipID, INT8 enabled);
	INT8 NESFDSAPU_IsEnabledChannel(UINT8 chipID);

#ifdef __cplusplus
};
#endif


#endif