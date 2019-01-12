#ifndef _VGMLogoViewer_h_
#define _VGMLogoViewer_h_

#include "VGMData.h"
#include "VGMDataObverser.h"
#include "VideoDevice.h"

class VGMLogoViewer : public VGMDataObverser
{
public:
	VGMLogoViewer(const string& name_, const string& filename_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_);
	virtual ~VGMLogoViewer();
protected:
	virtual void onNotifySomething(Obserable& vgmData);
	virtual void onNotifyOpen(Obserable& vgmData);
	virtual void onNotifyClose(Obserable& vgmData);
	virtual void onNotifyPlay(Obserable& vgmData);
	virtual void onNotifyStop(Obserable& vgmData);
	virtual void onNotifyPause(Obserable& vgmData);
	virtual void onNotifyResume(Obserable& vgmData);
	virtual void onNotifyUpdate(Obserable& vgmData);
private:
	bool read_png_file(const char* file_name);
public:
protected:
private:
	VideoDevice videoDevice;

	string name;
	string filename;
	UINT32 x;
	UINT32 y;
	UINT32 width;
	UINT32 height;

	UINT32 texture;
};

#endif