#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "VGMFile.h"
#include <zlib.h>
using namespace std;

class VGMFileImpl
{
public:
	string path;
	FILE *file;
	gzFile gzFile;
};

/////////////////////////////////////////////////////////
VGMFile::VGMFile(const string& path_, INT32 channels_, INT32 bitPerSample_, INT32 sampleRate_)
: VGMData(channels_, bitPerSample_, sampleRate_)
, impl(0)
{
	impl = new VGMFileImpl();

	impl->path = path_;
	impl->gzFile = 0;
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

BOOL VGMFile::onOpen()
{
	bool isvgz = (impl->path.find(".vgz") != -1);
	bool isvgm = (impl->path.find(".vgm") != -1);

	if (!isvgz && !isvgm)
	{
		return false;
	}

	if (isvgz)
	{
		impl->gzFile = gzopen(impl->path.c_str(), "r");
		if (!impl->gzFile)
			return false;
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

	if(impl->gzFile)
	{
		gzclose(impl->gzFile);
		impl->gzFile = 0;
	}
	else if (impl->file)
	{
		fclose(impl->file);
		impl->file = 0;
	}
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

BOOL VGMFile::onUpdate()
{
	return true;
}

INT32 VGMFile::onRead(VOID *buffer, UINT32 size)
{
	if(impl->gzFile)
	{
		return gzread(impl->gzFile, buffer, size);
	}
	else if (impl->file)
	{
		return fread(buffer, 1, size, impl->file);
	}
	else
	{
		return 0;
	}
}

INT32 VGMFile::onSeekSet(UINT32 offset)
{
	if(impl->gzFile)
	{
		return gzseek(impl->gzFile, offset, SEEK_SET);
	}
	else if (impl->file)
	{
		return fseek(impl->file, offset, SEEK_SET);
	}
	else
	{
		return 0;
	}
}

INT32 VGMFile::onSeekCur(UINT32 offset)
{
	if (impl->gzFile)
	{
		return gzseek(impl->gzFile, offset, SEEK_CUR);
	}
	else if (impl->file)
	{
		return fseek(impl->file, offset, SEEK_CUR);
	}
	else
	{
		return 0;
	}
}