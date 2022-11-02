#include "ROM.h"
#include <malloc.h>

ROM* ROM_Create()
{
	ROM* rom = (ROM*)malloc(sizeof(ROM));
	memset(rom, 0, sizeof(ROM));

	return rom;
}

void ROM_Release(ROM* rom)
{
	if (rom)
	{
		if (rom->buffer)
		{
			free(rom->buffer);
			rom->buffer = 0;
		}

		rom->length = 0;

		free(rom);
		rom = 0;
	}
}

void ROM_LoadData(ROM* rom, u32 startAddress_, u8* data_, u32 length_, u32 totalROMLength_)
{
	if (rom->length != totalROMLength_)
	{
		if(rom->buffer)
		{
			rom->buffer = realloc(rom->buffer, totalROMLength_);
		}
		else
		{
			rom->buffer = malloc(totalROMLength_);
		}

		rom->length = totalROMLength_;
	}

	assert(startAddress_ + length_ <= totalROMLength_);
	memcpy(&rom->buffer[startAddress_], data_, length_);
}

u32 ROM_getTotalSize(ROM* rom)
{
	return rom->length;
}

u8 ROM_getU8(ROM* rom, u32 address_)
{
	return *((u8*)(&rom->buffer[address_]));
}

u8* ROM_getU8Ptr(ROM* rom, u32 address_)
{
	return (u8*)(&rom->buffer[address_]);
}

u16 ROM_getU16(ROM* rom, u32 address_)
{
	return *((u16*)(&rom->buffer[address_]));
}

u16* ROM_getU16Ptr(ROM* rom, u32 address_)
{
	return (u16*)(&rom->buffer[address_]);
}

u32 ROM_getU32(ROM* rom, u32 address_)
{
	return *((u32*)(&rom->buffer[address_]));
}

u32* ROM_getU32Ptr(ROM* rom, u32 address_)
{
	return (u32*)(&rom->buffer[address_]);
}

s8 ROM_getS8(ROM* rom, u32 address_)
{
	return *((s8*)(&rom->buffer[address_]));
}

s8* ROM_getS8Ptr(ROM* rom, u32 address_)
{
	return (s8*)(&rom->buffer[address_]);
}

s16 ROM_getS16(ROM* rom, u32 address_)
{
	return *((s16*)(&rom->buffer[address_]));
}

s16* ROM_getS16Ptr(ROM* rom, u32 address_)
{
	return (s16*)(&rom->buffer[address_]);
}

s32 ROM_getS32(ROM* rom, u32 address_)
{
	return *((s32*)(&rom->buffer[address_]));
}

s32* ROM_getS32Ptr(ROM* rom, u32 address_)
{
	return (s32*)(&rom->buffer[address_]);
}


void ROM_get(ROM* rom, u8* buffer_, u32 address_, u32 length_)
{
	memcpy(buffer_, &rom->buffer[address_], length_);
}

void* ROM_getPtr(ROM* rom, u32 address_)
{
	return (void*)(&rom->buffer[address_]);
}
