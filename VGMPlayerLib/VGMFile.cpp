#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "VGMFile.h"
using namespace std;

/////////////////////////////////////////////////////////
VGMFile::VGMFile(const string& path_, INT32 channels_, INT32 bitPerSample_, INT32 sampleRate_)
: VGMData(channels_, bitPerSample_, sampleRate_)
, path(path_)
, file(0)
, gzFile(0)
{
}

VGMFile::~VGMFile()
{
}

BOOL VGMFile::onOpen()
{
	bool isvgz = (path.find(".vgz") != -1);
	bool isvgm = (path.find(".vgm") != -1);

	if (!isvgz && !isvgm)
	{
		return false;
	}

	if (isvgz)
	{
		gzFile = gzopen(path.c_str(), "r");
		if (!gzFile)
			return false;
	}
	else if (isvgm)
	{
		file = fopen(path.c_str(), "rb");
		if (!file)
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

	if (gzFile)
	{
		gzclose(gzFile);
		gzFile = 0;
	}
	else if (file)
	{
		fclose(file);
		file = 0;
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
	if (gzFile)
	{
		return gzread(gzFile, buffer, size);
	}
	else if (file)
	{
		return fread(buffer, 1, size, file);
	}
	else
	{
		return 0;
	}
}

INT32 VGMFile::onSeekSet(UINT32 offset)
{
	if (gzFile)
	{
		return gzseek(gzFile, offset, SEEK_SET);
	}
	else if (file)
	{
		return fseek(file, offset, SEEK_SET);
	}
	else
	{
		return 0;
	}
}

INT32 VGMFile::onSeekCur(UINT32 offset)
{
	if (gzFile)
	{
		return gzseek(gzFile, offset, SEEK_CUR);
	}
	else if (file)
	{
		return fseek(file, offset, SEEK_CUR);
	}
	else
	{
		return 0;
	}
}