#ifndef _VGMData_h_
#define _VGMData_h_

extern "C"
{
#include "vgmdef.h"
#include "ChipIncl.h"
};

#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include "Obserable.h"
using namespace std;

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)

#if 0
#pragma pack (push,1)
enum VGMCommand
{
	GAME_GEAR_PSG_PORT6_WRITE = 0x4F,				// dd
	SN76489_WRITE = 0x50,							// dd
	YM2413_WRITE = 0x51,							// aa dd
	YM2612_PORT0_WRITE = 0x52,						// aa dd
	YM2612_PORT1_WRITE = 0x53,						// aa dd
	YM2151_WRITE = 0x54,							// aa dd	
	YM2203_WRITE = 0x55,							// aa dd	
	YM2608_PORT0_WRITE = 0x56,						// aa dd	
	YM2608_PORT1_WRITE = 0x57,						// aa dd	
	YM2610_PORT0_WRITE = 0x58,						// aa dd	
	YM2610_PORT1_WRITE = 0x59,						// aa dd	
	YM3812_WRITE = 0x5A,							// aa dd	
	YM3526_WRITE = 0x5B,							// aa dd	
	Y8950_WRITE = 0x5C,								// aa dd	
	YMZ280B_WRITE = 0x5D,							// aa dd	
	YMF262_PORT0_WRITE = 0x5E,						// aa dd	
	YMF262_PORT1_WRITE = 0x5F,						// aa dd	

	WAIT_NNNN_SAMPLES = 0x61,						// nn nn
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

	YM2612_PORT0_ADDR_0X2A_WRITE_0X80 = 0x80,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X81 = 0x81,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X82 = 0x82,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X83 = 0x83,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X84 = 0x84,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X85 = 0x85,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X86 = 0x86,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X87 = 0x87,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X88 = 0x88,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X89 = 0x89,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X8A = 0x8A,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X8B = 0x8B,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X8C = 0x8C,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X8D = 0x8D,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X8E = 0x8E,
	YM2612_PORT0_ADDR_0X2A_WRITE_0X8F = 0x8F,

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
	SEGA_PCM = 0xC0,					// bbaa dd Sega PCM, write value dd to memory offset aabb

	K054539_WRITE = 0xD3,						// pp aa dd

	MultiPCM_SETBANK_OFFSET = 0xC3,
	MultiPCM_WRITE = 0xB5,

	QSOUND_WRITE = 0xC4,				// mmll rr

	SEEK_TO_OFFSET = 0xE0,
};

enum VGMDataBlockType
{
	YM2612_PCM_DATA = 0X00,
	RF5C68_PCM_DATA = 0X01,
	RF5C164_PCM_DATA = 0X02,
	PWM_PCM_DATA = 0X03,
	OKIM6258_ADPCM_DATA = 0X04,
	HUC6280_PCM_DATA = 0X05,
	SCSP_PCM_DATA = 0X06,
	NES_APU_DPCM_DATA = 0X07,


	YM2612_PCM_DATA_COMPRESSED = 0X40,
	RF5C68_PCM_DATA_COMPRESSED = 0X41,
	RF5C164_PCM_DATA_COMPRESSED = 0X42,
	PWM_PCM_DATA_COMPRESSED = 0X43,
	OKIM6258_ADPCM_DATA_COMPRESSED = 0X44,
	HUC6280_PCM_DATA_COMPRESSED = 0X45,
	SCSP_PCM_DATA_COMPRESSED = 0X46,
	NES_APU_DPCM_DATA_COMPRESSED = 0X47,


	DECOMPRESSION_TABLE = 0X7F,


	SEGA_PCM_ROM_DATA = 0X80,
	YM2608_DELTA_T_ROM_DATA = 0X81,
	YM2610_ADPCM_ROM_DATA = 0X82,
	YM2610_DELTA_T_ROM_DATA = 0X83,
	YMF278B_ROM_DATA = 0X84,
	YMF271_ROM_DATA = 0X85,
	YMZ280B_ROM_DATA = 0X86,
	YMF278B_RAM_DATA = 0X87,
	Y8950_DELTA_T_ROM_DATA = 0X88,
	MULTIPCM_ROM_DATA = 0X89,
	UPD7759_ROM_DATA = 0X8A,
	OKIM6295_ROM_DATA = 0X8B,
	K054539_ROM_DATA = 0X8C,
	C140_ROM_DATA = 0X8D,
	K053260_ROM_DATA = 0X8E,
	QSOUND_ROM_DATA = 0X8F,
	ES5505_ES5506_ROM_DATA = 0X90,
	X1_010_ROM_DATA = 0X91,
	C352_ROM_DATA = 0X92,
	GA20_ROM_DATA = 0X93,
	RF5C68_RAM_WRITE = 0XC0,
	RF5C164_RAM_WRITE = 0XC1,
	NES_APU_RAM_WRITE = 0XC2,
	SCSP_RAM_WRITE = 0XE0,
	ES5503_RAM_WRITE = 0XE1
};

class VGMHeader
{
public:
	INT8 ID[4];								// file identification (0x56 0x67 0x6d 0x20)
	UINT32 EOFOffset;							// Relative offset to end of file (i.e. file length - 4). This is mainly used to find the next track when concatenating player stubs and multiple files.
	UINT32 version;							// Version number in BCD-Code. e.g. Version 1.70 is stored as 0x00000171. This is used for backwards compatibility in players, and defines which header values are valid.
	UINT32 SN76489Clock;						// Input clock rate in Hz for the SN76489 PSG chip.A typical value is 3579545. It should be 0 if there is no PSG chip used.
											// Note: Bit 31 (0x80000000) is used on combination with the dual-chip-bit to indicate that this is a T6W28. (PSG variant used in Neo Geo Pocket)
	UINT32 YM2413Clock;						// Input clock rate in Hz for the YM2413 chip. A typical value is 3579545.
											// It should be 0 if there is no YM2413 chip used.
	UINT32 GD3Offset;							// Relative offset to GD3 tag. 0 if no GD3 tag.GD3 tags are descriptive tags similar in use to ID3 tags in MP3 files.See the GD3 specification for more details.The GD3 tag is usually stored immediately after the VGM data.
	UINT32 totalSamples;						// Total of all wait values in the file.
	UINT32 loopOffset;							// Relative offset to loop point, or 0 if no loop.For example, if the data for the one - off intro to a song was in bytes 0x0040 - 0x3FFF of the file, but the main looping section started at 0x4000, this would contain the value 0x4000 - 0x1C = 0x00003FE4.
	UINT32 loopSamples;						// Number of samples in one loop, or 0 if there is no loop.Total of all wait values between the loop point and the end of the file.

