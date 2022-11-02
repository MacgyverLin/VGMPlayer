#include "VGMData.h"
#include "SN76489.h"
#include "YM2612.h"
#include "YM2151.h"
#include "QSound.h"
#include "K053260.h"
#include "NESAPU.h"
#include "NESFDSAPU.h"
#include "HuC6280.h"
#include "SEGAPCM.h" 

#include "ROM.h"
#include "DataStream.h"

ROM* rom = nullptr;
DataStream* dataStream = nullptr;

VGMData::VGMData(const char* texturePath_, s32 channels_, s32 bitPerSample_, s32 sampleRate_)
	: Obserable()
	, info(texturePath_, channels_, bitPerSample_, sampleRate_)
{
	rom = ROM_Create();

	dataStream = DataStream_Create();
}

VGMData::~VGMData()
{
	if (dataStream)
	{
		DataStream_Release(dataStream);

		dataStream = nullptr;
	}

	if(rom)
	{
		ROM_Release(rom);
		
		rom = nullptr;
	}
}

u32 VGMData::GetVersion()
{
	return header.version;
}

void VGMData::Play()
{
	info.playing = TRUE;

	NotifyPlay();
}

void VGMData::Stop()
{
	info.playing = FALSE;

	NotifyStop();
}

void VGMData::Pause()
{
	info.paused = TRUE;

	NotifyPause();
}

void VGMData::Resume()
{
	info.paused = FALSE;

	NotifyResume();
}

boolean VGMData::IsPlaying()
{
	return info.playing;
}

boolean VGMData::IsPaused()
{
	return info.paused;
}

void VGMData::RequestUpdateData()
{
	info.updateDataRequest = TRUE;
}

const VGMHeader& VGMData::GetHeader() const
{
	return header;
}

const VGMData::Info& VGMData::GetInfo() const
{
	return info;
}

const VGMData::SystemChannels& VGMData::GetSystemChannels() const
{
	return systemChannels;
}

boolean VGMData::Open()
{
	OnOpen();

	memset(&header, 0, sizeof(header));
	if (Read(&header, 0x38) != 0x38)
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
		Read((u8*)(&header) + 0x38, byteRemained);

	int channelsCount = 0;
	if (header.YM2612Clock)
	{
		YM2612_Initialize(0, header.YM2612Clock, info.sampleRate);

		channelsCount += YM2612_GetChannelCount(0);
	}
	if (header.SN76489Clock)
	{
		SN76489_Initialize(0, header.SN76489Clock, info.sampleRate);

		channelsCount += SN76489_GetChannelCount(0);
	}
	if (header.YM2151Clock)
	{
		YM2151_Initialize(0, header.YM2151Clock, info.sampleRate);

		channelsCount += YM2151_GetChannelCount(0);
	}
	if (header.K053260Clock)
	{
		K053260_Initialize(0, header.K053260Clock, info.sampleRate);
		K053260_SetROM(0, rom);

		channelsCount += K053260_GetChannelCount(0);
	}
	if (header.NESAPUClock)
	{
		NESAPU_Initialize(0, header.NESAPUClock & 0x7fffffff, info.sampleRate);

		channelsCount += NESAPU_GetChannelCount(0);
	}
	if ((header.NESAPUClock & 0x80000000) != 0)
	{
		NESFDSAPU_Initialize(0, (header.NESAPUClock & 0x7fffffff), info.sampleRate);

		channelsCount += NESFDSAPU_GetChannelCount(0);
	}
	if (header.HuC6280Clock)
	{
		HUC6280_Initialize(0, header.HuC6280Clock, info.sampleRate);

		channelsCount += HUC6280_GetChannelCount(0);
	}
	if (header.QSoundClock)
	{
		QSound_Initialize(0, header.QSoundClock, info.sampleRate);
		QSound_SetROM(0, rom);

		channelsCount += QSound_GetChannelCount(0);
	}
	if (header.SegaPCMclock)
	{
		SEGAPCM_Initialize(0, header.SegaPCMclock, info.sampleRate);
		SEGAPCM_SetROM(0, rom);

		channelsCount += SEGAPCM_GetChannelCount(0);
	}

	systemChannels.SetChannelsCount(channelsCount);

	return TRUE;
}

