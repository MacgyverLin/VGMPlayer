#include "VGMData.h"
#include "SN76489.h"
#include "YM2612.h"
#include "YM2151.h"
#ifdef STM32
#include "VGMBoard.h"
#else
#include "QSound.h"
#include "SEGAPCM.h"
#include "K053260.h"
#include "NESAPU.h"
#include "NESFDSAPU.h"
#include "HuC6280.h"
#include "ROM.h"
ROM* rom = 0;
#endif

VGMData::VGMData(s32 channels_, s32 bitPerSample_, s32 sampleRate_)
	: Obserable()
{
	playInfo.paused = TRUE;
	playInfo.playing = FALSE;
	playInfo.channels = channels_;
	playInfo.bitPerSamples = bitPerSample_;
	playInfo.sampleRate = sampleRate_;

#ifdef STM32
#else
	bufferInfo.sampleIdx = 0;
	bufferInfo.samplesL.resize(VGM_SAMPLE_COUNT);
	bufferInfo.samplesR.resize(VGM_SAMPLE_COUNT);
	bufferInfo.outputSamples.resize(VGM_SAMPLE_COUNT);

	bufferInfo.needQueueOutputSamples = FALSE;
#endif

	updateDataRequest = FALSE;
	updateSampleCounts = 0;
#ifdef STM32
#else
	rom = ROM_Create();
#endif
}

VGMData::~VGMData()
{
#ifdef STM32
#else
	if(rom)
	{
		ROM_Release(rom);
		
		rom = 0;
	}
#endif
}

u32 VGMData::getVersion()
{
	return header.version;
}

void VGMData::play()
{
	playInfo.playing = TRUE;

	notifyPlay();
}

void VGMData::stop()
{
	playInfo.playing = FALSE;

	notifyStop();
}

void VGMData::pause()
{
	playInfo.paused = TRUE;

	notifyPause();
}

void VGMData::resume()
{
	playInfo.paused = FALSE;

	notifyResume();
}

boolean VGMData::isPlaying()
{
	return playInfo.playing;
}

boolean VGMData::isPaused()
{
	return playInfo.paused;
}

void VGMData::requestUpdateData()
{
	updateDataRequest = TRUE;
}

const VGMHeader& VGMData::getHeader() const
{
	return header;
}

const VGMData::PlayInfo& VGMData::getPlayInfo() const
{
	return playInfo;
}

#ifdef STM32
#else
const VGMData::BufferInfo& VGMData::getBufferInfo() const
{
	return bufferInfo;
}
#endif

boolean VGMData::open()
{
	onOpen();

	memset(&header, 0, sizeof(header));
	if (read(&header, 0x38) != 0x38)
	{
		return FALSE;
	}

	if (header.ID[0] != 'V' ||
		header.ID[1] != 'g' ||
		header.ID[2] != 'm' ||
		header.ID[3] != ' ')
	{
		//vgm_log("Data is not a .vgm format");

		return FALSE;
	}

	// seek to first data
	u32 dataStart;
	if (((header.VGMDataOffset == 0x0c) && (header.version >= 0x150)) || (header.version < 0x150))
	{
		dataStart = 0x40;
	}
	else
	{
		dataStart = 0x34 + header.VGMDataOffset;
	}

	u32 byteRemained = dataStart - 0x38;
	if (byteRemained > 0)
		read((u8*)(&header) + 0x38, byteRemained);

	if (header.YM2612Clock)
	{
		YM2612_Initialize(0, header.YM2612Clock, playInfo.sampleRate);
	}
	if (header.SN76489Clock)
	{
		SN76489_Initialize(0, header.SN76489Clock, playInfo.sampleRate);
	}
	if (header.YM2151Clock)
	{
		YM2151_Initialize(0, header.YM2151Clock, playInfo.sampleRate);
	}
#ifdef STM32
#else	
	if (header.K053260Clock)
	{
		K053260_Initialize(0, header.K053260Clock, playInfo.sampleRate);

		K053260_SetROM(0, rom);
	}
	if (header.NESAPUClock)
	{
		NESAPU_Initialize(0, header.NESAPUClock & 0x7fffffff, playInfo.sampleRate);
		if ((header.NESAPUClock & 0x80000000) != 0)
			NESFDSAPU_Initialize(0, (header.NESAPUClock & 0x7fffffff), playInfo.sampleRate);
	}
	if (header.HuC6280Clock)
	{
		HUC6280_Initialize(0, header.HuC6280Clock, playInfo.sampleRate);
	}
	if (header.QSoundClock)
	{
		QSound_Initialize(0, header.QSoundClock, playInfo.sampleRate);
		
		QSound_SetROM(0, rom);
	}
	if (header.SegaPCMclock)
	{
		SEGAPCM_Initialize(0, header.SegaPCMclock, playInfo.sampleRate);

		SEGAPCM_SetROM(0, rom);
	}
#endif

	return TRUE;
}