	// VGM 1.01 additions:
	UINT32 Rate;								// Rate of recording in Hz, used for rate scaling on playback.
											// It is typically 50 for PAL systems and 60 for NTSC systems.
											// It should be set to zero if rate scaling is not appropriate - for example, if the game adjusts its music engine for the system's speed. VGM 1.00 files will have a value of 0.

	// VGM 1.10 additions:
	UINT16 SN76489Feedback;					// The white noise feedback pattern for the SN76489 PSG.Known values are :
											// 0x0009	Sega Master System 2 / Game Gear / Mega Drive(SN76489 / SN76496 integrated into Sega VDP chip)
											// 0x0003	Sega Computer 3000H, BBC Micro(SN76489AN)
											// 0x0006	SN76494, SN76496
											// For version 1.01 and earlier files, the feedback pattern should be assumed to be 0x0009. If the PSG is not used then this may be omitted(left at zero).

	UINT8 SN76489ShiftRegisterWidth;			// SN76489 shift register width
												// The noise feedback shift register width, in bits.Known values are :
												// 16	Sega Master System 2 / Game Gear / Mega Drive(SN76489 / SN76496 integrated into Sega VDP chip)
												// 15	Sega Computer 3000H, BBC Micro(SN76489AN)
												// For version 1.01 and earlier files, the shift register width should be assumed to be 16. If the PSG is not used then this may be omitted(left at zero). 
	// VGM 1.51 additions:
	UINT8 SN76489Flags;							// Misc flags for the SN76489.Most of them don't make audible changes and can be ignored, if the SN76489 emulator lacks the features.
												// bit 0	frequency 0 is 0x400
												// bit 1	output negate flag
												// bit 2	stereo on / off(on when bit clear)
												// bit 3 / 8 Clock Divider on / off(on when bit clear)
												// bit 4 - 7	reserved(must be zero)
												// For version 1.51 and earlier files, all the flags should not be set.If the PSG is not used then this may be omitted(left at zero).

	// VGM 1.10 additions:
	UINT32 YM2612Clock;							// Input clock rate in Hz for the YM2612 chip.A typical value is 7670454.
												// It should be 0 if there us no YM2612 chip used.
												// For version 1.01 and earlier files, the YM2413 clock rate should be used for the clock rate of the YM2612.

	UINT32 YM2151Clock;							// Input clock rate in Hz for the YM2151 chip.A typical value is 3579545.
												// It should be 0 if there us no YM2151 chip used.
												// For version 1.01 and earlier files, the YM2413 clock rate should be used for the clock rate of the YM2151.

	// VGM 1.50 additions:
	UINT32 VGMDataOffset;						// Relative offset to VGM data stream.
												// If the VGM data starts at absolute offset 0x40, this will contain value 0x0000000C. 
												// For versions prior to 1.50, it should be 0 and the VGM data must start at offset 0x40.

	// VGM 1.51 additions:
	UINT32 SegaPCMclock;						// Input clock rate in Hz for the Sega PCM chip.A typical value is 4000000.
												// It should be 0 if there is no Sega PCM chip used.

	UINT32 SegaPCMInterfaceRegister;			// The interface register for the Sega PCM chip.
												// It should be 0 if there is no Sega PCM chip used.


	UINT32 RF5C68Clock;							// Input clock rate in Hz for the RF5C68 PCM chip.A typical value is 12500000.
												// It should be 0 if there is no RF5C68 chip used.

	UINT32 YM2203Clock;							// Input clock rate in Hz for the YM2203 chip.A typical value is 3000000.
												// It should be 0 if there is no YM2203 chip used.

	UINT32 YM2608Clock;							// Input clock rate in Hz for the YM2608 chip.A typical value is 8000000.
												// It should be 0 if there is no YM2608 chip used.

	UINT32 YM2610_YM2610B_Clock;				// Input clock rate in Hz for the YM2610 / B chip.A typical value is 8000000. 
												// It should be 0 if there is no YM2610 / B chip used.
												// Note: Bit 31 is used to set whether it is an YM2610 or
												// an YM2610B chip.If bit 31 is set it is an YM2610B, 
												// if bit 31 is clear it is an YM2610.

	UINT32 YM3812Clock;							// Input clock rate in Hz for the YM3812 chip.A typical value is 3579545.
												// It should be 0 if there is no YM3812 chip used.

	UINT32 YM3526Clock;							// Input clock rate in Hz for the YM3526 chip.A typical value is 3579545.
												// It should be 0 if there is no YM3526 chip used.

	UINT32 Y8950Clock;							// Input clock rate in Hz for the Y8950 chip.A typical value is 3579545.
												// It should be 0 if there is no Y8950 chip used.

	UINT32 YMF262Clock;							// Input clock rate in Hz for the YMF262 chip.A typical value is 14318180.
												// It should be 0 if there is no YMF262 chip used.

	UINT32 YMF278BClock;						// Input clock rate in Hz for the YMF278B chip.A typical value is 33868800.
												// It should be 0 if there is no YMF278B chip used.

	UINT32 YMF271Clock;							// Input clock rate in Hz for the YMF271 chip.A typical value is 16934400.
												// It should be 0 if there is no YMF271 chip used.

	UINT32 YMZ280BClock;						// Input clock rate in Hz for the YMZ280B chip.A typical value is 16934400.
												// It should be 0 if there is no YMZ280B chip used.

	UINT32 RF5C164Clock;						// Input clock rate in Hz for the RF5C164 PCM chip.A typical value is 12500000.
												// It should be 0 if there is no RF5C164 chip used.

	UINT32 PWMClock;							// Input clock rate in Hz for the PWM chip.A typical value is 23011361.
												// It should be 0 if there is no PWM chip used.

	UINT32 AY8910Clock;							// Input clock rate in Hz for the AY8910 chip.A typical value is 1789750.
												// It should be 0 if there is no AY8910 chip used.

	UINT8 AY8910ChipType;						// Defines the exact type of AY8910.The values are :

												// 0x00	AY8910
												// 0x01	AY8912
												// 0x02	AY8913
												// 0x03	AY8930
												// 0x10	YM2149
												// 0x11	YM3439
												// 0x12	YMZ284
												// 0x13	YMZ294
												// If the AY8910 is not used then this may be omitted(left at zero).

	UINT8 AY8910Flags;							// Misc flags for the AY8910.Default is 0x01. 
												// For additional description see ay8910.h in MAME source code.
												// bit 0	Legacy Output
												// bit 1	Single Output
												// bit 2	Discrete Output
												// bit 3	RAW Output
												// bit 4 - 7	reserved(must be zero)
												// If the AY8910 is not used then this may be omitted(left at zero).

