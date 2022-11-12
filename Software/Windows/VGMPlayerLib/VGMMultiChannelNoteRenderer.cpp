#include "VGMMultiChannelNoteRenderer.h"
#include "FFT.h"

VGMMultiChannelNoteRenderer::VGMMultiChannelNoteRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMMultiChannelNoteRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, waveScale(waveScale_)
	, skin(skin_)
	, font(nullptr)
{
}

VGMMultiChannelNoteRenderer::~VGMMultiChannelNoteRenderer()
{
}

void VGMMultiChannelNoteRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMInfo& vgmInfo = vgmData.GetInfo();


	font = videoDevice.CreateFont("arial.ttf", 12);

	logo.Load("..//images//QR.jpg");
}

void VGMMultiChannelNoteRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;


	const VGMInfo& vgmInfo = vgmData.GetInfo();


	videoDevice.DestroyFont(font);
	font = nullptr;
}

void VGMMultiChannelNoteRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelNoteRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& vgmInfo = vgmData.GetInfo();

	vgmData.ClearCommand(1.0f);

	//////////////////////////////////////////////////////////////////////////////
	// Draw Name
	videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
	videoDevice.LoadIdentity();
	videoDevice.Ortho2D(0, region.w, 0, region.h);
	videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);
	SetViewport(0, 0, 1, 1);

	videoDevice.SetFont(font);
	videoDevice.SetFontScale(1.0);
	videoDevice.SetFontColor(Color::BrightCyan);
	videoDevice.DrawText(name.c_str(), 0, region.h - font->GetSize());

	//////////////////////////////////////////////////////////////////////////////
	// draw channel
	int y = 0;
	for (list<CPUCommand>::const_iterator itr = vgmInfo.GetOutputCommands().begin(); itr != vgmInfo.GetOutputCommands().end(); itr++)
	{
		char buf[1024];
		sprintf(buf, "%f, %s", itr->seconds, itr->command.c_str());
		videoDevice.DrawText(buf, 0, y);

		y += font->GetSize();
	}
}