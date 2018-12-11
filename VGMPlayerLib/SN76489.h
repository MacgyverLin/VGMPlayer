#ifndef _SN76489_h_
#define _SN76489_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

INT32 SN76489_Initialize(UINT8 chipCount, UINT32 clock, UINT32 rate);
void SN76489_Shutdown(void);
void SN76489_Reset(UINT8 chipID);
void SN76489_Update(UINT8 chipID, INT32 **buffers, UINT32 length);
void SN76489_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data);
UINT32 SN76489_ReadStatus(UINT8 chipID);

#ifdef __cplusplus
};
#endif

#endif
