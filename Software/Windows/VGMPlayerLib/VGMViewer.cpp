#include "VGMViewer.h"
#include "FFT.h"
#include "VGMData.h"

VGMViewer::VGMViewer(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_)
	: VGMObverser()
	, name(name_)
	, x(x_)
	, y(y_)
	, width(width_ + (width_ % 2))
	, height(height_ + (height_ % 2))

	, vgmBackgroundRenderer(videoDevice, "Bkg", Rect(0, 0, width, height), VGMBackgroundRenderer::Skin())
#ifdef OLDLAYOUT
	, vgmWaveFormRenderer(videoDevice, "Output"					, Rect(0 + width / 2 * 0, height / 2 * 0, width / 2 * 1, height / 2 * 1), 5.0f, VGMWaveFormRenderer::Skin())
	, vgmSpectrumRenderer(videoDevice, "Spectrum"				, Rect(0 + width / 2 * 1, height / 2 * 0, width / 2 * 1, height / 2 * 1), 7.0f, VGMSpectrumRenderer::Skin())
	, vgmMultiChannelWaveFormRenderer(videoDevice, "Channels"	, Rect(0 + width / 2 * 0, height / 2 * 1, width / 2 * 1, height / 2 * 1), 1.0f, VGMMultiChannelWaveFormRenderer::Skin())
	, vgmMultiChannelNoteRenderer(videoDevice, "Channels"		, Rect(0 + width / 2 * 1, height / 2 * 1, width / 2 * 1, height / 2 * 1), 1.0f, VGMMultiChannelWaveFormRenderer::Skin())
#else
	, vgmWaveFormRenderer(videoDevice, "Output"					, Rect(0 + width / 3 * 0, height / 3 * 0, width / 3 * 2, height / 3 * 1), 5.0f, VGMWaveFormRenderer::Skin())
	, vgmSpectrumRenderer(videoDevice, "Spectrum"				, Rect(0 + width / 3 * 2, height / 3 * 0, width / 3 * 1, height / 3 * 1), 5.0f, VGMSpectrumRenderer::Skin())
	, vgmMultiChannelWaveFormRenderer(videoDevice, "Channels"	, Rect(0 + width / 3 * 2, height / 3 * 1, width / 3 * 1, height / 3 * 2), 1.0f, VGMMultiChannelWaveFormRenderer::Skin())
	, vgmMultiChannelNoteRenderer(videoDevice, "Notes"			, Rect(0				, height / 3 * 1, width / 3 * 2, height / 3 * 2), 1.0f, VGMMultiChannelNoteRenderer::Skin())
#endif
{
}

VGMViewer::~VGMViewer()
{
}

void VGMViewer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmBackgroundRenderer.OnNotifySomething(observable);
	vgmWaveFormRenderer.OnNotifySomething(observable);
	vgmSpectrumRenderer.OnNotifySomething(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifySomething(observable);
	vgmMultiChannelNoteRenderer.OnNotifySomething(observable);
}

void VGMViewer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	std::string caption = name + vgmData.GetInfo().vgmPath;
	std::string mp4File = vgmData.GetInfo().vgmPath;
	mp4File = mp4File.substr(0, mp4File.find_last_of('.')) + ".mp4";

	videoDevice.Open(caption.c_str(), x, y, width, height);
	videoEncoder.Initiate(mp4File.c_str(), width, height);

	vgmBackgroundRenderer.OnNotifyOpen(observable);
	vgmWaveFormRenderer.OnNotifyOpen(observable);
	vgmSpectrumRenderer.OnNotifyOpen(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyOpen(observable);
	vgmMultiChannelNoteRenderer.OnNotifyOpen(observable);
}

void VGMViewer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmBackgroundRenderer.OnNotifyClose(observable);
	vgmWaveFormRenderer.OnNotifyClose(observable);
	vgmSpectrumRenderer.OnNotifyClose(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyClose(observable);
	vgmMultiChannelNoteRenderer.OnNotifyClose(observable);

	videoEncoder.Terminate();
	videoDevice.Close();
}

void VGMViewer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmBackgroundRenderer.OnNotifyPlay(observable);
	vgmWaveFormRenderer.OnNotifyPlay(observable);
	vgmSpectrumRenderer.OnNotifyPlay(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyPlay(observable);
	vgmMultiChannelNoteRenderer.OnNotifyPlay(observable);
}

void VGMViewer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmBackgroundRenderer.OnNotifyStop(observable);
	vgmWaveFormRenderer.OnNotifyStop(observable);
	vgmSpectrumRenderer.OnNotifyStop(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyStop(observable);
	vgmMultiChannelNoteRenderer.OnNotifyStop(observable);
}

void VGMViewer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmBackgroundRenderer.OnNotifyPause(observable);
	vgmWaveFormRenderer.OnNotifyPause(observable);
	vgmSpectrumRenderer.OnNotifyPause(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyPause(observable);
	vgmMultiChannelNoteRenderer.OnNotifyPause(observable);
}

void VGMViewer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmBackgroundRenderer.OnNotifyResume(observable);
	vgmWaveFormRenderer.OnNotifyResume(observable);
	vgmSpectrumRenderer.OnNotifyResume(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyResume(observable);
	vgmMultiChannelNoteRenderer.OnNotifyResume(observable);
}

void VGMViewer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		videoDevice.MakeCurrent();
		videoDevice.ClearColor(Color(0.0, 0.0, 0.0, 1.0));
		videoDevice.Clear();

		/////////////////////////////////////////////////////////////////////////////////
		videoDevice.Viewport(0, 0, 1, 1);

		videoDevice.Disable(VideoDeviceEnum::BLEND);

		vgmBackgroundRenderer.OnNotifyUpdate(observable);
		vgmWaveFormRenderer.OnNotifyUpdate(observable);
		vgmSpectrumRenderer.OnNotifyUpdate(observable);
		vgmMultiChannelWaveFormRenderer.OnNotifyUpdate(observable);
		vgmMultiChannelNoteRenderer.OnNotifyUpdate(observable);

		videoDevice.Flush();

		Vector<unsigned char> colorBuffer;
		videoDevice.ReadPixels(colorBuffer);
		videoEncoder.Update(colorBuffer, systemChannels.GetOutputSampleBuffer());
	}
}