void VGMData::close()
{
	if (header.YM2612Clock)
	{
		YM2612_Shutdown(0);
	}
	if (header.SN76489Clock)
	{
		SN76489_Shutdown();
	}
	if (header.YM2151Clock)
	{
		YM2151_Shutdown(0);
	}
#ifdef STM32
#else	
	if (header.K053260Clock)
	{
		K053260_Shutdown(0);
	}
	if (header.NESAPUClock)
	{
		NESAPU_Shutdown(0);
		if ((header.NESAPUClock & 0x80000000) != 0)
			NESFDSAPU_Shutdown(0);
	}
	if (header.HuC6280Clock)
	{
		HUC6280_Shutdown(0);
	}
	if (header.QSoundClock)
	{
		QSound_Shutdown(0);
	}
	if (header.SegaPCMclock)
	{
		SEGAPCM_Shutdown(0);
	}
#endif
	onClose();
}

s32 VGMData::read(void *buffer, u32 size)
{
	return onRead(buffer, size);
}

s32 VGMData::seekSet(u32 size)
{
	return onSeekSet(size);
}

s32 VGMData::seekCur(u32 size)
{
	return onSeekCur(size);
}

void VGMData::fillOutputBuffer()
{
#ifdef STM32
#else
	int i, out_L, out_R;
	short *dest = (short *)(&bufferInfo.outputSamples[0]);
	s32* l = &bufferInfo.samplesL[0];
	s32* r = &bufferInfo.samplesR[0];

	for (i = 0; i < VGM_SAMPLE_COUNT; i++) // always fill by fix size VGM_SAMPLE_COUNT
	{
		out_L = l[i];
		out_R = r[i];
		r[i] = 0;
		l[i] = 0;

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
#endif
}

u32 VGMData::updateSamples(u32 updateSampleCounts)
{
#ifdef STM32
	return updateSampleCounts;
#else
	updateSampleCounts = VGMPlayer_MIN((VGM_SAMPLE_COUNT - bufferInfo.sampleIdx), updateSampleCounts); // never exceed bufferSize
	if (updateSampleCounts == 0)
		return 0;

	s32* sampleBuffers[2];
	sampleBuffers[0] = &bufferInfo.samplesL[bufferInfo.sampleIdx];
	sampleBuffers[1] = &bufferInfo.samplesR[bufferInfo.sampleIdx];

	assert(bufferInfo.samplesL.size() == VGM_SAMPLE_COUNT);
	if (header.YM2612Clock)
		YM2612_Update(0, sampleBuffers, updateSampleCounts);
	if (header.SN76489Clock)
		SN76489_Update(0, sampleBuffers, updateSampleCounts);
	if (header.YM2151Clock)
		YM2151_Update(0, sampleBuffers, updateSampleCounts);
	if (header.K053260Clock)
		K053260_Update(0, sampleBuffers, updateSampleCounts);
	if (header.NESAPUClock)
	{
		NESAPU_Update(0, sampleBuffers, updateSampleCounts);
		if (header.NESAPUClock & 0x80000000)
			NESFDSAPU_Update(0, sampleBuffers, updateSampleCounts);
	}
	if (header.HuC6280Clock)
		HUC6280_Update(0, sampleBuffers, updateSampleCounts);
	if (header.QSoundClock)
		QSound_Update(0, sampleBuffers, updateSampleCounts);
	if (header.SegaPCMclock)
		SEGAPCM_Update(0, sampleBuffers, updateSampleCounts);

	bufferInfo.sampleIdx = bufferInfo.sampleIdx + updateSampleCounts;	// updated samples, sampleIdx+
	assert(bufferInfo.sampleIdx <= VGM_SAMPLE_COUNT);
	if (bufferInfo.sampleIdx == VGM_SAMPLE_COUNT)
	{
		bufferInfo.sampleIdx = 0;
		bufferInfo.needQueueOutputSamples = TRUE;

		fillOutputBuffer();
	}

	return updateSampleCounts;
#endif
}

void VGMData::handleEndOfSound()
{
}

void VGMData::handleK053260ROM(s32 skipByte0x66, s32 blockType, s32 blockSize)
{
#ifdef STM32
#else	
	unsigned int entireRomSize;
	read(&entireRomSize, sizeof(entireRomSize));

	unsigned int startAddress;
	read(&startAddress, sizeof(startAddress));

	vector<u8> romData;
	romData.resize(blockSize - 8);
	read(&romData[0], blockSize - 8);

	// k053260_write_rom(0, entireRomSize, startAddress, blockSize - 8, romData);
	ROM_LoadData(rom, startAddress, &romData[0], blockSize - 8, entireRomSize);
#endif
}

void VGMData::handleQSoundROM(s32 skipByte0x66, s32 blockType, s32 blockSize)
{
#ifdef STM32
#else	
	unsigned int entireRomSize;
	read(&entireRomSize, sizeof(entireRomSize));

	unsigned int startAddress;
	read(&startAddress, sizeof(startAddress));

	vector<u8> romData;
	romData.resize(blockSize - 8);
	read(&romData[0], blockSize - 8);

	//QSound_SetROM(0, entireRomSize, startAddress, &romData[0], blockSize - 8);
	//rom->addROMSegment(startAddress, &romData[0], blockSize - 8, entireRomSize);
	ROM_LoadData(rom, startAddress, &romData[0], blockSize - 8, entireRomSize);
#endif
}

void VGMData::handleSEGAPCMROM(s32 skipByte0x66, s32 blockType, s32 blockSize)
{
#ifdef STM32
#else	
	unsigned int entireRomSize;
	read(&entireRomSize, sizeof(entireRomSize));

	unsigned int startAddress;
	read(&startAddress, sizeof(startAddress));

	vector<u8> romData;
	romData.resize(blockSize - 8);
	read(&romData[0], blockSize - 8);

	// k053260_write_rom(0, entireRomSize, startAddress, blockSize - 8, romData);
	ROM_LoadData(rom, startAddress, &romData[0], blockSize - 8, entireRomSize);
#endif
}

void VGMData::handleDataBlocks()
{
	u8 skipByte0x66;
	read(&skipByte0x66, sizeof(skipByte0x66));

	u8 blockType;
	read(&blockType, sizeof(blockType));

	u32 blockSize;
	read(&blockSize, sizeof(blockSize));

#ifdef STM32
	seekCur(blockSize);
#else	
	if (blockType == 0x80)
	{
		handleSEGAPCMROM(skipByte0x66, blockType, blockSize);
	}
	else if (blockType == 0x8e)
	{
		handleK053260ROM(skipByte0x66, blockType, blockSize);
	}
	else if (blockType == 0x8f)
	{
		handleQSoundROM(skipByte0x66, blockType, blockSize);
	}
	else
	{
		seekCur(blockSize);
	}
#endif
}

boolean VGMData::update()
{
	if(updateDataRequest)
	{
		if(updateSampleCounts > 0)
		{
#ifdef STM32
			s32 nnnn = VGMBoard_UpdateSamples(updateSampleCounts);
			updateSampleCounts -= nnnn;
#else	
			s32 nnnn = updateSamples(updateSampleCounts);
			updateSampleCounts -= nnnn;
#endif
		}
		else
		{
			u8 command;
			read(&command, sizeof(command));

			u8 aa;
			u8 dd;
			u16 NNNN;

			u8 ss;
			u8 tt;
			u8 pp;
			u8 cc;
			u8 ll;
			u8 uu;
			u8 mm;
			u8 rr;
			u8 bb;
			u32 ffffffff;
			u32 aaaaaaaa;
			u32 llllllll;
			u16 bbbb;
			u8 ff;

			switch (command)
			{
			case YM2612_PORT0_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));			
		
				YM2612_WriteRegister(0, 0, aa);
				YM2612_WriteRegister(0, 1, dd);
				break;

			case YM2612_PORT1_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));

				YM2612_WriteRegister(0, 2, aa);
				YM2612_WriteRegister(0, 3, dd);
				break;

			case YM2151_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));
				YM2151_WriteRegister(0, aa, dd);
				break;

			case YM2203_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));
				//YM2203_WriteRegister(0, aa, dd);

				break;

			case GAME_GEAR_PSG_PORT6_WRITE:
				read(&dd, sizeof(dd));
				SN76489_WriteRegister(0, 0, dd);
				break;

			case SN76489_WRITE:
				read(&dd, sizeof(dd));
				SN76489_WriteRegister(0, 0, dd);
				break;

			case UNKNOWN_CHIP_A5_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));
