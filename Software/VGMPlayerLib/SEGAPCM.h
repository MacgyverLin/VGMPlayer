#ifndef _SegaPCM_h_
#define _SegaPCM_h_

#include "vgmdef.h"
#include "ROM.h"

#ifdef __cplusplus
extern "C" {
#endif

#define   BANK_256    (11)
#define   BANK_512    (12)
#define   BANK_12M    (13)
#define   BANK_MASK7    (0x70<<16)
#define   BANK_MASKF    (0xf0<<16)
#define   BANK_MASKF8   (0xf8<<16)

s32 SEGAPCM_Initialize(u8 chipID, u32 clock, u32 sampleRate);
void SEGAPCM_Shutdown(u8 chipID);
void SEGAPCM_Reset(u8 chipID);
void SEGAPCM_WriteRegister(u8 chipID, u32 address, u32 data);
u8 SEGAPCM_ReadRegister(u8 chipID, u32 address);
void SEGAPCM_Update(u8 chipID, s32 baseChannel, s32** buffer, u32 length);
void SEGAPCM_SetROM(u8 chipID, ROM* rom);

void SEGAPCM_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
u8 SEGAPCM_GetChannelEnable(u8 chipID, u8 channel);
u32 SEGAPCM_GetChannelCount(u8 chipID);

#ifdef __cplusplus
};
#endif

#endif