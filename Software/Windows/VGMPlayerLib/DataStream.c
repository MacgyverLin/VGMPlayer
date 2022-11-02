#include "DataStream.h"
#include <malloc.h>

DataStream* DataStream_Create()
{
	DataStream* dataStream = (DataStream*)malloc(sizeof(DataStream));
	memset(dataStream, 0, sizeof(DataStream));

	return dataStream;
}

void DataStream_Release(DataStream* dataStream)
{
	if (dataStream)
	{
		if (dataStream->buffer)
		{
			free(dataStream->buffer);
			dataStream->buffer = 0;
		}

		dataStream->current = 0;
		dataStream->length = 0;

		free(dataStream);
		dataStream = 0;
	}
}

void DataStream_LoadData(DataStream* dataStream, u32 startAddress_, u8* data_, u32 length_, u32 totalDataStreamLength_)
{
	if (dataStream->length != totalDataStreamLength_)
	{
		if(dataStream->buffer)
		{
			dataStream->buffer = realloc(dataStream->buffer, totalDataStreamLength_);
		}
		else
		{
			dataStream->buffer = malloc(totalDataStreamLength_);
		}

		dataStream->length = totalDataStreamLength_;
	}

	dataStream->current = 0;
	assert(startAddress_ + length_ <= totalDataStreamLength_);
	memcpy(&dataStream->buffer[startAddress_], data_, length_);
}

u32 DataStream_getTotalSize(DataStream* dataStream)
{
	return dataStream->length;
}

u8 DataStream_getU8(DataStream* dataStream)
{
	if (dataStream->current + 1 >= dataStream->length)
	{
		dataStream->current = 0;
		//assert(0);
	}

	u8 r = *((u8*)(&dataStream->buffer[dataStream->current]));
	dataStream->current += 1;

	return r;
}

u16 DataStream_getU16(DataStream* dataStream)
{
	if (dataStream->current + 2 >= dataStream->length)
	{
		assert(0);
	}

	u16 r = *((u16*)(&dataStream->buffer[dataStream->current]));
	dataStream->current += 2;

	return r;
}

u32 DataStream_getU32(DataStream* dataStream)
{
	if (dataStream->current + 4 >= dataStream->length)
	{
		assert(0);
	}

	u32 r = *((u32*)(&dataStream->buffer[dataStream->current]));
	dataStream->current += 4;

	return r;
}