	UINT8 YM2203AY8910Flags;					// Misc flags for the AY8910.This one is specific for the AY8910 that's connected with/part of the YM2203.

	UINT8 YM2608AY8910Flags;					// Misc flags for the AY8910.This one is specific for the AY8910 that's connected with/part of the YM2608.

	// VGM 1.60 additions:
	UINT8 VolumeModifier;						// Volume = 2 ^ (VolumeModifier / 0x20) where VolumeModifier is a number from 
												// -63 to 192 (-63 = 0xC1, 0 = 0x00, 192 = 0xC0).
												// Also the value -63 gets replaced with -64 in order to make 
												// factor of 0.25 possible. Therefore the volume can reach levels 
												// between 0.25 and 64. 
												// Default is 0, which is equal to a factor of 1 or 100 % .
												// Note : Players should support the Volume Modifier in v1.50 files and higher.
												// This way Mega Drive VGMs can use the Volume Modifier without breaking compatibility with old players.

	UINT8 reserved0;							// Reserved byte for future use.It must be 0.
	UINT8 LoopBase;								// Modifies the number of loops that are played before the playback ends.
												// Set this value to eg. 1 to reduce the number of played loops by one.
												// This is useful, if the song is looped twice in the vgm, because there are minor differences 
												// between the first and second loop and the song repeats just the second loop.
												// The resulting number of loops that are played is calculated as following : 
												// NumLoops = NumLoopsModified - LoopBase Default is 0. Negative numbers are possible(80h...FFh = -128... - 1)

	// VGM 1.51 additions :
	UINT8 LoopModifier;							// Modifies the number of loops that are played before the playback ends.
												// You may want to use this, e.g. if a tune has a very short, but non - repetitive loop
												// (then set it to 0x20 f64 the loop number).
												// The resulting number of loops that are played is calculated as following :
												// NumLoops = ProgramNumLoops * LoopModifier / 0x10
												// Default is 0, which is equal to 0x10.

	// VGM 1.61 additions:
	UINT32 GameBoyDMGClock;						// Input clock rate in Hz for the GameBoy DMG chip, LR35902.A typical value is 4194304.
												// It should be 0 if there is no GB DMG chip used.

	UINT32 NESAPUClock;							// Input clock rate in Hz for the NES APU chip, N2A03.A typical value is 1789772.
												// It should be 0 if there is no NES APU chip used.
												// Note: Bit 31 (0x80000000) is used to enable the FDS sound addon.Set to enable, clear to disable.

	UINT32 MultiPCMClock;						// Input clock rate in Hz for the MultiPCM chip.A typical value is 8053975.
												// It should be 0 if there is no MultiPCM chip used.

	UINT32 uPD7759Clock;						// Input clock rate in Hz for the uPD7759 chip.A typical value is 640000.
												// It should be 0 if there is no uPD7759 chip used.

	UINT32 OKIM6258Clock;						// Input clock rate in Hz for the OKIM6258 chip.A typical value is 4000000.
												// It should be 0 if there is no OKIM6258 chip used.

	UINT8 OKIM6258Flags;						// Misc flags for the OKIM6258.Default is 0x00.
												// bit 0 - 1	Clock Divider(clock dividers are 1024, 768, 512, 512)
												// bit 2	3 / 4 - bit ADPCM select(default is 4 - bit, doesn't work currently)
												// bit 3	10 / 12 - bit Output(default is 10 - bit)
												// bit 4 - 7	reserved(must be zero)
												// If the OKIM6258 is not used then this may be omitted(left at zero).

	UINT8 K054539Flags;							// Misc flags for the K054539.Default is 0x01. See also k054539.h in MAME source code.
												// bit 0	Reverse Stereo
												// bit 1	Disable Reverb
												// bit 2	Update at KeyOn
												// bit 3 - 7	reserved(must be zero)
												// If the K054539 is not used then this may be omitted(left at zero).

	UINT8 C140ChipType;							// Defines the exact type of C140 and its banking method.The values are :
												// 0x00	C140, Namco System 2
												// 0x01	C140, Namco System 21
												// 0x02	219 ASIC, Namco NA - 1 / 2
												// If the C140 is not used then this may be omitted(left at zero).

	UINT8 reserved1;							// Reserved byte for future use.It must be 0.

	UINT32 OKIM6295Clock;						// Input clock rate in Hz for the OKIM6295 chip.A typical value is 8000000.
												// It should be 0 if there is no OKIM6295 chip used.

	UINT32 K051649Clock;						// Input clock rate in Hz for the K051649 chip.A typical value is 1500000.
												// It should be 0 if there is no K051649 chip used.

	UINT32 K054539Clock;						// Input clock rate in Hz for the K054539 chip.A typical value is 18432000.
												// It should be 0 if there is no K054539 chip used.

	UINT32 HuC6280Clock;						// Input clock rate in Hz for the HuC6280 chip.A typical value is 3579545.
												// It should be 0 if there is no HuC6280 chip used.

	UINT32 C140Clock;							// Input clock rate in Hz for the C140 chip.A typical value is 21390.
												// It should be 0 if there is no C140 chip used.

	UINT32 K053260Clock;						// Input clock rate in Hz for the K053260 chip.A typical value is 3579545.
												// It should be 0 if there is no K053260 chip used.

	UINT32 PokeyClock;							// Input clock rate in Hz for the Pokey chip.A typical value is 1789772.
												// It should be 0 if there is no Pokey chip used.

	UINT32 QSoundClock;							// Input clock rate in Hz for the QSound chip.A typical value is 4000000.
												// It should be 0 if there is no QSound chip used.

	// VGM 1.71 additions:
	UINT32 SCSPClock;							// Input clock rate in Hz for the SCSP chip.A typical value is 22579200.
												// It should be 0 if there is no SCSP chip used.

	// VGM 1.70 additions:
	UINT32 ExtraHeaderOffset;					// Relative offset to the extra header or 0 if no extra header is present.

	// VGM 1.71 additions :
	UINT32 WonderSwanClock;						// Input clock rate in Hz for the WonderSwan chip.A typical value is 3072000.
												// It should be 0 if there is no WonderSwan chip used.

	UINT32 VSUClock;							// Input clock rate in Hz for the VSU chip.A typical value is 5000000.
												// It should be 0 if there is no VSU chip used.

	UINT32 SAA1099Clock;						// Input clock rate in Hz for the SAA1099 chip.A typical value is 8000000 (or 7159000 / 7159090).
												// It should be 0 if there is no SAA1099 chip used.

