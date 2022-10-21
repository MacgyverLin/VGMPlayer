#include "VGMAudioPlayer.h"

VGMAudioPlayer::VGMAudioPlayer()
	: VGMDataObverser()
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


	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	boolean rval = outputDevice.Open(info.channels, info.bitPerSamples, info.sampleRate, VGM_OUTPUT_BUFFER_COUNT);
}

void VGMAudioPlayer::OnNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.GetHeader();
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

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
	const VGMData::Info& info = vgmData.GetInfo();
	const VGMData::SystemChannels& systemChannels = vgmData.GetSystemChannels();

	if (systemChannels.GetHasNewSamples())
	{
		if (!outputDevice.Queue((void*)(&systemChannels.GetOutputSample(0, 0)), VGM_SAMPLE_COUNT * (sizeof(s16) * 2) ))
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
