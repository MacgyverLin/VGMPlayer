#ifndef _NESAPU_H_
#define _NESAPU_H_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

	s32 NESAPU_Initialize(u8 chipID, u32 clock, u32 sampleRate);
	void NESAPU_Shutdown(u8 chipID);
	void NESAPU_Reset(u8 chipID);
	void NESAPU_WriteRegister(u8 chipID, u32 address, u32 data, s32* channel, f32* freq);
	u8 NESAPU_ReadRegister(u8 chipID, u32 address);
	void NESAPU_Update(u8 chipID, s32** bufs, u32 length);

	void NESAPU_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
	u8 NESAPU_GetChannelEnable(u8 chipID, u8 channel);
	u32 NESAPU_GetChannelCount(u8 chipID);

#ifdef __cplusplus
};
#endif


#endif