#ifdef STM32
#else	
				//UNKNOWN_CHIP_A5_WRITE_WriteRegister(0, aa, dd);
#endif
				break;

			case K053260_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));
#ifdef STM32
#else	
				K053260_WriteRegister(0, aa, dd);
#endif
				break;

			case NES_APU_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));
#ifdef STM32
#else	
				NESAPU_WriteRegister(0, aa, dd);
				NESFDSAPU_WriteRegister(0, aa, dd);
#endif
				break;

			case OKIM6258_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));
#ifdef STM32
#else	
				// OKIM6258_WriteRegister(0, aa, dd);
#endif
				break;

			case OKIM6295_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));
#ifdef STM32
#else	
				// OKIM6295_WriteRegister(0, aa, dd);
#endif
				break;

			case HUC6280_WRITE:
				read(&aa, sizeof(aa));
				read(&dd, sizeof(dd));
#ifdef STM32
#else				
				HUC6280_WriteRegister(0, aa, dd);
#endif
				break;

			case DATA_BLOCKS:
				handleDataBlocks();
				break;

			case WAIT_NNNN_SAMPLES:
				read(&NNNN, sizeof(NNNN));
				updateSampleCounts += (((u32)NNNN) * playInfo.sampleRate / 44100);
				break;

			case WAIT_735_SAMPLES:
				updateSampleCounts += (735 * playInfo.sampleRate / 44100);
				break;

			case WAIT_882_SAMPLES:
				updateSampleCounts += (882 * playInfo.sampleRate / 44100);
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
				updateSampleCounts += (((command & 0x0f) + 1) * playInfo.sampleRate / 44100);
				break;

			case DAC_SETUP_STREAM_CONTROL:
				read(&ss, sizeof(ss));
				read(&tt, sizeof(tt));
				read(&pp, sizeof(pp));
				read(&cc, sizeof(cc));
