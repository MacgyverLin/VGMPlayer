#ifndef	__NESFDSAPU__
#define	__NESFDSAPU__

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

	INT32 NESFDSAPU_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate);
	void NESFDSAPU_Shutdown(UINT8 chipID);
	void NESFDSAPU_Reset(UINT8 chipID);
	void NESFDSAPU_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data);
	UINT8 NESFDSAPU_ReadRegister(UINT8 chipID, UINT32 address);
	void NESFDSAPU_Update(UINT8 chipID, INT32** buffer, UINT32 length);

#ifdef __cplusplus
};
#endif


#endif