void VGMData::Close()
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

	OnClose();
}

f32 VGMData::GetFrameCounter() const
{
	return info.frameCounter;
}

s32 VGMData::Read(void* buffer, u32 size)
{
	return OnRead(buffer, size);
}

s32 VGMData::SeekSet(u32 size)
{
	return OnSeekSet(size);
}

s32 VGMData::SeekCur(u32 size)
{
	return OnSeekCur(size);
}

u8 VGMData::GetChannelEnable(u32 channel)
{
	if (header.YM2612Clock)
	{
		return YM2612_GetChannelEnable(0, channel);
	}
	if (header.SN76489Clock)
	{
		return SN76489_GetChannelEnable(0, channel);
	}
	if (header.YM2151Clock)
	{
		return YM2151_GetChannelEnable(0, channel);
	}
	if (header.K053260Clock)
	{
		return K053260_GetChannelEnable(0, channel);
	}
	if (header.NESAPUClock)
	{
		return NESAPU_GetChannelEnable(0, channel);
	}
	if (header.NESAPUClock & 0x80000000)
	{
		return NESFDSAPU_GetChannelEnable(0, channel);
	}
	if (header.HuC6280Clock)
	{
		return HUC6280_GetChannelEnable(0, channel);
	}
	if (header.QSoundClock)
	{
		return QSound_GetChannelEnable(0, channel);
	}
	if (header.SegaPCMclock)
	{
		return SEGAPCM_GetChannelEnable(0, channel);
	}

	return 0;
}

void VGMData::SetChannelEnable(u32 channel, bool enable)
{
	if (header.YM2612Clock)
	{
		YM2612_SetChannelEnable(0, channel, enable);
	}
	if (header.SN76489Clock)
	{
		SN76489_SetChannelEnable(0, channel, enable);
	}
	if (header.YM2151Clock)
	{
		YM2151_SetChannelEnable(0, channel, enable);
	}
	if (header.K053260Clock)
	{
		K053260_SetChannelEnable(0, channel, enable);
	}
	if (header.NESAPUClock)
	{
		NESAPU_SetChannelEnable(0, channel, enable);
	}
	if (header.NESAPUClock & 0x80000000)
	{
		NESFDSAPU_SetChannelEnable(0, channel, enable);
	}
	if (header.HuC6280Clock)
	{
		HUC6280_SetChannelEnable(0, channel, enable);
	}
	if (header.QSoundClock)
	{
		QSound_SetChannelEnable(0, channel, enable);
	}
	if (header.SegaPCMclock)
	{
		SEGAPCM_SetChannelEnable(0, channel, enable);
	}
}

u32 VGMData::HandleUpdateSamples(u32 updateSampleCounts)
{
	updateSampleCounts = systemChannels.BeginUpdateSamples(updateSampleCounts);

	int currentChannel = 0;
	if (header.YM2612Clock)
	{
		currentChannel = systemChannels.HandleUpdateSamples(YM2612_Update, YM2612_GetChannelCount, 0, currentChannel, updateSampleCounts);
	}
	if (header.SN76489Clock)
	{
		currentChannel = systemChannels.HandleUpdateSamples(SN76489_Update, SN76489_GetChannelCount, 0, currentChannel, updateSampleCounts);
	}
	if (header.YM2151Clock)
	{
		currentChannel = systemChannels.HandleUpdateSamples(YM2151_Update, YM2151_GetChannelCount, 0, currentChannel, updateSampleCounts);
	}
	if (header.K053260Clock)
	{
		currentChannel = systemChannels.HandleUpdateSamples(K053260_Update, K053260_GetChannelCount, 0, currentChannel, updateSampleCounts);
	}
	if (header.NESAPUClock)
	{
		currentChannel = systemChannels.HandleUpdateSamples(NESAPU_Update, NESAPU_GetChannelCount, 0, currentChannel, updateSampleCounts);
	}
	if(header.NESAPUClock & 0x80000000)
	{
		currentChannel = systemChannels.HandleUpdateSamples(NESFDSAPU_Update, NESFDSAPU_GetChannelCount, 0, currentChannel, updateSampleCounts);
	}
	if (header.HuC6280Clock)
	{
		currentChannel = systemChannels.HandleUpdateSamples(HUC6280_Update, HUC6280_GetChannelCount, 0, currentChannel, updateSampleCounts);
	}
	if (header.QSoundClock)
	{
		currentChannel = systemChannels.HandleUpdateSamples(QSound_Update, QSound_GetChannelCount, 0, currentChannel, updateSampleCounts);
	}
	if (header.SegaPCMclock)
	{
		currentChannel = systemChannels.HandleUpdateSamples(SEGAPCM_Update, SEGAPCM_GetChannelCount, 0, currentChannel, updateSampleCounts);
	}

	systemChannels.EndUpdateSamples(updateSampleCounts);

	return updateSampleCounts;
}

