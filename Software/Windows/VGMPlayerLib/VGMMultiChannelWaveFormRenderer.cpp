#include "VGMMultiChannelWaveFormRenderer.h"

VGMMultiChannelWaveFormRenderer::VGMMultiChannelWaveFormRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMMultiChannelWaveFormRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, waveScale(waveScale_)
	, skin(skin_)
	, font(nullptr)
{
}

VGMMultiChannelWaveFormRenderer::~VGMMultiChannelWaveFormRenderer()
{
}

void VGMMultiChannelWaveFormRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	font = videoDevice.CreateFont("arial.ttf", 12);
}

void VGMMultiChannelWaveFormRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	videoDevice.DestroyFont(font);
	font = nullptr;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}
 
void VGMMultiChannelWaveFormRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMMultiChannelWaveFormRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	if (systemChannels.HasSampleBufferUpdatedEvent())
	{
		/////////////////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(0, 1, 0, 1);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		SetViewport(0, 0, 1, 1);

		videoDevice.Disable(VideoDeviceEnum::BLEND);
		videoDevice.DrawWireRectangle
		(
			Vector2(0.0, 0.0), Color::Grey,
			Vector2(1.0, 0.0), Color::Grey,
			Vector2(1.0, 1.0), Color::Grey,
			Vector2(0.0, 1.0), Color::Grey
		);

		/////////////////////////////////////////////////////////////////////////////////
		int startX = 0;
		int endX = VGM_SAMPLE_BUFFER_SIZE; //sampleCount;
		int divX = 20;

		int startY = -32767; //sampleCount/2;
		int endY = 32768; //sampleCount;
		int divY = 3;

		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(startX, endX, startY, endY);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		//////////////////////////////////////////////////////////////////////////////
		// draw grid
		videoDevice.Enable(VideoDeviceEnum::BLEND);
		videoDevice.BlendFunc(VideoDeviceEnum::SRC_ALPHA, VideoDeviceEnum::ONE_MINUS_SRC_ALPHA);

		for (int ch = 0; ch < 16; ch++)
		{
			float channelViewportWidth = 1.0f / 4;
			float channelViewportHeight = 1.0f / 4;

			int x = ch % 4;
			int y = ch / 4;

			SetViewport(x * channelViewportWidth, y * channelViewportHeight, channelViewportWidth, channelViewportHeight);
			
			for (s32 i = startX; i < endX; i += (endX - startX) / divX)
			{
				videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
			}
			for (s32 i = startY; i < endY; i += (endY - startY) / 10)
			{
				videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
			}

			videoDevice.DrawWireRectangle
			(
				Vector2(startX, startY), Color::Grey,
				Vector2(startX, endY), Color::Grey,
				Vector2(endX, endY), Color::Grey,
				Vector2(endX, startY), Color::Grey
			);
		}

		//////////////////////////////////////////////////////////////////////////////
		// draw channel	
		for (int ch = 0; ch < systemChannels.GetChannelsCount(); ch++)
		{
			float channelViewportWidth  = 1.0f / 4;
			float channelViewportHeight = 1.0f / 4;

			int x = ch % 4;
			int y = ch / 4;

			float vx = x * channelViewportWidth;
			float vy = y * channelViewportHeight;
			float vw = channelViewportWidth;
			float vh = channelViewportHeight;

			SetViewport(vx, vy, vw, vh);

			float i = ((float)(ch % 3) + 3) / 5;
			// Color c = Color(48.0f * i / 255.0f, 19.0f * i / 255.0f, 210.0f * i / 255.0f, 1.0f);
			Color c = Color::BrightCyan;
			for (s32 i = startX; i < endX - 3; i += 3)
			{
				s32 y0 = systemChannels.GetChannelLeftSample(ch, i + 0) * waveScale;
				s32 y1 = systemChannels.GetChannelLeftSample(ch, i + 3) * waveScale;
				videoDevice.DrawLine(Vector2(i, y0), c, Vector2(i + 3, y1), c);
			}
		}

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
	}
}
