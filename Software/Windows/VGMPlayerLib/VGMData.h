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

	UINT8 ChannelCount;
	
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
	UINT32 lngHzPSG;			// 4
	UINT32 lngHzYM2413;			// 9
	UINT32 lngGD3Offset;
	UINT32 lngTotalSamples;
	UINT32 lngLoopOffset;
	UINT32 lngLoopSamples;
	UINT32 lngRate;
	UINT16 shtPSG_Feedback;
	UINT8 bytPSG_SRWidth;
	UINT8 bytPSG_Flags;
	UINT32 lngHzYM2612;			// 6
	UINT32 lngHzYM2151;			// 8
	UINT32 lngDataOffset;
	UINT32 lngHzSPCM;			// 8
	UINT32 lngSPCMIntf;
	UINT32 lngHzRF5C68;			// 8
	UINT32 lngHzYM2203;			// 3	
	UINT32 lngHzYM2608;			// 7
	UINT32 lngHzYM2610;			// 7
	UINT32 lngHzYM3812;			// 9
	UINT32 lngHzYM3526;			// 9
	UINT32 lngHzY8950;			// 8
	UINT32 lngHzYMF262;			// 18
	UINT32 lngHzYMF278B;		// 18
	UINT32 lngHzYMF271;			// 12
	UINT32 lngHzYMZ280B;		// 8
	UINT32 lngHzRF5C164;		// 8
	UINT32 lngHzPWM;
	UINT32 lngHzAY8910;			// 3
	UINT8 bytAYType;
	UINT8 bytAYFlag;
	UINT8 bytAYFlagYM2203;		// 3
	UINT8 bytAYFlagYM2608;		// 6
	UINT8 bytVolumeModifier;
	UINT8 bytReserved2;
	INT8 bytLoopBase;
	UINT8 bytLoopModifier;
	UINT32 lngHzGBDMG;
	UINT32 lngHzNESAPU;
	UINT32 lngHzMultiPCM;
	UINT32 lngHzUPD7759;		// 1
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

struct CPUCommand
{
	FLOAT32 seconds;
	UINT32 ch;
	UINT32 chValue;
	std::string command;
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

	UINT32 GetOutputChannelBufferCount() const
	{
		return OutputChannelBuffers.size();
	}

	vector<WAVE_32BS*>& GetOutputChannelBuffers()
	{
		return OutputChannelBuffers;
	}

	WAVE_32BS** GetOutputChannelBuffersPtr()
	{
		return &OutputChannelBuffers[0];
	}
	
	const WAVE_32BS* GetOutputChannelBuffer(int channel) const
	{
		assert(channel < OutputChannelBuffers.size());

		return OutputChannelBuffers[channel];
	}

	UINT32 GetOutputChannelBufferSampleCount(int channel)
	{
		return VGM_SAMPLE_BUFFER_SIZE;
	}

	const WAVE_32BS& GetOutputChannelBufferSample(int channel, int sampleIdx) const
	{
		const  WAVE_32BS* sample = GetOutputChannelBuffer(channel);
		
		assert(sampleIdx < VGM_SAMPLE_BUFFER_SIZE);
		return sample[sampleIdx];
	}

	const WAVE_16BS* GetOutputBufferPtr() const
	{
		return &OutputBuffer[0];
	}

	WAVE_16BS* GetOutputBufferPtr()
	{
		return &OutputBuffer[0];
	}

	const vector<WAVE_16BS>& GetOutputBuffer() const
	{
		return OutputBuffer;
	}

	UINT32 GetOutputBufferSampleCount()
	{
		return OutputBuffer.size();
	}

	const WAVE_16BS& GetOutputBufferSample(int sampleIdx) const
	{
		return OutputBuffer[sampleIdx];
	}

	const list<CPUCommand>& GetOutputCommands() const
	{
		return OutputCPUCommands;
	}

	///////////////////////////////////////////////////////////////////////
	string path;
	string texturePath;
	INT32 channels;
	INT32 bitPerSamples;

	vector<WAVE_32BS*> ChannelBuffers;
	vector<WAVE_32BS*> OutputChannelBuffers;
	vector<WAVE_16BS> OutputBuffer;
	list<CPUCommand> OutputCPUCommands;

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
	void ClearCommand(float time);
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
	UINT32 FillBuffer(UINT32 BufferSize, bool fillBuffer);

	void ResampleChipStream(CA_LIST* CLst, WAVE_32BS* RetOutputSample, UINT32 Length, UINT32& BaseChannelIdx);
	INT32 RecalcFadeVolume(void);
	void InterpretFile(UINT32 SampleCount);
	void Chips_GeneralActions(UINT8 Mode);
	UINT16 GetChipVolume(VGM_HEADER* FileHead, UINT8 ChipID, UINT8 ChipNum, UINT8 ChipCnt);
	void GeneralChipLists(void);
	void SetupResampler(UINT32 CurCSet, UINT32 CurChip, UINT32& BaseChannelIdx, bool UsePairedChip);
	// void SetupResampler(CAUD_ATTR* CAA, UINT32& BaseChannelIdx, UINT32 ChnCnt);
	void InterpretVGM(UINT32 SampleCount);
	CPUCommand GetCommand(UINT32 ch, UINT32 chValue);

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
