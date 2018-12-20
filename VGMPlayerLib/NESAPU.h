#ifndef _NESAPU_H_
#define _NESAPU_H_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

	INT32 NESAPU_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate);
	void NESAPU_Shutdown(UINT8 chipID);
	void NESAPU_Reset(UINT8 chipID);
	void NESAPU_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data);
	UINT8 NESAPU_ReadRegister(UINT8 chipID, UINT32 address);
	void NESAPU_Update(UINT8 chipID, INT32** buffer, UINT32 length);

#ifdef __cplusplus
};
#endif


#endif