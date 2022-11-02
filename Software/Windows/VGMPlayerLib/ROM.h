#ifndef _ROM_h_
#define _ROM_h_

#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	u8* buffer;
	u32 length;
}ROM;

ROM* ROM_Create(void);
void ROM_Release(ROM* rom);
void ROM_LoadData(ROM* rom, u32 startAddress_, u8* data_, u32 length_, u32 totalROMLength_);
u32 ROM_getTotalSize(ROM* rom);

u8 ROM_getU8(ROM* rom, u32 address_);
u8* ROM_getU8Ptr(ROM* rom, u32 address_);
u16 ROM_getU16(ROM* rom, u32 address_);
u16* ROM_getU16Ptr(ROM* rom, u32 address_);
u32 ROM_getU32(ROM* rom, u32 address_);
u32* ROM_getU32Ptr(ROM* rom, u32 address_);

s8 ROM_getS8(ROM* rom, u32 address_);
s8* ROM_getS8Ptr(ROM* rom, u32 address_);
s16 ROM_getS16(ROM* rom, u32 address_);
s16* ROM_getS16Ptr(ROM* rom, u32 address_);
s32 ROM_getS32(ROM* rom, u32 address_);
s32* ROM_getS32Ptr(ROM* rom, u32 address_);

f32* ROM_getF32Ptr(ROM* rom, u32 address_);

void* ROM_getPtr(ROM* rom, u32 address_);

void ROM_get(ROM* rom, u8* buffer_, u32 address_, u32 length_);


#ifdef __cplusplus
};
#endif

#endif
