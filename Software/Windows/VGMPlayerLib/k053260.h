#ifndef _K053260_h_
#define _K053260_h_

#include "vgmdef.h"
#include "ROM.h"

#ifdef __cplusplus
extern "C" {
#endif

s32 K053260_Initialize(u8 chipID, u32 clock, u32 sampleRate);
void K053260_Shutdown(u8 chipID);
void K053260_Reset(u8 chipID);
void K053260_WriteRegister(u8 chipID, u32 address, u32 data, s32* channel, f32* freq);
u8 K053260_ReadRegister(u8 chipID, u32 address);
void K053260_Update(u8 chipID, s32** buffer, u32 length);
void K053260_SetROM(u8 chipID, ROM* rom);

void K053260_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
u8 K053260_GetChannelEnable(u8 chipID, u8 channel);
u32 K053260_GetChannelCount(u8 chipID);

#ifdef __cplusplus
};
#endif

#endif
