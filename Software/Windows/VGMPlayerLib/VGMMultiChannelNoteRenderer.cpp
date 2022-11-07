#include "VGMMultiChannelNoteRenderer.h"
#include "FFT.h"

VGMMultiChannelNoteRenderer::VGMMultiChannelNoteRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMMultiChannelNoteRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, waveScale(waveScale_)
	, skin(skin_)
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
	
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();

	font = videoDevice.CreateFont("arial.ttf", 24);
}

void VGMMultiChannelNoteRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();

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
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& systemChannels = vgmData.GetOutputChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		/////////////////////////////////////////////////////////////////////////////////
		const vector<string>& outputCommand = systemChannels.GetOutputCommands();

		for (int i = 0; i < outputCommand.size(); i++)
			commandStrings.push_back(outputCommand[i]);

#define MAX_COMMAND_STRINGS 40
		if (commandStrings.size() > MAX_COMMAND_STRINGS)
		{
			int remove = commandStrings.size() - MAX_COMMAND_STRINGS;

			std::list<string>::iterator startItr = commandStrings.begin();
			std::list<string>::iterator endItr = commandStrings.begin();
			std::advance(endItr, remove);

			commandStrings.erase(startItr, endItr);
		}

		/////////////////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, 1, 0, 1);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);
		SetViewport(0, 0, 1, 1);

		videoDevice.Disable(VideoDeviceEnum::BLEND);
		videoDevice.DrawWireRectangle
		(
			Vector2(0, 0), Color::Grey,
			Vector2(1, 0), Color::Grey,
			Vector2(1, 1), Color::Grey,
			Vector2(0, 1), Color::Grey
		);

		/////////////////////////////////////////////////////////////////////////////////
		Rect region = GetRegion();

		videoDevice.Enable(VideoDeviceEnum::BLEND);
		videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE_MINUS_SRC_ALPHA);

		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, region.w, 0, region.h);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);
		SetViewport(0, 0, 1, 1);

		videoDevice.SetFont(font);
		videoDevice.SetFontColor(Color::White);

		float scale = 20.0 * 1.0f / systemChannels.GetChannelsCount();
		videoDevice.SetFontScale(1.0);

		int y = 0;
		for (auto& s : commandStrings)
		{
			videoDevice.DrawText(s.c_str(), 0, y);

			y += 24;
		}
	}
}