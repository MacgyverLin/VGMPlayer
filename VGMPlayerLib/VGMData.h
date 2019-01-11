#ifndef _VGMData_h_
#define _VGMData_h_

// https://vgmrips.net/wiki/VGM_Specification
// http://www.project2612.org/list.php?page=T
// Unused space(marked with *) is reserved for future expansion, and must be zero.
// All integer values are *unsigned* and written in "Intel" byte order(Little Endian), so for example 0x12345678 is written as 0x78 0x56 0x34 0x12.
// All pointer offsets are written as relative to the current position in the file, so for example the GD3 offset at 0x14 in the header is the file position of the GD3 tag minus 0x14.
// All header sizes are valid for all versions from 1.50 on, as long as header has at least 64 bytes.If the VGM data starts at an offset that is lower than 0x100, all overlapping header bytes have to be handled as they were zero.
// VGMs run with a rate of 44100 samples per second.All sample values use this unit.
#include "vgmdef.h"
#include "Obserable.h"
#include <vector>
using namespace std;

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)

#pragma pack (push,1)
enum VGMCommand
{
	GAME_GEAR_PSG_PORT6_WRITE = 0x4F,	// dd
	SN76489_WRITE = 0x50,				// dd
	YM2413_WRITE = 0x51,				// aa dd
	YM2612_PORT0_WRITE = 0x52,			// aa dd
	YM2612_PORT1_WRITE = 0x53,			// aa dd

	YM2151_WRITE = 0x54,				// aa dd	
	YM2203_WRITE = 0x55,				// aa dd	
	YM2608_PORT0_WRITE = 0x56,			// aa dd	
	YM2608_PORT1_WRITE = 0x57,			// aa dd	
	YM2610_PORT0_WRITE = 0x58,			// aa dd	
	YM2610_PORT1_WRITE = 0x59,			// aa dd	
	YM3812_WRITE = 0x5A,				// aa dd	
	YM3526_WRITE = 0x5B,				// aa dd	
	Y8950_WRITE = 0x5C,					// aa dd	
	YMZ280B_WRITE = 0x5D,				// aa dd	
	YMF262_PORT0_WRITE = 0x5E,			// aa dd	
	YMF262_PORT1_WRITE = 0x5F,			// aa dd	

	WAIT_NNNN_SAMPLES = 0x61,			// nn nn
	WAIT_735_SAMPLES = 0x62,
	WAIT_882_SAMPLES = 0x63,
	END_OF_SOUND = 0x66,
	DATA_BLOCKS = 0x67,

	WAIT_1_SAMPLES = 0x70,
	WAIT_2_SAMPLES = 0x71,
	WAIT_3_SAMPLES = 0x72,
	WAIT_4_SAMPLES = 0x73,
	WAIT_5_SAMPLES = 0x74,
	WAIT_6_SAMPLES = 0x75,
	WAIT_7_SAMPLES = 0x76,
	WAIT_8_SAMPLES = 0x77,
	WAIT_9_SAMPLES = 0x78,
	WAIT_10_SAMPLES = 0x79,
	WAIT_11_SAMPLES = 0x7A,
	WAIT_12_SAMPLES = 0x7B,
	WAIT_13_SAMPLES = 0x7C,
	WAIT_14_SAMPLES = 0x7D,
	WAIT_15_SAMPLES = 0x7E,
	WAIT_16_SAMPLES = 0x7F,

	DAC_SETUP_STREAM_CONTROL = 0x90,
	DAC_SET_STREAM_DATA = 0x91,
	DAC_SET_STREAM_FREQUENCY = 0x92,
	DAC_START_STREAM = 0x93,
	DAC_STOP_STREAM = 0x94,
	DAC_START_STEAM_FAST = 0x95,

	
	UNKNOWN_CHIP_A5_WRITE = 0xA5,
	K053260_WRITE = 0xBA,				// aa dd
	NES_APU_WRITE = 0xB4, 				// aa dd
	OKIM6258_WRITE = 0xB7,				// aa dd
	OKIM6295_WRITE = 0xB8,				// aa dd
	HUC6280_WRITE = 0xB9, 				// aa dd
};


