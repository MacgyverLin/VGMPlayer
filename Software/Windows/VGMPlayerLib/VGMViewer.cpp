#include "VGMViewer.h"
#include "FFT.h"

/*
Rect GetRegion4(int width, int height)
{
	return Rect(0, height / 4 * 3.0f, width, height / 4 * 1.0f);
}

Rect GetRegion3(int width, int height)
{
	return Rect(0, height / 4 * 2.5f, width, height / 4 * 0.5f);
}

Rect GetRegion2(int width, int height)
{
	return Rect(0, height / 4 * 1.5f, width, height / 4 * 1.0f);
}

Rect GetRegion1(int width, int height)
{
	return Rect(0, height / 4 * 1.0f, width, height / 4 * 0.5f);
}

Rect GetRegion0(int width, int height)
{
	return Rect(0, height / 4 * 0.0f, width, height / 4 * 1.0f);
}
*/

Rect GetRegion4(int width, int height)
{
	return Rect(0, height / 4 * 2.5f, width, height / 4 * 1.5f);
}

Rect GetRegion3(int width, int height)
{
	return Rect(0, height / 4 * 2.0f, width, height / 4 * 0.5f);
}

Rect GetRegion2(int width, int height)
{
	return Rect(0, height / 4 * 1.0f, width, height / 4 * 1.0f);
}

Rect GetRegion1(int width, int height)
{
	return Rect(0, height / 4 * 0.5f, width, height / 4 * 0.5f);
}

Rect GetRegion0(int width, int height)
{
	return Rect(0, height / 4 * 0.0f, width, height / 4 * 0.5f);
}


VGMViewer::VGMViewer(const string& name_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_)
	: VGMObverser()
	, name(name_)
	, x(x_)
	, y(y_)
	, width(width_)
	, height(height_)

	, vgmBackgroundRenderer(videoDevice, "Background", Rect(0, 0, width, height), VGMBackgroundRenderer::Skin())

	, vgmTitleRenderer(videoDevice, "Title", GetRegion4(width, height), VGMTitleRenderer::Skin())

	, vgmWaveFormRenderer(videoDevice, "Output Wave", GetRegion3(width, height), 2.0f, VGMWaveFormRenderer::Skin())
	, vgmMultiChannelWaveFormRenderer(videoDevice, "Channels  Wave", GetRegion2(width, height), 2.0f, VGMMultiChannelWaveFormRenderer::Skin())
	, vgmSpectrumRenderer(videoDevice, "Spectrum", GetRegion1(width, height), 0.7f, VGMSpectrumRenderer::Skin())
	, vgmMultiChannelNoteRenderer(videoDevice, "Notes", GetRegion0(width, height), 2.0f, VGMMultiChannelNoteRenderer::Skin())
{
}

VGMViewer::~VGMViewer()
{
}

void VGMViewer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmBackgroundRenderer.OnNotifySomething(observable);

	vgmTitleRenderer.OnNotifySomething(observable);
	vgmWaveFormRenderer.OnNotifySomething(observable);
	vgmSpectrumRenderer.OnNotifySomething(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifySomething(observable);
	vgmMultiChannelNoteRenderer.OnNotifySomething(observable);
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

void VGMViewer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	std::string caption = name + vgmData.GetPath();
	std::string mp4File = vgmData.GetPath();
	mp4File = mp4File.substr(0, mp4File.find_last_of('.')) + ".mp4";

	videoDevice.Open(caption.c_str(), x, y, width, height);
	videoEncoder.Initiate(mp4File.c_str(), width, height);

	std::string title = vgmData.GetPath();
	title = title.substr(0, mp4File.find_last_of('.'));
	title = title.substr(mp4File.find_first_of('/'));

	title = ReplaceAll(title, "\\", " ");
	title = ReplaceAll(title, "/", " ");
	vgmTitleRenderer.SetName(title.c_str());

	vgmTitleRenderer.OnNotifyOpen(observable);
	vgmBackgroundRenderer.OnNotifyOpen(observable);
	vgmWaveFormRenderer.OnNotifyOpen(observable);
	vgmSpectrumRenderer.OnNotifyOpen(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyOpen(observable);
	vgmMultiChannelNoteRenderer.OnNotifyOpen(observable);
}

void VGMViewer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmTitleRenderer.OnNotifyClose(observable);
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

	vgmTitleRenderer.OnNotifyPlay(observable);
	vgmBackgroundRenderer.OnNotifyPlay(observable);
	vgmWaveFormRenderer.OnNotifyPlay(observable);
	vgmSpectrumRenderer.OnNotifyPlay(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyPlay(observable);
	vgmMultiChannelNoteRenderer.OnNotifyPlay(observable);
}

void VGMViewer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmTitleRenderer.OnNotifyStop(observable);
	vgmBackgroundRenderer.OnNotifyStop(observable);
	vgmWaveFormRenderer.OnNotifyStop(observable);
	vgmSpectrumRenderer.OnNotifyStop(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyStop(observable);
	vgmMultiChannelNoteRenderer.OnNotifyStop(observable);
}

void VGMViewer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmTitleRenderer.OnNotifyPause(observable);
	vgmBackgroundRenderer.OnNotifyPause(observable);
	vgmWaveFormRenderer.OnNotifyPause(observable);
	vgmSpectrumRenderer.OnNotifyPause(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyPause(observable);
	vgmMultiChannelNoteRenderer.OnNotifyPause(observable);
}

void VGMViewer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	vgmTitleRenderer.OnNotifyResume(observable);
	vgmBackgroundRenderer.OnNotifyResume(observable);
	vgmWaveFormRenderer.OnNotifyResume(observable);
	vgmSpectrumRenderer.OnNotifyResume(observable);
	vgmMultiChannelWaveFormRenderer.OnNotifyResume(observable);
	vgmMultiChannelNoteRenderer.OnNotifyResume(observable);
}

void VGMViewer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	
	if (vgmData.RequireFillBuffer())
	{
		videoDevice.MakeCurrent();
		videoDevice.ClearColor(Color(0.0, 0.0, 0.0, 1.0));
		videoDevice.Clear();

		vgmBackgroundRenderer.OnNotifyUpdate(observable);
		vgmTitleRenderer.OnNotifyUpdate(observable);
		vgmWaveFormRenderer.OnNotifyUpdate(observable);
		vgmSpectrumRenderer.OnNotifyUpdate(observable);
		vgmMultiChannelWaveFormRenderer.OnNotifyUpdate(observable);
		vgmMultiChannelNoteRenderer.OnNotifyUpdate(observable);

		videoDevice.Flush();

		Vector<unsigned char> colorBuffer;
		videoDevice.ReadPixels(colorBuffer);
		videoEncoder.Update(colorBuffer, vgmInfo.GetOutputBuffer());
	}
}