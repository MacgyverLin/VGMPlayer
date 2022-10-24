#include "VGMFile.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <zlib.h>
using namespace std;

#include "VideoDevice.h"

class VGMFileImpl
{
public:
	char path[MAX_PATH];

	FILE *file;
	gzFile gzFile;
};

/////////////////////////////////////////////////////////
VGMFile::VGMFile(const char* path_, const char* texturePath_, s32 channels_, s32 bitPerSample_, s32 sampleRate_)
: VGMData(texturePath_, channels_, bitPerSample_, sampleRate_)
, impl(0)
{
	impl = new VGMFileImpl();

	strncpy(impl->path, path_, MAX_PATH);
	impl->file = 0;
	impl->gzFile = 0;
}

VGMFile::~VGMFile()
{
	if (impl)
	{
		delete impl;
		impl = 0;
	}
}

boolean VGMFile::OnOpen()
{
	boolean isvgz = (strstr(impl->path, ".vgz") != 0);
	boolean isvgm = (strstr(impl->path, ".vgm") != 0);
	if (!isvgz && !isvgm)
	{
		return FALSE;
	}

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

	NotifyOpen();

	return TRUE;
}

void VGMFile::OnClose()
{
	NotifyClose();

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
}

void VGMFile::OnPlay()
{
}

void VGMFile::OnStop()
{
}

void VGMFile::OnPause()
{
}

void VGMFile::OnResume()
{
}

boolean VGMFile::OnUpdate()
{
	return TRUE;
}

s32 VGMFile::OnRead(void *buffer, u32 size)
{
	if (impl->file)
	{
		return fread(buffer, 1, size, impl->file);
	}
	else if(impl->gzFile)
	{
		return gzread(impl->gzFile, buffer, size);
	}
	else
	{
		return 0;
	}
}

s32 VGMFile::OnSeekSet(u32 offset)
{
	if (impl->file)
	{
		return fseek(impl->file, offset, SEEK_SET);
	}	
	else if(impl->gzFile)
	{
		return gzseek(impl->gzFile, offset, SEEK_SET);
	}
	else
	{
		return 0;
	}
}

s32 VGMFile::OnSeekCur(u32 offset)
{
	int c = gztell(impl->gzFile);

	if (impl->file)
	{
		return fseek(impl->file, offset, SEEK_CUR);
	}
	else if (impl->gzFile)
	{
		return gzseek(impl->gzFile, offset, SEEK_CUR);
	}
	else
	{
		return 0;
	}
}