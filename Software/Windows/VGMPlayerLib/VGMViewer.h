#ifndef _VGMViewer_h_
#define _VGMViewer_h_

#include "Platform.h"
#include "VGMData.h"
#include "VGMObverser.h"

#include "VideoDevice.h"
#include "VideoEncoder.h"

#include "VGMBackgroundRenderer.h"
#include "VGMWaveFormRenderer.h"
#include "VGMSpectrumRenderer.h"
#include "VGMMultiChannelWaveFormRenderer.h"
#include "VGMMultiChannelNoteRenderer.h"

class VGMViewer : public VGMObverser
{
public:
	VGMViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_);
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
	u32 x;
	u32 y;
	u32 width;
	u32 height;

	VideoDevice videoDevice;
	VideoEncoder videoEncoder;

	VGMBackgroundRenderer vgmBackgroundRenderer;
	VGMWaveFormRenderer vgmWaveFormRenderer;
	VGMSpectrumRenderer vgmSpectrumRenderer;
	VGMMultiChannelWaveFormRenderer vgmMultiChannelWaveFormRenderer;
#ifdef OLDLAYOUT
	VGMMultiChannelWaveFormRenderer vgmMultiChannelNoteRenderer;
#else
	VGMMultiChannelNoteRenderer vgmMultiChannelNoteRenderer;
#endif
};

#endif