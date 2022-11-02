#ifndef _QSound_h_
#define _QSound_h_

#include "vgmdef.h"
#include "ROM.h"

#ifdef __cplusplus
extern "C" {
#endif

s32 QSound_Initialize(u8 chipID, u32 clock, u32 sampleRate);
void QSound_Shutdown(u8 chipID);
void QSound_Reset(u8 chipID);
void QSound_WriteRegister(u8 chipID, u32 addr, u32 data, s32* channel, f32* freq);
u8 QSound_ReadRegister(u8 chipID, u32 address);
void QSound_Update(u8 chipID, s32** buffer, u32 length);
void QSound_SetROM(u8 chipID, ROM* rom);

void QSound_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
u8 QSound_GetChannelEnable(u8 chipID, u8 channel);
u32 QSound_GetChannelCount(u8 chipID);

#ifdef __cplusplus
};
#endif

#endif
