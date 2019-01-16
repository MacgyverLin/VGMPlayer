#ifndef _ROM_h_
#define _ROM_h_

#include "vgmdef.h"
#include "Array.h"

class ROM
{
public:
	ROM();
	~ROM();

	void addROMSegment(u32 startAddress_, u32 length_, u8* data_, u32 wholeSize_);
	
	u8 getU8(u32 address_);
	u16 getU16(u32 address_);
	u32 getU32(u32 address_);
	void get(u8* buffer_, u32 address_, u32 length_);

	u8* getU8Ptr(u32 address_);
	u16* getU16Ptr(u32 address_);
	u32* getU32Ptr(u32 address_);
	void* getPtr(u32 address_);
private:
	Vector<u8> buffer;
};

#endif