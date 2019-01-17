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
void QSound_WriteRegister(u8 chipID, u32 address, u32 data);
u8 QSound_ReadRegister(u8 chipID, u32 address);
void QSound_Update(u8 chipID, s32** buffer, u32 length);
void QSound_SetROM(u8 chipID, ROM* rom);

#ifdef __cplusplus
};
#endif

#endif
