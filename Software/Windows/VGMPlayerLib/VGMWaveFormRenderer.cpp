#include "VGMWaveFormRenderer.h"

VGMWaveFormRenderer::VGMWaveFormRenderer(VideoDevice& videoDevice_, const char* name_, Rect region_, float waveScale_, const VGMWaveFormRenderer::Skin& skin_)
	: VGMRenderer(videoDevice_, name_, region_)
	, waveScale(waveScale_)
	, skin(skin_)
{
}

VGMWaveFormRenderer::~VGMWaveFormRenderer()
{
}

void VGMWaveFormRenderer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();
}

void VGMWaveFormRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;

	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();
}

void VGMWaveFormRenderer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMWaveFormRenderer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
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
			Vector2(0, 0), Color::Grey,
			Vector2(1, 0), Color::Grey,
			Vector2(1, 1), Color::Grey,
			Vector2(0, 1), Color::Grey
		);


		/////////////////////////////////////////////////////////////////////////////////
		int startX = 0;
		int endX = VGM_SAMPLE_BUFFER_SIZE;
		int divX = 10;

		int startY = -32767;
		int endY = 32768;
		int divY = 10;

		/////////////////////////////////////////////////////////////////////////////////
		videoDevice.MatrixMode(VideoDeviceEnum::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(startX, endX, startY, endY);
		videoDevice.MatrixMode(VideoDeviceEnum::MODELVIEW);

		videoDevice.Enable(VideoDeviceEnum::BLEND);
		for (int ch = 0; ch < 2; ch++)
		{
			SetViewport(0, ch * 1.0f / 2.0f, 1.0f, 1.0f / 2.0f);

			videoDevice.BlendFunc(VideoDeviceEnum::ONE, VideoDeviceEnum::ONE);
			videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
			for (s32 i = startX; i < endX; i += (endX - startX) / divX)
			{
				videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
			}
			for (s32 i = startY; i < endY; i += (endY - startY) / 10)
			{
				videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
			}
			videoDevice.DrawLine(Vector2(startX, 0), skin.axisColor, Vector2(endX, 0), skin.axisColor);

			const Color& c = (ch % 2) ? skin.rightColor : skin.leftColor;
			for (s32 i = startX; i < endX - 3; i += 3)
			{
				s32 y0 = systemChannels.GetOutputSample(ch, i + 0) * waveScale;
				s32 y1 = systemChannels.GetOutputSample(ch, i + 3) * waveScale;
				videoDevice.DrawLine(Vector2(i, y0), c, Vector2(i + 3, y1), c);
			}
		}
	}
}
