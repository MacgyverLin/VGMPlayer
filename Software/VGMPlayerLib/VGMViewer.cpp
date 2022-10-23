#include "VGMViewer.h"
#include "FFT.h"
#include <GL/glu.h>
#include <GL/gl.h>

#define FACTOR 0.9
#define WIDTH  (640 * FACTOR)
#define HEIGHT (480 * FACTOR)
#define TITLE_BAR 0
#define GAP 5

VGMViewer::VGMViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_)
	: VGMObverser()
	, name(name_)
	, x(x_)
	, y(y_)
	, width(width_)
	, height(height_)

//	, vgmWaveFormRenderer				("Output  ", 0                ,          0, width / 3, height * 1 / 3, 5.0f, VGMWaveFormRenderer::Skin())
//	, vgmSpectrumRenderer				("Spectrum", 0 + width / 3 * 1,          0, width / 3, height * 1 / 3, 5.0f, VGMSpectrumRenderer::Skin())
//	, vgmMultiChannelWaveFormRenderer	("Channels", 0 + width / 3 * 2,          0, width / 3, height * 1 / 3, 1.0f, VGMMultiChannelWaveFormRenderer::Skin())
//	, vgmMultiChannelNoteRenderer		("Notes   ", 0                , height / 3, width / 1, height * 2 / 3, 1.0f, VGMMultiChannelNoteRenderer::Skin())
	, vgmWaveFormRenderer				("Output  ", 0 + width / 3 * 2, height / 3 * 0, width * 1 / 3, height * 1 / 3, 5.0f, VGMWaveFormRenderer::Skin())
	, vgmSpectrumRenderer				("Spectrum", 0 + width / 3 * 2, height / 3 * 1, width * 1 / 3, height * 1 / 3, 5.0f, VGMSpectrumRenderer::Skin())
	, vgmMultiChannelWaveFormRenderer	("Channels", 0 + width / 3 * 2, height / 3 * 2, width * 1 / 3, height * 1 / 3, 1.0f, VGMMultiChannelWaveFormRenderer::Skin())
	, vgmMultiChannelNoteRenderer		("Notes   ", 0                , 0             , width * 2 / 3, height * 3 / 3, 1.0f, VGMMultiChannelNoteRenderer::Skin())
{
}

VGMViewer::~VGMViewer()
{
}

void VGMViewer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmWaveFormRenderer.OnNotifySomething(observable);
	vgmSpectrumRenderer.OnNotifySomething(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifySomething(observable);
	vgmMultiChannelNoteRenderer.OnNotifySomething(observable);
}

void VGMViewer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	videoDevice.Open(name.c_str(), x, y, width, height);

	vgmWaveFormRenderer.OnNotifyOpen(observable);
	vgmSpectrumRenderer.OnNotifyOpen(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyOpen(observable);
	vgmMultiChannelNoteRenderer.OnNotifyOpen(observable);
}

void VGMViewer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmWaveFormRenderer.OnNotifyClose(observable);
	vgmSpectrumRenderer.OnNotifyClose(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyClose(observable);
	vgmMultiChannelNoteRenderer.OnNotifyClose(observable);

	videoDevice.Close();
}

void VGMViewer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmWaveFormRenderer.OnNotifyPlay(observable);
	vgmSpectrumRenderer.OnNotifyPlay(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyPlay(observable);
	vgmMultiChannelNoteRenderer.OnNotifyPlay(observable);
}

void VGMViewer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmWaveFormRenderer.OnNotifyStop(observable);
	vgmSpectrumRenderer.OnNotifyStop(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyStop(observable);
	vgmMultiChannelNoteRenderer.OnNotifyStop(observable);
}

void VGMViewer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmWaveFormRenderer.OnNotifyPause(observable);
	vgmSpectrumRenderer.OnNotifyPause(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyPause(observable);
	vgmMultiChannelNoteRenderer.OnNotifyPause(observable);
}

void VGMViewer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmWaveFormRenderer.OnNotifyResume(observable);
	vgmSpectrumRenderer.OnNotifyResume(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyResume(observable);
	vgmMultiChannelNoteRenderer.OnNotifyResume(observable);
}

void VGMViewer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		videoDevice.MakeCurrent();
		videoDevice.Clear(Color(0.0, 0.0, 0.0, 1.0));

		vgmWaveFormRenderer.OnNotifyUpdate(observable);
		vgmSpectrumRenderer.OnNotifyUpdate(observable);
		vgmMultiChannelWaveFormRenderer.OnNotifyUpdate(observable);
		vgmMultiChannelNoteRenderer.OnNotifyUpdate(observable);

		videoDevice.Flush();
	}
}