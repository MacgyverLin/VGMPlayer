#include "ROM.h"

////////////////////////////////////////////////////////////////////
ROM::ROM()
{
	//memset(romSegments, 0, sizeof(ROMSegment*) * MAX_ROMSEGMENT_COUNT);
}

ROM::~ROM()
{
}

void ROM::addROMSegment(u32 startAddress_, u32 length_, u8* data_, u32 totalSize_)
{
	if (buffer.size() != totalSize_)
	{
		buffer.resize(totalSize_);
	}

	memcpy(&buffer[startAddress_], data_, length_);
}

u8 ROM::getU8(u32 address_)
{
	return *((u8*)(&buffer[address_]));
}

u16 ROM::getU16(u32 address_)
{
	return *((u16*)(&buffer[address_]));
}

u32 ROM::getU32(u32 address_)
{
	return *((u32*)(&buffer[address_]));
}

void ROM::get(u8* buffer_, u32 address_, u32 length_)
{
	memcpy(buffer_, &buffer[address_], length_);
}

u8* ROM::getU8Ptr(u32 address_)
{
	return (u8*)(&buffer[address_]);
}

u16* ROM::getU16Ptr(u32 address_)
{
	return (u16*)(&buffer[address_]);
}

u32* ROM::getU32Ptr(u32 address_)
{
	return (u32*)(&buffer[address_]);
}

void* ROM::getPtr(u32 address_)
{
	return (void*)(&buffer[address_]);
}