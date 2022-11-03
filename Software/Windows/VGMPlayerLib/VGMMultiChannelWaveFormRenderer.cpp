#include "VGMMultiChannelWaveFormRenderer.h"

VGMMultiChannelWaveFormRenderer::VGMMultiChannelWaveFormRenderer(const char* name_, u32 x_, u32 y_, u32 width_, u32 height_, float waveScale_, const VGMMultiChannelWaveFormRenderer::Skin& skin_)
	: VGMRenderer(name_, x_, y_, width_, height_)
	, waveScale(waveScale_)
	, skin(skin_)
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

	texture.Load(info.texturePath);
}

void VGMMultiChannelWaveFormRenderer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();
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
		int startX = 0;
		int endX = VGM_SAMPLE_BUFFER_SIZE; //sampleCount;
		int divX = 10;

		int startY = -32767; //sampleCount/2;
		int endY = 32768; //sampleCount;
		int divY = 3;

		videoDevice.MatrixMode(VideoDevice::Constant::PROJECTION);
		videoDevice.LoadIdentity();
		videoDevice.Ortho2D(startX, endX, startY, endY);
		videoDevice.MatrixMode(VideoDevice::Constant::MODELVIEW);
		
		SetViewport(0, 0, 1, 1);

		videoDevice.Disable(VideoDevice::Constant::BLEND);
		videoDevice.DrawTexSolidRectangle
		(
			texture,
			Vector2(startX, startY), skin.bgColor, Vector2(0, 0),
			Vector2(endX, startY), skin.bgColor, Vector2(1, 0),
			Vector2(endX, endY), skin.bgColor, Vector2(1, 1),
			Vector2(startX, endY), skin.bgColor, Vector2(0, 1)
		);

		videoDevice.Enable(VideoDevice::Constant::BLEND);
		for (int ch = 0; ch < systemChannels.GetChannelsCount(); ch++)
		{
			float channelViewportHeight = 1.0f / systemChannels.GetChannelsCount() / 2;

			//////////////////////////////////////////////////////////////////////////////
			// draw grid
			SetViewport(0, (ch * 2 + 0) * channelViewportHeight, 1, channelViewportHeight);

			videoDevice.BlendFunc(VideoDevice::Constant::ONE, VideoDevice::Constant::ONE);
			videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
			for (s32 i = startX; i < endX; i += (endX - startX) / divX)
			{
				videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
			}
			for (s32 i = startY; i < endY; i += (endY - startY) / divY)
			{
				videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
			}
			videoDevice.DrawLine(Vector2(startX, 0), skin.axisColor, Vector2(endX, 0), skin.axisColor);

			int step = 1;
			Color c = skin.leftColor;
			for (s32 i = startX; i < endX - step; i += step)
			{
				s32 y0 = systemChannels.GetChannelLeftSample(ch + 0, i +    0) * 5 * waveScale;
				s32 y1 = systemChannels.GetChannelLeftSample(ch + 0, i + step) * 5 * waveScale;
				videoDevice.DrawLine(Vector2(i + 0, y0), c, Vector2(i + step, y1), c);
			}

			//////////////////////////////////////////////////////////////////////////////
			// draw grid
			SetViewport(0, (ch * 2 + 1) * channelViewportHeight, 1, channelViewportHeight);

			videoDevice.BlendFunc(VideoDevice::Constant::ONE, VideoDevice::Constant::ONE);
			videoDevice.DrawLine(Vector2(startX, 0), skin.gridColor, Vector2(endX, 0), skin.gridColor);
			for (s32 i = startX; i < endX; i += (endX - startX) / divX)
			{
				videoDevice.DrawLine(Vector2(i, startY), skin.gridColor, Vector2(i, endY), skin.gridColor);
			}
			for (s32 i = startY; i < endY; i += (endY - startY) / divY)
			{
				videoDevice.DrawLine(Vector2(startX, i), skin.gridColor, Vector2(endX, i), skin.gridColor);
			}
			videoDevice.DrawLine(Vector2(startX, 0), skin.axisColor, Vector2(endX, 0), skin.axisColor);

			step = 1;
			c = skin.rightColor;
			for (s32 i = startX; i < endX - step; i += step)
			{
				s32 y0 = systemChannels.GetChannelRightSample(ch + 0, i +    0) * 5 * waveScale;
				s32 y1 = systemChannels.GetChannelRightSample(ch + 0, i + step) * 5 * waveScale;
				videoDevice.DrawLine(Vector2(i + 0, y0), c, Vector2(i + step, y1), c);
			}
		}
	}
}
