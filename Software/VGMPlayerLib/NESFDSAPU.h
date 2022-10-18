#ifndef	__NESFDSAPU__
#define	__NESFDSAPU__

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

	s32 NESFDSAPU_Initialize(u8 chipID, u32 clock, u32 sampleRate);
	void NESFDSAPU_Shutdown(u8 chipID);
	void NESFDSAPU_Reset(u8 chipID);
	void NESFDSAPU_WriteRegister(u8 chipID, u32 address, u8 data);
	u8 NESFDSAPU_ReadRegister(u8 chipID, u32 address);
	void NESFDSAPU_Update(u8 chipID, s32 baseChannel, s32** buffer, u32 length);

	void NESFDSAPU_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
	u8 NESFDSAPU_GetChannelEnable(u8 chipID, u8 channel);
	u32 NESFDSAPU_GetChannelCount(u8 chipID);

#ifdef __cplusplus
};
#endif


#endif