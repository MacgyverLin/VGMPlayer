#include "VGMTitleRenderer.h"

VGMTitleRenderer::VGMTitleRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, const VGMTitleRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, skin(skin_)
	, font(nullptr)
{
}

VGMTitleRenderer::~VGMTitleRenderer()
{
}

void VGMTitleRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMTitleRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	texture.Load(info.texturePath);

	float scale = 16.0f / 300;
	font = videoDevice.CreateFont("arial.ttf", region.h * scale);

	titleString += name + " |";
	titleString += name + " |";
	titleString += name + " |";
	titleString += name + " |";
}

void VGMTitleRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.DestroyFont(font);
	font = nullptr;
}

void VGMTitleRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMTitleRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMTitleRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMTitleRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMTitleRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		float scaleY = (((float)region.h / texture.GetHeight()));
		float texWidth = scaleY * texture.GetWidth();
		float texHeight = region.h;

		float x0 = (region.w - texWidth) / 2.0f;
		float y0 = (region.h - texHeight) / 2.0f;
		float x1 = x0 + texWidth;
		float y1 = y0 + texHeight;

		static float x = 0;
		x -= 2.0f;
		if (x < -1000)
			x = region.w;

		/////////////////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, region.w, 0, region.h);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		/////////////////////////////////////////////////////////////////////////////////
		SetViewport(0, 0, 1, 1);

		videoDevice.ClearColor(Color::Black);
		videoDevice.Clear();

		videoDevice.Disable(VideoDeviceEnum::BLEND);


		texture.SetWrapS(VideoDeviceEnum::REPEAT);
		texture.SetWrapR(VideoDeviceEnum::REPEAT);
		texture.SetWrapT(VideoDeviceEnum::REPEAT);
		videoDevice.DrawTexSolidRectangle
		(
			texture,
			Vector2(x0	, y0), Color::White, Vector2(0, 0),
			Vector2(x1	, y0), Color::White, Vector2(1, 0),
			Vector2(x1	, y1), Color::White, Vector2(1, 1),
			Vector2(x0  , y1), Color::White, Vector2(0, 1)
		);

		//////////////////////////////////////////////////////////////////////////////
		// Draw Name
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, region.w, 0, region.h);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);
		SetViewport(0, 0, 1, 1);

		videoDevice.SetFont(font);
		videoDevice.SetFontScale(1.0);
		
		videoDevice.SetFontColor(Color::Black);
		videoDevice.DrawText(titleString.c_str(), x + 1, font->GetSize() / 2 - 1);

		videoDevice.SetFontColor(Color::BrightCyan);
		videoDevice.DrawText(titleString.c_str(), x + 0, font->GetSize() / 2);
	}
}