	UINT32 ES5503Clock;							// Input clock rate in Hz for the ES5503 chip.A typical value is 7159090.
												// It should be 0 if there is no ES5503 chip used.

	UINT32 ES5505_ES5506Clock;					// Input clock rate in Hz for the ES5505 / ES5506 chip.
												// It should be 0 if there is no ES5505 / ES5506 chip used.
												// Note: Bit 31 is used to set whether it is an ES5505 or an ES5506 chip.
												// If bit 31 is set it is an ES5506, if bit 31 is clear it is an ES5505.

	UINT8 ES5503NumberOfChannels;				// Defines the internal number of output channels for the ES5503.
												// Possible values are 1 to 8. A typical value is 2.
												// If the ES5503 is not used then this may be omitted(left at zero).

	UINT8 ES5505_ES5506NumberOfChannels;		// Defines the internal number of output channels for the ES5506.
												// Possible values are 1 to 4 for the ES5505 and 1 to 8 for the ES5506.A typical value is 1.
												// If the ES5506 is not used then this may be omitted(left at zero).

	UINT8 C352ClockDivider;						// Defines the clock divider for the C352 chip, divided by 4 in order to achieve a divider range of 0 to 1020. A typical value is 288.
												// If the C352 is not used then this may be omitted(left at zero).

	UINT8 reserved2;							// Reserved bytes for future use.They must be 0.

	UINT32 X1_010Clock;							// Input clock rate in Hz for the X1 - 010 chip.A typical value is 16000000.
												// It should be 0 if there is no X1 - 010 chip used.

	UINT32 C352Clock;							// Input clock rate in Hz for the C352 chip.A typical value is 24192000.
												// It should be 0 if there is no C352 chip used.

	UINT32 GA20Clock;							// Input clock rate in Hz for the GA20 chip.A typical value is 3579545.
												// It should be 0 if there is no GA20 chip used.

	UINT32 reserved3;							// Reserved bytes for future use.They must be 0.
	UINT32 reserved4;							// Reserved bytes for future use.They must be 0.
	UINT32 reserved5;							// Reserved bytes for future use.They must be 0.
	UINT32 reserved6;							// Reserved bytes for future use.They must be 0.
	UINT32 reserved7;							// Reserved bytes for future use.They must be 0.
	UINT32 reserved8;							// Reserved bytes for future use.They must be 0.
	UINT32 reserved9;							// Reserved bytes for future use.They must be 0.
};
#pragma pack(pop)
#endif


typedef struct waveform_16bit_stereo
{
	INT16 Left;
	INT16 Right;
} WAVE_16BS;

typedef struct waveform_32bit_stereo
{
	INT32 Left;
	INT32 Right;
} WAVE_32BS;

typedef struct chip_audio_attributes CAUD_ATTR;
struct chip_audio_attributes
{
	UINT32 SmpRate;
	UINT16 Volume;
	UINT8 ChipType;
	UINT8 ChipID;		// 0 - 1st chip, 1 - 2nd chip, etc.
	// Resampler Type:
	//	00 - Old
	//	01 - Upsampling
	//	02 - Copy
	//	03 - Downsampling
	UINT8 Resampler;
	strm_func StreamUpdate;
	UINT32 SmpP;		// Current Sample (Playback Rate)
	UINT32 SmpLast;		// Sample Number Last
	UINT32 SmpNext;		// Sample Number Next
	WAVE_32BS LSmpl;	// Last Sample
	WAVE_32BS NSmpl;	// Next Sample
	CAUD_ATTR* Paired;
};

typedef struct chip_audio_struct
{
	CAUD_ATTR SN76496;
	CAUD_ATTR YM2413;
	CAUD_ATTR YM2612;
	CAUD_ATTR YM2151;
	CAUD_ATTR SegaPCM;
	CAUD_ATTR RF5C68;
	CAUD_ATTR YM2203;
	CAUD_ATTR YM2608;
	CAUD_ATTR YM2610;
	CAUD_ATTR YM3812;
	CAUD_ATTR YM3526;
	CAUD_ATTR Y8950;
	CAUD_ATTR YMF262;
	CAUD_ATTR YMF278B;
	CAUD_ATTR YMF271;
	CAUD_ATTR YMZ280B;
	CAUD_ATTR RF5C164;
	CAUD_ATTR PWM;
	CAUD_ATTR AY8910;
	CAUD_ATTR GameBoy;
	CAUD_ATTR NES;
	CAUD_ATTR MultiPCM;
	CAUD_ATTR UPD7759;
	CAUD_ATTR OKIM6258;
	CAUD_ATTR OKIM6295;
	CAUD_ATTR K051649;
	CAUD_ATTR K054539;
	CAUD_ATTR HuC6280;
	CAUD_ATTR C140;
	CAUD_ATTR K053260;
	CAUD_ATTR Pokey;
	CAUD_ATTR QSound;
	CAUD_ATTR SCSP;
	CAUD_ATTR WSwan;
	CAUD_ATTR VSU;
	CAUD_ATTR SAA1099;
	CAUD_ATTR ES5503;
	CAUD_ATTR ES5506;
	CAUD_ATTR X1_010;
	CAUD_ATTR C352;
	CAUD_ATTR GA20;
	//	CAUD_ATTR OKIM6376;
} CHIP_AUDIO;

#define CHIP_COUNT	0x29
typedef struct chip_options
{
	bool Disabled;
	UINT8 EmuCore;
	UINT8 ChnCnt;
	// Special Flags:
	//	YM2612:	Bit 0 - DAC Highpass Enable, Bit 1 - SSG-EG Enable
	//	YM-OPN:	Bit 0 - Disable AY8910-Part
	UINT16 SpecialFlags;

	// Channel Mute Mask - 1 Channel is represented by 1 bit
	UINT32 ChnMute1;
	// Mask 2 - used by YMF287B for OPL4 Wavetable Synth and by YM2608/YM2610 for PCM
	UINT32 ChnMute2;
	// Mask 3 - used for the AY-part of some OPN-chips
	UINT32 ChnMute3;

	INT16* Panning;
} CHIP_OPTS;