class VGMHeader
{
public:
	s8 ID[4];								// file identification (0x56 0x67 0x6d 0x20)
	u32 EOFOffset;							// Relative offset to end of file (i.e. file length - 4). This is mainly used to find the next track when concatenating player stubs and multiple files.
	u32 version;							// Version number in BCD-Code. e.g. Version 1.70 is stored as 0x00000171. This is used for backwards compatibility in players, and defines which header values are valid.
	u32 SN76489Clock;						// Input clock rate in Hz for the SN76489 PSG chip.A typical value is 3579545. It should be 0 if there is no PSG chip used.
											// Note: Bit 31 (0x80000000) is used on combination with the dual-chip-bit to indicate that this is a T6W28. (PSG variant used in Neo Geo Pocket)
	u32 YM2413Clock;						// Input clock rate in Hz for the YM2413 chip. A typical value is 3579545.
											// It should be 0 if there is no YM2413 chip used.
	u32 GD3Offset;							// Relative offset to GD3 tag. 0 if no GD3 tag.GD3 tags are descriptive tags similar in use to ID3 tags in MP3 files.See the GD3 specification for more details.The GD3 tag is usually stored immediately after the VGM data.
	u32 totalSamples;						// Total of all wait values in the file.
	u32 loopOffset;							// Relative offset to loop point, or 0 if no loop.For example, if the data for the one - off intro to a song was in bytes 0x0040 - 0x3FFF of the file, but the main looping section started at 0x4000, this would contain the value 0x4000 - 0x1C = 0x00003FE4.
	u32 loopSamples;						// Number of samples in one loop, or 0 if there is no loop.Total of all wait values between the loop point and the end of the file.

	// VGM 1.01 additions:
	u32 Rate;								// Rate of recording in Hz, used for rate scaling on playback.
											// It is typically 50 for PAL systems and 60 for NTSC systems.
											// It should be set to zero if rate scaling is not appropriate - for example, if the game adjusts its music engine for the system's speed. VGM 1.00 files will have a value of 0.

	// VGM 1.10 additions:
	u16 SN76489Feedback;					// The white noise feedback pattern for the SN76489 PSG.Known values are :
											// 0x0009	Sega Master System 2 / Game Gear / Mega Drive(SN76489 / SN76496 integrated into Sega VDP chip)
											// 0x0003	Sega Computer 3000H, BBC Micro(SN76489AN)
											// 0x0006	SN76494, SN76496
											// For version 1.01 and earlier files, the feedback pattern should be assumed to be 0x0009. If the PSG is not used then this may be omitted(left at zero).

	u8 SN76489ShiftRegisterWidth;			// SN76489 shift register width
												// The noise feedback shift register width, in bits.Known values are :
												// 16	Sega Master System 2 / Game Gear / Mega Drive(SN76489 / SN76496 integrated into Sega VDP chip)
												// 15	Sega Computer 3000H, BBC Micro(SN76489AN)
												// For version 1.01 and earlier files, the shift register width should be assumed to be 16. If the PSG is not used then this may be omitted(left at zero). 
	// VGM 1.51 additions:
	u8 SN76489Flags;							// Misc flags for the SN76489.Most of them don't make audible changes and can be ignored, if the SN76489 emulator lacks the features.
												// bit 0	frequency 0 is 0x400
												// bit 1	output negate flag
												// bit 2	stereo on / off(on when bit clear)
												// bit 3 / 8 Clock Divider on / off(on when bit clear)
												// bit 4 - 7	reserved(must be zero)
												// For version 1.51 and earlier files, all the flags should not be set.If the PSG is not used then this may be omitted(left at zero).

