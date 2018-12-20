#ifndef _YM2151_h_
#define _YM2151_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

INT32 YM2151_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate);
void YM2151_Shutdown(UINT8 chipID);
void YM2151_Reset(UINT8 chipID);
void YM2151_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data);
UINT8 YM2151_ReadRegister(UINT8 chipID, UINT32 address);
void YM2151_Update(UINT8 chipID, INT32** buffer, UINT32 length);

#ifdef __cplusplus
};
#endif

#endif /*_H_YM2151_*/