#ifndef _YM2151_h_
#define _YM2151_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

s32 YM2151_Initialize(u8 chipID, u32 clock, u32 sampleRate);
void YM2151_Shutdown(u8 chipID);
void YM2151_Reset(u8 chipID);
void YM2151_WriteRegister(u8 chipID, u32 address, u32 data, s32* channel, f32* freq);
u8 YM2151_ReadRegister(u8 chipID, u32 address);
void YM2151_Update(u8 chipID, s32** bufs, u32 length);

void YM2151_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
u8 YM2151_GetChannelEnable(u8 chipID, u8 channel);
u32 YM2151_GetChannelCount(u8 chipID);

#ifdef __cplusplus
};
#endif

#endif /*_H_YM2151_*/
