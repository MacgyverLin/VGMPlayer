#ifndef _VGMViewer_h_
#define _VGMViewer_h_

#include "Platform.h"
#include "VGMData.h"
#include "VGMObverser.h"

#include "VideoDevice.h"
#include "VideoEncoder.h"

#include "VGMTitleRenderer.h"
#include "VGMBackgroundRenderer.h"
#include "VGMWaveFormRenderer.h"
#include "VGMSpectrumRenderer.h"
#include "VGMMultiChannelWaveFormRenderer.h"
#include "VGMMultiChannelNoteRenderer.h"

class VGMViewer : public VGMObverser
{
public:
	VGMViewer(const string& name_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_);
	virtual ~VGMViewer();
protected:
	virtual void OnNotifySomething(Obserable& vgmData);
	virtual void OnNotifyOpen(Obserable& vgmData);
	virtual void OnNotifyClose(Obserable& vgmData);
	virtual void OnNotifyPlay(Obserable& vgmData);
	virtual void OnNotifyStop(Obserable& vgmData);
	virtual void OnNotifyPause(Obserable& vgmData);
	virtual void OnNotifyResume(Obserable& vgmData);
	virtual void OnNotifyUpdate(Obserable& vgmData);
private:
public:
protected:
private:
	string name;
	UINT32 x;
	UINT32 y;
	UINT32 width;
	UINT32 height;

	VideoDevice videoDevice;
	VideoEncoder videoEncoder;

	VGMBackgroundRenderer vgmBackgroundRenderer;

	VGMTitleRenderer vgmTitleRenderer;
	VGMWaveFormRenderer vgmWaveFormRenderer;
	VGMSpectrumRenderer vgmSpectrumRenderer;
	VGMMultiChannelWaveFormRenderer vgmMultiChannelWaveFormRenderer;
	VGMMultiChannelNoteRenderer vgmMultiChannelNoteRenderer;
};

#endif