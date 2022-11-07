#ifndef _YM2610_h_
#define _YM2610_h_

#include "vgmdef.h"
#include "fm.h"
#include "ay8910.h"
#include "emu2149.h"

s32 YM2610_Initialize(u8 chipID, u32 clock, u32 sampleRate);
void YM2610_Shutdown(u8 chipID);
void YM2610_Reset(u8 chipID);
void YM2610_Update(u8 chipID, s32** buffer, u32 length);
void YM2610_WriteRegister(u8 chipID, u32 addr, u32 data, s32* channel, f32* freq);
u8 YM2610_ReadRegister(u8 chipID, u32 address);

void YM2610_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
u8 YM2610_GetChannelEnable(u8 chipID, u8 channel);
u32 YM2610_GetChannelCount(u8 chipID);

#endif