typedef struct chips_options
{
	CHIP_OPTS SN76496;
	CHIP_OPTS YM2413;
	CHIP_OPTS YM2612;
	CHIP_OPTS YM2151;
	CHIP_OPTS SegaPCM;
	CHIP_OPTS RF5C68;
	CHIP_OPTS YM2203;
	CHIP_OPTS YM2608;
	CHIP_OPTS YM2610;
	CHIP_OPTS YM3812;
	CHIP_OPTS YM3526;
	CHIP_OPTS Y8950;
	CHIP_OPTS YMF262;
	CHIP_OPTS YMF278B;
	CHIP_OPTS YMF271;
	CHIP_OPTS YMZ280B;
	CHIP_OPTS RF5C164;
	CHIP_OPTS PWM;
	CHIP_OPTS AY8910;
	CHIP_OPTS GameBoy;
	CHIP_OPTS NES;
	CHIP_OPTS MultiPCM;
	CHIP_OPTS UPD7759;
	CHIP_OPTS OKIM6258;
	CHIP_OPTS OKIM6295;
	CHIP_OPTS K051649;
	CHIP_OPTS K054539;
	CHIP_OPTS HuC6280;
	CHIP_OPTS C140;
	CHIP_OPTS K053260;
	CHIP_OPTS Pokey;
	CHIP_OPTS QSound;
	CHIP_OPTS SCSP;
	CHIP_OPTS WSwan;
	CHIP_OPTS VSU;
	CHIP_OPTS SAA1099;
	CHIP_OPTS ES5503;
	CHIP_OPTS ES5506;
	CHIP_OPTS X1_010;
	CHIP_OPTS C352;
	CHIP_OPTS GA20;
	//	CHIP_OPTS OKIM6376;
} CHIPS_OPTION;


typedef struct chip_aud_list CA_LIST;
struct chip_aud_list
{
	CAUD_ATTR* CAud;
	CHIP_OPTS* COpts;
	CA_LIST* next;
};

typedef struct daccontrol_data
{
	bool Enable;
	UINT8 Bank;
} DACCTRL_DATA;

typedef struct pcmbank_table
{
	UINT8 ComprType;
	UINT8 CmpSubType;
	UINT8 BitDec;
	UINT8 BitCmp;
	UINT16 EntryCount;
	void* Entries;
} PCMBANK_TBL;

typedef struct _vgm_file_header
{
	UINT32 fccVGM;
	UINT32 lngEOFOffset;
	UINT32 lngVersion;
	UINT32 lngHzPSG;
	UINT32 lngHzYM2413;
	UINT32 lngGD3Offset;
	UINT32 lngTotalSamples;
	UINT32 lngLoopOffset;
	UINT32 lngLoopSamples;
	UINT32 lngRate;
	UINT16 shtPSG_Feedback;
	UINT8 bytPSG_SRWidth;
	UINT8 bytPSG_Flags;
	UINT32 lngHzYM2612;
	UINT32 lngHzYM2151;
	UINT32 lngDataOffset;
	UINT32 lngHzSPCM;
	UINT32 lngSPCMIntf;
	UINT32 lngHzRF5C68;
	UINT32 lngHzYM2203;
	UINT32 lngHzYM2608;
	UINT32 lngHzYM2610;
	UINT32 lngHzYM3812;
	UINT32 lngHzYM3526;
	UINT32 lngHzY8950;
	UINT32 lngHzYMF262;
	UINT32 lngHzYMF278B;
	UINT32 lngHzYMF271;
	UINT32 lngHzYMZ280B;
	UINT32 lngHzRF5C164;
	UINT32 lngHzPWM;
	UINT32 lngHzAY8910;
	UINT8 bytAYType;
	UINT8 bytAYFlag;
	UINT8 bytAYFlagYM2203;
	UINT8 bytAYFlagYM2608;
	UINT8 bytVolumeModifier;
	UINT8 bytReserved2;
	INT8 bytLoopBase;
	UINT8 bytLoopModifier;
	UINT32 lngHzGBDMG;
	UINT32 lngHzNESAPU;
	UINT32 lngHzMultiPCM;
	UINT32 lngHzUPD7759;
	UINT32 lngHzOKIM6258;
	UINT8 bytOKI6258Flags;
	UINT8 bytK054539Flags;
	UINT8 bytC140Type;
	UINT8 bytReservedFlags;
	UINT32 lngHzOKIM6295;
	UINT32 lngHzK051649;
	UINT32 lngHzK054539;
	UINT32 lngHzHuC6280;
	UINT32 lngHzC140;
	UINT32 lngHzK053260;
	UINT32 lngHzPokey;
	UINT32 lngHzQSound;
	UINT32 lngHzSCSP;
	//	UINT32 lngHzOKIM6376;
		//UINT8 bytReserved[0x04];
	UINT32 lngExtraOffset;
	UINT32 lngHzWSwan;
	UINT32 lngHzVSU;
	UINT32 lngHzSAA1099;
	UINT32 lngHzES5503;
	UINT32 lngHzES5506;
	UINT8 bytES5503Chns;
	UINT8 bytES5506Chns;
	UINT8 bytC352ClkDiv;
	UINT8 bytESReserved;
	UINT32 lngHzX1_010;
	UINT32 lngHzC352;
	UINT32 lngHzGA20;
} VGM_HEADER;
typedef struct _vgm_header_extra
{
	UINT32 DataSize;
	UINT32 Chp2ClkOffset;
	UINT32 ChpVolOffset;
} VGM_HDR_EXTRA;

typedef struct _vgm_extra_chip_data32
{
	UINT8 Type;
	UINT32 Data;
} VGMX_CHIP_DATA32;

typedef struct _vgm_extra_chip_data16
{
	UINT8 Type;
	UINT8 Flags;
	UINT16 Data;
} VGMX_CHIP_DATA16;

typedef struct _vgm_extra_chip_extra32
{
	UINT8 ChipCnt;
	VGMX_CHIP_DATA32* CCData;
} VGMX_CHP_EXTRA32;

typedef struct _vgm_extra_chip_extra16
{
	UINT8 ChipCnt;
	VGMX_CHIP_DATA16* CCData;
} VGMX_CHP_EXTRA16;

typedef struct _vgm_header_extra_data
{
	VGMX_CHP_EXTRA32 Clocks;
	VGMX_CHP_EXTRA16 Volumes;
} VGM_EXTRA;

#define VOLUME_MODIF_WRAP	0xC0
typedef struct _vgm_gd3_tag
{
	UINT32 fccGD3;
	UINT32 lngVersion;
	UINT32 lngTagLength;
	wchar_t* strTrackNameE;
	wchar_t* strTrackNameJ;
	wchar_t* strGameNameE;
	wchar_t* strGameNameJ;
	wchar_t* strSystemNameE;
	wchar_t* strSystemNameJ;
	wchar_t* strAuthorNameE;
	wchar_t* strAuthorNameJ;
	wchar_t* strReleaseDate;
	wchar_t* strCreator;
	wchar_t* strNotes;
} GD3_TAG;

typedef struct _vgm_pcm_bank_data
{
	UINT32 DataSize;
	UINT8* Data;
	UINT32 DataStart;
} VGM_PCM_DATA;

