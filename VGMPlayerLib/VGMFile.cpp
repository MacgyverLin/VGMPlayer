#include "VGMFile.h"
#ifdef STM32
#else
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <zlib.h>
#endif
using namespace std;

class VGMFileImpl
{
public:
	string path;
	FILE *file;
#ifdef STM32
#else
	gzFile gzFile;
#endif
};

/////////////////////////////////////////////////////////
VGMFile::VGMFile(const string& path_, s32 channels_, s32 bitPerSample_, s32 sampleRate_)
: VGMData(channels_, bitPerSample_, sampleRate_)
, impl(0)
{
	impl = new VGMFileImpl();

	impl->path = path_;
#ifdef STM32
#else	
	impl->gzFile = 0;
#endif
	impl->file = 0;
}

VGMFile::~VGMFile()
{
	if (impl)
	{
		delete impl;
		impl = 0;
	}
}

boolean VGMFile::onOpen()
{
	boolean isvgz = (impl->path.find(".vgz") != -1);
	boolean isvgm = (impl->path.find(".vgm") != -1);

	if (!isvgz && !isvgm)
	{
		return false;
	}

	if (isvgz)
	{
#ifdef STM32
			return false;
#else		
		impl->gzFile = gzopen(impl->path.c_str(), "r");
		if (!impl->gzFile)
			return false;
#endif
	}
	else if (isvgm)
	{
		impl->file = fopen(impl->path.c_str(), "rb");
		if(!impl->file)
			return false;
	}

	notifyOpen();

	return true;
}

void VGMFile::onClose()
{
	notifyClose();

	const VGMHeader& header = getHeader();
	const VGMData::PlayInfo& playInfo = getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = getBufferInfo();

	if (impl->file)
	{
		fclose(impl->file);
		impl->file = 0;
	}	
#ifdef STM32
#else
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
	return true;
}

s32 VGMFile::onRead(void *buffer, u32 size)
{
	if (impl->file)
	{
		return fread(buffer, 1, size, impl->file);
	}
#ifdef STM32
#else	
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
	if (impl->file)
	{
		return fseek(impl->file, offset, SEEK_SET);
	}	
#ifdef STM32
#else	
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
	if (impl->file)
	{
		return fseek(impl->file, offset, SEEK_CUR);
	}	
#ifdef STM32
#else	
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
