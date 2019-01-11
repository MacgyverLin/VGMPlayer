#ifndef _NESAPU_H_
#define _NESAPU_H_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

	s32 NESAPU_Initialize(u8 chipID, u32 clock, u32 sampleRate);
	void NESAPU_Shutdown(u8 chipID);
	void NESAPU_Reset(u8 chipID);
	void NESAPU_WriteRegister(u8 chipID, u32 address, u8 data);
	u8 NESAPU_ReadRegister(u8 chipID, u32 address);
	void NESAPU_Update(u8 chipID, s32** buffer, u32 length);

#ifdef __cplusplus
};
#endif


#endif