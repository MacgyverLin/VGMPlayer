#ifndef _VGMFile_h_
#define _VGMFile_h_

#include <zlib.h>
#include "VGMData.h"

class VGMFile : public VGMData
{
public:
	VGMFile(const string& path, INT32 channels_, INT32 bitPerSample_, INT32 sampleRate_);
	virtual ~VGMFile();
protected:
	virtual BOOL onOpen();
	virtual void onClose();
	virtual void onPlay();
	virtual void onStop();
	virtual void onPause();
	virtual void onResume();
	virtual BOOL onUpdate();
	virtual INT32 onRead(VOID *buffer, UINT32 size);
	virtual INT32 onSeekSet(UINT32 size);
	virtual INT32 onSeekCur(UINT32 size);
private:

public:
protected:
private:
	string path;
	FILE *file;
	gzFile gzFile;
};

#endif