	// VGM 1.10 additions:
	u32 YM2612Clock;							// Input clock rate in Hz for the YM2612 chip.A typical value is 7670454.
												// It should be 0 if there us no YM2612 chip used.
												// For version 1.01 and earlier files, the YM2413 clock rate should be used for the clock rate of the YM2612.

	u32 YM2151Clock;							// Input clock rate in Hz for the YM2151 chip.A typical value is 3579545.
												// It should be 0 if there us no YM2151 chip used.
												// For version 1.01 and earlier files, the YM2413 clock rate should be used for the clock rate of the YM2151.

	// VGM 1.50 additions:
	u32 VGMDataOffset;						// Relative offset to VGM data stream.
												// If the VGM data starts at absolute offset 0x40, this will contain value 0x0000000C. 
												// For versions prior to 1.50, it should be 0 and the VGM data must start at offset 0x40.

	// VGM 1.51 additions:
	u32 SegaPCMclock;						// Input clock rate in Hz for the Sega PCM chip.A typical value is 4000000.
												// It should be 0 if there is no Sega PCM chip used.

	u32 SegaPCMInterfaceRegister;			// The interface register for the Sega PCM chip.
												// It should be 0 if there is no Sega PCM chip used.


	u32 RF5C68Clock;							// Input clock rate in Hz for the RF5C68 PCM chip.A typical value is 12500000.
												// It should be 0 if there is no RF5C68 chip used.

	u32 YM2203Clock;							// Input clock rate in Hz for the YM2203 chip.A typical value is 3000000.
												// It should be 0 if there is no YM2203 chip used.

	u32 YM2608Clock;							// Input clock rate in Hz for the YM2608 chip.A typical value is 8000000.
												// It should be 0 if there is no YM2608 chip used.

	u32 YM2610_YM2610B_Clock;				// Input clock rate in Hz for the YM2610 / B chip.A typical value is 8000000. 
												// It should be 0 if there is no YM2610 / B chip used.
												// Note: Bit 31 is used to set whether it is an YM2610 or
												// an YM2610B chip.If bit 31 is set it is an YM2610B, 
												// if bit 31 is clear it is an YM2610.

	u32 YM3812Clock;							// Input clock rate in Hz for the YM3812 chip.A typical value is 3579545.
												// It should be 0 if there is no YM3812 chip used.

	u32 YM3526Clock;							// Input clock rate in Hz for the YM3526 chip.A typical value is 3579545.
												// It should be 0 if there is no YM3526 chip used.

	u32 Y8950Clock;							// Input clock rate in Hz for the Y8950 chip.A typical value is 3579545.
												// It should be 0 if there is no Y8950 chip used.

	u32 YMF262Clock;							// Input clock rate in Hz for the YMF262 chip.A typical value is 14318180.
												// It should be 0 if there is no YMF262 chip used.

	u32 YMF278BClock;						// Input clock rate in Hz for the YMF278B chip.A typical value is 33868800.
												// It should be 0 if there is no YMF278B chip used.

	u32 YMF271Clock;							// Input clock rate in Hz for the YMF271 chip.A typical value is 16934400.
												// It should be 0 if there is no YMF271 chip used.

	u32 YMZ280BClock;						// Input clock rate in Hz for the YMZ280B chip.A typical value is 16934400.
												// It should be 0 if there is no YMZ280B chip used.

	u32 RF5C164Clock;						// Input clock rate in Hz for the RF5C164 PCM chip.A typical value is 12500000.
												// It should be 0 if there is no RF5C164 chip used.

	u32 PWMClock;							// Input clock rate in Hz for the PWM chip.A typical value is 23011361.
												// It should be 0 if there is no PWM chip used.

	u32 AY8910Clock;							// Input clock rate in Hz for the AY8910 chip.A typical value is 1789750.
												// It should be 0 if there is no AY8910 chip used.

	u8 AY8910ChipType;						// Defines the exact type of AY8910.The values are :

