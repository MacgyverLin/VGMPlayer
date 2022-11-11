#include "VGMAudioPlayer.h"

VGMAudioPlayer::VGMAudioPlayer()
	: VGMObverser()
{
}

VGMAudioPlayer::~VGMAudioPlayer()
{
}

UINT32 VGMAudioPlayer::GetQueued()
{
	return outputDevice.GetQueued();
}

void VGMAudioPlayer::OnNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMAudioPlayer::OnNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	

	boolean rval = outputDevice.Open(vgmInfo.channels, vgmInfo.bitPerSamples, vgmInfo.SampleRate, VGM_OUTPUT_BUFFER_COUNT);
}

void VGMAudioPlayer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	

	outputDevice.Close();
}

void VGMAudioPlayer::OnNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMAudioPlayer::OnNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMAudioPlayer::OnNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMAudioPlayer::OnNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData&)observable;
}

void VGMAudioPlayer::OnNotifyUpdate(Obserable& observable, bool needUpdateSample)
{
	VGMData& vgmData = (VGMData&)observable;
	const VGMInfo& vgmInfo = vgmData.GetInfo();
	
	if (needUpdateSample)
	{
		if (!outputDevice.Queue(vgmInfo.GetOutputBufferPtr(), VGM_SAMPLE_BUFFER_SIZE * (sizeof(WAVE_16BS))))
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
