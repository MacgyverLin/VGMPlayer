#include "VGMAudioPlayer.h"

VGMAudioPlayer::VGMAudioPlayer()
	: VGMObverser()
{
}

VGMAudioPlayer::~VGMAudioPlayer()
{
}

u32 VGMAudioPlayer::GetQueued()
{
	return outputDevice.GetQueued();
}

void VGMAudioPlayer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

	boolean rval = outputDevice.Open(info.channels, info.bitPerSamples, info.sampleRate, VGM_OUTPUT_BUFFER_COUNT);
}

void VGMAudioPlayer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

	outputDevice.Close();
}

void VGMAudioPlayer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMAudioPlayer::OnNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
	const VGMInfo& info = vgmData.GetInfo();
	const VGMOutputChannels& outputChannels = vgmData.GetOutputChannels();

	if (outputChannels.HasSampleBufferUpdatedEvent())
	{
		if (!outputDevice.Queue((void*)(&outputChannels.GetOutputSample(0, 0)), VGM_SAMPLE_BUFFER_SIZE * (sizeof(s16) * 2) ))
			return;

		if (outputDevice.GetDeviceState() != 3)
		{
			outputDevice.Play();

			//outputDevice.setVolume(1.0);
			//outputDevice.setPlayRate(1.0);
		}
	}

	outputDevice.Update();
}