												// 0x00	AY8910
												// 0x01	AY8912
												// 0x02	AY8913
												// 0x03	AY8930
												// 0x10	YM2149
												// 0x11	YM3439
												// 0x12	YMZ284
												// 0x13	YMZ294
												// If the AY8910 is not used then this may be omitted(left at zero).

	u8 AY8910Flags;							// Misc flags for the AY8910.Default is 0x01. 
												// For additional description see ay8910.h in MAME source code.
												// bit 0	Legacy Output
												// bit 1	Single Output
												// bit 2	Discrete Output
												// bit 3	RAW Output
												// bit 4 - 7	reserved(must be zero)
												// If the AY8910 is not used then this may be omitted(left at zero).

	u8 YM2203AY8910Flags;					// Misc flags for the AY8910.This one is specific for the AY8910 that's connected with/part of the YM2203.

	u8 YM2608AY8910Flags;					// Misc flags for the AY8910.This one is specific for the AY8910 that's connected with/part of the YM2608.

	// VGM 1.60 additions:
	u8 VolumeModifier;						// Volume = 2 ^ (VolumeModifier / 0x20) where VolumeModifier is a number from 
												// -63 to 192 (-63 = 0xC1, 0 = 0x00, 192 = 0xC0).
												// Also the value -63 gets replaced with -64 in order to make 
												// factor of 0.25 possible. Therefore the volume can reach levels 
												// between 0.25 and 64. 
												// Default is 0, which is equal to a factor of 1 or 100 % .
												// Note : Players should support the Volume Modifier in v1.50 files and higher.
												// This way Mega Drive VGMs can use the Volume Modifier without breaking compatibility with old players.

	u8 reserved0;							// Reserved byte for future use.It must be 0.
	u8 LoopBase;								// Modifies the number of loops that are played before the playback ends.
												// Set this value to eg. 1 to reduce the number of played loops by one.
												// This is useful, if the song is looped twice in the vgm, because there are minor differences 
												// between the first and second loop and the song repeats just the second loop.
												// The resulting number of loops that are played is calculated as following : 
												// NumLoops = NumLoopsModified - LoopBase Default is 0. Negative numbers are possible(80h...FFh = -128... - 1)

	// VGM 1.51 additions :
	u8 LoopModifier;							// Modifies the number of loops that are played before the playback ends.
												// You may want to use this, e.g. if a tune has a very short, but non - repetitive loop
												// (then set it to 0x20 f64 the loop number).
												// The resulting number of loops that are played is calculated as following :
												// NumLoops = ProgramNumLoops * LoopModifier / 0x10
												// Default is 0, which is equal to 0x10.

	// VGM 1.61 additions:
	u32 GameBoyDMGClock;						// Input clock rate in Hz for the GameBoy DMG chip, LR35902.A typical value is 4194304.
												// It should be 0 if there is no GB DMG chip used.

	u32 NESAPUClock;							// Input clock rate in Hz for the NES APU chip, N2A03.A typical value is 1789772.
												// It should be 0 if there is no NES APU chip used.
												// Note: Bit 31 (0x80000000) is used to enable the FDS sound addon.Set to enable, clear to disable.

	u32 MultiPCMClock;						// Input clock rate in Hz for the MultiPCM chip.A typical value is 8053975.
												// It should be 0 if there is no MultiPCM chip used.

	u32 uPD7759Clock;						// Input clock rate in Hz for the uPD7759 chip.A typical value is 640000.
												// It should be 0 if there is no uPD7759 chip used.

	u32 OKIM6258Clock;						// Input clock rate in Hz for the OKIM6258 chip.A typical value is 4000000.
												// It should be 0 if there is no OKIM6258 chip used.

	u8 OKIM6258Flags;						// Misc flags for the OKIM6258.Default is 0x00.
												// bit 0 - 1	Clock Divider(clock dividers are 1024, 768, 512, 512)
												// bit 2	3 / 4 - bit ADPCM select(default is 4 - bit, doesn't work currently)
												// bit 3	10 / 12 - bit Output(default is 10 - bit)
												// bit 4 - 7	reserved(must be zero)
												// If the OKIM6258 is not used then this may be omitted(left at zero).

