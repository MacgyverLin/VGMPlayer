#ifndef _HuC6280_h_
#define _HuC6280_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

void HuC6280_Initialize(INT32 chip, INT32 clock, INT32 sampleRate);
void HuC6280_Shutdown();
void HuC6280_Reset(INT32 chip);
void HuC6280_Update(INT32 chip, INT32 **buffer, INT32 length);
void HuC6280_WriteRegister(INT32 chip, INT32 address, UINT8 data);
UINT32 HuC6280_ReadStatus(INT32 chip, INT32 address);

#ifdef __cplusplus
};
#endif

#endif