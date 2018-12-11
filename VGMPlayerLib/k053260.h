#ifndef _k053260_h_
#define _k053260_h_

#include "vgmdef.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
	
void K053260_Initialize(INT32 chip, INT32 clock, INT32 sampleRate);
void K053260_Shutdown();
void K053260_Reset(INT32 chip);
void K053260_Update(INT32 chip, INT32 **pBuf, INT32 length);
void K053260_WriteRegister(INT32 chip, INT32 address, UINT8 data);
UINT32 K053260_ReadStatus(INT32 chip, INT32 address);
void K053260_SetROM(INT32 chip, INT32 totalROMSize, INT32 startAddress, UINT8 *rom, INT32 nLen);

#define BURN_SND_K053260_ROUTE_1		0
#define BURN_SND_K053260_ROUTE_2		1

#define K053260PCMSetAllRoutes(i, v, d)						\
	K053260SetRoute(i, BURN_SND_K053260_ROUTE_1, v, d);	\
	K053260SetRoute(i, BURN_SND_K053260_ROUTE_2, v, d);

#ifdef __cplusplus
};
#endif

#endif