	u8 K054539Flags;							// Misc flags for the K054539.Default is 0x01. See also k054539.h in MAME source code.
												// bit 0	Reverse Stereo
												// bit 1	Disable Reverb
												// bit 2	Update at KeyOn
												// bit 3 - 7	reserved(must be zero)
												// If the K054539 is not used then this may be omitted(left at zero).

	u8 C140ChipType;							// Defines the exact type of C140 and its banking method.The values are :
												// 0x00	C140, Namco System 2
												// 0x01	C140, Namco System 21
												// 0x02	219 ASIC, Namco NA - 1 / 2
												// If the C140 is not used then this may be omitted(left at zero).

	u8 reserved1;							// Reserved byte for future use.It must be 0.

	u32 OKIM6295Clock;						// Input clock rate in Hz for the OKIM6295 chip.A typical value is 8000000.
												// It should be 0 if there is no OKIM6295 chip used.

	u32 K051649Clock;						// Input clock rate in Hz for the K051649 chip.A typical value is 1500000.
												// It should be 0 if there is no K051649 chip used.

	u32 K054539Clock;						// Input clock rate in Hz for the K054539 chip.A typical value is 18432000.
												// It should be 0 if there is no K054539 chip used.

	u32 HuC6280Clock;						// Input clock rate in Hz for the HuC6280 chip.A typical value is 3579545.
												// It should be 0 if there is no HuC6280 chip used.

	u32 C140Clock;							// Input clock rate in Hz for the C140 chip.A typical value is 21390.
												// It should be 0 if there is no C140 chip used.

	u32 K053260Clock;						// Input clock rate in Hz for the K053260 chip.A typical value is 3579545.
												// It should be 0 if there is no K053260 chip used.

	u32 PokeyClock;							// Input clock rate in Hz for the Pokey chip.A typical value is 1789772.
												// It should be 0 if there is no Pokey chip used.

	u32 QSoundClock;							// Input clock rate in Hz for the QSound chip.A typical value is 4000000.
												// It should be 0 if there is no QSound chip used.

	// VGM 1.71 additions:
	u32 SCSPClock;							// Input clock rate in Hz for the SCSP chip.A typical value is 22579200.
												// It should be 0 if there is no SCSP chip used.

	// VGM 1.70 additions:
	u32 ExtraHeaderOffset;					// Relative offset to the extra header or 0 if no extra header is present.

	// VGM 1.71 additions :
	u32 WonderSwanClock;						// Input clock rate in Hz for the WonderSwan chip.A typical value is 3072000.
												// It should be 0 if there is no WonderSwan chip used.

	u32 VSUClock;							// Input clock rate in Hz for the VSU chip.A typical value is 5000000.
												// It should be 0 if there is no VSU chip used.

	u32 SAA1099Clock;						// Input clock rate in Hz for the SAA1099 chip.A typical value is 8000000 (or 7159000 / 7159090).
												// It should be 0 if there is no SAA1099 chip used.

	u32 ES5503Clock;							// Input clock rate in Hz for the ES5503 chip.A typical value is 7159090.
												// It should be 0 if there is no ES5503 chip used.

	u32 ES5505_ES5506Clock;					// Input clock rate in Hz for the ES5505 / ES5506 chip.
												// It should be 0 if there is no ES5505 / ES5506 chip used.
												// Note: Bit 31 is used to set whether it is an ES5505 or an ES5506 chip.
												// If bit 31 is set it is an ES5506, if bit 31 is clear it is an ES5505.

	u8 ES5503NumberOfChannels;				// Defines the internal number of output channels for the ES5503.
												// Possible values are 1 to 8. A typical value is 2.
												// If the ES5503 is not used then this may be omitted(left at zero).

