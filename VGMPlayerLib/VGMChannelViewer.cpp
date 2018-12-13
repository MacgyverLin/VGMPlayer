#include "VGMChannelViewer.h"

VGMChannelViewer::VGMChannelViewer(UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_, const Color& bg_)
: VGMDataObverser()
, x(x_)
, y(y_)
, width(width_)
, height(height_)
, bg(bg_)
{
}

VGMChannelViewer::~VGMChannelViewer()
{
}

void VGMChannelViewer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.open(x, y, width, height);
}

void VGMChannelViewer::onNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.close();
}

void VGMChannelViewer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMChannelViewer::onNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.makeCurrent();

	videoDevice.clear(bg);
	Color lineColor(0.0, 0.3, 0.3, 1.0);

	videoDevice.drawLine(Vertex(-1, 0), Vertex(1, 0), lineColor);

	float totalSamples = 1 * VGM_SAMPLE_COUNT;
	float step = totalSamples / width;
	for(float i=0, sampleIdx=0; i<width-1; i++, sampleIdx+=step)
	{
		float x0 = (i  ) / (width / 2.0) - 1.0;
		float x1 = (i+1) / (width / 2.0) - 1.0;
		float y0 = bufferInfo.outputSamples[sampleIdx].l      / 30000.0;
		float y1 = bufferInfo.outputSamples[sampleIdx+step].l / 30000.0;
		videoDevice.drawLine(Vertex(x0, y0), Vertex(x1, y1), lineColor);
	}

	videoDevice.flush();
}