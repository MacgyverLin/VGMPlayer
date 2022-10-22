#ifndef _SN76489_h_
#define _SN76489_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

	s32 SN76489_Initialize(u8 chipCount, u32 clock, u32 sampleRate);
	void SN76489_Shutdown(void);
	void SN76489_Reset(u8 chipID);
	void SN76489_WriteRegister(u8 chipID, u32 address, u8 data);
	u8 SN76489_ReadRegister(u8 chipID, u32 address);
	void SN76489_Update(u8 chipID, s32 **bufs, u32 length);

	void SN76489_SetChannelEnable(u8 chipID, u8 channel, u8 enable);
	u8 SN76489_GetChannelEnable(u8 chipID, u8 channel);
	u32 SN76489_GetChannelCount(u8 chipID);

#ifdef __cplusplus
};
#endif

#endif