	u8 ES5505_ES5506NumberOfChannels;		// Defines the internal number of output channels for the ES5506.
												// Possible values are 1 to 4 for the ES5505 and 1 to 8 for the ES5506.A typical value is 1.
												// If the ES5506 is not used then this may be omitted(left at zero).

	u8 C352ClockDivider;						// Defines the clock divider for the C352 chip, divided by 4 in order to achieve a divider range of 0 to 1020. A typical value is 288.
												// If the C352 is not used then this may be omitted(left at zero).

	u8 reserved2;							// Reserved bytes for future use.They must be 0.

	u32 X1_010Clock;							// Input clock rate in Hz for the X1 - 010 chip.A typical value is 16000000.
												// It should be 0 if there is no X1 - 010 chip used.

	u32 C352Clock;							// Input clock rate in Hz for the C352 chip.A typical value is 24192000.
												// It should be 0 if there is no C352 chip used.

	u32 GA20Clock;							// Input clock rate in Hz for the GA20 chip.A typical value is 3579545.
												// It should be 0 if there is no GA20 chip used.

	u32 reserved3;							// Reserved bytes for future use.They must be 0.
	u32 reserved4;							// Reserved bytes for future use.They must be 0.
	u32 reserved5;							// Reserved bytes for future use.They must be 0.
	u32 reserved6;							// Reserved bytes for future use.They must be 0.
	u32 reserved7;							// Reserved bytes for future use.They must be 0.
	u32 reserved8;							// Reserved bytes for future use.They must be 0.
	u32 reserved9;							// Reserved bytes for future use.They must be 0.
};

#pragma pack(pop)

class VGMData : public Obserable
{
public:
	class PlayInfo
	{
	public:
		boolean paused;
		boolean playing;
		s32 channels;
		s32 bitPerSamples;
		s32 sampleRate;
	};
#pragma pack (push,1)
	class OutputSample
	{
	public:
		s16 l;
		s16 r;

		s16 get(u8 idx)
		{
			if(idx==0)
				return l;
			else
				return r;
		}
	};
#pragma pack (pop)
	class BufferInfo
	{
	public:
		u32 sampleIdx;
		vector<s32> samplesL;
		vector<s32> samplesR;
		vector<OutputSample> outputSamples;
		
		boolean needQueueOutputSamples;
	};
	VGMData(s32 channels_, s32 bitPerSample_, s32 sampleRate_);
	virtual ~VGMData();

	u32 getVersion();

	boolean open();
	void close();
	void play();
	void stop();
	void pause();
	void resume();
	boolean update();

	boolean isPlaying();
	boolean isPaused();

	void requestUpdateData();

	const VGMHeader& getHeader() const;
	const VGMData::PlayInfo& getPlayInfo() const;
	const VGMData::BufferInfo& getBufferInfo() const;
protected:
	virtual s32 read(void *buffer, u32 size);
	virtual s32 seekSet(u32 size);
	virtual s32 seekCur(u32 size);

	virtual boolean onOpen() = 0;
	virtual void onClose() = 0;
	virtual void onPlay() = 0;
	virtual void onStop() = 0;
	virtual void onPause() = 0;
	virtual void onResume() = 0;
	virtual boolean onUpdate() = 0;
	virtual s32 onRead(void *buffer, u32 size) = 0;
	virtual s32 onSeekSet(u32 size) = 0;
	virtual s32 onSeekCur(u32 size) = 0;

	/////////////////////////////////////////////////
	// data decode
protected:
	void handleDataBlocks();
	void handleK053260ROM(s32 skipByte0x66, s32 blockType, s32 blockSize);
	
	u32 updateSamples(u32 updateSampleCounts);
	void handleEndOfSound();

	void fillOutputBuffer();
private:
public:
protected:
	VGMHeader header;
	PlayInfo playInfo;
	BufferInfo bufferInfo;

	boolean updateDataRequest;
	u32 updateSampleCounts;
private:
};

#endif