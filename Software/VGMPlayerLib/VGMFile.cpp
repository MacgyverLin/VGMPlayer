#include "VGMFile.h"
#ifdef STM32
#include <ff.h>
#include <string>
using namespace std;
#else
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <zlib.h>
#endif
using namespace std;

#ifdef STM32
#define MAX_PATH 256
#else
#endif

class VGMFileImpl
{
public:
	char path[MAX_PATH];
#ifdef STM32
	FIL fil;
	boolean opened;
#else
	FILE *file;
	gzFile gzFile;
#endif
};

/////////////////////////////////////////////////////////
VGMFile::VGMFile(const char* path_, s32 channels_, s32 bitPerSample_, s32 sampleRate_)
: VGMData(channels_, bitPerSample_, sampleRate_)
, impl(0)
{
	impl = new VGMFileImpl();

	strncpy(impl->path, path_, MAX_PATH);
#ifdef STM32
	impl->opened = FALSE;
#else	
	impl->file = 0;
	impl->gzFile = 0;
#endif
}

VGMFile::~VGMFile()
{
	if (impl)
	{
#ifdef STM32
		if(impl->opened)
		{
			f_close(&impl->fil);
			impl->opened = FALSE;
		}
#else
#endif
		delete impl;
		impl = 0;
	}
}

boolean VGMFile::onOpen()
{
	boolean isvgz = (strstr(impl->path, ".vgz") != 0);
	boolean isvgm = (strstr(impl->path, ".vgm") != 0);
	if (!isvgz && !isvgm)
	{
		return FALSE;
	}

#ifdef STM32
	if(isvgm)
	{
		u8 res = f_open(&impl->fil, (const TCHAR*)impl->path, FA_READ);
		impl->opened = (res==FR_OK);

		return impl->opened;
	}
	else// if (isvgz)
	{
		// printf("vgz is not supported for STM32 platform");
		return FALSE;
	}
#else
	if(isvgm)
	{
		impl->file = fopen(impl->path, "rb");
		if(!impl->file)
			return FALSE;
	}
	else// if (isvgz)
	{
		impl->gzFile = gzopen(impl->path, "r");
		if (!impl->gzFile)
			return FALSE;
	}
#endif

	notifyOpen();

	return TRUE;
}

void VGMFile::onClose()
{
	notifyClose();

#ifdef STM32
	if (impl->opened)
	{
		f_close(&impl->fil);
		impl->opened = FALSE;
	}
#else
	if (impl->file)
	{
		fclose(impl->file);
		impl->file = 0;
	}
	else if(impl->gzFile)
	{
		gzclose(impl->gzFile);
		impl->gzFile = 0;
	}
#endif
}

void VGMFile::onPlay()
{
}

void VGMFile::onStop()
{
}

void VGMFile::onPause()
{
}

void VGMFile::onResume()
{
}

boolean VGMFile::onUpdate()
{
	return TRUE;
}

s32 VGMFile::onRead(void *buffer, u32 size)
{
#ifdef STM32
	UINT br;

	if(impl->opened)
	{
		f_read(&impl->fil, buffer, size, &br);
		return size;
	}
#else	
	if (impl->file)
	{
		return fread(buffer, 1, size, impl->file);
	}
	else if(impl->gzFile)
	{
		return gzread(impl->gzFile, buffer, size);
	}
#endif
	else
	{
		return 0;
	}
}

s32 VGMFile::onSeekSet(u32 offset)
{
#ifdef STM32
	if (impl->opened)
	{
		f_lseek(&impl->fil, offset);
		return offset;
	}	
#else	
	if (impl->file)
	{
		return fseek(impl->file, offset, SEEK_SET);
	}	
	else if(impl->gzFile)
	{
		return gzseek(impl->gzFile, offset, SEEK_SET);
	}
#endif
	else
	{
		return 0;
	}
}

s32 VGMFile::onSeekCur(u32 offset)
{
#ifdef STM32
	if (impl->opened)
	{
		f_lseek(&impl->fil, impl->fil.fptr + offset);
		return offset;
	}	
#else
	if (impl->file)
	{
		return fseek(impl->file, offset, SEEK_CUR);
	}
	else if (impl->gzFile)
	{
		return gzseek(impl->gzFile, offset, SEEK_CUR);
	}
#endif
	else
	{
		return 0;
	}
}
