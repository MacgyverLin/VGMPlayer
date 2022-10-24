#ifndef _DataStream_h_
#define _DataStream_h_

#include "vgmdef.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	u8* buffer;
	u32 length;
	u32 current;
}DataStream;

DataStream* DataStream_Create(void);
void DataStream_Release(DataStream* dataStream);
void DataStream_LoadData(DataStream* dataStream, u32 startAddress_, u8* data_, u32 length_, u32 totalDataStreamLength_);
u32 DataStream_getTotalSize(DataStream* dataStream);
u8 DataStream_getU8(DataStream* dataStream);
u16 DataStream_getU16(DataStream* dataStream);
u32 DataStream_getU32(DataStream* dataStream);

#ifdef __cplusplus
};
#endif

#endif