typedef struct _vgm_pcm_bank
{
	UINT32 BankCount;
	VGM_PCM_DATA* Bank;
	UINT32 DataSize;
	UINT8* Data;
	UINT32 DataPos;
	UINT32 BnkPos;
} VGM_PCM_BANK;


#define FCC_VGM	0x206D6756	// 'Vgm '
#define FCC_GD3	0x20336447	// 'Gd3 '

// Structures for DRO and CMF files
typedef struct _cmf_file_header
{
	UINT32 fccCMF;
	UINT16 shtVersion;
	UINT16 shtOffsetInsData;
	UINT16 shtOffsetMusData;
	UINT16 shtTickspQuarter;
	UINT16 shtTickspSecond;
	UINT16 shtOffsetTitle;
	UINT16 shtOffsetAuthor;
	UINT16 shtOffsetComments;
	UINT8 bytChnUsed[0x10];
	UINT16 shtInstrumentCount;
	UINT16 shtTempo;
} CMF_HEADER;

typedef struct _cmf_instrument_table
{
	UINT8 Character[0x02];
	UINT8 ScaleLevel[0x02];
	UINT8 AttackDelay[0x02];
	UINT8 SustnRelease[0x02];
	UINT8 WaveSelect[0x02];
	UINT8 FeedbConnect;
	UINT8 Reserved[0x5];
} CMF_INSTRUMENT;

typedef struct _dro_file_header
{
	char cSignature[0x08];
	UINT16 iVersionMajor;
	UINT16 iVersionMinor;
} DRO_HEADER;

typedef struct _dro_version_header_1
{
	UINT32 iLengthMS;
	UINT32 iLengthBytes;
	UINT32 iHardwareType;
} DRO_VER_HEADER_1;

typedef struct _dro_version_header_2
{
	UINT32 iLengthPairs;
	UINT32 iLengthMS;
	UINT8 iHardwareType;
	UINT8 iFormat;
	UINT8 iCompression;
	UINT8 iShortDelayCode;
	UINT8 iLongDelayCode;
	UINT8 iCodemapLength;
} DRO_VER_HEADER_2;

#define FCC_CMF		0x464D5443	// 'CTMF'
#define FCC_DRO1	0x41524244	// 'DBRA'
#define FCC_DRO2	0x4C504F57	// 'WOPL'

#pragma pack (push,1)
template<class T>
class TOutputBuffer
{
public:
	TOutputBuffer(int size)
		: buffer(size * 2)
	{
	}

	~TOutputBuffer()
	{
	}

	operator T* ()
	{
		return &buffer[0];
	}

	operator const T* () const
	{
		return &buffer[0];
	}

	T& Get(int i, int channel)
	{
		return buffer[(i << 1) + channel];
	}

	const T& Get(int channel, int i) const
	{
		return buffer[(i << 1) + channel];
	}

	const vector<T>& GetBuffer() const
	{
		return buffer;
	}

	T* GetBuffer()
	{
		return &buffer[0];
	}
private:
	vector<T> buffer;
};

typedef TOutputBuffer<INT16> OutputSampleBuffer;
#pragma pack (pop)

class VGMChannel
{
public:
	class Command
	{
	public:
		Command(float frameCounter, UINT32 address_, UINT16 data_)
			: address(address_)
			, data(data_)
		{
		}

		float frameCounter;
		UINT32 address;
		UINT16 data;
	};

	VGMChannel()
		: commands()
		, leftSamples(VGM_SAMPLE_BUFFER_SIZE)
		, rightSamples(VGM_SAMPLE_BUFFER_SIZE)
	{
	}

	~VGMChannel()
	{
	}

	vector<Command> commands;
	vector<INT32> leftSamples;
	vector<INT32> rightSamples;
};

class VGMOutputChannels
{
public:
	VGMOutputChannels()
		: channels(0)

		, currentSampleIdx(0)

		, channelSampleBuffers(0)
		, outputSampleBuffer(VGM_SAMPLE_BUFFER_SIZE)
	{
	}

	~VGMOutputChannels()
	{
	}

	void SetChannelsCount(int size)
	{
		channels.resize(size);

		channelSampleBuffers.resize(size * 2);

		outputCommands.resize(size);
	}

	INT32 GetChannelsCount() const
	{
		return channels.size();
	}
	//////////////////////////////////////////////////////
public:
	int BeginUpdateSamples(int updateSampleCounts)
	{
		updateSampleCounts = VGMPlayer_MIN((VGM_SAMPLE_BUFFER_SIZE - currentSampleIdx), updateSampleCounts); // never exceed bufferSize
		if (updateSampleCounts == 0)
			return 0;

		channelSampleBuffers.resize(channels.size() * 2);
		for (size_t i = 0; i < channels.size(); i++)
		{
			channelSampleBuffers[i * 2 + 0] = &channels[i].leftSamples[currentSampleIdx];
			channelSampleBuffers[i * 2 + 1] = &channels[i].rightSamples[currentSampleIdx];
		}

		return updateSampleCounts;
	}

	int HandleUpdateSamples(void (*chipUpdate)(UINT8, INT32**, UINT32),
		UINT32(*chipGetChannelCount)(UINT8),
		UINT8 chipID, INT32 baseChannel, UINT32 length)
	{
		chipUpdate(chipID, &channelSampleBuffers[baseChannel], length);

		baseChannel += chipGetChannelCount(chipID);

		return baseChannel;
	}

	void EndUpdateSamples(int updateSampleCounts)
	{
		currentSampleIdx = currentSampleIdx + updateSampleCounts;	// updated samples, sampleIdx+
		assert(currentSampleIdx <= VGM_SAMPLE_BUFFER_SIZE);
		if (currentSampleIdx == VGM_SAMPLE_BUFFER_SIZE) //  1/ 50 frame
		{
			currentSampleIdx = 0;

			// FillOutputSampleBuffer();

			FillOutputCommand();
		}
	}
private:
	void FillOutputSampleBuffer()
	{
		INT16* dest = outputSampleBuffer;

		INT32 div = channels.size();
		for (INT32 i = 0; i < VGM_SAMPLE_BUFFER_SIZE; i++) // always fill by fix size VGM_SAMPLE_COUNT
		{
			INT32 outL = 0;
			for (size_t ch = 0; ch < channels.size(); ch++)
				outL += channels[ch].leftSamples[i];
			outL = outL / div;

			INT32 outR = 0;
			for (size_t ch = 0; ch < channels.size(); ch++)
				outR += channels[ch].rightSamples[i];
			outR = outR / div;

			*dest++ = outL;
			*dest++ = outR;
		}
	}

