#include "VGMAudioPlayer.h"

VGMAudioPlayer::VGMAudioPlayer()
	: VGMDataObverser()
{
}

VGMAudioPlayer::~VGMAudioPlayer()
{
}

u32 VGMAudioPlayer::getQueued()
{
	return outputDevice.getQueued();
}

void VGMAudioPlayer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::onNotifyOpen(Obserable& observable)
{
#ifdef STM32
#else
	VGMData& vgmData = (VGMData &)observable;


	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	boolean rval = outputDevice.open(playInfo.channels, playInfo.bitPerSamples, playInfo.sampleRate, VGM_OUTPUT_BUFFER_COUNT);
#endif
}

void VGMAudioPlayer::onNotifyClose(Obserable& observable)
{
#ifdef STM32
#else	
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	outputDevice.close();
#endif
}

void VGMAudioPlayer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::onNotifyUpdate(Obserable& observable)
{
#ifdef STM32
#else	
	VGMData& vgmData = (VGMData &)observable;
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	if (bufferInfo.needQueueOutputSamples)
	{
		if (!outputDevice.queue((void*)(&bufferInfo.outputSamples[0].l),
			VGM_SAMPLE_COUNT * sizeof(VGMData::OutputSample)))
			return;

		if (outputDevice.getDeviceState() != 3)
		{
			outputDevice.play();

			//outputDevice.setVolume(1.0);
			//outputDevice.setPlayRate(1.0);
		}
	}

	outputDevice.update();
#endif
}