void VGMData::HandleEndOfSound()
{
}

void VGMData::HandleDataBlock0x00To0x3F(u32 skipByte0x66, u8 blockType, u32 blockSize)
{
	vector<u8> dataStreamData;
	dataStreamData.resize(blockSize);
	Read(&dataStreamData[0], blockSize);

	DataStream_LoadData(dataStream, 0, &dataStreamData[0], blockSize, blockSize);
}

void VGMData::HandleDataBlock0x40To0x7E(u32 skipByte0x66, u8 blockType, u32 blockSize)
{
	/*
	u32 compressionType;
	Read(&compressionType, sizeof(compressionType));

	u32 sizeOfUncompressedData;
	Read(&sizeOfUncompressedData, sizeof(sizeOfUncompressedData));

	  tt (8 bits) = compression type
					   00 - bit packing compression
					   01 - DPCM compression
	   ss ss ss ss (32 bits) = size of uncompressed data (for memory allocation)
							   It is assumed that each decompressed value uses
							   ceil(bd/8) bytes.
	   (attr) = attribute bytes used by the decompression-algorithm
	   bit packing compression:
		   bd (8 bits) = Bits decompressed
		   bc (8 bits) = Bits compressed
		   st (8 bits) = compression sub-type
						   00 - copy (high bits aren't used)
						   01 - shift left (low bits aren't used)
						   02 - use table (data = index into decompression table,
										   see data block 7F)
		   aa aa (16 bits) = value that is added (ignored if table is used)
		   The data block is treated as a bitstream with bc bits per value. The
		   top bits in each byte are read first. The extracted bits of each value
		   are transformed into a value with at least bd bits using method st.
		   Finally, aaaa is added to get the resulting value.
	   DPCM-Compression: (uses a decompression table)
		   bd (8 bits) = Bits decompressed
		   bc (8 bits) = Bits compressed
		   st (8 bits) = [reserved for future use, must be 00]
		   aa aa (16 bits) = start value
		   The data is read as a bitstream (see bit packing). The read value is used as
		   index into a delta-table (defined by data block 7F). The delta value
		   is added to the "state" value, which is also the result value.
		   The "state" value is initialized with aaaa at the beginning.
	   (data) = compressed data, of size (block size - 0x0A - attr size)
	*/
}

void VGMData::HandleDataBlock0x80To0xBF(u32 skipByte0x66, u8 blockType, u32 blockSize)
{
	u32 entireRomSize;
	Read(&entireRomSize, sizeof(entireRomSize));

	u32 startAddress;
	Read(&startAddress, sizeof(startAddress));

	u32 dataSize = (blockSize & 0x7fffffff) - 8;
	if (dataSize > 0)
	{
		vector<u8> romData;
		romData.resize(dataSize);
		Read(&romData[0], dataSize);

		ROM_LoadData(rom, startAddress, &romData[0], dataSize, entireRomSize);
	}
}

void VGMData::HandleDataBlocks()
{
	u8 skipByte0x66;
	Read(&skipByte0x66, sizeof(skipByte0x66));

	u8 blockType;
	Read(&blockType, sizeof(blockType));

	u32 blockSize;
	Read(&blockSize, sizeof(blockSize));

	if (blockType >= 0x00 && blockType <= 0x3F)
		HandleDataBlock0x00To0x3F(skipByte0x66, blockType, blockSize);
	else if (blockType >= 0x40 && blockType <= 0x7E)
		HandleDataBlock0x40To0x7E(skipByte0x66, blockType, blockSize);
	else if (blockType >= 0x80 && blockType <= 0xBF)
		HandleDataBlock0x80To0xBF(skipByte0x66, blockType, blockSize);
	else
		SeekCur(blockSize);
}

