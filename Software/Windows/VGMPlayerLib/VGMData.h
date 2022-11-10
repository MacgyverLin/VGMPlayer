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
	
	INT32* ChannelStreamBufs[CHANNEL_BUFFER_COUNT];
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
	void SetupResampler(UINT32 CurCSet, UINT32 CurChip, UINT32& BaseChannelIdx, bool UsePairedChip);
	// void SetupResampler(CAUD_ATTR* CAA, UINT32& BaseChannelIdx, UINT32 ChnCnt);
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
