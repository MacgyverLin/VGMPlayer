#ifndef _MultiPCM_h_
#define _MultiPCM_h_

#include "vgmdef.h"
#include "ROM.h"

#ifdef __cplusplus
extern "C" {
#endif

	s32 MultiPCM_Initialize(u8 chipID, u32 clock, u32 sampleRate);
	void MultiPCM_Shutdown(u8 chipID);
	void MultiPCM_Reset(u8 chipID);
	void MultiPCM_WriteRegister(u8 chipID, u32 address, u32 data, s32* channel, f32* freq);
	u8 MultiPCM_ReadRegister(u8 chipID, u32 address);
	void MultiPCM_Update(u8 chipID, s32** buffer, u32 length);
	void MultiPCM_SetROM(u8 chipID, ROM* rom);

	void MultiPCM_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
	u8 MultiPCM_GetChannelEnable(u8 chipID, u8 channel);
	u32 MultiPCM_GetChannelCount(u8 chipID);

	void MultiPCM_SetBank(u8 chipID, u32 leftoffs, u32 rightoffs);

#ifdef __cplusplus
};
#endif

#endif