#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "VGMData.h"
#include "SN76489.h"
#include "YM2612.h"
#include "YM2151.h"
#include "K053260.h"
#include "NESAPU.h"
using namespace std;

VGMData::VGMData(INT32 channels_, INT32 bitPerSample_, INT32 sampleRate_)
: Obserable()
{
	playInfo.paused = true;
	playInfo.playing = false;
	playInfo.channels = channels_;
	playInfo.bitPerSamples = bitPerSample_;
	playInfo.sampleRate = sampleRate_;

	bufferInfo.sampleIdx = 0;
	bufferInfo.samplesL.resize(VGM_SAMPLE_COUNT);
	bufferInfo.samplesR.resize(VGM_SAMPLE_COUNT);

	bufferInfo.outputSampleBatchCount = 0;
	bufferInfo.outputSamples.resize(VGM_SAMPLE_COUNT * VGM_OUTPUT_BUFFER_COUNT);

	updateDataRequest = false;
}

VGMData::~VGMData()
{
}

UINT32 VGMData::getVersion()
{
	return header.version;
}

void VGMData::play()
{
	playInfo.playing = true;

	notifyPlay();
}

void VGMData::stop()
{
	playInfo.playing = false;

	notifyStop();
}

void VGMData::pause()
{
	playInfo.paused = true;

	notifyPause();
}

void VGMData::resume()
{
	playInfo.paused = false;

	notifyResume();
}

BOOL VGMData::isPlaying()
{
	return playInfo.playing;
}

BOOL VGMData::isPaused()
{
	return playInfo.paused;
}

void VGMData::requestUpdateData()
{
	updateDataRequest = true;
}

const VGMHeader& VGMData::getHeader() const
{
	return header;
}

const VGMData::PlayInfo& VGMData::getPlayInfo() const
{
	return playInfo;
}

const VGMData::BufferInfo& VGMData::getBufferInfo() const
{
	return bufferInfo;
}

BOOL VGMData::open()
{
	onOpen();

	int s = sizeof(header);
	if (read(&header, 256) != 256)
	{
		return false;
	}

	if (header.ID[0] != 'V' ||
		header.ID[1] != 'g' ||
		header.ID[2] != 'm' ||
		header.ID[3] != ' ')
	{
		vgm_log("Data is not a .vgm format");

		return false;
	}

	// seek to first data
	if (header.version < 0x150)
	{
		//assert(vgmData->header.VGMDataOffset == 0);
	}
	else if (header.version >= 0x150)
	{
		if (header.VGMDataOffset == 0x0c)
		{
			seekSet(0x40);		// point to 0x40
		}
		else
		{
			seekSet(0x34 + header.VGMDataOffset);		// point to 0x40
		}
	}

	if (header.YM2612Clock)
	{
		YM2612_Initialize(1, header.YM2612Clock, playInfo.sampleRate);
	}
	if (header.SN76489Clock)
	{
		SN76489_Initialize(1, header.SN76489Clock, playInfo.sampleRate);
	}

	if (header.YM2151Clock)
	{
		YM2151_Initialize(1, header.YM2151Clock, playInfo.sampleRate);
	}
	if (header.K053260Clock)
	{
		K053260_Initialize(0, header.K053260Clock, playInfo.sampleRate);
	}
	if (header.NESAPUClock)
	{
		if (header.NESAPUClock & 0x80000000)
		{
			NESAPU_Initialize(0, header.NESAPUClock & 0x7fffffff, playInfo.sampleRate);
		}
		else
		{
			NESAPU_Initialize(0, header.NESAPUClock & 0x7fffffff, playInfo.sampleRate);
		}
	}

	return true;
#if 0
	if (read(&header, 256) != 256)
	{
		return false;
	}

	if (header.ID[0] != 'V' ||
		header.ID[1] != 'g' ||
		header.ID[2] != 'm' ||
		header.ID[3] != ' ')
	{
		vgm_log("Data is not a .vgm format");

		return false;
	}

	if (header.Rate == 0)
		header.Rate = 60;

	// seek to first data
	if (header.version < 0x150)
	{
		assert(header.VGMDataOffset == 0);
	}
	else if (header.version == 0x150)
	{
		if (header.VGMDataOffset == 0x0c)
		{
			seekSet(0x40);		// point to 0x40
		}
		else
		{
			seekSet(0x34 + header.VGMDataOffset);		// point to 0x40
		}
	}
	else if (header.version == 0x151)
	{
		assert(false && "1.51 not tested yet");

		int remain = offsetof(VGMHeader, SegaPCMclock) - offsetof(VGMHeader, GameBoyDMGClock);
		if (read(&header.SegaPCMclock, remain) != remain)
		{
			return false;
		}
		header.VolumeModifier = 0;
		header.reserved0 = 0;

		if (header.VGMDataOffset == 0x0c)
		{
			seekSet(0x40);		// point to 0x40
		}
		else
		{
			seekSet(0x34 + header.VGMDataOffset);		// point to 0x40
		}
	}
	else if (header.version == 0x160)
	{
		assert(false && "1.60 not tested yet");

		int remain = offsetof(VGMHeader, SegaPCMclock) - offsetof(VGMHeader, GameBoyDMGClock);
		if (read(&header.SegaPCMclock, remain) != remain)
		{
			return false;
		}

		if (header.VGMDataOffset == 0x0c)
		{
			seekSet(0x40);		// point to 0x40
		}
		else
		{
			seekSet(0x34 + header.VGMDataOffset);		// point to 0x40
		}
	}
	else if (header.version == 0x161)
	{
		assert(false && "1.61 not tested yet");

		int remain = offsetof(VGMHeader, SegaPCMclock) - offsetof(VGMHeader, SCSPClock);
		if (read(&header.SegaPCMclock, remain) != remain)
		{
			return false;
		}

		if (header.VGMDataOffset == 0x0c)
		{
			seekSet(0x40);		// point to 0x40
		}
		else
		{
			seekSet(0x34 + header.VGMDataOffset);		// point to 0x40
		}
	}
	else if (header.version == 0x170)
	{
		assert(false && "1.70 not tested yet");

		int remain = offsetof(VGMHeader, SegaPCMclock) - offsetof(VGMHeader, WonderSwanClock);
		if (read(&header.SegaPCMclock, remain) != remain)
		{
			return false;
		}
		header.SCSPClock = 0;

		if (header.VGMDataOffset == 0x0c)
		{
			seekSet(0x40);		// point to 0x40
		}
		else
		{
			seekSet(0x34 + header.VGMDataOffset);		// point to 0x40
		}
	}
	else if (header.version == 0x171)
	{
		assert(false && "1.71 not tested yet");

		int remain = offsetof(VGMHeader, SegaPCMclock) - offsetof(VGMHeader, reserved9);
		if (read(&header.SegaPCMclock, remain) != remain)
		{
			return false;
		}

		if (header.VGMDataOffset == 0x0c)
		{
			seekSet(0x40);		// point to 0x40
		}
		else
		{
			seekSet(0x34 + header.VGMDataOffset);		// point to 0x40
		}
	}
#endif
}

