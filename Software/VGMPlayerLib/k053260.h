#ifndef _K053260_h_
#define _K053260_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

s32 K053260_Initialize(u8 chipID, u32 clock, u32 sampleRate);
void K053260_Shutdown(u8 chipID);
void K053260_Reset(u8 chipID);
void K053260_WriteRegister(u8 chipID, u32 address, u8 data);
u8 K053260_ReadRegister(u8 chipID, u32 address);
void K053260_Update(u8 chipID, s32** buffer, u32 length);
void K053260_SetROM(u8 chipID, u32 totalROMSize, u32 startAddress, u8 *rom, u32 nLen);

#ifdef __cplusplus
};
#endif

#endif