	template< typename... Args >
	std::string string_format(const char* format, Args... args)
	{
		size_t length = std::snprintf(nullptr, 0, format, args...);
		if (length <= 0)
		{
			return "";
		}

		char* buf = new char[length + 1];
		std::snprintf(buf, length + 1, format, args...);

		std::string str(buf);
		delete[] buf;
		return std::move(str);
	}

	void FillOutputCommand()
	{
		outputCommands.clear();

		size_t maxCommandCounts = 0;
		for (size_t i = 0; i < channels.size(); i++)
		{
			if (maxCommandCounts < channels[i].commands.size())
			{
				maxCommandCounts = channels[i].commands.size();
			}
		}

		for (size_t i = 0; i < maxCommandCounts; i++)
		{
			string line;

			for (size_t ch = 0; ch < channels.size(); ch++)
			{
				if (i < channels[ch].commands.size())
				{
					VGMChannel::Command& command = channels[ch].commands[i];
					line += string_format("%04X   ", command.data);
				}
				else
				{
					line += string_format("0000  ");
				}
			}

			outputCommands.push_back(line);
		}

		for (size_t i = 0; i < outputCommands.size(); i++)
		{
			//vgm_log("%s\n", outputCommands[i].c_str());
		}
	}
public:
	const INT32& GetChannelLeftSample(int ch, int i) const
	{
		return channels[ch].leftSamples[i];
	}

	const INT32& GetChannelRightSample(int ch, int i) const
	{
		return channels[ch].rightSamples[i];
	}

	int GetChannelCommandCount(int ch) const
	{
		return channels[ch].commands.size();
	}

	const VGMChannel::Command& GetChannelCommand(int ch, int i) const
	{
		return channels[ch].commands[i];
	}

	const INT16& GetOutputSample(int channel, int i) const
	{
		return outputSampleBuffer.Get(channel, i);
	}

	const std::vector<INT16>& GetOutputSampleBuffer() const
	{
		return outputSampleBuffer.GetBuffer();
	}

	INT16* GetOutputSampleBuffer()
	{
		return outputSampleBuffer.GetBuffer();
	}

	const vector<string>& GetOutputCommands() const
	{
		return outputCommands;
	}
private:
	vector<VGMChannel> channels;

	UINT32 currentSampleIdx;
	vector<INT32*> channelSampleBuffers;
	OutputSampleBuffer outputSampleBuffer;

	vector<string> outputCommands;
};

class VGMInfo
{
public:
	VGMInfo(const char* path_, const char* texturePath_, INT32 channels_, INT32 bitPerSample_, INT32 sampleRate_)
	{
		UINT8* firstPtr = (UINT8*)&first;
		UINT8* lastPtr = (UINT8*)&last;
		int count = lastPtr - firstPtr + 4;

		memset(firstPtr, 0, count);

		path = path_;
		texturePath = texturePath_;

		this->channels = channels_;
		this->bitPerSamples = bitPerSample_;

		SampleRate = sampleRate_;
	}

	~VGMInfo()
	{
	}

	///////////////////////////////////////////////////////////////////////
	string path;
	string texturePath;
	INT32 channels;
	INT32 bitPerSamples;

	VGMOutputChannels outputChannels;

	///////////////////////////////////////////////////////////////////////
	// Options Variables

	UINT32 first;

	UINT32 SampleRate;	// Note: also used by some sound cores to determinate the chip sample rate

	UINT32 VGMMaxLoop;
	UINT32 VGMPbRate;	// in Hz, ignored if this value or VGM's lngRate Header value is 0

	bool WaveOutOpen;
	bool StreamPause;
	HWAVEOUT hWaveOut;

	CMF_HEADER CMFHead;
	UINT16 CMFInsCount;
	CMF_INSTRUMENT* CMFIns;

	DRO_HEADER DROHead;
	DRO_VER_HEADER_2 DROInf;
	UINT8* DROCodemap;

#ifdef ADDITIONAL_FORMATS
	UINT32 CMFMaxLoop;
#endif
	UINT32 FadeTime;
	UINT32 PauseTime;	// current Pause Time

	float VolumeLevel;
	bool SurroundSound;
	UINT8 HardStopOldVGMs;
	bool FadeRAWLog;
	//bool FullBufFill;	// Fill Buffer until it's full
	bool PauseEmulate;

	bool DoubleSSGVol;

	UINT8 ResampleMode;	// 00 - HQ both, 01 - LQ downsampling, 02 - LQ both
	UINT8 CHIP_SAMPLING_MODE;
	INT32 CHIP_SAMPLE_RATE;

	UINT16 FMPort;
	bool FMForce;
	//bool FMAccurate;
	bool FMBreakFade;
	float FMVol;
	bool FMOPL2Pan;

	CHIPS_OPTION ChipOpts[0x02];

	UINT8 OPL_MODE;
	UINT8 OPL_CHIPS;
#ifdef WIN32
	bool WINNT_MODE;
#endif
	char* AppPaths[8];

	bool AutoStopSkip;

	bool PreferJapTag;

	UINT8 NextPLCmd;
	UINT8 PLMode;	// set to 1 to show Playlist text
	bool FirstInit;
	UINT8 Show95Cmds;
	UINT8 LogToWave;
	UINT16 ForceAudioBuf;
	UINT8 OutputDevID;

	UINT8 lastMMEvent;

	UINT8 FileMode;
	VGM_HEADER VGMHead;
	VGM_HDR_EXTRA VGMHeadX;
	VGM_EXTRA VGMH_Extra;
	UINT32 VGMDataLen;
	UINT8* VGMData;
	GD3_TAG VGMTag;

#define PCM_BANK_COUNT	0x40
	VGM_PCM_BANK PCMBank[PCM_BANK_COUNT];
	PCMBANK_TBL PCMTbl;
	UINT8 DacCtrlUsed;
	UINT8 DacCtrlUsg[0xFF];
	DACCTRL_DATA DacCtrl[0xFF];

#ifdef WIN32
	HANDLE hPlayThread;
#else
	pthread_t hPlayThread;
#endif
	bool PlayThreadOpen;
	bool PauseThread;
	bool CloseThread;
	bool ThreadPauseEnable;
	volatile bool ThreadPauseConfrm;
	bool ThreadNoWait;	// don't reset the timer

	CHIP_AUDIO ChipAudio[0x02];
	CAUD_ATTR CA_Paired[0x02][0x03];
	float MasterVol;

