#ifndef _YM2151_h_
#define _YM2151_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 16- and 8-bit samples (signed) are supported*/
#define SAMPLE_BITS 32

#if (SAMPLE_BITS==32)
typedef INT32 SAMP;
#endif
#if (SAMPLE_BITS==16)
typedef INT16 SAMP;
#endif
#if (SAMPLE_BITS==8)
typedef INT8 SAMP;
#endif

INT32 YM2151_Initialize(UINT8 chipCount, UINT32 clock, UINT32 rate);
void YM2151_Shutdown(void);
void YM2151_Reset(UINT8 chipID);
void YM2151_Update(UINT8 chipID, INT32 **buffers, UINT32 length);
void YM2151_WriteRegister(UINT8 chipID, UINT8 address, UINT8 data);
UINT32 YM2151_ReadStatus(UINT8 chipID);

#ifdef __cplusplus
};
#endif

#endif /*_H_YM2151_*/