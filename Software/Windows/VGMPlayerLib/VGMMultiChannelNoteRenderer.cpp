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
		const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

	font = videoDevice.CreateFont("arial.ttf", 12);

	logo.Load("..//images//QR.jpg");
}

void VGMMultiChannelNoteRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	
	const VGMInfo& vgmInfo = vgmData.GetInfo();
		const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

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
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

	
		/////////////////////////////////////////////////////////////////////////////////
		const vector<string>& outputCommand = outputChannels.GetOutputCommands();

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

		float scale = 20.0 * 1.0f / outputChannels.GetChannelsCount();
		videoDevice.SetFontScale(1.0);

		int y = 0;
		for (auto& s : commandStrings)
		{
			videoDevice.DrawText(s.c_str(), 0, y);

			y += 24;
		}

		float scaleY = (((float)region.h * 0.3f / logo.GetHeight()));
		float texWidth = scaleY * logo.GetWidth();
		float texHeight = region.h * 0.3f;

		float x0 = (region.w - texWidth) / 2.0f;
		float y0 = (region.h - texHeight) / 2.0f;
		float x1 = x0 + texWidth;
		float y1 = y0 + texHeight;
		
		x0 = region.w - texWidth;
		y0 = 0;
		x1 = x0 + texWidth;
		y1 = y0 + texHeight;

		videoDevice.DrawTexSolidRectangle
		(
			logo,
			Vector2(x0, y0), Color::White, Vector2(0, 0),
			Vector2(x1, y0), Color::White, Vector2(1, 0),
			Vector2(x1, y1), Color::White, Vector2(1, 1),
			Vector2(x0, y1), Color::White, Vector2(0, 1)
		);
}