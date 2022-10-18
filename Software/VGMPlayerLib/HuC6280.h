#ifndef _HuC6280_h_
#define _HuC6280_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

s32 HUC6280_Initialize(u8 chipID, u32 clock, u32 sampleRate);
void HUC6280_Shutdown(u8 chipID);
void HUC6280_Reset(u8 chipID);
void HUC6280_Update(u8 chipID, s32 baseChannel, s32 **buffer, u32 length);
void HUC6280_WriteRegister(u8 chipID, u32 address, u8 data);
u8 HUC6280_ReadRegister(u8 chipID, u32 address);

void HUC6280_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
u8 HUC6280_GetChannelEnable(u8 chipID, u8 channel);
u32 HUC6280_GetChannelCount(u8 chipID);

#ifdef __cplusplus
};
#endif

#endif