const char* GetTimeCode(int frameCounter)
{
	static char buf[256];

	int seconds = frameCounter / 30;
	int minutes = seconds / 60;
	int hours = minutes / 60;
	
	int frames = frameCounter % 30;
	seconds = seconds % 60;
	minutes = minutes % 60;
	hours = hours % 24;

	sprintf(buf, "%02d:%02d:%02d:%02d", hours, minutes, seconds, frames);
	return buf;
}

boolean VGMData::Update()
{
	NotifyUpdate();

	systemChannels.ClearSampleBufferUpdatedEvent();

	if (info.updateDataRequest)
	{
		info.updateDataRequest = false;

		if (info.updateSampleCounts > 0)
		{
			s32 nnnn = HandleUpdateSamples(info.updateSampleCounts);
			info.updateSampleCounts -= nnnn;

			info.frameCounter += (((float)nnnn) / info.sampleRate) * VGM_FRAME_PER_SECOND;
		}
		else
		{
			u8 command;
			Read(&command, sizeof(command));

			u32 dataStreamSize;

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
			u32 dddddddd;
			u32 i;
			u32 count;
			u16 aabb;

			switch (command)
			{
			case YM2612_PORT0_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));

				systemChannels.WriteRegister(YM2612_WritePort0, 0, aa, dd, info.frameCounter);
				break;

			case YM2612_PORT1_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));

				systemChannels.WriteRegister(YM2612_WritePort1, 0, aa, dd, info.frameCounter);
				break;

			case YM2612_PORT0_ADDR_0X2A_WRITE_0X80:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X81:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X82:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X83:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X84:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X85:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X86:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X87:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X88:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X89:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X8A:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X8B:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X8C:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X8D:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X8E:
			case YM2612_PORT0_ADDR_0X2A_WRITE_0X8F:
				dd = DataStream_getU8(dataStream);
				systemChannels.WriteRegister(YM2612_WritePort0, 0, 0x2a, dd, info.frameCounter);
		
				count = command & 0x0f;
				info.updateSampleCounts += count * info.sampleRate / 44100;

				break;
				
			case SN76489_WRITE:
				Read(&dd, sizeof(dd));

				systemChannels.WriteRegister(SN76489_WriteRegister, 0, 0, dd, info.frameCounter);
				break;

			case GAME_GEAR_PSG_PORT6_WRITE:
				Read(&dd, sizeof(dd));

				systemChannels.WriteRegister(SN76489_WriteRegister, 0, 0, dd, info.frameCounter);
				break;

			case YM2151_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));
			
				systemChannels.WriteRegister(YM2151_WriteRegister, 0, aa, dd, info.frameCounter);
				break;

			case K053260_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));

				systemChannels.WriteRegister(K053260_WriteRegister, 0, aa, dd, info.frameCounter);
				break;

			case NES_APU_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));

				systemChannels.WriteRegister(NESAPU_WriteRegister, 0, aa, dd, info.frameCounter);
				systemChannels.WriteRegister(NESFDSAPU_WriteRegister, 0, aa, dd, info.frameCounter);
				break;

			case HUC6280_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));

				systemChannels.WriteRegister(HUC6280_WriteRegister, 0, aa, dd, info.frameCounter);
				break;

			case QSOUND_WRITE:
				Read(&mm, sizeof(mm));
				Read(&ll, sizeof(ll));
				Read(&rr, sizeof(rr));
				
				systemChannels.WriteRegister(QSound_WriteRegister, 0, rr, (((u32)mm << 8) | (u32)ll), info.frameCounter);
				break;

			case SEGA_PCM:
				Read(&ll, sizeof(ll));
				Read(&uu, sizeof(uu));
				Read(&dd, sizeof(dd));
		
				systemChannels.WriteRegister(SEGAPCM_WriteRegister, 0, (((u32)uu) << 8) | ((u32)ll), dd, info.frameCounter);
				break;

			case YM2203_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));

				//systemChannels.WriteRegister(YM2203_WriteRegister, 0, aa, dd, info.frameCounter);
				break;

			case OKIM6258_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));
				
				// systemChannels.WriteRegister(OKIM6258_WriteRegister, 0, aa, dd, info.frameCounter);
				break;

			case OKIM6295_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));

				// systemChannels.WriteRegister(OKIM6295_WriteRegister, 0, aa, dd, info.frameCounter);
				break;

			case K054539_WRITE:
				Read(&pp, sizeof(pp));
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));

				// systemChannels.WriteRegister(OKIM6295_WriteRegister, 0, aa, dd, info.frameCounter);
				break;

				

			case UNKNOWN_CHIP_A5_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));
				break;

			case MultiPCM_SETBANK_OFFSET: //0xC3:
				Read(&cc, sizeof(cc));
				Read(&aabb, sizeof(aabb));

				//MultiPCM_SetBank(0, (aabb & 7) * 0x80000, (aabb & 7) * 0x80000);
				//MultiPCM_SetBank(0, ((aabb >> 3) & 7) * 0x80000, (aabb & 7) * 0x80000);
				break;

			case MultiPCM_WRITE:
				Read(&aa, sizeof(aa));
				Read(&dd, sizeof(dd));
				//systemChannels.WriteRegister(MultiPCM_WriteRegister, 0, aa, dd, info.frameCounter);
				break;					

			////////////////////////////////////
			case DATA_BLOCKS:
				HandleDataBlocks();
				break;

			////////////////////////////////////
			case DAC_SETUP_STREAM_CONTROL:
				Read(&ss, sizeof(ss));
				Read(&tt, sizeof(tt));
				Read(&pp, sizeof(pp));
				Read(&cc, sizeof(cc));
				//DACSetUpStreamControl(ss, tt, pp, cc);
				break;

			case DAC_SET_STREAM_DATA:
				Read(&ss, sizeof(ss));
				Read(&dd, sizeof(dd));
				Read(&ll, sizeof(ll));
				Read(&bb, sizeof(bb));
				//DACSetStreamData(ss, dd, ll, bb);
				break;

			case DAC_SET_STREAM_FREQUENCY:
				Read(&ss, sizeof(ss));
				Read(&ffffffff, sizeof(ffffffff));
				//DACSetStreamFrequency(ss, ffffffff);
				break;

			case DAC_START_STREAM:
				Read(&ss, sizeof(ss));
				Read(&aaaaaaaa, sizeof(aaaaaaaa));
				Read(&mm, sizeof(mm));
				Read(&llllllll, sizeof(llllllll));
				//DACStartStream(ss, aaaaaaaa, mm, llllllll);
				break;

			case DAC_STOP_STREAM:
				Read(&ss, sizeof(ss));
				//DACStopStream(ss);
				break;

			case DAC_START_STEAM_FAST:
				Read(&ss, sizeof(ss));
				Read(&bbbb, sizeof(bbbb));
				Read(&ff, sizeof(ff));
				//DACStartStreamFast(ss, bbbb, ff);
				break;
			
			case SEEK_TO_OFFSET:
				Read(&dddddddd, sizeof(dddddddd));
				SeekCur(dddddddd);
				break;

			case WAIT_NNNN_SAMPLES:
				Read(&NNNN, sizeof(NNNN));
				info.updateSampleCounts += (((u32)NNNN) * info.sampleRate / 44100);
				break;

			case WAIT_735_SAMPLES:
				info.updateSampleCounts += (735 * info.sampleRate / 44100);
				break;

			case WAIT_882_SAMPLES:
				info.updateSampleCounts += (882 * info.sampleRate / 44100);
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
				info.updateSampleCounts += (((command & 0x0f) + 1) * info.sampleRate / 44100);
				break;

			case END_OF_SOUND:
				HandleEndOfSound();

				if (header.loopOffset)
				{
					SeekSet(header.loopOffset + 0x1c); // loop to head
					return FALSE;
				}
				else
				{
					return FALSE;  //can't find end loop to head
				}
				break;

			default:
				//printf("UnHandled Command: 0x%02x\n", command);
				return FALSE;
			};
		}
	}

	return TRUE;
}
