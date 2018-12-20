#ifndef _K053260_h_
#define _K053260_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

INT32 K053260_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate);
void K053260_Shutdown(UINT8 chipID);
void K053260_Reset(UINT8 chipID);
void K053260_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data);
UINT8 K053260_ReadRegister(UINT8 chipID, UINT32 address);
void K053260_Update(UINT8 chipID, INT32** buffer, UINT32 length);
void K053260_SetROM(UINT8 chipID, UINT32 totalROMSize, UINT32 startAddress, UINT8 *rom, UINT32 nLen);

#ifdef __cplusplus
};
#endif

#endif