void VGMData::close()
{
	if (header.YM2612Clock)
	{
		YM2612_Shutdown();
	}
	if (header.SN76489Clock)
	{
		SN76489_Shutdown();
	}
	if (header.YM2151Clock)
	{
		YM2151_Shutdown();
	}
	if (header.K053260Clock)
	{
		K053260_Shutdown();
	}
	if (header.NESAPUClock)
	{
		NESAPU_Shutdown();
	}

	onClose();
}

INT32 VGMData::read(VOID *buffer, UINT32 size)
{
	return onRead(buffer, size);
}

INT32 VGMData::seekSet(UINT32 size)
{
	return onSeekSet(size);
}

INT32 VGMData::seekCur(UINT32 size)
{
	return onSeekCur(size);
}

void VGMData::fillOutputBuffer()
{
	int i, out_L, out_R;
	short *dest = (short *)(&bufferInfo.outputSamples[bufferInfo.outputSampleBatchCount * VGM_SAMPLE_COUNT]);
	INT32* l = &bufferInfo.samplesL[0];
	INT32* r = &bufferInfo.samplesR[0];

	for (i = 0; i < VGM_SAMPLE_COUNT; i++) // always fill by fix size VGM_SAMPLE_COUNT
	{
		out_L = l[i];
		out_R = r[i];
		r[i] = 0;
		l[i] = 0;

		// *dest++ = VGMPlayer_MIN(VGMPlayer_MAX(l[i], -0x7FFF), 0x7FFF)
		// *dest++ = VGMPlayer_MIN(VGMPlayer_MAX(r[i], -0x7FFF), 0x7FFF)

		if (out_L < -0x7FFF)
			*dest++ = -0x7FFF;
		else if (out_L > 0x7FFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = out_L;

		if (out_R < -0x7FFF)
			*dest++ = -0x7FFF;
		else if (out_R > 0x7FFF)
			*dest++ = 0x7FFF;
		else
			*dest++ = out_R;
	}

	bufferInfo.outputSampleBatchCount += 1;
	if (bufferInfo.outputSampleBatchCount >= VGM_OUTPUT_BUFFER_COUNT)
	{
		int a = 1;
	}
}

void VGMData::handleWaitNNNNSample(unsigned short NNNN)
{
	INT32* sampleBuffers[2];

	int samplesToUpdate = NNNN * playInfo.sampleRate / 44100;
	while (samplesToUpdate > 0)
	{
		sampleBuffers[0] = &bufferInfo.samplesL[bufferInfo.sampleIdx];
		sampleBuffers[1] = &bufferInfo.samplesR[bufferInfo.sampleIdx];

		assert(bufferInfo.samplesL.size() == VGM_SAMPLE_COUNT);
		INT32 batchSampleSize = VGMPlayer_MIN((VGM_SAMPLE_COUNT - bufferInfo.sampleIdx), samplesToUpdate); // never exceed bufferSize

		if (header.YM2612Clock)
			YM2612_Update(0, sampleBuffers, batchSampleSize);
		if (header.SN76489Clock)
			SN76489_Update(0, sampleBuffers, batchSampleSize);
		if (header.YM2151Clock)
			YM2151_Update(0, sampleBuffers, batchSampleSize);
		if (header.K053260Clock)
			K053260_Update(0, sampleBuffers, batchSampleSize);
		if (header.NESAPUClock)
			NESAPU_Update(0, sampleBuffers, batchSampleSize);

		samplesToUpdate -= batchSampleSize;								// updated samples, remain-
		bufferInfo.sampleIdx = bufferInfo.sampleIdx + batchSampleSize;	// updated samples, sampleIdx+
		assert(bufferInfo.sampleIdx <= VGM_SAMPLE_COUNT);
		if (bufferInfo.sampleIdx == VGM_SAMPLE_COUNT)
		{
			bufferInfo.sampleIdx = 0;

			fillOutputBuffer();
		}
	}
}

void VGMData::handleEndOfSound()
{
}

void VGMData::handleK053260ROM(INT32 skipByte0x66, INT32 blockType, INT32 blockSize)
{
	unsigned int entireRomSize;
	read(&entireRomSize, sizeof(entireRomSize));

	unsigned int startAddress;
	read(&startAddress, sizeof(startAddress));

	vector<UINT8> romData;
	romData.resize(blockSize - 8);
	read(&romData[0], blockSize - 8);

	// k053260_write_rom(0, entireRomSize, startAddress, blockSize - 8, romData);
	K053260_SetROM(0, entireRomSize, startAddress, &romData[0], blockSize - 8);
}

void VGMData::handleDataBlocks()
{
	unsigned char skipByte0x66;

	read(&skipByte0x66, sizeof(skipByte0x66));

	unsigned char blockType;
	read(&blockType, sizeof(blockType));

	unsigned int blockSize;
	read(&blockSize, sizeof(blockSize));

	if (blockType == 0x8e)
	{
		handleK053260ROM(skipByte0x66, blockType, blockSize);
	}
	else
	{
		seekCur(blockSize);
	}
}

BOOL VGMData::update()
{
	if (updateDataRequest)
	{
		while(bufferInfo.outputSampleBatchCount < 1)
		{
			UINT8 command;
			read(&command, sizeof(command));

			UINT8 aa;
			UINT8 dd;
			UINT16 NNNN;
			switch (command)
			{
			case YM2612_PORT0_WRITE:
				read(&aa, sizeof(aa));
				YM2612_WriteRegister(0, 0, aa);

				read(&dd, sizeof(dd));
				YM2612_WriteRegister(0, 1, dd);
				break;

			case YM2612_PORT1_WRITE:
				read(&aa, sizeof(aa));
				YM2612_WriteRegister(0, 2, aa);

				read(&dd, sizeof(dd));
				YM2612_WriteRegister(0, 3, dd);
				break;

			case YM2151_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));
				YM2151_WriteRegister(0, aa, dd);

				break;

			case GAME_GEAR_PSG_PORT6_WRITE:
				read(&dd, sizeof(dd));
				SN76489_WriteRegister(0, -1, dd);

				break;

			case SN76489_WRITE:
				read(&dd, sizeof(dd));
				SN76489_WriteRegister(0, -1, dd);
				break;

			case WAIT_NNNN_SAMPLES:
				read(&NNNN, sizeof(NNNN));
				if (NNNN >= VGM_SAMPLE_COUNT*3/4 * VGM_OUTPUT_BUFFER_COUNT)
				{
					int a = 1;
				}
				else
				{
					handleWaitNNNNSample(NNNN);
				}
				break;

			case WAIT_735_SAMPLES:
				handleWaitNNNNSample(735);
				break;

			case WAIT_882_SAMPLES:
				handleWaitNNNNSample(882);
				break;

			case WAIT_1_SAMPLES:
			case WAIT_2_SAMPLES:
			case WAIT_3_SAMPLES:
			case WAIT_4_SAMPLES:
			case WAIT_5_SAMPLES:
			case WAIT_6_SAMPLES:
			case WAIT_7_SAMPLES:
			case WAIT_8_SAMPLES:
			case WAIT_9_SAMPLES:
			case WAIT_10_SAMPLES:
			case WAIT_11_SAMPLES:
			case WAIT_12_SAMPLES:
			case WAIT_13_SAMPLES:
			case WAIT_14_SAMPLES:
			case WAIT_15_SAMPLES:
			case WAIT_16_SAMPLES:
				handleWaitNNNNSample((command & 0x0f) + 1);
				break;

			case END_OF_SOUND:
				handleEndOfSound();
				printf("END_OF_SOUND();\n");
				return false;
				break;

			case DATA_BLOCKS:
				handleDataBlocks();
				break;

			case K053260_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));

				K053260_WriteRegister(0, aa, dd);
				break;

			case NES_APU_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));

				NESAPU_WriteRegister(0, aa, dd);
				break;

			default:
				printf("UnHandled Command: 0x%02x\n", command);
			};
		}

		updateDataRequest = false;
	};

	notifyUpdate();
	bufferInfo.outputSampleBatchCount = 0;

	return true;
}