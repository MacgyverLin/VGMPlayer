#ifndef _HuC6280_h_
#define _HuC6280_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

INT32 HUC6280_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate);
void HUC6280_Shutdown(UINT8 chipID);
void HUC6280_Reset(UINT8 chipID);
void HUC6280_Update(UINT8 chipID, INT32 **buffer, UINT32 length);
void HUC6280_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data);
UINT8 HUC6280_ReadRegister(UINT8 chipID, UINT32 address);

#ifdef __cplusplus
};
#endif

#endif