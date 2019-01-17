#include "ROM.h"
#include <malloc.h>
ROM* ROM_Create()
{
#ifdef STM32
	ROM* rom = (ROM*)mymalloc(sizeof(ROM));
#else
	ROM* rom = (ROM*)malloc(sizeof(ROM));
#endif
	rom->buffer = 0;
	rom->length = 0;

	return rom;
}

void ROM_Release(ROM* rom)
{
	if (rom)
	{
		if (rom->buffer)
		{
#ifdef STM32
			myfree(rom->buffer);
#else
			free(rom->buffer);
#endif
			rom->buffer = 0;
		}

		rom->length = 0;

#ifdef STM32
		myfree(rom);
#else
		free(rom);
#endif
		rom = 0;
	}
}

void ROM_LoadData(ROM* rom, u32 startAddress_, u8* data_, u32 length_, u32 totalROMLength_)
{
	if (rom->length != totalROMLength_)
	{
#ifdef STM32
		u8 *newrom = (u8 *)mymalloc(totalROMLength_);
#else
		u8 *newrom = (u8 *)malloc(totalROMLength_);
#endif
		if(rom->buffer)
		{
			memcpy(newrom, rom->buffer, rom->length);
#ifdef STM32
			myfree(rom->buffer);
#else
			free(rom->buffer);
#endif
		}

		rom->buffer = newrom;
		rom->length = totalROMLength_;
	}

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

u16 ROM_getU16(ROM* rom, u32 address_)
{
	return *((u16*)(&rom->buffer[address_]));
}

u32 ROM_getU32(ROM* rom, u32 address_)
{
	return *((u32*)(&rom->buffer[address_]));
}

void ROM_get(ROM* rom, u8* buffer_, u32 address_, u32 length_)
{
	memcpy(buffer_, &rom->buffer[address_], length_);
}

u8* ROM_getU8Ptr(ROM* rom, u32 address_)
{
	return (u8*)(&rom->buffer[address_]);
}

u16* ROM_getU16Ptr(ROM* rom, u32 address_)
{
	return (u16*)(&rom->buffer[address_]);
}

u32* ROM_getU32Ptr(ROM* rom, u32 address_)
{
	return (u32*)(&rom->buffer[address_]);
}

void* ROM_getPtr(ROM* rom, u32 address_)
{
	return (void*)(&rom->buffer[address_]);
}
