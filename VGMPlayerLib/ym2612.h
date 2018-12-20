#ifndef _YM2612_H_
#define _YM2612_H_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

	INT32 YM2612_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate);
	void YM2612_Shutdown(UINT8 chipID);
	void YM2612_Reset(UINT8 chipID);
	void YM2612_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data);
	UINT8 YM2612_ReadStatus(UINT8 chipID);
	void YM2612_Update(UINT8 chipID, INT32 **buffers, UINT32 length);

#ifdef __cplusplus
};
#endif

#endif