	CA_LIST ChipListBuffer[0x200];
	CA_LIST* ChipListAll;	// all chips needed for playback (in general)
	CA_LIST* ChipListPause;	// all chips needed for EmulateWhilePaused
	//CA_LIST* ChipListOpt;	// ChipListAll minus muted chips
	CA_LIST* CurChipList;	// pointer to Pause or All [Opt]

#define SMPL_BUFSIZE	0x100
	INT32* StreamBufs[0x02];

#ifdef MIXER_MUTING

#ifdef WIN32
	HMIXER hmixer;
	MIXERCONTROL mixctrl;
#else
	int hmixer;
	UINT16 mixer_vol;
#endif

#else	//#ifndef MIXER_MUTING
	float VolumeBak;
#endif

	UINT32 VGMPos;
	INT32 VGMSmplPos;
	INT32 VGMSmplPlayed;
	INT32 VGMSampleRate;
	UINT32 VGMPbRateMul;
	UINT32 VGMPbRateDiv;
	UINT32 VGMSmplRateMul;
	UINT32 VGMSmplRateDiv;
	UINT32 PauseSmpls;
	bool VGMEnd;
	bool EndPlay;
	bool PausePlay;
	bool FadePlay;
	bool ForceVGMExec;
	UINT8 PlayingMode;
	bool UseFM;
	UINT32 PlayingTime;
	UINT32 FadeStart;
	UINT32 VGMMaxLoopM;
	UINT32 VGMCurLoop;
	float VolumeLevelM;
	float FinalVol;
	bool ResetPBTimer;

	bool Interpreting;

#ifdef CONSOLE_MODE
	bool ErrorHappened;
	UINT8 CmdList[0x100];
#endif

	// UINT8 IsVGMInit;
	UINT16 Last95Drum;	// for optvgm debugging
	UINT16 Last95Max;	// for optvgm debugging
	UINT32 Last95Freq;	// for optvgm debugging

	UINT32 last;
};

class VGMDataImpl;

class VGMData : public Obserable
{
public:
	VGMData(const char* path_, const char* texturePath_, INT32 channels_, INT32 bitPerSample_, INT32 sampleRate_);
	~VGMData();

	UINT32 GetVersion();
	const char* GetPath() const;

	void Something();
	boolean Open();
	void Close();
	void Play();
	void Stop();
	boolean IsPlaying();
	void Pause();
	void Resume();
	boolean IsPaused();

	boolean Update(bool needUpdateSample);

	const VGMInfo& GetInfo() const;
	const VGMOutputChannels& GetOutputChannels() const;
public:
	bool IsRequestedUpdateSample();
	void RequestedUpdateSample();
	void ClearRequestedUpdateSample();
private:
protected:
private:
	void VGMPlay_Init(void);
	void VGMPlay_Init2(void);
	void VGMPlay_Deinit(void);
	void ReadOptions(const char* AppName);

	bool OpenVGMFile(const char* FileName);
	void CloseVGMFile(void);
	void FreeGD3Tag(GD3_TAG* TagData);

	void PlayVGM(void);
	void StopVGM(void);
	void RestartVGM(void);
	void RestartPlaying(void);
	void PauseVGM(bool Pause);
	void SeekVGM(bool Relative, INT32 PlayBkSamples);


	void open_fm_option(UINT8 ChipType, UINT8 OptType, UINT32 OptVal);
	void opl_chip_reset(void);
	void open_real_fm(void);
	void setup_real_fm(UINT8 ChipType, UINT8 ChipID);
	void close_real_fm(void);

	UINT32 gcd(UINT32 x, UINT32 y);
	UINT32 GetChipClock(VGM_HEADER* FileHead, UINT8 ChipID, UINT8* RetSubType);
	INT32 SampleVGM2Pbk_I(INT32 SampleVal);
	INT32 SamplePbk2VGM_I(INT32 SampleVal);
	INT32 SampleVGM2Playback(INT32 SampleVal);
	INT32 SamplePlayback2VGM(INT32 SampleVal);
	bool SetMuteControl(bool mute);
	void RefreshVolume();
	void StartSkipping(void);
	void StopSkipping(void);
	UINT8 StartThread(void);
	UINT8 StopThread(void);
	UINT32 FillBuffer(WAVE_16BS* Buffer, UINT32 BufferSize);

	void ResampleChipStream(CA_LIST* CLst, WAVE_32BS* RetSample, UINT32 Length);
	INT32 RecalcFadeVolume(void);
	void InterpretFile(UINT32 SampleCount);
	void Chips_GeneralActions(UINT8 Mode);
	UINT16 GetChipVolume(VGM_HEADER* FileHead, UINT8 ChipID, UINT8 ChipNum, UINT8 ChipCnt);
	void GeneralChipLists(void);
	void SetupResampler(CAUD_ATTR* CAA);
	void InterpretVGM(UINT32 SampleCount);

	UINT8 GetDACFromPCMBank(void);
	UINT8* GetPointerFromPCMBank(UINT8 Type, UINT32 DataPos);
	void ReadPCMTable(UINT32 DataSize, const UINT8* Data);
	void AddPCMData(UINT8 Type, UINT32 DataSize, const UINT8* Data);
	bool DecompressDataBlk(VGM_PCM_DATA* Bank, UINT32 DataSize, const UINT8* Data);

	UINT32 GetGZFileLength(const char* FileName);
	UINT32 GetGZFileLength_Internal(FILE* hFile);
	bool OpenVGMFile_Internal(UINT32 FileSize);

	void ReadVGMHeader(VGM_HEADER* RetVGMHead);
	UINT8 ReadGD3Tag(UINT32 GD3Offset, GD3_TAG* RetGD3Tag);
	void ReadChipExtraData32(UINT32 StartOffset, VGMX_CHP_EXTRA32* ChpExtra);
	void ReadChipExtraData16(UINT32 StartOffset, VGMX_CHP_EXTRA16* ChpExtra);
	wchar_t* ReadWStrFromFile(UINT32* FilePos, UINT32 EOFPos);
	wchar_t* MakeEmptyWStr(void);
	void PauseStream(bool PauseOn);
	void InterpretOther(UINT32 SampleCount);
	UINT32 GetMIDIDelay(UINT32* DelayLen);
	UINT16 MIDINote2FNum(UINT8 Note, INT8 Pitch);
	void SendMIDIVolume(UINT8 ChipID, UINT8 Channel, UINT8 Command, UINT8 ChnIns, UINT8 Volume);
	bool OpenOtherFile(const char* FileName);
public:
	static void ChangeChipSampleRate(void* DataPtr, UINT32 NewSmplRate);
	UINT32 HandlePlayingThread();
public:
protected:
	VGMDataImpl *impl;
private:
};

#endif