#ifdef STM32
#else				
				//DACSetUpStreamControl(ss, tt, pp, cc);
#endif
				break;
			case DAC_SET_STREAM_DATA:
				read(&ss, sizeof(ss));
				read(&dd, sizeof(dd));
				read(&ll, sizeof(ll));
				read(&bb, sizeof(bb));
#ifdef STM32
#else				
				//DACSetStreamData(ss, dd, ll, bb);
#endif
				break;
			case DAC_SET_STREAM_FREQUENCY:
				read(&ss, sizeof(ss));
				read(&ffffffff, sizeof(ffffffff));
#ifdef STM32
#else				
				//DACSetStreamFrequency(ss, ffffffff);
#endif
				break;
			case DAC_START_STREAM:
				read(&ss, sizeof(ss));
				read(&aaaaaaaa, sizeof(aaaaaaaa));
				read(&mm, sizeof(mm));
				read(&llllllll, sizeof(llllllll));
#ifdef STM32
#else				
				//DACStartStream(ss, aaaaaaaa, mm, llllllll);
#endif
				break;
			case DAC_STOP_STREAM:
				read(&ss, sizeof(ss));
#ifdef STM32
#else				
				//DACStopStream(ss);
#endif
				break;
			case DAC_START_STEAM_FAST:
				read(&ss, sizeof(ss));
				read(&bbbb, sizeof(bbbb));
				read(&ff, sizeof(ff));
#ifdef STM32
#else				
				//DACStartStreamFast(ss, bbbb, ff);
#endif
				break;

			case QSOUND_WRITE:
				read(&mm, sizeof(mm));
				read(&ll, sizeof(ll));
				read(&rr, sizeof(rr));
#ifdef STM32
#else		
				QSound_WriteRegister(0, rr, (((u32)mm<<8) | (u32)ll));
#endif
				break;

			case SEGA_PCM:
				read(&ll, sizeof(ll));
				read(&uu, sizeof(uu));
				read(&dd, sizeof(dd));
#ifdef STM32
#else		
				SEGAPCM_WriteRegister(0, (((u32)uu)<<8) | ((u32)ll), dd);
#endif
				break;

			case END_OF_SOUND:
				handleEndOfSound();

				if (header.loopOffset)
				{
					seekSet(header.loopOffset + 0x1c);
					//printf("Loop();\n");
					return FALSE;
				}
				else
				{
					//printf("END_OF_SOUND();\n");
					return FALSE;
				}
				break;

			default:
				//printf("UnHandled Command: 0x%02x\n", command);
				return FALSE;
			};
		}

		updateDataRequest = FALSE;
	}

	notifyUpdate();
#ifdef STM32
#else
	bufferInfo.needQueueOutputSamples = FALSE;
#endif
	return TRUE;
}
