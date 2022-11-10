#include "VGMData.h"
#include <zlib.h>

///////////////////////////////
class VGMDataImpl
{
public:
	VGMDataImpl(const char* path_, const char* texturePath_, INT32 channels_, INT32 bitPerSample_, INT32 sampleRate_)
		: vgmInfo(path_, texturePath_, channels_, bitPerSample_, sampleRate_)
	{
	}

	~VGMDataImpl()
	{
	}

	VGMInfo vgmInfo;

	gzFile gzfile;
};

UINT16 ReadLE16(const UINT8* Data)
{
	// read 16-Bit Word (Little Endian/Intel Byte Order)
#ifdef VGM_LITTLE_ENDIAN
	return *(UINT16*)Data;
#else
	return (Data[0x01] << 8) | (Data[0x00] << 0);
#endif
}

UINT16 ReadBE16(const UINT8* Data)
{
	// read 16-Bit Word (Big Endian/Motorola Byte Order)
#ifdef VGM_BIG_ENDIAN
	return *(UINT16*)Data;
#else
	return (Data[0x00] << 8) | (Data[0x01] << 0);
#endif
}

UINT32 ReadLE24(const UINT8* Data)
{
	// read 24-Bit Word (Little Endian/Intel Byte Order)
#ifdef VGM_LITTLE_ENDIAN
	return	(*(UINT32*)Data) & 0x00FFFFFF;
#else
	return	(Data[0x02] << 16) | (Data[0x01] << 8) | (Data[0x00] << 0);
#endif
}

UINT32 ReadLE32(const UINT8* Data)
{
	// read 32-Bit Word (Little Endian/Intel Byte Order)
#ifdef VGM_LITTLE_ENDIAN
	return	*(UINT32*)Data;
#else
	return	(Data[0x03] << 24) | (Data[0x02] << 16) |
		(Data[0x01] << 8) | (Data[0x00] << 0);
#endif
}

INT16 Limit2Short(INT32 Value)
{
	INT32 NewValue;

	NewValue = Value;
	if (NewValue < -0x8000)
		NewValue = -0x8000;
	if (NewValue > 0x7FFF)
		NewValue = 0x7FFF;

	return (INT16)NewValue;
}

static void null_update(UINT8 ChipID, stream_sample_t** outputs, int samples, stream_sample_t** channeoutputs, int channelcount)
{
	::memset(outputs[0x00], 0x00, sizeof(stream_sample_t) * samples);
	::memset(outputs[0x01], 0x00, sizeof(stream_sample_t) * samples);

	return;
}

static void dual_opl2_stereo(UINT8 ChipID, stream_sample_t** outputs, int samples, stream_sample_t** channeoutputs, int channelcount)
{
	ym3812_stream_update(ChipID, outputs, samples, channeoutputs, channelcount);

	// Dual-OPL with Stereo
	if (ChipID & 0x01)
		::memset(outputs[0x00], 0x00, sizeof(stream_sample_t) * samples);	// Mute Left Chanel
	else
		::memset(outputs[0x01], 0x00, sizeof(stream_sample_t) * samples);	// Mute Right Chanel

	return;
}


VGMData::VGMData(const char* path_, const char* texturePath_, INT32 channels_, INT32 bitPerSample_, INT32 sampleRate_)
	: Obserable()
{
	assert(!impl);

	impl = new VGMDataImpl(path_, texturePath_, channels_, bitPerSample_, sampleRate_);
}

VGMData::~VGMData()
{
	if (impl)
	{
		delete impl;
		impl = nullptr;
	}
}

UINT32 VGMData::GetVersion()
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return vgmInfo.VGMHead.lngVersion;
}

const char* VGMData::GetPath() const
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return vgmInfo.path.c_str();
}

void VGMData::Something()
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	NotifySomething();
}

boolean VGMData::Open()
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	VGMPlay_Init();

	ReadOptions("VGMPlay.ini");
	VGMPlay_Init2();

	vgmInfo.VGMMaxLoop = 1;
	vgmInfo.FadeTime = 5000;

	bool result = false;
	if (OpenVGMFile(impl->vgmInfo.path.c_str()))
		result = true;
	else if (OpenOtherFile(impl->vgmInfo.path.c_str()))
		result = true;

	vgmInfo.outputChannels.SetChannelsCount(16);

	NotifyOpen();

	return result;
}

void VGMData::Close()
{
	NotifyClose();

	CloseVGMFile();

	VGMPlay_Deinit();
}

void VGMData::Play()
{
	PlayVGM();

	NotifyPlay();
}

void VGMData::Stop()
{
	NotifyStop();
}

boolean VGMData::IsPlaying()
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return !vgmInfo.EndPlay;
}

void VGMData::Pause()
{
	NotifyPause();
}

void VGMData::Resume()
{
	NotifyResume();
}

boolean VGMData::IsPaused()
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return vgmInfo.PausePlay;
}

boolean VGMData::Update(bool needUpdateSample)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	if (needUpdateSample)
	{
		UINT32 sampleCount = vgmInfo.outputChannels.BeginUpdateSamples(VGM_SAMPLE_BUFFER_SIZE);

		sampleCount = FillBuffer((WAVE_16BS*)vgmInfo.outputChannels.GetOutputSampleBuffer(), sampleCount);

		vgmInfo.outputChannels.EndUpdateSamples(sampleCount);
	}

	NotifyUpdate(needUpdateSample);

	return TRUE;
}

const VGMInfo& VGMData::GetInfo() const
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return impl->vgmInfo;
}

const VGMOutputChannels& VGMData::GetOutputChannels() const
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return impl->vgmInfo.outputChannels;
}

const UINT8 CHN_COUNT[CHIP_COUNT] =
{ 0x04, 0x09, 0x06, 0x08, 0x10, 0x08, 0x03, 0x00,
	0x00, 0x09, 0x09, 0x09, 0x12, 0x00, 0x0C, 0x08,
	0x08, 0x00, 0x03, 0x04, 0x05, 0x1C, 0x00, 0x00,
	0x04, 0x05, 0x08, 0x08, 0x18, 0x04, 0x04, 0x10,
	0x20, 0x04, 0x06, 0x06, 0x20, 0x20, 0x10, 0x20,
	0x04
};

void VGMData::VGMPlay_Init(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT8 CurChip;
	UINT8 CurCSet;
	UINT8 CurChn;
	CHIP_OPTS* TempCOpt;
	CAUD_ATTR* TempCAud;

	vgmInfo.SampleRate = 44100;
	vgmInfo.FadeTime = 5000;
	vgmInfo.PauseTime = 0;

	vgmInfo.HardStopOldVGMs = 0x00;
	vgmInfo.FadeRAWLog = false;
	vgmInfo.VolumeLevel = 1.0f;
	//FullBufFill = false;
	vgmInfo.FMPort = 0x0000;
	vgmInfo.FMForce = false;
	//FMAccurate = false;
	vgmInfo.FMBreakFade = false;
	vgmInfo.FMVol = 0.0f;
	vgmInfo.FMOPL2Pan = false;
	vgmInfo.SurroundSound = false;
	vgmInfo.VGMMaxLoop = 0x02;
	vgmInfo.VGMPbRate = 0;

	vgmInfo.WaveOutOpen = false;
	vgmInfo.PauseThread = true;
	vgmInfo.StreamPause = false;
	vgmInfo.hWaveOut = NULL;
#ifdef ADDITIONAL_FORMATS
	vgmInfo.CMFMaxLoop = 0x01;
#endif
	vgmInfo.ResampleMode = 0x00;
	vgmInfo.CHIP_SAMPLING_MODE = 0x00;
	vgmInfo.CHIP_SAMPLE_RATE = 0x00000000;
	vgmInfo.PauseEmulate = false;
	vgmInfo.DoubleSSGVol = false;

	for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
	{
		TempCAud = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet];
		for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, TempCAud++)
		{
			TempCOpt = (CHIP_OPTS*)&vgmInfo.ChipOpts[CurCSet] + CurChip;

			TempCOpt->Disabled = false;
			TempCOpt->EmuCore = 0x00;
			TempCOpt->SpecialFlags = 0x00;
			TempCOpt->ChnCnt = CHN_COUNT[CurChip]; // 0x00;
			TempCOpt->ChnMute1 = 0x00;
			TempCOpt->ChnMute2 = 0x00;
			TempCOpt->ChnMute3 = 0x00;
			TempCOpt->Panning = NULL;

			// Set up some important fields to prevent in_vgm from crashing
			// when clicking on Muting checkboxes after init.
			TempCAud->ChipType = 0xFF;
			TempCAud->ChipID = CurCSet;
			TempCAud->Paired = NULL;
		}
		vgmInfo.ChipOpts[CurCSet].GameBoy.SpecialFlags = 0x0003;
		// default options, 0x8000 skips the option write and keeps NSFPlay's default values
		// TODO: Is this really necessary??
		vgmInfo.ChipOpts[CurCSet].NES.SpecialFlags = 0x8000 |
			(0x00 << 12) | (0x3B << 4) | (0x01 << 2) | (0x03 << 0);
		vgmInfo.ChipOpts[CurCSet].SCSP.SpecialFlags = 0x0001;	// bypass SCSP DSP

		TempCAud = vgmInfo.CA_Paired[CurCSet];
		for (CurChip = 0x00; CurChip < 0x03; CurChip++, TempCAud++)
		{
			TempCAud->ChipType = 0xFF;
			TempCAud->ChipID = CurCSet;
			TempCAud->Paired = NULL;
		}

		// currently the only chips with Panning support are
		// SN76496 and YM2413, it should be not a problem that it's hardcoded.
		TempCOpt = (CHIP_OPTS*)&vgmInfo.ChipOpts[CurCSet].SN76496;
		TempCOpt->ChnCnt = 0x04;
		TempCOpt->Panning = (INT16*)malloc(sizeof(INT16) * TempCOpt->ChnCnt);
		for (CurChn = 0x00; CurChn < TempCOpt->ChnCnt; CurChn++)
			TempCOpt->Panning[CurChn] = 0x00;

		TempCOpt = (CHIP_OPTS*)&vgmInfo.ChipOpts[CurCSet].YM2413;
		TempCOpt->ChnCnt = 0x0E;	// 0x09 + 0x05
		TempCOpt->Panning = (INT16*)malloc(sizeof(INT16) * TempCOpt->ChnCnt);
		for (CurChn = 0x00; CurChn < TempCOpt->ChnCnt; CurChn++)
			TempCOpt->Panning[CurChn] = 0x00;
	}

	for (CurChn = 0; CurChn < 8; CurChn++)
		vgmInfo.AppPaths[CurChn] = NULL;
	vgmInfo.AppPaths[0] = const_cast<char*>("");

	vgmInfo.FileMode = 0xFF;

	vgmInfo.PausePlay = false;

#ifdef _DEBUG
	if (sizeof(CHIP_AUDIO) != sizeof(CAUD_ATTR) * CHIP_COUNT)
	{
		fprintf(stderr, "Fatal Error! vgmInfo.ChipAudio structure invalid!\n");
		getchar();
		exit(-1);
	}
	if (sizeof(CHIPS_OPTION) != sizeof(CHIP_OPTS) * CHIP_COUNT)
	{
		fprintf(stderr, "Fatal Error! vgmInfo.ChipOpts structure invalid!\n");
		getchar();
		exit(-1);
	}
#endif

	return;
}

void VGMData::VGMPlay_Init2(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	// has to be called after the configuration is loaded
#if 0
	if (vgmInfo.FMPort)
	{
#if defined(WIN32) && defined(_MSC_VER)
		__try
		{
			// should work well with WinXP Compatibility Mode
			_inp(vgmInfo.FMPort);
			vgmInfo.WINNT_MODE = false;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			vgmInfo.WINNT_MODE = true;
		}
#endif

		if (!vgmInfo.OPL_MODE)	// OPL not forced
			OPL_Hardware_Detecton();
		if (!vgmInfo.OPL_MODE)	// no OPL chip found
			vgmInfo.FMPort = 0x0000;	// disable FM
	}

	if (vgmInfo.FMPort)
	{
		// prepare FM Hardware Access and open MIDI Mixer
#ifdef WIN32
#ifdef MIXER_MUTING
		mixerOpen(&hmixer, 0x00, 0x00, 0x00, 0x00);
		GetMixerControl();
#endif

		if (vgmInfo.WINNT_MODE)
		{
			if (OpenPortTalk())
				return;
		}
#else	//#ifndef WIN32
#ifdef MIXER_MUTING
		hmixer = open("/dev/mixer", O_RDWR);
#endif

		if (OpenPortTalk())
			return;
#endif
}

#endif

	vgmInfo.StreamBufs[0x00] = (INT32*)malloc(SMPL_BUFSIZE * sizeof(INT32));
	vgmInfo.StreamBufs[0x01] = (INT32*)malloc(SMPL_BUFSIZE * sizeof(INT32));

	for (int i = 0; i < CHANNEL_BUFFER_COUNT; i++)
	{
		vgmInfo.ChannelStreamBufs[i] = (INT32*)malloc(SMPL_BUFSIZE * sizeof(INT32));
	}

	if (vgmInfo.CHIP_SAMPLE_RATE <= 0)
		vgmInfo.CHIP_SAMPLE_RATE = vgmInfo.SampleRate;
	vgmInfo.PlayingMode = 0xFF;

	return;
}

void VGMData::VGMPlay_Deinit(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT8 CurChip;
	UINT8 CurCSet;
	CHIP_OPTS* TempCOpt;

	if (vgmInfo.FMPort)
	{
#ifdef MIXER_MUTING
#ifdef WIN32
		mixerClose(hmixer);
#else
		close(hmixer);
#endif
#endif
	}

	::free(vgmInfo.StreamBufs[0x00]);	vgmInfo.StreamBufs[0x00] = NULL;
	::free(vgmInfo.StreamBufs[0x01]);	vgmInfo.StreamBufs[0x01] = NULL;
	for (int i = 0; i < CHANNEL_BUFFER_COUNT; i++)
	{
		::free(vgmInfo.ChannelStreamBufs[i]);	vgmInfo.ChannelStreamBufs[i] = NULL;
	}

	for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
	{
		for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++)
		{
			TempCOpt = (CHIP_OPTS*)&vgmInfo.ChipOpts[CurCSet] + CurChip;

			if (TempCOpt->Panning != NULL)
			{
				::free(TempCOpt->Panning);	TempCOpt->Panning = NULL;
			}
		}
	}

	return;
}

void VGMData::ReadOptions(const char* AppName)
{
#if 0
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	const UINT8 CHN_COUNT[CHIP_COUNT] =
	{ 0x04, 0x09, 0x06, 0x08, 0x10, 0x08, 0x03, 0x00,
		0x00, 0x09, 0x09, 0x09, 0x12, 0x00, 0x0C, 0x08,
		0x08, 0x00, 0x03, 0x04, 0x05, 0x1C, 0x00, 0x00,
		0x04, 0x05, 0x08, 0x08, 0x18, 0x04, 0x04, 0x10,
		0x20, 0x04, 0x06, 0x06, 0x20, 0x20, 0x10, 0x20,
		0x04
	};
	const UINT8 CHN_MASK_CNT[CHIP_COUNT] =
	{ 0x04, 0x0E, 0x07, 0x08, 0x10, 0x08, 0x03, 0x06,
		0x06, 0x0E, 0x0E, 0x0E, 0x17, 0x18, 0x0C, 0x08,
		0x08, 0x00, 0x03, 0x04, 0x05, 0x1C, 0x00, 0x00,
		0x04, 0x05, 0x08, 0x08, 0x18, 0x04, 0x04, 0x10,
		0x20, 0x04, 0x06, 0x06, 0x20, 0x20, 0x10, 0x20,
		0x04
	};
	const char* FNList[3];
	char* FileName;
	FILE* hFile;
	char TempStr[0x40];
	UINT32 StrLen;
	UINT32 TempLng;
	char* LStr;
	char* RStr;
	UINT8 IniSection;
	UINT8 CurChip;
	CHIP_OPTS* TempCOpt;
	CHIP_OPTS* TempCOpt2;
	UINT8 CurChn;
	char* TempPnt;
	bool TempFlag;

	// most defaults are set by VGMPlay_Init()
	vgmInfo.FadeTimeN = vgmInfo.FadeTime;
	vgmInfo.FadeTimePL = 2000;
	vgmInfo.PauseTimeJ = vgmInfo.PauseTime;
	vgmInfo.PauseTimeL = 0;
	vgmInfo.Show95Cmds = 0x00;
	vgmInfo.LogToWave = 0x00;
	vgmInfo.OutputDevID = 0;
	vgmInfo.ForceAudioBuf = 0x00;
	vgmInfo.PreferJapTag = false;

	if (AppName == NULL)
	{
		printerr("Argument \"Application-Path\" is NULL!\nSkip loading INI.\n");
		return;
	}

	// AppName: "C:\VGMPlay\VGMPlay.exe"
	RStr = strrchr(AppName, DIR_CHR);
	if (RStr != NULL)
		RStr++;
	else
		RStr = (char*)AppName;
	FileName = (char*)malloc(strlen(RStr) + 0x05);	// ".ini" + 00
	strcpy(FileName, RStr);
	// FileName: "VGMPlay.exe"

	RStr = GetFileExtension(FileName);
	if (RStr == NULL)
	{
		RStr = FileName + strlen(FileName);
		*RStr = '.';
		RStr++;
	}
	strcpy(RStr, "ini");
	// FileName: "VGMPlay.ini" or "vgmplay.ini"

	// on Linux platforms, it searches for "vgmplay.ini" first and
	// file names are case sensitive
	FNList[0] = FileName;
	FNList[1] = "VGMPlay.ini";
	FNList[2] = NULL;
	LStr = FileName;
	FileName = FindFile_List(FNList);
	free(LStr);
	if (FileName == NULL)
	{
		printerr("Failed to load INI.\n");
		return;
	}
	hFile = fopen(FileName, "rt");
	free(FileName);
	if (hFile == NULL)
	{
		printerr("Failed to load INI.\n");
		return;
	}

	IniSection = 0x00;
	while (!feof(hFile))
	{
		LStr = fgets(TempStr, 0x40, hFile);
		if (LStr == NULL)
			break;
		if (TempStr[0x00] == ';')	// Comment line
			continue;

		StrLen = strlen(TempStr) - 0x01;
		//if (TempStr[StrLen] == '\n')
		//	TempStr[StrLen] = '\0';
		while (TempStr[StrLen] < 0x20)
		{
			TempStr[StrLen] = '\0';
			if (!StrLen)
				break;
			StrLen--;
		}
		if (!StrLen)
			continue;
		StrLen++;

		LStr = &TempStr[0x00];
		while (*LStr == ' ')
			LStr++;
		if (LStr[0x00] == ';')	// Comment line
			continue;

		if (LStr[0x00] == '[')
			RStr = strchr(TempStr, ']');
		else
			RStr = strchr(TempStr, '=');
		if (RStr == NULL)
			continue;

		if (LStr[0x00] == '[')
		{
			// Line pattern: [Group]
			LStr++;
			RStr = strchr(TempStr, ']');
			if (RStr != NULL)
				RStr[0x00] = '\0';

			if (!stricmp_u(LStr, "General"))
			{
				IniSection = 0x00;
			}
			else
			{
				IniSection = 0xFF;
				for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++)
				{
					if (!stricmp_u(LStr, GetChipName(CurChip)))
					{
						IniSection = 0x80 | CurChip;
						break;
					}
				}
				if (IniSection == 0xFF)
					continue;
			}
		}
		else
		{
			// Line pattern: Option = Value
			TempLng = RStr - TempStr;
			TempStr[TempLng] = '\0';

			// Prepare Strings (trim the spaces)
			RStr = &TempStr[TempLng - 0x01];
			while (*RStr == ' ')
				*(RStr--) = '\0';

			RStr = &TempStr[StrLen - 0x01];
			while (*RStr == ' ')
				*(RStr--) = '\0';
			RStr = &TempStr[TempLng + 0x01];
			while (*RStr == ' ')
				RStr++;

			switch (IniSection)
			{
			case 0x00:	// General Sction
				if (!stricmp_u(LStr, "SampleRate"))
				{
					vgmInfo.SampleRate = strtoul(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "PlaybackRate"))
				{
					vgmInfo.VGMPbRate = strtoul(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "DoubleSSGVol"))
				{
					vgmInfo.DoubleSSGVol = GetBoolFromStr(RStr);
				}
				else if (!stricmp_u(LStr, "PreferJapTag"))
				{
					vgmInfo.PreferJapTag = GetBoolFromStr(RStr);
				}
				else if (!stricmp_u(LStr, "FadeTime"))
				{
					vgmInfo.FadeTimeN = strtoul(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "FadeTimePL"))
				{
					vgmInfo.FadeTimePL = strtoul(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "JinglePause"))
				{
					vgmInfo.PauseTimeJ = strtoul(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "HardStopOld"))
				{
					vgmInfo.HardStopOldVGMs = (UINT8)strtoul(RStr, &TempPnt, 0);
					if (TempPnt == RStr)
						vgmInfo.HardStopOldVGMs = GetBoolFromStr(RStr) ? 0x01 : 0x00;
				}
				else if (!stricmp_u(LStr, "FadeRAWLogs"))
				{
					vgmInfo.FadeRAWLog = GetBoolFromStr(RStr);
				}
				else if (!stricmp_u(LStr, "Volume"))
				{
					vgmInfo.VolumeLevel = (float)strtod(RStr, NULL);
				}
				else if (!stricmp_u(LStr, "LogSound"))
				{
					//LogToWave = GetBoolFromStr(RStr);
					vgmInfo.LogToWave = (UINT8)strtoul(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "MaxLoops"))
				{
					vgmInfo.VGMMaxLoop = strtoul(RStr, NULL, 0);
				}
#ifdef ADDITIONAL_FORMATS
				else if (!stricmp_u(LStr, "MaxLoopsCMF"))
				{
					vgmInfo.CMFMaxLoop = strtoul(RStr, NULL, 0);
				}
#endif
				else if (!stricmp_u(LStr, "ResamplingMode"))
				{
					vgmInfo.ResampleMode = (UINT8)strtol(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "ChipSmplMode"))
				{
					vgmInfo.CHIP_SAMPLING_MODE = (UINT8)strtol(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "ChipSmplRate"))
				{
					vgmInfo.CHIP_SAMPLE_RATE = strtol(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "OutputDevice"))
				{
					vgmInfo.OutputDevID = (UINT8)strtol(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "AudioBuffers"))
				{
					vgmInfo.ForceAudioBuf = (UINT16)strtol(RStr, NULL, 0);
					if (vgmInfo.ForceAudioBuf < 0x04)
						vgmInfo.ForceAudioBuf = 0x00;
				}
				else if (!stricmp_u(LStr, "SurroundSound"))
				{
					vgmInfo.SurroundSound = GetBoolFromStr(RStr);
				}
				else if (!stricmp_u(LStr, "EmulatePause"))
				{
					vgmInfo.PauseEmulate = GetBoolFromStr(RStr);
				}
				else if (!stricmp_u(LStr, "ShowStreamCmds"))
				{
					vgmInfo.Show95Cmds = (UINT8)strtol(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "FMPort"))
				{
					vgmInfo.FMPort = (UINT16)strtoul(RStr, NULL, 16);
				}
				else if (!stricmp_u(LStr, "FMForce"))
				{
					vgmInfo.FMForce = GetBoolFromStr(RStr);
				}
				else if (!stricmp_u(LStr, "FMVolume"))
				{
					vgmInfo.FMVol = (float)strtod(RStr, NULL);
				}
				else if (!stricmp_u(LStr, "FMOPL2Pan"))
				{
					vgmInfo.FMOPL2Pan = GetBoolFromStr(RStr);
				}
				/*else if (! stricmp_u(LStr, "AccurateFM"))
				{
					FMAccurate = GetBoolFromStr(RStr);
				}*/
				else if (!stricmp_u(LStr, "FMSoftStop"))
				{
					vgmInfo.FMBreakFade = GetBoolFromStr(RStr);
				}
				break;
			case 0x80:	// SN76496
			case 0x81:	// YM2413
			case 0x82:	// YM2612
			case 0x83:	// YM2151
			case 0x84:	// SegaPCM
			case 0x85:	// RF5C68
			case 0x86:	// YM2203
			case 0x87:	// YM2608
			case 0x88:	// YM2610
			case 0x89:	// YM3812
			case 0x8A:	// YM3526
			case 0x8B:	// Y8950
			case 0x8C:	// YMF262
			case 0x8D:	// YMF278B
			case 0x8E:	// YMF271
			case 0x8F:	// YMZ280B
			case 0x90:	// RF5C164
			case 0x91:	// PWM
			case 0x92:	// AY8910
			case 0x93:	// GameBoy
			case 0x94:	// NES
			case 0x95:	// MultiPCM
			case 0x96:	// UPD7759
			case 0x97:	// OKIM6258
			case 0x98:	// OKIM6295
			case 0x99:	// K051649
			case 0x9A:	// K054539
			case 0x9B:	// HuC6280
			case 0x9C:	// C140
			case 0x9D:	// K053260
			case 0x9E:	// Pokey
			case 0x9F:	// QSound
			case 0xA0:	// SCSP
			case 0xA1:	// WonderSwan
			case 0xA2:	// VSU
			case 0xA3:	// SAA1099
			case 0xA4:	// ES5503
			case 0xA5:	// ES5506
			case 0xA6:	// X1_010
			case 0xA7:	// C352
			case 0xA8:	// GA20
				CurChip = IniSection & 0x7F;
				TempCOpt = (CHIP_OPTS*)&vgmInfo.ChipOpts[0x00] + CurChip;

				if (!stricmp_u(LStr, "Disabled"))
				{
					TempCOpt->Disabled = GetBoolFromStr(RStr);
				}
				else if (!stricmp_u(LStr, "EmulatorType"))
				{
					TempCOpt->EmuCore = (UINT8)strtol(RStr, NULL, 0);
				}
				else if (!stricmp_u(LStr, "MuteMask"))
				{
					if (!CHN_COUNT[CurChip])
						break;	// must use MuteMaskFM and MuteMask???
					TempCOpt->ChnMute1 = strtoul(RStr, NULL, 0);
					if (CHN_MASK_CNT[CurChip] < 0x20)
						TempCOpt->ChnMute1 &= (1 << CHN_MASK_CNT[CurChip]) - 1;
				}
				else if (!strnicmp_u(LStr, "MuteCh", 0x06))
				{
					if (!CHN_COUNT[CurChip])
						break;	// must use MuteFM and Mute???
					CurChn = (UINT8)strtol(LStr + 0x06, &TempPnt, 0);
					if (TempPnt == NULL || *TempPnt)
						break;
					if (CurChn >= CHN_COUNT[CurChip])
						break;
					TempFlag = GetBoolFromStr(RStr);
					TempCOpt->ChnMute1 &= ~(0x01 << CurChn);
					TempCOpt->ChnMute1 |= TempFlag << CurChn;
				}
				else
				{
					switch (CurChip)
					{
						//case 0x00:	// SN76496
					case 0x02:	// YM2612
						if (!stricmp_u(LStr, "MuteDAC"))
						{
							CurChn = 0x06;
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->ChnMute1 &= ~(0x01 << CurChn);
							TempCOpt->ChnMute1 |= TempFlag << CurChn;
						}
						else if (!stricmp_u(LStr, "DACHighpass"))
						{
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->SpecialFlags &= ~(0x01 << 0);
							TempCOpt->SpecialFlags |= TempFlag << 0;
						}
						else if (!stricmp_u(LStr, "SSG-EG"))
						{
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->SpecialFlags &= ~(0x01 << 1);
							TempCOpt->SpecialFlags |= TempFlag << 1;
						}
						else if (!stricmp_u(LStr, "PseudoStereo"))
						{
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->SpecialFlags &= ~(0x01 << 2);
							TempCOpt->SpecialFlags |= TempFlag << 2;
						}
						else if (!stricmp_u(LStr, "NukedType"))
						{
							TempLng = (UINT32)strtoul(RStr, NULL, 0) & 0x03;
							TempCOpt->SpecialFlags &= ~(0x03 << 3);
							TempCOpt->SpecialFlags |= TempLng << 3;
						}
						break;
						//case 0x03:	// YM2151
						//case 0x04:	// SegaPCM
						//case 0x05:	// RF5C68
					case 0x06:	// YM2203
						if (!stricmp_u(LStr, "DisableAY"))
						{
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->SpecialFlags &= ~(0x01 << 0);
							TempCOpt->SpecialFlags |= TempFlag << 0;
						}
						break;
					case 0x07:	// YM2608
					case 0x08:	// YM2610
						if (!stricmp_u(LStr, "DisableAY"))
						{
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->SpecialFlags &= ~(0x01 << 0);
							TempCOpt->SpecialFlags |= TempFlag << 0;
						}
						else if (!stricmp_u(LStr, "MuteMask_FM"))
						{
							TempCOpt->ChnMute1 = strtoul(RStr, NULL, 0);
							TempCOpt->ChnMute1 &= (1 << CHN_MASK_CNT[CurChip]) - 1;
						}
						else if (!stricmp_u(LStr, "MuteMask_PCM"))
						{
							TempCOpt->ChnMute2 = strtoul(RStr, NULL, 0);
							TempCOpt->ChnMute2 &= (1 << (CHN_MASK_CNT[CurChip] + 1)) - 1;
						}
						else if (!strnicmp_u(LStr, "MuteFMCh", 0x08))
						{
							CurChn = (UINT8)strtol(LStr + 0x08, &TempPnt, 0);
							if (TempPnt == NULL || *TempPnt)
								break;
							if (CurChn >= CHN_MASK_CNT[CurChip])
								break;
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->ChnMute1 &= ~(0x01 << CurChn);
							TempCOpt->ChnMute1 |= TempFlag << CurChn;
						}
						else if (!strnicmp_u(LStr, "MutePCMCh", 0x09))
						{
							CurChn = (UINT8)strtol(LStr + 0x09, &TempPnt, 0);
							if (TempPnt == NULL || *TempPnt)
								break;
							if (CurChn >= CHN_MASK_CNT[CurChip])
								break;
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->ChnMute2 &= ~(0x01 << CurChn);
							TempCOpt->ChnMute2 |= TempFlag << CurChn;
						}
						else if (!stricmp_u(LStr, "MuteDT"))
						{
							CurChn = 0x06;
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->ChnMute2 &= ~(0x01 << CurChn);
							TempCOpt->ChnMute2 |= TempFlag << CurChn;
						}
						break;
					case 0x01:	// YM2413
					case 0x09:	// YM3812
					case 0x0A:	// YM3526
					case 0x0B:	// Y8950
					case 0x0C:	// YMF262
						CurChn = 0xFF;
						if (!stricmp_u(LStr, "MuteBD"))
							CurChn = 0x00;
						else if (!stricmp_u(LStr, "MuteSD"))
							CurChn = 0x01;
						else if (!stricmp_u(LStr, "MuteTOM"))
							CurChn = 0x02;
						else if (!stricmp_u(LStr, "MuteTC"))
							CurChn = 0x03;
						else if (!stricmp_u(LStr, "MuteHH"))
							CurChn = 0x04;
						else if (CurChip == 0x0B && !stricmp_u(LStr, "MuteDT"))
							CurChn = 0x05;
						if (CurChn != 0xFF)
						{
							if (CurChip < 0x0C)
								CurChn += 9;
							else
								CurChn += 18;
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->ChnMute1 &= ~(0x01 << CurChn);
							TempCOpt->ChnMute1 |= TempFlag << CurChn;
						}
						break;
					case 0x0D:	// YMF278B
						if (!stricmp_u(LStr, "MuteMask_FM"))
						{
							TempCOpt->ChnMute1 = strtoul(RStr, NULL, 0);
							TempCOpt->ChnMute1 &= (1 << CHN_MASK_CNT[CurChip - 0x01]) - 1;
						}
						else if (!stricmp_u(LStr, "MuteMask_WT"))
						{
							TempCOpt->ChnMute2 = strtoul(RStr, NULL, 0);
							TempCOpt->ChnMute2 &= (1 << CHN_MASK_CNT[CurChip]) - 1;
						}
						else if (!strnicmp_u(LStr, "MuteFMCh", 0x08))
						{
							CurChn = (UINT8)strtol(LStr + 0x08, &TempPnt, 0);
							if (TempPnt == NULL || *TempPnt)
								break;
							if (CurChn >= CHN_COUNT[CurChip - 0x01])
								break;
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->ChnMute1 &= ~(0x01 << CurChn);
							TempCOpt->ChnMute1 |= TempFlag << CurChn;
						}
						else if (!strnicmp_u(LStr, "MuteFM", 0x06))
						{
							CurChn = 0xFF;
							if (!stricmp_u(LStr + 6, "BD"))
								CurChn = 0x00;
							else if (!stricmp_u(LStr + 6, "SD"))
								CurChn = 0x01;
							else if (!stricmp_u(LStr + 6, "TOM"))
								CurChn = 0x02;
							else if (!stricmp_u(LStr + 6, "TC"))
								CurChn = 0x03;
							else if (!stricmp_u(LStr + 6, "HH"))
								CurChn = 0x04;
							if (CurChn != 0xFF)
							{
								CurChn += 18;
								TempFlag = GetBoolFromStr(RStr);
								TempCOpt->ChnMute1 &= ~(0x01 << CurChn);
								TempCOpt->ChnMute1 |= TempFlag << CurChn;
							}
						}
						else if (!strnicmp_u(LStr, "MuteWTCh", 0x08))
						{
							CurChn = (UINT8)strtol(LStr + 0x08, &TempPnt, 0);
							if (TempPnt == NULL || *TempPnt)
								break;
							if (CurChn >= CHN_MASK_CNT[CurChip])
								break;
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->ChnMute2 &= ~(0x01 << CurChn);
							TempCOpt->ChnMute2 |= TempFlag << CurChn;
						}
						break;
						//case 0x0E:	// YMF271
						//case 0x0F:	// YMZ280B
							/*if (! stricmp_u(LStr, "DisableFix"))
							{
								DISABLE_YMZ_FIX = GetBoolFromStr(RStr);
							}
							break;*/
							//case 0x10:	// RF5C164
							//case 0x11:	// PWM
							//case 0x12:	// AY8910
					case 0x13:	// GameBoy
						if (!stricmp_u(LStr, "BoostWaveChn"))
						{
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->SpecialFlags &= ~(0x01 << 0);
							TempCOpt->SpecialFlags |= TempFlag << 0;
						}
						break;
					case 0x14:	// NES
						if (!stricmp_u(LStr, "SharedOpts"))
						{
							// 2 bits
							TempLng = (UINT32)strtol(RStr, NULL, 0) & 0x03;
							TempCOpt->SpecialFlags &= ~(0x03 << 0) & 0x7FFF;
							TempCOpt->SpecialFlags |= TempLng << 0;
						}
						else if (!stricmp_u(LStr, "APUOpts"))
						{
							// 2 bits
							TempLng = (UINT32)strtol(RStr, NULL, 0) & 0x03;
							TempCOpt->SpecialFlags &= ~(0x03 << 2) & 0x7FFF;
							TempCOpt->SpecialFlags |= TempLng << 2;
						}
						else if (!stricmp_u(LStr, "DMCOpts"))
						{
							// 8 bits (6 bits used)
							TempLng = (UINT32)strtol(RStr, NULL, 0) & 0xFF;
							TempCOpt->SpecialFlags &= ~(0xFF << 4) & 0x7FFF;
							TempCOpt->SpecialFlags |= TempLng << 4;
						}
						else if (!stricmp_u(LStr, "FDSOpts"))
						{
							// 1 bit
							TempLng = (UINT32)strtol(RStr, NULL, 0) & 0x01;
							TempCOpt->SpecialFlags &= ~(0x01 << 12) & 0x7FFF;
							TempCOpt->SpecialFlags |= TempLng << 12;
						}
						break;
					case 0x17:	// OKIM6258
						if (!stricmp_u(LStr, "Enable10Bit"))
						{
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->SpecialFlags &= ~(0x01 << 0);
							TempCOpt->SpecialFlags |= TempFlag << 0;
						}
						break;
					case 0x20:	// SCSP
						if (!stricmp_u(LStr, "BypassDSP"))
						{
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->SpecialFlags &= ~(0x01 << 0);
							TempCOpt->SpecialFlags |= TempFlag << 0;
						}
						break;
					case 0x27:	// C352
						if (!stricmp_u(LStr, "DisableRear"))
						{
							TempFlag = GetBoolFromStr(RStr);
							TempCOpt->SpecialFlags &= ~(0x01 << 0);
							TempCOpt->SpecialFlags |= TempFlag << 0;
						}
						break;
					}
				}
				break;
			case 0xFF:	// Dummy Section
				break;
			}
		}
	}

	TempCOpt = (CHIP_OPTS*)&vgmInfo.ChipOpts[0x00];
	TempCOpt2 = (CHIP_OPTS*)&vgmInfo.ChipOpts[0x01];
	for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, TempCOpt++, TempCOpt2++)
	{
		TempCOpt2->Disabled = TempCOpt->Disabled;
		TempCOpt2->EmuCore = TempCOpt->EmuCore;
		TempCOpt2->SpecialFlags = TempCOpt->SpecialFlags;
		TempCOpt2->ChnMute1 = TempCOpt->ChnMute1;
		TempCOpt2->ChnMute2 = TempCOpt->ChnMute2;
		TempCOpt2->ChnMute3 = TempCOpt->ChnMute3;
	}

	fclose(hFile);

#ifdef WIN32
	WinNT_Check();
#endif
	if (CHIP_SAMPLE_RATE <= 0)
		CHIP_SAMPLE_RATE = SampleRate;

	return;
#endif
			}

bool VGMData::OpenVGMFile(const char* FileName)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile& gzfile = impl->gzfile;

	UINT32 FileSize;
	bool RetVal;

	FileSize = GetGZFileLength(FileName);

	gzfile = gzopen(FileName, "rb");
	if (gzfile == NULL)
		return false;

	RetVal = OpenVGMFile_Internal(FileSize);

	gzclose(gzfile);
	return RetVal;
}

bool VGMData::OpenVGMFile_Internal(UINT32 FileSize)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile& gzfile = impl->gzfile;

	UINT32 fccHeader;
	UINT32 CurPos;
	UINT32 HdrLimit;

	//gzseek(hFile, 0x00, SEEK_SET);
	gzrewind(gzfile);
	gzread(gzfile, &fccHeader, 4);
	if (fccHeader != FCC_VGM)
		return false;

	if (vgmInfo.FileMode != 0xFF)
		CloseVGMFile();

	vgmInfo.FileMode = 0x00;
	vgmInfo.VGMDataLen = FileSize;

	gzseek(gzfile, 0x00, SEEK_SET);
	//gzrewind(hFile);
	ReadVGMHeader(&vgmInfo.VGMHead);
	if (vgmInfo.VGMHead.fccVGM != FCC_VGM)
	{
		fprintf(stderr, "VGM signature matched on the first read, but not on the second one!\n");
		fprintf(stderr, "This is a known zlib bug where gzseek fails. Please install a fixed zlib.\n");
		return false;
	}

	vgmInfo.VGMSampleRate = 44100;
	if (!vgmInfo.VGMDataLen)
		vgmInfo.VGMDataLen = vgmInfo.VGMHead.lngEOFOffset;
	if (!vgmInfo.VGMHead.lngEOFOffset || vgmInfo.VGMHead.lngEOFOffset > vgmInfo.VGMDataLen)
	{
		fprintf(stderr, "Warning! Invalid EOF Offset 0x%02X! (should be: 0x%02X)\n",
			vgmInfo.VGMHead.lngEOFOffset, vgmInfo.VGMDataLen);
		vgmInfo.VGMHead.lngEOFOffset = vgmInfo.VGMDataLen;
	}
	if (vgmInfo.VGMHead.lngLoopOffset && !vgmInfo.VGMHead.lngLoopSamples)
	{
		// 0-Sample-Loops causes the program to hangs in the playback routine
		logerror("Warning! Ignored Zero-Sample-Loop!\n");
		vgmInfo.VGMHead.lngLoopOffset = 0x00000000;
	}
	if (vgmInfo.VGMHead.lngDataOffset < 0x00000040)
	{
		logerror("Warning! Invalid Data Offset 0x%02X!\n", vgmInfo.VGMHead.lngDataOffset);
		vgmInfo.VGMHead.lngDataOffset = 0x00000040;
	}

	memset(&vgmInfo.VGMHeadX, 0x00, sizeof(VGM_HDR_EXTRA));
	memset(&vgmInfo.VGMH_Extra, 0x00, sizeof(VGM_EXTRA));

	// Read Data
	vgmInfo.VGMDataLen = vgmInfo.VGMHead.lngEOFOffset;
	vgmInfo.VGMData = (UINT8*)malloc(vgmInfo.VGMDataLen);
	if (vgmInfo.VGMData == NULL)
		return false;
	//gzseek(hFile, 0x00, SEEK_SET);
	gzrewind(gzfile);
	gzread(gzfile, vgmInfo.VGMData, vgmInfo.VGMDataLen);

	// Read Extra Header Data
	if (vgmInfo.VGMHead.lngExtraOffset)
	{
		UINT32* TempPtr;

		CurPos = vgmInfo.VGMHead.lngExtraOffset;
		TempPtr = (UINT32*)&vgmInfo.VGMHeadX;
		// Read Header Size
		vgmInfo.VGMHeadX.DataSize = ReadLE32(&vgmInfo.VGMData[CurPos]);
		if (vgmInfo.VGMHeadX.DataSize > sizeof(VGM_HDR_EXTRA))
			vgmInfo.VGMHeadX.DataSize = sizeof(VGM_HDR_EXTRA);
		HdrLimit = CurPos + vgmInfo.VGMHeadX.DataSize;
		CurPos += 0x04;
		TempPtr++;

		// Read all relative offsets of this header and make them absolute.
		for (; CurPos < HdrLimit; CurPos += 0x04, TempPtr++)
		{
			*TempPtr = ReadLE32(&vgmInfo.VGMData[CurPos]);
			if (*TempPtr)
				*TempPtr += CurPos;
		}

		ReadChipExtraData32(vgmInfo.VGMHeadX.Chp2ClkOffset, &vgmInfo.VGMH_Extra.Clocks);
		ReadChipExtraData16(vgmInfo.VGMHeadX.ChpVolOffset, &vgmInfo.VGMH_Extra.Volumes);
	}

	// Read GD3 Tag
	HdrLimit = ReadGD3Tag(vgmInfo.VGMHead.lngGD3Offset, &vgmInfo.VGMTag);
	if (HdrLimit == 0x10)
	{
		vgmInfo.VGMHead.lngGD3Offset = 0x00000000;
		//return false;
	}
	if (!vgmInfo.VGMHead.lngGD3Offset)
	{
		// replace all NULL pointers with empty strings
		vgmInfo.VGMTag.strTrackNameE = MakeEmptyWStr();
		vgmInfo.VGMTag.strTrackNameJ = MakeEmptyWStr();
		vgmInfo.VGMTag.strGameNameE = MakeEmptyWStr();
		vgmInfo.VGMTag.strGameNameJ = MakeEmptyWStr();
		vgmInfo.VGMTag.strSystemNameE = MakeEmptyWStr();
		vgmInfo.VGMTag.strSystemNameJ = MakeEmptyWStr();
		vgmInfo.VGMTag.strAuthorNameE = MakeEmptyWStr();
		vgmInfo.VGMTag.strAuthorNameJ = MakeEmptyWStr();
		vgmInfo.VGMTag.strReleaseDate = MakeEmptyWStr();
		vgmInfo.VGMTag.strCreator = MakeEmptyWStr();
		vgmInfo.VGMTag.strNotes = MakeEmptyWStr();
	}

	return true;
}

UINT32 VGMData::GetGZFileLength(const char* FileName)
{
	FILE* hFile;
	UINT32 FileSize;

	hFile = fopen(FileName, "rb");
	if (hFile == NULL)
		return 0xFFFFFFFF;

	FileSize = GetGZFileLength_Internal(hFile);

	fclose(hFile);
	return FileSize;
}

UINT32 VGMData::GetGZFileLength_Internal(FILE* hFile)
{
	UINT32 FileSize;
	UINT16 gzHead;
	size_t RetVal;

	RetVal = fread(&gzHead, 0x02, 0x01, hFile);
	if (RetVal >= 1)
	{
		gzHead = ReadBE16((UINT8*)&gzHead);
		if (gzHead != 0x1F8B)
		{
			RetVal = 0;	// no .gz signature - treat as normal file
	}
		else
		{
			// .gz File
			fseek(hFile, -4, SEEK_END);
			// Note: In the error case it falls back to fseek/ftell.
			RetVal = fread(&FileSize, 0x04, 0x01, hFile);
#ifndef VGM_LITTLE_ENDIAN
			FileSize = ReadLE32((UINT8*)&FileSize);
#endif
		}
}
	if (RetVal <= 0)
	{
		// normal file
		fseek(hFile, 0x00, SEEK_END);
		FileSize = ftell(hFile);
	}

	return FileSize;
		}

void VGMData::CloseVGMFile(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	if (vgmInfo.FileMode == 0xFF)
		return;

	vgmInfo.VGMHead.fccVGM = 0x00;
	::free(vgmInfo.VGMH_Extra.Clocks.CCData);		vgmInfo.VGMH_Extra.Clocks.CCData = NULL;
	::free(vgmInfo.VGMH_Extra.Volumes.CCData);	vgmInfo.VGMH_Extra.Volumes.CCData = NULL;
	::free(vgmInfo.VGMData);	vgmInfo.VGMData = NULL;

	if (vgmInfo.FileMode == 0x00)
		FreeGD3Tag(&vgmInfo.VGMTag);

	vgmInfo.FileMode = 0xFF;

	return;
}

void VGMData::FreeGD3Tag(GD3_TAG* TagData)
{
	if (TagData == NULL)
		return;

	TagData->fccGD3 = 0x00;
	::free(TagData->strTrackNameE);	TagData->strTrackNameE = NULL;
	::free(TagData->strTrackNameJ);	TagData->strTrackNameJ = NULL;
	::free(TagData->strGameNameE);	TagData->strGameNameE = NULL;
	::free(TagData->strGameNameJ);	TagData->strGameNameJ = NULL;
	::free(TagData->strSystemNameE);	TagData->strSystemNameE = NULL;
	::free(TagData->strSystemNameJ);	TagData->strSystemNameJ = NULL;
	::free(TagData->strAuthorNameE);	TagData->strAuthorNameE = NULL;
	::free(TagData->strAuthorNameJ);	TagData->strAuthorNameJ = NULL;
	::free(TagData->strReleaseDate);	TagData->strReleaseDate = NULL;
	::free(TagData->strCreator);		TagData->strCreator = NULL;
	::free(TagData->strNotes);		TagData->strNotes = NULL;

	return;
}

void VGMData::PlayVGM(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT8 CurChip;
	UINT8 FMVal;
	INT32 TempSLng;

	if (vgmInfo.PlayingMode != 0xFF)
		return;

	//PausePlay = false;
	vgmInfo.FadePlay = false;
	vgmInfo.MasterVol = 1.0f;
	printf("%f\n", vgmInfo.MasterVol);
	vgmInfo.ForceVGMExec = false;
	vgmInfo.AutoStopSkip = false;
	vgmInfo.FadeStart = 0;
	vgmInfo.ForceVGMExec = true;
	vgmInfo.PauseThread = true;

	// FM Check
	FMVal = 0x00;
	if (vgmInfo.FMPort)
	{
		// check usage of devices that use the FM port, and ones that don't use it
		for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++)
		{
			if (!GetChipClock(&vgmInfo.VGMHead, CurChip, NULL))
				continue;

			// supported chips are:
			//	SN76496 (0x00, special behaviour), YM2413 (0x01)
			//	YM3812 (0x09), YM3526 (0x0A), Y8950 (0x0B), YMF262 (0x0C)
			if (CurChip == 0x00 || CurChip == 0x12)
				// The SN76496 and AY8910 have a special state because of
				FMVal |= 0x04;		// bad FM emulation and fast software emulation.
			else if (CurChip == 0x01 || (CurChip >= 0x09 && CurChip <= 0x0C))
				FMVal |= 0x02;
			else
				FMVal |= 0x01;
		}

		if (!vgmInfo.FMForce)
		{
			if (FMVal & 0x01)	// one or more software emulators used?
				FMVal &= ~0x02;	// use only software emulation

			if (FMVal == 0x04)
				FMVal = 0x01;	// FM SN76496 emulaton is bad
			else if ((FMVal & 0x05) == 0x05)
				FMVal &= ~0x04;	// prefer software SN76496 instead of unsynced output
		}
		FMVal = (FMVal & ~0x04) | ((FMVal & 0x04) >> 1);
	}
	switch (FMVal)
	{
	case 0x00:
	case 0x01:
		vgmInfo.PlayingMode = 0x00;	// Normal Mode
		break;
	case 0x02:
		vgmInfo.PlayingMode = 0x01;	// FM only Mode
		break;
	case 0x03:
		vgmInfo.PlayingMode = 0x02;	// Normal/FM mixed Mode (NOT in sync, Hardware is a lot faster)
		//PlayingMode = 0x00;	// Force Normal Mode until I get them in sync - Mixed Mode
								// sounds terrible
		break;
	}
	vgmInfo.UseFM = (vgmInfo.PlayingMode > 0x00);

	if (vgmInfo.VGMHead.bytVolumeModifier <= VOLUME_MODIF_WRAP)
		TempSLng = vgmInfo.VGMHead.bytVolumeModifier;
	else if (vgmInfo.VGMHead.bytVolumeModifier == (VOLUME_MODIF_WRAP + 0x01))
		TempSLng = VOLUME_MODIF_WRAP - 0x100;
	else
		TempSLng = vgmInfo.VGMHead.bytVolumeModifier - 0x100;
	vgmInfo.VolumeLevelM = (float)(vgmInfo.VolumeLevel * pow(2.0, TempSLng / (double)0x20));
	if (vgmInfo.UseFM)
	{
		if (vgmInfo.FMVol > 0.0f)
			vgmInfo.VolumeLevelM = vgmInfo.FMVol;
		else if (vgmInfo.FMVol < 0.0f)
			vgmInfo.VolumeLevelM *= -vgmInfo.FMVol;
	}
	vgmInfo.FinalVol = vgmInfo.VolumeLevelM;

	if (!vgmInfo.VGMMaxLoop)
	{
		vgmInfo.VGMMaxLoopM = 0x00;
	}
	else
	{
		TempSLng = (vgmInfo.VGMMaxLoop * vgmInfo.VGMHead.bytLoopModifier + 0x08) / 0x10 - vgmInfo.VGMHead.bytLoopBase;
		vgmInfo.VGMMaxLoopM = (TempSLng >= 0x01) ? TempSLng : 0x01;
	}

	if (!vgmInfo.VGMPbRate || !vgmInfo.VGMHead.lngRate)
	{
		vgmInfo.VGMPbRateMul = 1;
		vgmInfo.VGMPbRateDiv = 1;
	}
	else
	{
		// I prefer small Multiplers and Dividers, as they're used very often
		TempSLng = gcd(vgmInfo.VGMHead.lngRate, vgmInfo.VGMPbRate);
		vgmInfo.VGMPbRateMul = vgmInfo.VGMHead.lngRate / TempSLng;
		vgmInfo.VGMPbRateDiv = vgmInfo.VGMPbRate / TempSLng;
	}
	vgmInfo.VGMSmplRateMul = vgmInfo.SampleRate * vgmInfo.VGMPbRateMul;
	vgmInfo.VGMSmplRateDiv = vgmInfo.VGMSampleRate * vgmInfo.VGMPbRateDiv;
	// same as above - to speed up the VGM <-> Playback calculation
	TempSLng = gcd(vgmInfo.VGMSmplRateMul, vgmInfo.VGMSmplRateDiv);
	vgmInfo.VGMSmplRateMul /= TempSLng;
	vgmInfo.VGMSmplRateDiv /= TempSLng;

	vgmInfo.PlayingTime = 0;
	vgmInfo.EndPlay = false;

	vgmInfo.VGMPos = vgmInfo.VGMHead.lngDataOffset;
	vgmInfo.VGMSmplPos = 0;
	vgmInfo.VGMSmplPlayed = 0;
	vgmInfo.VGMEnd = false;
	vgmInfo.VGMCurLoop = 0x00;
	vgmInfo.PauseSmpls = (vgmInfo.PauseTime * vgmInfo.SampleRate + 500) / 1000;
	if (vgmInfo.VGMPos >= vgmInfo.VGMHead.lngEOFOffset)
		vgmInfo.VGMEnd = true;

#ifdef CONSOLE_MODE
	memset(vgmInfo.CmdList, 0x00, 0x100 * sizeof(UINT8));
#endif

	if (!vgmInfo.PauseEmulate)
	{
		switch (vgmInfo.PlayingMode)
		{
		case 0x00:
			//PauseStream(PausePlay);
			break;
		case 0x01:
			//PauseThread = PausePlay;
			SetMuteControl(vgmInfo.PausePlay);
			break;
		case 0x02:
			//PauseStream(PausePlay);
			SetMuteControl(vgmInfo.PausePlay);
			break;
		}
	}

	Chips_GeneralActions(0x00);	// Start chips
	// also does Reset (0x01), Muting Mask (0x10) and Panning (0x20)

	if (vgmInfo.UseFM)
	{
		// TODO: get FirstInit working
		//if (! FirstInit)
		{
			StartSkipping();	// don't apply OPL Reset to make Track changes smooth
			vgmInfo.AutoStopSkip = true;
		}
		open_real_fm();
	}

	switch (vgmInfo.PlayingMode)
	{
	case 0x00:	// the application controls the playback thread
		break;
	case 0x01:	// FM Playback needs an independent thread
		vgmInfo.ResetPBTimer = false;
		if (StartThread())
		{
			fprintf(stderr, "Error starting Playing Thread!\n");
			return;
		}
#ifdef CONSOLE_MODE
		PauseStream(true);
#endif
		break;
	case 0x02:	// like Mode 0x00, but Hardware is also controlled (not synced)
		break;
	}

	vgmInfo.Last95Drum = 0xFFFF;
	vgmInfo.Last95Freq = 0;
	vgmInfo.Last95Max = 0xFFFF;
	ym2612_setISVGMInit(true);	//vgmInfo.IsVGMInit = true;
	vgmInfo.Interpreting = false;
	InterpretFile(0);
	ym2612_setISVGMInit(false); // vgmInfo.IsVGMInit = false;

	vgmInfo.PauseThread = false;
	vgmInfo.AutoStopSkip = true;
	vgmInfo.ForceVGMExec = false;

	return;
}

void VGMData::StopVGM(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	if (vgmInfo.PlayingMode == 0xFF)
		return;

	if (!vgmInfo.PauseEmulate)
	{
		if (vgmInfo.UseFM && vgmInfo.PausePlay)
			SetMuteControl(false);
	}

	switch (vgmInfo.PlayingMode)
	{
	case 0x00:
		break;
	case 0x01:
		StopThread();
		/*if (SmoothTrackChange)
		{
			if (ThreadPauseEnable)
			{
				ThreadPauseConfrm = false;
				PauseThread = true;
				while(! ThreadPauseConfrm)
					Sleep(1);	// Wait until the Thread is finished
			}
		}*/
		break;
	case 0x02:
		break;
	}

	Chips_GeneralActions(0x02);	// Stop chips
	if (vgmInfo.UseFM)
		close_real_fm();
	vgmInfo.PlayingMode = 0xFF;

	return;
}

void VGMData::RestartVGM(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	if (vgmInfo.PlayingMode == 0xFF || !vgmInfo.VGMSmplPlayed)
		return;

	RestartPlaying();

	return;
}

void VGMData::RestartPlaying(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	bool OldPThread;

	OldPThread = vgmInfo.PauseThread;
	if (vgmInfo.ThreadPauseEnable)
	{
		vgmInfo.ThreadNoWait = false;
		vgmInfo.ThreadPauseConfrm = false;
		vgmInfo.PauseThread = true;
		while (!vgmInfo.ThreadPauseConfrm)
			Sleep(1);	// Wait until the Thread is finished
	}
	vgmInfo.Interpreting = true;	// Avoid any Thread-Call

	vgmInfo.VGMPos = vgmInfo.VGMHead.lngDataOffset;
	vgmInfo.VGMSmplPos = 0;
	vgmInfo.VGMSmplPlayed = 0;
	vgmInfo.VGMEnd = false;
	vgmInfo.EndPlay = false;
	vgmInfo.VGMCurLoop = 0x00;
	vgmInfo.PauseSmpls = (vgmInfo.PauseTime * vgmInfo.SampleRate + 500) / 1000;

	Chips_GeneralActions(0x01);	// Reset Chips
	// also does Muting Mask (0x10) and Panning (0x20)

	if (vgmInfo.UseFM)
	{
		open_real_fm();	// reset OPL chip and reload settings
		StartSkipping();
		vgmInfo.AutoStopSkip = true;
	}

	vgmInfo.Last95Drum = 0xFFFF;
	vgmInfo.Last95Freq = 0;
	vgmInfo.Interpreting = false;
	vgmInfo.ForceVGMExec = true;
	ym2612_setISVGMInit(true);	//vgmInfo.IsVGMInit = true;
	InterpretFile(0);
	ym2612_setISVGMInit(false); // vgmInfo.IsVGMInit = false;
	vgmInfo.ForceVGMExec = false;
#ifndef CONSOLE_MODE
	vgmInfo.FadePlay = false;
	vgmInfo.MasterVol = 1.0f;
	vgmInfo.FadeStart = 0;
	vgmInfo.FinalVol = vgmInfo.VolumeLevelM;
	vgmInfo.PlayingTime = 0;
#endif
	vgmInfo.PauseThread = OldPThread;

	return;
}

void VGMData::PauseVGM(bool Pause)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	if (vgmInfo.PlayingMode == 0xFF || Pause == vgmInfo.PausePlay)
		return;

	// now uses a temporary variable to avoid gaps (especially with DAC Stream Controller)
	if (!vgmInfo.PauseEmulate)
	{
		if (Pause && vgmInfo.ThreadPauseEnable)
		{
			vgmInfo.ThreadPauseConfrm = false;
			vgmInfo.PauseThread = true;
		}
		switch (vgmInfo.PlayingMode)
		{
		case 0x00:
#ifdef CONSOLE_MODE
			PauseStream(Pause);
#endif
			break;
		case 0x01:
			vgmInfo.ThreadNoWait = false;
			SetMuteControl(Pause);
			break;
		case 0x02:
#ifdef CONSOLE_MODE
			PauseStream(Pause);
#endif
			SetMuteControl(Pause);
			break;
		}
		if (Pause && vgmInfo.ThreadPauseEnable)
		{
			while (!vgmInfo.ThreadPauseConfrm)
				Sleep(1);	// Wait until the Thread is finished
		}
		vgmInfo.PauseThread = Pause;
	}
	vgmInfo.PausePlay = Pause;

	return;
}

void VGMData::SeekVGM(bool Relative, INT32 PlayBkSamples)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	INT32 Samples;
	UINT32 LoopSmpls;

	if (vgmInfo.PlayingMode == 0xFF || (Relative && !PlayBkSamples))
		return;

	LoopSmpls = vgmInfo.VGMCurLoop * SampleVGM2Pbk_I(vgmInfo.VGMHead.lngLoopSamples);
	if (!Relative)
		Samples = PlayBkSamples - (LoopSmpls + vgmInfo.VGMSmplPlayed);
	else
		Samples = PlayBkSamples;

	vgmInfo.ThreadNoWait = false;
	vgmInfo.PauseThread = true;
	if (vgmInfo.UseFM)
		StartSkipping();
	if (Samples < 0)
	{
		Samples = LoopSmpls + vgmInfo.VGMSmplPlayed + Samples;
		if (Samples < 0)
			Samples = 0;
		RestartPlaying();
	}

	vgmInfo.ForceVGMExec = true;
	InterpretFile(Samples);
	vgmInfo.ForceVGMExec = false;
#ifdef CONSOLE_MODE
	if (vgmInfo.FadePlay && vgmInfo.FadeStart)
		vgmInfo.FadeStart += Samples;
#endif
	if (vgmInfo.UseFM)
		StopSkipping();
	vgmInfo.PauseThread = vgmInfo.PausePlay;

	return;
}

void VGMData::open_fm_option(UINT8 ChipType, UINT8 OptType, UINT32 OptVal)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

#ifndef DISABLE_HW_SUPPORT
	vgmInfo.OptArr[OptType & 0x0F] = OptVal;
#endif
	return;
}

void VGMData::opl_chip_reset(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
#ifndef DISABLE_HW_SUPPORT
	UINT16 Reg;
	float FnlVolBak;

	FnlVolBak = vgmInfo.FinalVol;
	vgmInfo.FinalVol = 1.0f;
	memset(vgmInfo.OPLRegForce, 0x01, 0x200);

	OPL_HW_WriteReg(0x105, 0x01);	// OPL3 Enable
	OPL_HW_WriteReg(0x001, 0x20);	// Test Register
	OPL_HW_WriteReg(0x002, 0x00);	// Timer 1
	OPL_HW_WriteReg(0x003, 0x00);	// Timer 2
	OPL_HW_WriteReg(0x004, 0x00);	// IRQ Mask Clear
	OPL_HW_WriteReg(0x104, 0x00);	// 4-Op-Mode Disable
	OPL_HW_WriteReg(0x008, 0x00);	// Keyboard Split

	// make sure all internal calulations finish sound generation
	for (Reg = 0x00; Reg < 0x09; Reg++)
	{
		OPL_HW_WriteReg(0x0C0 | Reg, 0x00);	// silence all notes (OPL3)
		OPL_HW_WriteReg(0x1C0 | Reg, 0x00);
	}
	for (Reg = 0x00; Reg < 0x16; Reg++)
	{
		if ((Reg & 0x07) >= 0x06)
			continue;
		OPL_HW_WriteReg(0x040 | Reg, 0x3F);	// silence all notes (OPL2)
		OPL_HW_WriteReg(0x140 | Reg, 0x3F);

		OPL_HW_WriteReg(0x080 | Reg, 0xFF);	// set Sustain/Release Rate to FASTEST
		OPL_HW_WriteReg(0x180 | Reg, 0xFF);
		OPL_HW_WriteReg(0x060 | Reg, 0xFF);
		OPL_HW_WriteReg(0x160 | Reg, 0xFF);

		OPL_HW_WriteReg(0x020 | Reg, 0x00);	// NULL the rest
		OPL_HW_WriteReg(0x120 | Reg, 0x00);

		OPL_HW_WriteReg(0x0E0 | Reg, 0x00);
		OPL_HW_WriteReg(0x1E0 | Reg, 0x00);
	}
	OPL_HW_WriteReg(0x0BD, 0x00);	// Rhythm Mode
	for (Reg = 0x00; Reg < 0x09; Reg++)
	{
		OPL_HW_WriteReg(0x0B0 | Reg, 0x00);	// turn all notes off (-> Release Phase)
		OPL_HW_WriteReg(0x1B0 | Reg, 0x00);
		OPL_HW_WriteReg(0x0A0 | Reg, 0x00);
		OPL_HW_WriteReg(0x1A0 | Reg, 0x00);
	}

	// although this would be a more proper reset, it sometimes produces clicks
	/*for (Reg = 0x020; Reg <= 0x0FF; Reg ++)
		OPL_HW_WriteReg(Reg, 0x00);
	for (Reg = 0x120; Reg <= 0x1FF; Reg ++)
		OPL_HW_WriteReg(Reg, 0x00);*/

		// Now do a proper reset of all other registers.
	for (Reg = 0x040; Reg < 0x0A0; Reg++)
	{
		if ((Reg & 0x07) >= 0x06 || (Reg & 0x1F) >= 0x18)
			continue;
		OPL_HW_WriteReg(0x000 | Reg, 0x00);
		OPL_HW_WriteReg(0x100 | Reg, 0x00);
}
	for (Reg = 0x00; Reg < 0x09; Reg++)
	{
		OPL_HW_WriteReg(0x0C0 | Reg, 0x30);	// must be 30 to make OPL2 VGMs sound on OPL3
		OPL_HW_WriteReg(0x1C0 | Reg, 0x30);	// if they don't send the C0 reg
	}

	memset(vgmInfo.OPLRegForce, 0x01, 0x200);
	vgmInfo.FinalVol = FnlVolBak;
#endif	// DISABLE_HW_SUPPORT

	return;
}

void VGMData::open_real_fm(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
#ifndef DISABLE_HW_SUPPORT
	UINT8 CurChip;
	CHIP_MAP* CurMap;
	UINT8 CurC2;
	bool OldSM;

	//SkipMode = false;
	OldSM = SkipMode;
	opl_chip_reset();
	OpenedFM = true;

	for (CurChip = 0x00; CurChip < ChipCount; CurChip++)
	{
		SelChip = CurChip;
		CurMap = ChipMap + SelChip;
		switch (CurMap->ChipType)
		{
		case 0x00:	// SN76496 and T6W28
			if (CurMap->ChipOpt[0x0F] & 0x80000000)
			{
				// Avoid Bugs
				CurMap->RegBase = (CurMap->ChipOpt[0x0F] >> 8) & 0xFF;
				CurMap->ChnBase = (CurMap->ChipOpt[0x0F] >> 0) & 0xFF;
			}

			start_sn76496_opl(CurMap->ChipID, CurMap->ChipOpt[0x00], CurMap->ChipOpt[0x05]);
			if (CurMap->ChipOpt[0x00] & 0x80000000)
			{
				// Set up T6W28
				for (CurC2 = 0x00; CurC2 < CurChip; CurC2++)
				{
					if (ChipMap[CurC2].ChipType == CurMap->ChipType)
					{
						CurMap->ChipOpt[0x0F] = 0x80000000 |
							(CurMap->RegBase << 8) | (CurMap->ChnBase << 0);
						CurMap->RegBase = ChipMap[CurC2].RegBase;
						CurMap->ChnBase = ChipMap[CurC2].ChnBase;
						sn76496_refresh_t6w28_opl(ChipMap[CurC2].ChipID);
						break;
					}
				}
			}
			break;
		case 0x01:	// YM2413
			switch (YM2413_EMU_CORE)
			{
			case YM2413_EC_DEFAULT:
				start_ym2413_opl(CurMap->ChipID);
				break;
			case YM2413_EC_MEKA:
				FM_OPL_Init(NULL);
				break;
			}
			break;
		case 0x09:	// YM3812
			break;
		case 0x0A:	// YM3526
			break;
		case 0x0B:	// Y8950
			break;
		case 0x0C:	// YMF262
			break;
		case 0x0D:	// YMF278B
			break;
		case 0x12:	// AY8910
			start_ay8910_opl(CurMap->ChipID, CurMap->ChipOpt[0x00], CurMap->ChipOpt[0x01]);
			break;
	}
}
#endif	// DISABLE_HW_SUPPORT

	return;
}

void VGMData::setup_real_fm(UINT8 ChipType, UINT8 ChipID)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
#ifndef DISABLE_HW_SUPPORT
	CHIP_MAP* CurMap;
	UINT8 CurChip;
	UINT8 CurSet;
	UINT8 CurChn;
	bool ExitLoop;

	SelChip = ChipCount;
	ChipArr[(ChipType << 1) | (ChipID & 0x01)] = SelChip;
	CurMap = ChipMap + SelChip;
	CurMap->ChipType = ChipType;
	CurMap->ChipID = ChipID & 0x7F;

	switch (ChipType)
	{
	case 0x00:	// SN76496 and T6W28
	case 0x12:	// AY8910
		ExitLoop = false;
		for (CurSet = 0x00; CurSet < 0x02; CurSet++)
		{
			for (CurChn = 0x00; CurChn < 0x09; CurChn++)
			{
				if (!RegChnUsage[(CurSet << 4) | CurChn])
				{
					ExitLoop = true;
					break;
				}
			}
			if (ExitLoop)
				break;
		}
		CurSet %= 0x02;
		CurChn %= 0x09;

		CurMap->RegBase = CurSet;
		CurMap->ChnBase = CurChn;
		memcpy(CurMap->ChipOpt, OptArr, 0x10 * sizeof(UINT32));
		CurMap->ChipOpt[0x0F] = 0x00;
		if (ChipType == 0x00)
		{
			for (CurChn = 0x00; CurChn < 0x04; CurChn++)
			{
				RegChnUsage[(CurMap->RegBase << 4) | (CurMap->ChnBase + CurChn)] = true;
			}
		}
		else
		{
			for (CurChn = 0x00; CurChn < 0x03; CurChn++)
			{
				RegChnUsage[(CurMap->RegBase << 4) | (CurMap->ChnBase + CurChn)] = true;
			}
		}
		break;
	case 0x01:	// YM2413
		CurMap->RegBase = 0x00;
		CurMap->ChnBase = 0x00;

		for (CurChip = 0x00; CurChip < SelChip; CurChip++)
		{
			if (ChipMap[CurChip].ChipType == 0x01)
			{
				CurMap->RegBase = 0x01;
				break;
			}

			ChipMap[CurChip].RegBase = 0x01;
		}
		for (CurChn = 0x00; CurChn < 0x09; CurChn++)
		{
			RegChnUsage[(CurMap->RegBase << 4) | CurChn] = true;
		}
		break;
	case 0x09:	// YM3812
		for (CurSet = 0x00; CurSet < 0x02; CurSet++)
		{
			if (!RegChnUsage[(CurSet << 4) | 0x00])
				break;
		}
		CurSet %= 0x02;
		CurMap->RegBase = CurSet;
		CurMap->ChnBase = 0x00;
		break;
	case 0x0A:	// YM3526
		for (CurSet = 0x00; CurSet < 0x02; CurSet++)
		{
			if (!RegChnUsage[(CurSet << 4) | 0x00])
				break;
		}
		CurSet %= 0x02;
		CurMap->RegBase = CurSet;
		CurMap->ChnBase = 0x00;
		break;
	case 0x0B:	// Y8950
		for (CurSet = 0x00; CurSet < 0x02; CurSet++)
		{
			if (!RegChnUsage[(CurSet << 4) | 0x00])
				break;
		}
		CurSet %= 0x02;
		CurMap->RegBase = CurSet;
		CurMap->ChnBase = 0x00;
		break;
	case 0x0C:	// YMF262
		CurMap->RegBase = 0x00;
		CurMap->ChnBase = 0x00;
		break;
	case 0x0D:	// YMF278B
		CurMap->RegBase = 0x00;
		CurMap->ChnBase = 0x00;
		break;
	}
	ChipCount++;
#endif	// DISABLE_HW_SUPPORT

	return;
}

void VGMData::close_real_fm(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
#ifndef DISABLE_HW_SUPPORT
	UINT8 CurChip;
	UINT8 CurChn;
	UINT8 CurOp;
	UINT16 Reg;
	UINT8 RegOp;
	bool SoftFade;

	SoftFade = (FMBreakFade && FinalVol > 0.01f);
	for (CurChip = 0x00; CurChip < 0x02; CurChip++)
	{
		for (CurChn = 0x00; CurChn < 0x09; CurChn++)
		{
			// Make sure that the ReleaseRate takes effect ...
			for (CurOp = 0x00; CurOp < 0x02; CurOp++)
			{
				if (!CurOp && !(OPLReg[(CurChip << 8) | (0xC0 | CurChn)] & 0x01))
					continue;

				RegOp = (CurChn / 0x03) * 0x08 | (CurChn % 0x03) + (CurOp * 0x03);
				Reg = (CurChip << 8) | 0x80 | RegOp;
				if (SoftFade)
				{
					if ((OPLReg[Reg] & 0x0F) < 0x03)	// Force a soft fading
						OPL_HW_WriteReg(Reg, (OPLReg[Reg] & 0xF0) | 0x04);
				}
				else
				{
					// stop sound immediately after Note-Off
					OPL_HW_WriteReg(Reg, (OPLReg[Reg] & 0xF0) | 0x0F);
				}
			}

			// ... and turn off all notes.
			if (SoftFade)
			{
				// soft way (turn off and let fade)
				Reg = (CurChip << 8) | (0xB0 | CurChn);
				OPL_HW_WriteReg(Reg, OPLReg[Reg] & ~0x20);
			}
			else
			{
				// hard way (turn off and set frequency to zero)
				Reg = (CurChip << 8) | (0xA0 | CurChn);
				OPL_HW_WriteReg(Reg | 0x00, 0x00);	// A0 - Frequency LSB
				OPL_HW_WriteReg(Reg | 0x10, 0x00);	// B0 - Frequency MSB / Block / Key On
			}
		}

		// Set Values that are compatible with Windows' FM Driver
		OPL_HW_WriteReg(0x104, 0x00);	// 4-Op-Mode Disable
		OPL_HW_WriteReg(0x001, 0x00);	// Wave Select Disable
		OPL_HW_WriteReg(0x002, 0x00);	// Timer 1
		OPL_HW_WriteReg(0x003, 0x00);	// Timer 2
		OPL_HW_WriteReg(0x004, 0x00);	// IRQ mask clear
		OPL_HW_WriteReg(0x008, 0x00);	// Keyboard Split
		OPL_HW_WriteReg(0x0BD, 0xC0);	// Rhythm Mode
	}
	OpenedFM = false;
	ChipCount = 0x00;
	memset(RegChnUsage, 0x00, sizeof(bool) * 0x20);
#endif	// DISABLE_HW_SUPPORT

	return;
}


UINT32 VGMData::gcd(UINT32 x, UINT32 y)
{
	UINT32 shift;
	UINT32 diff;

	// Thanks to Wikipedia for this algorithm
	// http://en.wikipedia.org/wiki/Binary_GCD_algorithm
	if (!x || !y)
		return x | y;

	for (shift = 0; ((x | y) & 1) == 0; shift++)
	{
		x >>= 1;
		y >>= 1;
	}

	while ((x & 1) == 0)
		x >>= 1;

	do
	{
		while ((y & 1) == 0)
			y >>= 1;

		if (x < y)
		{
			y -= x;
		}
		else
		{
			diff = x - y;
			x = y;
			y = diff;
		}
		y >>= 1;
	} while (y);

	return x << shift;
}


UINT32 VGMData::GetChipClock(VGM_HEADER* FileHead, UINT8 ChipID, UINT8* RetSubType)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT32 Clock;
	UINT8 SubType;
	UINT8 CurChp;
	bool AllowBit31;

	SubType = 0x00;
	AllowBit31 = 0x00;
	switch (ChipID & 0x7F)
	{
	case 0x00:
		Clock = FileHead->lngHzPSG;
		AllowBit31 = 0x01;	// T6W28 Mode
		if (RetSubType != NULL && !(Clock & 0x80000000))	// The T6W28 is handled differently.
		{
			switch (FileHead->bytPSG_SRWidth)
			{
			case 0x0F:	//  0x4000
				if (FileHead->bytPSG_Flags & 0x08)	// Clock Divider == 1?
					SubType = 0x05;	// SN94624
				else
					SubType = 0x01;	// SN76489
				break;
			case 0x10:	//  0x8000
				if (FileHead->shtPSG_Feedback == 0x0009)
					SubType = 0x06;	// SEGA PSG
				else if (FileHead->shtPSG_Feedback == 0x0022)
				{
					if (FileHead->bytPSG_Flags & 0x10)	// if Tandy noise mode enabled
						SubType = (FileHead->bytPSG_Flags & 0x02) ? 0x07 : 0x08;	// NCR8496 / PSSJ-3
					else
						SubType = 0x07;	// NCR8496
				}
				break;
			case 0x11:	// 0x10000
				if (FileHead->bytPSG_Flags & 0x08)	// Clock Divider == 1?
					SubType = 0x03;	// SN76494
				else
					SubType = 0x02;	// SN76489A/SN76496
				break;
			}
			/*
								 FbMask  Noise Taps  Negate Stereo Dv Freq0		Fb	SR	Flags
				01 SN76489		 0x4000, 0x01, 0x02, TRUE,  FALSE, 8, TRUE		03	0F	07 (02|04|00|01) [unverified]
				02 SN76489A		0x10000, 0x04, 0x08, FALSE, FALSE, 8, TRUE		0C	11	05 (00|04|00|01)
				03 SN76494		0x10000, 0x04, 0x08, FALSE, FALSE, 1, TRUE		0C	11	0D (00|04|08|01)
				04 SN76496		0x10000, 0x04, 0x08, FALSE, FALSE, 8, TRUE		0C	11	05 (00|04|00|01) [same as SN76489A]
				05 SN94624		 0x4000, 0x01, 0x02, TRUE,  FALSE, 1, TRUE		03	0F	0F (02|04|08|01) [unverified, SN76489A without /8]
				06 GameGear PSG	 0x8000, 0x01, 0x08, TRUE,  TRUE,  8, FALSE		09	10	02 (02|00|00|00)
				06 SEGA VDP PSG	 0x8000, 0x01, 0x08, TRUE,  FALSE, 8, FALSE		09	10	06 (02|04|00|00)
				07 NCR8496		 0x8000, 0x02, 0x20, TRUE,  FALSE, 8, TRUE		22	10	07 (02|04|00|01)
				08 PSSJ-3		 0x8000, 0x02, 0x20, FALSE, FALSE, 8, TRUE		22	10	05 (00|04|00|01)
				01 U8106		 0x4000, 0x01, 0x02, TRUE,  FALSE, 8, TRUE		03	0F	07 (02|04|00|01) [unverified, same as SN76489]
				02 Y2404		0x10000, 0x04, 0x08, FALSE, FALSE; 8, TRUE		0C	11	05 (00|04|00|01) [unverified, same as SN76489A]
				-- T6W28		0x10000, 0x04, 0x08, ????,  FALSE, 8, ????		0C	11	?? (??|??|00|01) [It IS stereo, but not in GameGear way].
			*/
		}
		break;
	case 0x01:
		Clock = FileHead->lngHzYM2413;
		AllowBit31 = 0x01;	// VRC7 Mode
		break;
	case 0x02:
		Clock = FileHead->lngHzYM2612;
		AllowBit31 = 0x01;	// YM3438 Mode
		break;
	case 0x03:
		Clock = FileHead->lngHzYM2151;
		break;
	case 0x04:
		Clock = FileHead->lngHzSPCM;
		break;
	case 0x05:
		if (ChipID & 0x80)
			return 0;
		Clock = FileHead->lngHzRF5C68;
		break;
	case 0x06:
		Clock = FileHead->lngHzYM2203;
		break;
	case 0x07:
		Clock = FileHead->lngHzYM2608;
		break;
	case 0x08:
		Clock = FileHead->lngHzYM2610;
		AllowBit31 = 0x01;	// YM2610B Mode
		break;
	case 0x09:
		Clock = FileHead->lngHzYM3812;
		AllowBit31 = 0x01;	// Dual OPL2, panned to the L/R speakers
		break;
	case 0x0A:
		Clock = FileHead->lngHzYM3526;
		break;
	case 0x0B:
		Clock = FileHead->lngHzY8950;
		break;
	case 0x0C:
		Clock = FileHead->lngHzYMF262;
		break;
	case 0x0D:
		Clock = FileHead->lngHzYMF278B;
		break;
	case 0x0E:
		Clock = FileHead->lngHzYMF271;
		break;
	case 0x0F:
		Clock = FileHead->lngHzYMZ280B;
		break;
	case 0x10:
		if (ChipID & 0x80)
			return 0;
		Clock = FileHead->lngHzRF5C164;
		AllowBit31 = 0x01;	// hack for Cosmic Fantasy Stories
		break;
	case 0x11:
		if (ChipID & 0x80)
			return 0;
		Clock = FileHead->lngHzPWM;
		break;
	case 0x12:
		Clock = FileHead->lngHzAY8910;
		SubType = FileHead->bytAYType;
		break;
	case 0x13:
		Clock = FileHead->lngHzGBDMG;
		break;
	case 0x14:
		Clock = FileHead->lngHzNESAPU;
		AllowBit31 = 0x01;	// FDS Enable
		break;
	case 0x15:
		Clock = FileHead->lngHzMultiPCM;
		break;
	case 0x16:
		Clock = FileHead->lngHzUPD7759;
		AllowBit31 = 0x01;	// Master/Slave Bit
		break;
	case 0x17:
		Clock = FileHead->lngHzOKIM6258;
		break;
	case 0x18:
		Clock = FileHead->lngHzOKIM6295;
		AllowBit31 = 0x01;	// Pin 7 State
		break;
	case 0x19:
		Clock = FileHead->lngHzK051649;
		AllowBit31 = 0x01;	// SCC/SCC+ Bit
		break;
	case 0x1A:
		Clock = FileHead->lngHzK054539;
		break;
	case 0x1B:
		Clock = FileHead->lngHzHuC6280;
		break;
	case 0x1C:
		Clock = FileHead->lngHzC140;
		SubType = FileHead->bytC140Type;
		break;
	case 0x1D:
		Clock = FileHead->lngHzK053260;
		break;
	case 0x1E:
		Clock = FileHead->lngHzPokey;
		break;
	case 0x1F:
		if (ChipID & 0x80)
			return 0;
		Clock = FileHead->lngHzQSound;
		break;
	case 0x20:
		Clock = FileHead->lngHzSCSP;
		break;
	case 0x21:
		Clock = FileHead->lngHzWSwan;
		break;
	case 0x22:
		Clock = FileHead->lngHzVSU;
		break;
	case 0x23:
		Clock = FileHead->lngHzSAA1099;
		break;
	case 0x24:
		Clock = FileHead->lngHzES5503;
		break;
	case 0x25:
		Clock = FileHead->lngHzES5506;
		AllowBit31 = 0x01;	// ES5505/5506 switch
		break;
	case 0x26:
		Clock = FileHead->lngHzX1_010;
		break;
	case 0x27:
		Clock = FileHead->lngHzC352;
		AllowBit31 = 0x01;	// disable rear channels
		break;
	case 0x28:
		Clock = FileHead->lngHzGA20;
		break;
	default:
		return 0;
	}
	if (ChipID & 0x80)
	{
		VGMX_CHP_EXTRA32* TempCX;

		if (!(Clock & 0x40000000))
			return 0;

		ChipID &= 0x7F;
		TempCX = &vgmInfo.VGMH_Extra.Clocks;
		for (CurChp = 0x00; CurChp < TempCX->ChipCnt; CurChp++)
		{
			if (TempCX->CCData[CurChp].Type == ChipID)
			{
				if (TempCX->CCData[CurChp].Data)
					Clock = TempCX->CCData[CurChp].Data;
				break;
			}
		}
	}

	if (RetSubType != NULL)
		*RetSubType = SubType;
	if (AllowBit31)
		return Clock & 0xBFFFFFFF;
	else
		return Clock & 0x3FFFFFFF;
}

INT32 VGMData::SampleVGM2Pbk_I(INT32 SampleVal)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return (INT32)((INT64)SampleVal * vgmInfo.VGMSmplRateMul / vgmInfo.VGMSmplRateDiv);
}

INT32 VGMData::SamplePbk2VGM_I(INT32 SampleVal)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return (INT32)((INT64)SampleVal * vgmInfo.VGMSmplRateDiv / vgmInfo.VGMSmplRateMul);
}

INT32 VGMData::SampleVGM2Playback(INT32 SampleVal)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return (INT32)((INT64)SampleVal * vgmInfo.VGMSmplRateMul / vgmInfo.VGMSmplRateDiv);
}

INT32 VGMData::SamplePlayback2VGM(INT32 SampleVal)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	return (INT32)((INT64)SampleVal * vgmInfo.VGMSmplRateDiv / vgmInfo.VGMSmplRateMul);
}

bool VGMData::SetMuteControl(bool mute)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

#ifdef MIXER_MUTING

#ifdef WIN32
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED vol;
	HGLOBAL hmem;
	MMRESULT RetVal;

	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = mixctrl.dwControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);

	hmem = GlobalAlloc(0x40, sizeof(MIXERCONTROLDETAILS_UNSIGNED));
	mxcd.paDetails = GlobalLock(hmem);
	vol.dwValue = mute;

	memcpy(mxcd.paDetails, &vol, sizeof(MIXERCONTROLDETAILS_UNSIGNED));
	RetVal = mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);
	GlobalFree(hmem);

	if (RetVal != MMSYSERR_NOERROR)
		return false;

	return true;
#else
	UINT16 mix_vol;
	int RetVal;

	ioctl(hmixer, MIXER_READ(SOUND_MIXER_SYNTH), &mix_vol);
	if (mix_vol)
		mixer_vol = mix_vol;
	mix_vol = mute ? 0x0000 : mixer_vol;

	RetVal = ioctl(hmixer, MIXER_WRITE(SOUND_MIXER_SYNTH), &mix_vol);

	return !RetVal;
#endif

#else	//#indef MIXER_MUTING
	float TempVol;

	TempVol = vgmInfo.MasterVol;
	if (TempVol > 0.0f)
		vgmInfo.VolumeBak = TempVol;

	vgmInfo.MasterVol = mute ? 0.0f : vgmInfo.VolumeBak;
	vgmInfo.FinalVol = vgmInfo.VolumeLevelM * vgmInfo.MasterVol * vgmInfo.MasterVol;
	RefreshVolume();

	return true;
#endif
}

void VGMData::RefreshVolume()
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

#ifndef DISABLE_HW_SUPPORT
	UINT8 CurChip;
	UINT8 CurChn;
	UINT16 RegVal;

	for (CurChip = 0x00; CurChip < 0x02; CurChip++)
	{
		for (CurChn = 0x00; CurChn < 0x09; CurChn++)
		{
			RegVal = (CurChip << 8) | 0x40 | (CurChn / 0x03) * 0x08 | (CurChn % 0x03);
			OPL_HW_WriteReg(RegVal + 0x00, OPLReg[RegVal + 0x00]);
			OPL_HW_WriteReg(RegVal + 0x03, OPLReg[RegVal + 0x03]);
		}
}

	return;
#endif
}

void VGMData::StartSkipping(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

#ifndef DISABLE_HW_SUPPORT
	if (SkipMode)
		return;

	SkipMode = true;
	memcpy(OPLRegBak, OPLReg, 0x200);
#endif

	return;
}

void VGMData::StopSkipping(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

#ifndef DISABLE_HW_SUPPORT
	UINT16 Reg;
	UINT16 OpReg;
	UINT8 RRBuffer[0x40];

	if (!SkipMode)
		return;

	SkipMode = false;

	// At first, turn all notes off that need it
	memcpy(RRBuffer + 0x00, &OPLReg[0x080], 0x20);
	memcpy(RRBuffer + 0x20, &OPLReg[0x180], 0x20);
	for (Reg = 0xB0; Reg < 0xB9; Reg++)
	{
		OpReg = Reg & 0x0F;
		OpReg = (OpReg / 3) * 0x08 + (OpReg % 3);
		if (!(OPLReg[0x100 | Reg] & 0x20))
		{
			OPL_HW_WriteReg(0x180 + OpReg, (OPLReg[0x180 + OpReg] & 0xF0) | 0x0F);
			OPL_HW_WriteReg(0x183 + OpReg, (OPLReg[0x183 + OpReg] & 0xF0) | 0x0F);
			OPLRegForce[0x180 + OpReg] |= 0x01;
			OPLRegForce[0x183 + OpReg] |= 0x01;

			OPLRegForce[0x100 | Reg] |= (OPLReg[0x100 | Reg] != OPLRegBak[0x100 | Reg]);
			OPL_HW_WriteReg(0x100 | Reg, OPLReg[0x100 | Reg]);
		}
		if (!(OPLReg[0x000 | Reg] & 0x20))
		{
			OPL_HW_WriteReg(0x080 + OpReg, (OPLReg[0x080 + OpReg] & 0xF0) | 0x0F);
			OPL_HW_WriteReg(0x083 + OpReg, (OPLReg[0x083 + OpReg] & 0xF0) | 0x0F);
			OPLRegForce[0x080 + OpReg] |= 0x01;
			OPLRegForce[0x083 + OpReg] |= 0x01;

			OPLRegForce[0x000 | Reg] |= (OPLReg[0x000 | Reg] != OPLRegBak[0x000 | Reg]);
			OPL_HW_WriteReg(0x000 | Reg, OPLReg[0x000 | Reg]);
		}
	}
	memcpy(&OPLReg[0x080], RRBuffer + 0x00, 0x20);
	memcpy(&OPLReg[0x180], RRBuffer + 0x20, 0x20);

	// Now the actual save restore.
	Reg = 0x105;	// OPL3 Enable/Disable - this must be the very first thing sent
	OPLRegForce[Reg] |= (OPLReg[Reg] != OPLRegBak[Reg]);
	OPL_HW_WriteReg(Reg, OPLReg[Reg]);

	// Registers 0x00 to 0x1F and 0x100 to 0x11F MUST be sent first
	for (Reg = 0x00; Reg < 0x20; Reg++)
	{
		// Write Port 1 first, so that Port 0 writes override them, if OPL3 mode is disabled
		OPLRegForce[0x100 | Reg] |= (OPLReg[0x100 | Reg] != OPLRegBak[0x100 | Reg]);
		OPL_HW_WriteReg(0x100 | Reg, OPLReg[0x100 | Reg]);
		OPLRegForce[0x000 | Reg] |= (OPLReg[0x000 | Reg] != OPLRegBak[0x000 | Reg]);
		OPL_HW_WriteReg(0x000 | Reg, OPLReg[0x000 | Reg]);
	}

	Reg = 0x200;
	do
	{
		Reg--;
		OPLRegForce[Reg] |= (OPLReg[Reg] != OPLRegBak[Reg]);
		OPL_HW_WriteReg(Reg, OPLReg[Reg]);

		if ((Reg & 0xFF) == 0xC0)
			Reg -= 0x20;	// Skip the frequency-registers
		if ((Reg & 0xFF) == 0x20)
			Reg -= 0x20;	// Skip the registers that are already sent
	} while (Reg);

	for (Reg = 0xA0; Reg < 0xC0; Reg++)
	{
		// Writing to BA/BB on my YMF744 is like writing to B8/B9, so I need to filter such
		// writes out.
		if ((Reg & 0x0F) >= 0x09)
			continue;
		OPLRegForce[0x100 | Reg] |= (OPLReg[0x100 | Reg] != OPLRegBak[0x100 | Reg]);
		OPL_HW_WriteReg(0x100 | Reg, OPLReg[0x100 | Reg]);
		OPLRegForce[0x000 | Reg] |= (OPLReg[0x000 | Reg] != OPLRegBak[0x000 | Reg]);
		OPL_HW_WriteReg(0x000 | Reg, OPLReg[0x000 | Reg]);
	}

	Reg = 0x0BD;	// Rhythm Register / Vibrato/Tremolo Depth
	OPLRegForce[Reg] |= (OPLReg[Reg] != OPLRegBak[Reg]);
	OPL_HW_WriteReg(Reg, OPLReg[Reg]);
#endif

	return;
}

#ifdef WIN32
UINT32 VGMData::HandlePlayingThread()
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	LARGE_INTEGER CPUFreq;
	LARGE_INTEGER TimeNow;
	LARGE_INTEGER TimeLast;
	UINT64 TimeDiff;
	UINT64 SampleTick;
	UINT64 Ticks;
	BOOL RetVal;

	vgmInfo.hPlayThread = GetCurrentThread();
#ifndef _DEBUG
	RetVal = SetThreadPriority(vgmInfo.hPlayThread, THREAD_PRIORITY_TIME_CRITICAL);
#else
	RetVal = SetThreadPriority(vgmInfo.hPlayThread, THREAD_PRIORITY_ABOVE_NORMAL);
#endif
	if (!RetVal)
	{
		// Error by setting priority
	}

	QueryPerformanceFrequency(&CPUFreq);
	QueryPerformanceCounter(&TimeNow);
	TimeLast = TimeNow;
	SampleTick = CPUFreq.QuadPart / vgmInfo.SampleRate;

	while (!vgmInfo.CloseThread)
	{
		while (vgmInfo.PlayingMode != 0x01 && !vgmInfo.CloseThread)
			Sleep(1);

		if (!vgmInfo.PauseThread)
		{
			TimeDiff = TimeNow.QuadPart - TimeLast.QuadPart;
			if (TimeDiff >= SampleTick)
			{
				Ticks = TimeDiff * vgmInfo.SampleRate / CPUFreq.QuadPart;
				if (Ticks > vgmInfo.SampleRate / 2)
					Ticks = vgmInfo.SampleRate / 50;
				FillBuffer(NULL, (UINT32)Ticks);
				if (!vgmInfo.ResetPBTimer)
				{
					TimeLast = TimeNow;
				}
				else
				{
					QueryPerformanceCounter(&TimeLast);
					TimeLast.QuadPart -= TimeDiff;
					vgmInfo.ResetPBTimer = false;
				}
			}

			// I tried to make sample-accurate replaying through Hardware FM
			// to make PSG-PCM clear, but it didn't work
			//if (! FMAccurate)
			Sleep(1);
			//else
			//	Sleep(0);
		}
		else
		{
			vgmInfo.ThreadPauseConfrm = true;
			if (!vgmInfo.ThreadNoWait)
				TimeLast = TimeNow;
			Sleep(1);
		}
		QueryPerformanceCounter(&TimeNow);
	}

	vgmInfo.hPlayThread = NULL;
	return 0x00000000;
}
#else
UINT64 TimeSpec2Int64(const struct timespec* ts)
{
	return (UINT64)ts->tv_sec * 1000000000 + ts->tv_nsec;
}

void* PlayingThread(void* Arg)
{
	UINT64 CPUFreq;
	UINT64 TimeNow;
	UINT64 TimeLast;
	UINT64 TimeDiff;
	UINT64 SampleTick;
	UINT64 Ticks;
	struct timespec TempTS;
	int RetVal;

	//RetVal = clock_getres(CLOCK_MONOTONIC, &TempTS);
	//CPUFreq = TimeSpec2Int64(&TempTS);
	CPUFreq = 1000000000;
	RetVal = clock_gettime(CLOCK_MONOTONIC, &TempTS);
	TimeNow = TimeSpec2Int64(&TempTS);
	TimeLast = TimeNow;
	SampleTick = CPUFreq / SampleRate;

	while (!CloseThread)
	{
		while (PlayingMode != 0x01 && !CloseThread)
			Sleep(1);

		if (!PauseThread)
		{
			TimeDiff = TimeNow - TimeLast;
			if (TimeDiff >= SampleTick)
			{
				Ticks = TimeDiff * SampleRate / CPUFreq;
				if (Ticks > SampleRate / 2)
					Ticks = SampleRate / 50;
				FillBuffer(NULL, (UINT32)Ticks);
				if (!ResetPBTimer)
				{
					TimeLast = TimeNow;
				}
				else
				{
					RetVal = clock_gettime(CLOCK_MONOTONIC, &TempTS);
					TimeLast = TimeSpec2Int64(&TempTS) - TimeDiff;
					ResetPBTimer = false;
				}
			}
			//if (! FMAccurate)
			Sleep(1);
			//else
			//	Sleep(0);
		}
		else
		{
			ThreadPauseConfrm = true;
			if (ThreadNoWait)
				TimeLast = TimeNow;
			Sleep(1);
		}
		RetVal = clock_gettime(CLOCK_MONOTONIC, &TempTS);
		TimeNow = TimeSpec2Int64(&TempTS);
	}

	return NULL;
}
#endif

DWORD WINAPI PlayingThread(void* Arg)
{
	VGMData* vgmData = (VGMData*)Arg;

	return vgmData->HandlePlayingThread();
}

UINT8 VGMData::StartThread(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

#ifdef WIN32
	HANDLE PlayThreadHandle;
	DWORD PlayThreadID;
	//char TestStr[0x80];

	if (vgmInfo.PlayThreadOpen)
		return 0xD0;	// Thread is already active

	vgmInfo.PauseThread = true;
	vgmInfo.ThreadNoWait = false;
	vgmInfo.ThreadPauseConfrm = false;
	vgmInfo.CloseThread = false;
	vgmInfo.ThreadPauseEnable = true;

	PlayThreadHandle = CreateThread(NULL, 0x00, &PlayingThread, this, 0x00, &PlayThreadID);
	if (PlayThreadHandle == NULL)
		return 0xC8;		// CreateThread failed
	CloseHandle(PlayThreadHandle);

	vgmInfo.PlayThreadOpen = true;
	//PauseThread = false;	is done after File Init

	return 0x00;
#else
	UINT32 RetVal;

	PauseThread = true;
	ThreadNoWait = false;
	ThreadPauseConfrm = false;
	CloseThread = false;
	ThreadPauseEnable = true;

	RetVal = pthread_create(&hPlayThread, NULL, &PlayingThread, NULL);
	if (RetVal)
		return 0xC8;		// CreateThread failed

	PlayThreadOpen = true;

	return 0x00;
#endif
}

UINT8 VGMData::StopThread(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

#ifdef WIN32
	UINT16 Cnt;
#endif

	if (!vgmInfo.PlayThreadOpen)
		return 0xD8;	// Thread is not active

#ifdef WIN32
	vgmInfo.CloseThread = true;
	for (Cnt = 0; Cnt < 100; Cnt++)
	{
		Sleep(1);
		if (vgmInfo.hPlayThread == NULL)
			break;
	}
#else
	CloseThread = true;
	pthread_join(hPlayThread, NULL);
#endif
	vgmInfo.PlayThreadOpen = false;
	vgmInfo.ThreadPauseEnable = false;

	return 0x00;
}

UINT32 VGMData::FillBuffer(WAVE_16BS* Buffer, UINT32 BufferSize)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT32 CurSmpl;
	WAVE_32BS TempBuf;
	INT32 CurMstVol;
	UINT32 RecalcStep;
	CA_LIST* CurCLst;

	//memset(Buffer, 0x00, sizeof(WAVE_16BS) * BufferSize);

	RecalcStep = vgmInfo.FadePlay ? vgmInfo.SampleRate / 44100 : 0;
	CurMstVol = RecalcFadeVolume();

	if (Buffer == NULL)
	{
		//for (CurSmpl = 0x00; CurSmpl < BufferSize; CurSmpl ++)
		//	InterpretFile(1);
		InterpretFile(BufferSize);

		if (vgmInfo.FadePlay && !vgmInfo.FadeStart)
		{
			vgmInfo.FadeStart = vgmInfo.PlayingTime;
			RecalcStep = vgmInfo.FadePlay ? vgmInfo.SampleRate / 100 : 0;
		}
		//if (RecalcStep && ! (CurSmpl % RecalcStep))
		if (RecalcStep)
			CurMstVol = RecalcFadeVolume();

		if (vgmInfo.VGMEnd)
		{
			if (vgmInfo.PauseSmpls <= BufferSize)
			{
				vgmInfo.PauseSmpls = 0;
				vgmInfo.EndPlay = true;
			}
			else
			{
				vgmInfo.PauseSmpls -= BufferSize;
			}
		}

		return BufferSize;
	}

	vgmInfo.CurChipList = (vgmInfo.VGMEnd || vgmInfo.PausePlay) ? vgmInfo.ChipListPause : vgmInfo.ChipListAll;

	for (CurSmpl = 0x00; CurSmpl < BufferSize; CurSmpl++)
	{
		InterpretFile(1);

		// Sample Structures
		//	00 - SN76496
		//	01 - YM2413
		//	02 - YM2612
		//	03 - YM2151
		//	04 - SegaPCM
		//	05 - RF5C68
		//	06 - YM2203
		//	07 - YM2608
		//	08 - YM2610/YM2610B
		//	09 - YM3812
		//	0A - YM3526
		//	0B - Y8950
		//	0C - YMF262
		//	0D - YMF278B
		//	0E - YMF271
		//	0F - YMZ280B
		//	10 - RF5C164
		//	11 - PWM
		//	12 - AY8910
		//	13 - GameBoy
		//	14 - NES APU
		//	15 - MultiPCM
		//	16 - UPD7759
		//	17 - OKIM6258
		//	18 - OKIM6295
		//	19 - K051649
		//	1A - K054539
		//	1B - HuC6280
		//	1C - C140
		//	1D - K053260
		//	1E - Pokey
		//	1F - QSound
		//	20 - YMF292/SCSP
		//	21 - WonderSwan
		//	22 - VSU
		//	23 - SAA1099
		//	24 - ES5503
		//	25 - ES5506
		//	26 - X1-010
		//	27 - C352
		//	28 - GA20
		TempBuf.Left = 0x00;
		TempBuf.Right = 0x00;
		CurCLst = vgmInfo.CurChipList;
		while (CurCLst != NULL)
		{
			if (!CurCLst->COpts->Disabled)
			{
				ResampleChipStream(CurCLst, &TempBuf, 1);
			}
			CurCLst = CurCLst->next;
		}

		// ChipData << 9 [ChipVol] >> 5 << 8 [MstVol] >> 11  ->  9-5+8-11 = <<1
		TempBuf.Left = ((TempBuf.Left >> 5) * CurMstVol) >> 11;
		TempBuf.Right = ((TempBuf.Right >> 5) * CurMstVol) >> 11;
		if (vgmInfo.SurroundSound)
			TempBuf.Right *= -1;
		Buffer[CurSmpl].Left = Limit2Short(TempBuf.Left);
		Buffer[CurSmpl].Right = Limit2Short(TempBuf.Right);

		if (vgmInfo.FadePlay && !vgmInfo.FadeStart)
		{
			vgmInfo.FadeStart = vgmInfo.PlayingTime;
			RecalcStep = vgmInfo.FadePlay ? vgmInfo.SampleRate / 100 : 0;
		}
		if (RecalcStep && !(CurSmpl % RecalcStep))
			CurMstVol = RecalcFadeVolume();

		if (vgmInfo.VGMEnd)
		{
			if (!vgmInfo.PauseSmpls)
			{
				//if (! FullBufFill)
				if (!vgmInfo.EndPlay)
				{
					vgmInfo.EndPlay = true;
					break;
				}
			}
			else //if (PauseSmpls)
			{
				vgmInfo.PauseSmpls--;
			}
		}
	}

	return CurSmpl;
}

// I recommend 11 bits as it's fast and accurate
#define FIXPNT_BITS		11
#define FIXPNT_FACT		(1 << FIXPNT_BITS)
#if (FIXPNT_BITS <= 11)
typedef UINT32	SLINT;	// 32-bit is a lot faster
#else
typedef UINT64	SLINT;
#endif
#define FIXPNT_MASK		(FIXPNT_FACT - 1)

#define getfriction(x)	((x) & FIXPNT_MASK)
#define getnfriction(x)	((FIXPNT_FACT - (x)) & FIXPNT_MASK)
#define fpi_floor(x)	((x) & ~FIXPNT_MASK)
#define fpi_ceil(x)		((x + FIXPNT_MASK) & ~FIXPNT_MASK)
#define fp2i_floor(x)	((x) / FIXPNT_FACT)
#define fp2i_ceil(x)	((x + FIXPNT_MASK) / FIXPNT_FACT)

void VGMData::ResampleChipStream(CA_LIST* CLst, WAVE_32BS* RetSample, UINT32 Length)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	CAUD_ATTR* CAA;
	CHIP_OPTS* COpts;
	INT32* CurBufL;
	INT32* CurBufR;
	INT32* StreamPnt[0x02];
	UINT32 InBase;
	UINT32 InPos;
	UINT32 InPosNext;
	UINT32 OutPos;
	UINT32 SmpFrc;	// Sample Friction
	UINT32 InPre;
	UINT32 InNow;
	SLINT InPosL;
	INT64 TempSmpL;
	INT64 TempSmpR;
	INT32 TempS32L;
	INT32 TempS32R;
	INT32 SmpCnt;	// must be signed, else I'm getting calculation errors
	INT32 CurSmpl;
	UINT64 ChipSmpRate;

	COpts = CLst->COpts;
	CAA = CLst->CAud;
	CurBufL = vgmInfo.StreamBufs[0x00];
	CurBufR = vgmInfo.StreamBufs[0x01];

	// This Do-While-Loop gets and resamples the chip output of one or more chips.
	// It's a loop to support the AY8910 paired with the YM2203/YM2608/YM2610.

	UINT32 BaseChannelIdx = 0;
	do
	{
		switch (CAA->Resampler)
		{
		case 0x00:	// old, but very fast resampler
			CAA->SmpLast = CAA->SmpNext;
			CAA->SmpP += Length;
			CAA->SmpNext = (UINT32)((UINT64)CAA->SmpP * CAA->SmpRate / vgmInfo.SampleRate);
			if (CAA->SmpLast >= CAA->SmpNext)
			{
				RetSample->Left += CAA->LSmpl.Left * CAA->Volume;
				RetSample->Right += CAA->LSmpl.Right * CAA->Volume;
			}
			else
			{
				SmpCnt = CAA->SmpNext - CAA->SmpLast;

				CAA->StreamUpdate(CAA->ChipID, vgmInfo.StreamBufs, SmpCnt, &vgmInfo.ChannelStreamBufs[BaseChannelIdx], COpts->ChnCnt);

				if (SmpCnt == 1)
				{
					RetSample->Left += CurBufL[0x00] * CAA->Volume;
					RetSample->Right += CurBufR[0x00] * CAA->Volume;
					CAA->LSmpl.Left = CurBufL[0x00];
					CAA->LSmpl.Right = CurBufR[0x00];
				}
				else if (SmpCnt == 2)
				{
					RetSample->Left += (CurBufL[0x00] + CurBufL[0x01]) * CAA->Volume >> 1;
					RetSample->Right += (CurBufR[0x00] + CurBufR[0x01]) * CAA->Volume >> 1;
					CAA->LSmpl.Left = CurBufL[0x01];
					CAA->LSmpl.Right = CurBufR[0x01];
				}
				else
				{
					TempS32L = CurBufL[0x00];
					TempS32R = CurBufR[0x00];
					for (CurSmpl = 0x01; CurSmpl < SmpCnt; CurSmpl++)
					{
						TempS32L += CurBufL[CurSmpl];
						TempS32R += CurBufR[CurSmpl];
					}
					RetSample->Left += TempS32L * CAA->Volume / SmpCnt;
					RetSample->Right += TempS32R * CAA->Volume / SmpCnt;
					CAA->LSmpl.Left = CurBufL[SmpCnt - 1];
					CAA->LSmpl.Right = CurBufR[SmpCnt - 1];
				}
			}
			break;
		case 0x01:	// Upsampling
			ChipSmpRate = CAA->SmpRate;
			InPosL = (SLINT)(FIXPNT_FACT * CAA->SmpP * ChipSmpRate / vgmInfo.SampleRate);
			InPre = (UINT32)fp2i_floor(InPosL);
			InNow = (UINT32)fp2i_ceil(InPosL);
			/*if (InNow - CAA->SmpNext >= SMPL_BUFSIZE)
			{
				fprintf(stderr, "Sample Buffer Overflow!\n");
#ifdef _DEBUG
				*(char*)NULL = 0;
#endif
				CAA->SmpLast = 0;
				CAA->SmpNext = 0;
				CAA->SmpP = 0;
				break;
			}*/

			CurBufL[0x00] = CAA->LSmpl.Left;
			CurBufR[0x00] = CAA->LSmpl.Right;
			CurBufL[0x01] = CAA->NSmpl.Left;
			CurBufR[0x01] = CAA->NSmpl.Right;
			StreamPnt[0x00] = &CurBufL[0x02];
			StreamPnt[0x01] = &CurBufR[0x02];
			CAA->StreamUpdate(CAA->ChipID, StreamPnt, InNow - CAA->SmpNext, &vgmInfo.ChannelStreamBufs[BaseChannelIdx], COpts->ChnCnt);

			InBase = FIXPNT_FACT + (UINT32)(InPosL - (SLINT)CAA->SmpNext * FIXPNT_FACT);
			/*if (fp2i_floor(InBase) >= SMPL_BUFSIZE)
			{
				fprintf(stderr, "Sample Buffer Overflow!\n");
#ifdef _DEBUG
				*(char*)NULL = 0;
#endif
				CAA->SmpLast = 0;
				CAA->SmpP = 0;
				break;
			}*/
			SmpCnt = FIXPNT_FACT;
			CAA->SmpLast = InPre;
			CAA->SmpNext = InNow;
			for (OutPos = 0x00; OutPos < Length; OutPos++)
			{
				InPos = InBase + (UINT32)(FIXPNT_FACT * OutPos * ChipSmpRate / vgmInfo.SampleRate);

				InPre = fp2i_floor(InPos);
				InNow = fp2i_ceil(InPos);
				SmpFrc = getfriction(InPos);

				// Linear interpolation
				TempSmpL = ((INT64)CurBufL[InPre] * (FIXPNT_FACT - SmpFrc)) +
					((INT64)CurBufL[InNow] * SmpFrc);
				TempSmpR = ((INT64)CurBufR[InPre] * (FIXPNT_FACT - SmpFrc)) +
					((INT64)CurBufR[InNow] * SmpFrc);
				RetSample[OutPos].Left += (INT32)(TempSmpL * CAA->Volume / SmpCnt);
				RetSample[OutPos].Right += (INT32)(TempSmpR * CAA->Volume / SmpCnt);
			}
			CAA->LSmpl.Left = CurBufL[InPre];
			CAA->LSmpl.Right = CurBufR[InPre];
			CAA->NSmpl.Left = CurBufL[InNow];
			CAA->NSmpl.Right = CurBufR[InNow];
			CAA->SmpP += Length;
			break;
		case 0x02:	// Copying
			CAA->SmpNext = CAA->SmpP * CAA->SmpRate / vgmInfo.SampleRate;
			CAA->StreamUpdate(CAA->ChipID, vgmInfo.StreamBufs, Length, &vgmInfo.ChannelStreamBufs[BaseChannelIdx], COpts->ChnCnt);

			for (OutPos = 0x00; OutPos < Length; OutPos++)
			{
				RetSample[OutPos].Left += CurBufL[OutPos] * CAA->Volume;
				RetSample[OutPos].Right += CurBufR[OutPos] * CAA->Volume;
			}
			CAA->SmpP += Length;
			CAA->SmpLast = CAA->SmpNext;
			break;
		case 0x03:	// Downsampling
			ChipSmpRate = CAA->SmpRate;
			InPosL = (SLINT)(FIXPNT_FACT * (CAA->SmpP + Length) * ChipSmpRate / vgmInfo.SampleRate);
			CAA->SmpNext = (UINT32)fp2i_ceil(InPosL);

			CurBufL[0x00] = CAA->LSmpl.Left;
			CurBufR[0x00] = CAA->LSmpl.Right;
			StreamPnt[0x00] = &CurBufL[0x01];
			StreamPnt[0x01] = &CurBufR[0x01];
			CAA->StreamUpdate(CAA->ChipID, StreamPnt, CAA->SmpNext - CAA->SmpLast, &vgmInfo.ChannelStreamBufs[BaseChannelIdx], COpts->ChnCnt);

			InPosL = (SLINT)(FIXPNT_FACT * CAA->SmpP * ChipSmpRate / vgmInfo.SampleRate);
			// I'm adding 1.0 to avoid negative indexes
			InBase = FIXPNT_FACT + (UINT32)(InPosL - (SLINT)CAA->SmpLast * FIXPNT_FACT);
			InPosNext = InBase;
			for (OutPos = 0x00; OutPos < Length; OutPos++)
			{
				//InPos = InBase + (UINT32)(FIXPNT_FACT * OutPos * ChipSmpRate / SampleRate);
				InPos = InPosNext;
				InPosNext = InBase + (UINT32)(FIXPNT_FACT * (OutPos + 1) * ChipSmpRate / vgmInfo.SampleRate);

				// first frictional Sample
				SmpFrc = getnfriction(InPos);
				if (SmpFrc)
				{
					InPre = fp2i_floor(InPos);
					TempSmpL = (INT64)CurBufL[InPre] * SmpFrc;
					TempSmpR = (INT64)CurBufR[InPre] * SmpFrc;
				}
				else
				{
					TempSmpL = TempSmpR = 0x00;
				}
				SmpCnt = SmpFrc;

				// last frictional Sample
				SmpFrc = getfriction(InPosNext);
				InPre = fp2i_floor(InPosNext);
				if (SmpFrc)
				{
					TempSmpL += (INT64)CurBufL[InPre] * SmpFrc;
					TempSmpR += (INT64)CurBufR[InPre] * SmpFrc;
					SmpCnt += SmpFrc;
				}

				// whole Samples in between
				//InPre = fp2i_floor(InPosNext);
				InNow = fp2i_ceil(InPos);
				SmpCnt += (InPre - InNow) * FIXPNT_FACT;	// this is faster
				while (InNow < InPre)
				{
					TempSmpL += (INT64)CurBufL[InNow] * FIXPNT_FACT;
					TempSmpR += (INT64)CurBufR[InNow] * FIXPNT_FACT;
					//SmpCnt ++;
					InNow++;
				}

				RetSample[OutPos].Left += (INT32)(TempSmpL * CAA->Volume / SmpCnt);
				RetSample[OutPos].Right += (INT32)(TempSmpR * CAA->Volume / SmpCnt);
			}

			CAA->LSmpl.Left = CurBufL[InPre];
			CAA->LSmpl.Right = CurBufR[InPre];
			CAA->SmpP += Length;
			CAA->SmpLast = CAA->SmpNext;
			break;
		default:
			CAA->SmpP += vgmInfo.SampleRate;
			break;	// do absolutely nothing
		}

		if (CAA->SmpLast >= CAA->SmpRate)
		{
			CAA->SmpLast -= CAA->SmpRate;
			CAA->SmpNext -= CAA->SmpRate;
			CAA->SmpP -= vgmInfo.SampleRate;
		}

		CAA = CAA->Paired;
		BaseChannelIdx += COpts->ChnCnt;
	} while (CAA != NULL);

	return;
}

INT32 VGMData::RecalcFadeVolume(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	float TempSng;

	if (vgmInfo.FadePlay)
	{
		if (!vgmInfo.FadeStart)
			vgmInfo.FadeStart = vgmInfo.PlayingTime;

		TempSng = (vgmInfo.PlayingTime - vgmInfo.FadeStart) / (float)vgmInfo.SampleRate;
		vgmInfo.MasterVol = 1.0f - TempSng / (vgmInfo.FadeTime * 0.001f);
		if (vgmInfo.MasterVol < 0.0f)
		{
			vgmInfo.MasterVol = 0.0f;
			//vgmInfo.EndPlay = true;
			vgmInfo.VGMEnd = true;
		}
		vgmInfo.FinalVol = vgmInfo.VolumeLevelM * vgmInfo.MasterVol * vgmInfo.MasterVol;
	}

	return (INT32)(0x100 * vgmInfo.FinalVol + 0.5f);
}

void VGMData::InterpretFile(UINT32 SampleCount)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT32 TempLng;
	UINT8 CurChip;

	//if (Interpreting && SampleCount == 1)
	//	return;
	while (vgmInfo.Interpreting)
		Sleep(1);

	if (vgmInfo.DacCtrlUsed && SampleCount > 1)	// handle skipping
	{
		for (CurChip = 0x00; CurChip < vgmInfo.DacCtrlUsed; CurChip++)
		{
			daccontrol_update(vgmInfo.DacCtrlUsg[CurChip], SampleCount - 1, vgmInfo.SampleRate);
		}
	}

	vgmInfo.Interpreting = true;
	if (!vgmInfo.FileMode)
		InterpretVGM(SampleCount);
#ifdef ADDITIONAL_FORMATS
	else
		InterpretOther(SampleCount);
#endif

	if (vgmInfo.DacCtrlUsed && SampleCount)
	{
		// calling this here makes "Emulating while Paused" nicer
		for (CurChip = 0x00; CurChip < vgmInfo.DacCtrlUsed; CurChip++)
		{
			daccontrol_update(vgmInfo.DacCtrlUsg[CurChip], 1, vgmInfo.SampleRate);
		}
	}

	if (vgmInfo.UseFM && vgmInfo.FadePlay)
	{
		//TempLng = PlayingTime % (SampleRate / 5);
		//if (! TempLng)
		TempLng = vgmInfo.PlayingTime / (vgmInfo.SampleRate / 5) - (vgmInfo.PlayingTime + SampleCount) / (vgmInfo.SampleRate / 5);
		if (TempLng)
			RefreshVolume();
	}
	if (vgmInfo.AutoStopSkip && SampleCount)
	{
		StopSkipping();
		vgmInfo.AutoStopSkip = false;
		vgmInfo.ResetPBTimer = true;
	}

	if (!vgmInfo.PausePlay || vgmInfo.ForceVGMExec)
		vgmInfo.VGMSmplPlayed += SampleCount;
	vgmInfo.PlayingTime += SampleCount;

	//if (FadePlay && ! FadeTime)
	//	EndPlay = true;

	vgmInfo.Interpreting = false;

	return;
}

void VGMData::Chips_GeneralActions(UINT8 Mode)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT32 AbsVol;
	//UINT16 ChipVol;
	CAUD_ATTR* CAA;
	CHIP_OPTS* COpt;
	UINT8 ChipCnt;
	UINT8 CurChip;
	UINT8 CurCSet;	// Chip Set
	UINT32 MaskVal;
	UINT32 ChipClk;
	UINT32 BaseChannelIdx;

	BaseChannelIdx = 0;
	switch (Mode)
	{
	case 0x00:	// Start Chips
		for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
		{
			CAA = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet];
			for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, CAA++)
			{
				CAA->SmpRate = 0x00;
				CAA->Volume = 0x00;
				CAA->ChipType = 0xFF;
				CAA->ChipID = CurCSet;
				CAA->Resampler = 0x00;
				CAA->StreamUpdate = &null_update;
				CAA->Paired = NULL;
			}
			CAA = vgmInfo.CA_Paired[CurCSet];
			for (CurChip = 0x00; CurChip < 0x03; CurChip++, CAA++)
			{
				CAA->SmpRate = 0x00;
				CAA->Volume = 0x00;
				CAA->ChipType = 0xFF;
				CAA->ChipID = CurCSet;
				CAA->Resampler = 0x00;
				CAA->StreamUpdate = &null_update;
				CAA->Paired = NULL;
			}
		}

		// Initialize Sound Chips
		AbsVol = 0x00;
		if (vgmInfo.VGMHead.lngHzPSG)
		{
			//ChipVol = vgmInfo.UseFM ? 0x00 : 0x80;
			sn764xx_set_emu_core(vgmInfo.ChipOpts[0x00].SN76496.EmuCore);
			vgmInfo.ChipOpts[0x01].SN76496.EmuCore = vgmInfo.ChipOpts[0x00].SN76496.EmuCore;

			ChipCnt = (vgmInfo.VGMHead.lngHzPSG & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].SN76496;
				CAA->ChipType = 0x00;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				ChipClk &= ~0x80000000;
				ChipClk |= vgmInfo.VGMHead.lngHzPSG & ((CurChip & 0x01) << 31);
				if (!vgmInfo.UseFM)
				{
					CAA->SmpRate = device_start_sn764xx(CurChip, ChipClk,
						vgmInfo.VGMHead.bytPSG_SRWidth,
						vgmInfo.VGMHead.shtPSG_Feedback,
						(vgmInfo.VGMHead.bytPSG_Flags & 0x02) >> 1,
						(vgmInfo.VGMHead.bytPSG_Flags & 0x04) >> 2,
						(vgmInfo.VGMHead.bytPSG_Flags & 0x08) >> 3,
						(vgmInfo.VGMHead.bytPSG_Flags & 0x01) >> 0,
						vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
					CAA->StreamUpdate = &sn764xx_stream_update;

					CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
					if (!CurChip || !(ChipClk & 0x80000000))
						AbsVol += CAA->Volume;
				}
				else
				{
					open_fm_option(CAA->ChipType, 0x00, ChipClk);
					open_fm_option(CAA->ChipType, 0x01, vgmInfo.VGMHead.bytPSG_SRWidth);
					open_fm_option(CAA->ChipType, 0x02, vgmInfo.VGMHead.shtPSG_Feedback);
					open_fm_option(CAA->ChipType, 0x04, (vgmInfo.VGMHead.bytPSG_Flags & 0x02) >> 1);
					open_fm_option(CAA->ChipType, 0x05, (vgmInfo.VGMHead.bytPSG_Flags & 0x04) >> 2);
					open_fm_option(CAA->ChipType, 0x06, (vgmInfo.VGMHead.bytPSG_Flags & 0x08) >> 3);
					open_fm_option(CAA->ChipType, 0x07, (vgmInfo.VGMHead.bytPSG_Flags & 0x01) >> 0);
					setup_real_fm(CAA->ChipType, CurChip);

					CAA->SmpRate = 0x00000000;
					CAA->Volume = 0x0000;
				}
			}
			if (vgmInfo.VGMHead.lngHzPSG & 0x80000000)
				ChipCnt = 0x01;
		}
		if (vgmInfo.VGMHead.lngHzYM2413)
		{
			//ChipVol = vgmInfo.UseFM ? 0x00 : 0x200/*0x155*/;
			if (!vgmInfo.UseFM)
				ym2413_set_emu_core(vgmInfo.ChipOpts[0x00].YM2413.EmuCore);
			else
				ym2413opl_set_emu_core(vgmInfo.ChipOpts[0x00].YM2413.EmuCore);
			vgmInfo.ChipOpts[0x01].YM2413.EmuCore = vgmInfo.ChipOpts[0x00].YM2413.EmuCore;

			ChipCnt = (vgmInfo.VGMHead.lngHzYM2413 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YM2413;
				CAA->ChipType = 0x01;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				if (!vgmInfo.UseFM)
				{
					CAA->SmpRate = device_start_ym2413(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
					CAA->StreamUpdate = &ym2413_stream_update;

					CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
					// WHY has this chip such a low volume???
					//AbsVol += (CAA->Volume + 1) * 3 / 4;
					AbsVol += CAA->Volume / 2;
				}
				else
				{
					setup_real_fm(CAA->ChipType, CurChip);

					CAA->SmpRate = 0x00000000;
					CAA->Volume = 0x0000;
				}
			}
		}
		if (vgmInfo.VGMHead.lngHzYM2612)
		{
			//ChipVol = 0x100;
			ym2612_set_emu_core(vgmInfo.ChipOpts[0x00].YM2612.EmuCore);
			ym2612_set_options((UINT8)vgmInfo.ChipOpts[0x00].YM2612.SpecialFlags);
			vgmInfo.ChipOpts[0x01].YM2612.EmuCore = vgmInfo.ChipOpts[0x00].YM2612.EmuCore;
			vgmInfo.ChipOpts[0x01].YM2612.SpecialFlags = vgmInfo.ChipOpts[0x00].YM2612.SpecialFlags;
			ChipCnt = (vgmInfo.VGMHead.lngHzYM2612 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YM2612;
				CAA->ChipType = 0x02;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_ym2612(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &ym2612_stream_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzYM2151)
		{
			//ChipVol = 0x100;
			ym2151_set_emu_core(vgmInfo.ChipOpts[0x00].YM2151.EmuCore);
			ChipCnt = (vgmInfo.VGMHead.lngHzYM2151 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YM2151;
				CAA->ChipType = 0x03;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_ym2151(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &ym2151_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzSPCM)
		{
			//ChipVol = 0x180;
			ChipCnt = (vgmInfo.VGMHead.lngHzSPCM & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].SegaPCM;
				CAA->ChipType = 0x04;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_segapcm(CurChip, ChipClk, vgmInfo.VGMHead.lngSPCMIntf,
					vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &SEGAPCM_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzRF5C68)
		{
			//ChipVol = 0xB0;	// that's right according to MAME, but it's almost too loud
			ChipCnt = 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].RF5C68;
				CAA->ChipType = 0x05;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_rf5c68(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &rf5c68_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzYM2203)
		{
			//ChipVol = 0x100;
			ym2203_set_ay_emu_core(vgmInfo.ChipOpts[0x00].YM2203.EmuCore);
			vgmInfo.ChipOpts[0x01].YM2203.EmuCore = vgmInfo.ChipOpts[0x00].YM2203.EmuCore;
			vgmInfo.ChipOpts[0x01].YM2203.SpecialFlags = vgmInfo.ChipOpts[0x00].YM2203.SpecialFlags;

			ChipCnt = (vgmInfo.VGMHead.lngHzYM2203 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YM2203;
				COpt = &vgmInfo.ChipOpts[CurChip].YM2203;
				CAA->ChipType = 0x06;
				CAA->Paired = &vgmInfo.CA_Paired[CurChip][0x00];
				CAA->Paired->ChipType = 0x80 | CAA->ChipType;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_ym2203(CurChip, ChipClk, COpt->SpecialFlags & 0x01,
					vgmInfo.VGMHead.bytAYFlagYM2203,
					(int*)&CAA->Paired->SmpRate,
					vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &ym2203_stream_update;
				CAA->Paired->StreamUpdate = &ym2203_stream_update_ay;
				ym2203_set_srchg_cb(CurChip, &ChangeChipSampleRate, CAA, CAA->Paired);

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				CAA->Paired->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->Paired->ChipType,
					CurChip, ChipCnt);
				AbsVol += CAA->Volume + CAA->Paired->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzYM2608)
		{
			//ChipVol = 0x80;
			ym2608_set_ay_emu_core(vgmInfo.ChipOpts[0x00].YM2608.EmuCore);
			vgmInfo.ChipOpts[0x01].YM2608.EmuCore = vgmInfo.ChipOpts[0x00].YM2608.EmuCore;
			vgmInfo.ChipOpts[0x01].YM2608.SpecialFlags = vgmInfo.ChipOpts[0x00].YM2608.SpecialFlags;

			ChipCnt = (vgmInfo.VGMHead.lngHzYM2608 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YM2608;
				COpt = &vgmInfo.ChipOpts[CurChip].YM2608;
				CAA->ChipType = 0x07;
				CAA->Paired = &vgmInfo.CA_Paired[CurChip][0x01];
				CAA->Paired->ChipType = 0x80 | CAA->ChipType;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_ym2608(CurChip, ChipClk, COpt->SpecialFlags & 0x01,
					vgmInfo.VGMHead.bytAYFlagYM2608,
					(int*)&CAA->Paired->SmpRate, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &ym2608_stream_update;
				CAA->Paired->StreamUpdate = &ym2608_stream_update_ay;
				ym2608_set_srchg_cb(CurChip, &ChangeChipSampleRate, CAA, CAA->Paired);

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				CAA->Paired->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->Paired->ChipType,
					CurChip, ChipCnt);
				AbsVol += CAA->Volume + CAA->Paired->Volume;
				//CAA->Volume = ChipVol;
				//CAA->Paired->Volume = ChipVol * 2;
			}
		}
		if (vgmInfo.VGMHead.lngHzYM2610)
		{
			//ChipVol = 0x80;
			ym2610_set_ay_emu_core(vgmInfo.ChipOpts[0x00].YM2610.EmuCore);
			vgmInfo.ChipOpts[0x01].YM2610.EmuCore = vgmInfo.ChipOpts[0x00].YM2610.EmuCore;
			vgmInfo.ChipOpts[0x01].YM2610.SpecialFlags = vgmInfo.ChipOpts[0x00].YM2610.SpecialFlags;

			ChipCnt = (vgmInfo.VGMHead.lngHzYM2610 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YM2610;
				COpt = &vgmInfo.ChipOpts[CurChip].YM2610;
				CAA->ChipType = 0x08;
				CAA->Paired = &vgmInfo.CA_Paired[CurChip][0x02];
				CAA->Paired->ChipType = 0x80 | CAA->ChipType;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_ym2610(CurChip, ChipClk, COpt->SpecialFlags & 0x01,
					(int*)&CAA->Paired->SmpRate, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = (ChipClk & 0x80000000) ? ym2610b_stream_update : ym2610_stream_update;
				CAA->Paired->StreamUpdate = &ym2610_stream_update_ay;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				CAA->Paired->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->Paired->ChipType,
					CurChip, ChipCnt);
				AbsVol += CAA->Volume + CAA->Paired->Volume;
				//CAA->Volume = ChipVol;
				//CAA->Paired->Volume = ChipVol * 2;
			}
		}
		if (vgmInfo.VGMHead.lngHzYM3812)
		{
			//ChipVol = vgmInfo.UseFM ? 0x00 : 0x100;
			ym3812_set_emu_core(vgmInfo.ChipOpts[0x00].YM3812.EmuCore);
			vgmInfo.ChipOpts[0x01].YM3812.EmuCore = vgmInfo.ChipOpts[0x00].YM3812.EmuCore;

			ChipCnt = (vgmInfo.VGMHead.lngHzYM3812 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YM3812;
				CAA->ChipType = 0x09;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				if (!vgmInfo.UseFM)
				{
					CAA->SmpRate = device_start_ym3812(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
					CAA->StreamUpdate = (ChipClk & 0x80000000) ? dual_opl2_stereo : ym3812_stream_update;
					CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
					if (!CurChip || !(ChipClk & 0x80000000))
						AbsVol += CAA->Volume * 2;
				}
				else
				{
					setup_real_fm(CAA->ChipType, CurChip);

					CAA->SmpRate = 0x00000000;
					CAA->Volume = 0x0000;
				}
			}
		}
		if (vgmInfo.VGMHead.lngHzYM3526)
		{
			//ChipVol = vgmInfo.UseFM ? 0x00 : 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzYM3526 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YM3526;
				CAA->ChipType = 0x0A;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				if (!vgmInfo.UseFM)
				{
					CAA->SmpRate = device_start_ym3526(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
					CAA->StreamUpdate = &ym3526_stream_update;

					CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
					AbsVol += CAA->Volume * 2;
				}
				else
				{
					setup_real_fm(CAA->ChipType, CurChip);

					CAA->SmpRate = 0x00000000;
					CAA->Volume = 0x0000;
				}
			}
		}
		if (vgmInfo.VGMHead.lngHzY8950)
		{
			//ChipVol = vgmInfo.UseFM ? 0x00 : 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzY8950 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].Y8950;
				CAA->ChipType = 0x0B;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				if (!vgmInfo.UseFM)
				{
					CAA->SmpRate = device_start_y8950(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
					CAA->StreamUpdate = &y8950_stream_update;

					CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
					AbsVol += CAA->Volume * 2;
				}
				else
				{
					setup_real_fm(CAA->ChipType, CurChip);

					CAA->SmpRate = 0x00000000;
					CAA->Volume = 0x0000;
				}
			}
		}
		if (vgmInfo.VGMHead.lngHzYMF262)
		{
			//ChipVol = vgmInfo.UseFM ? 0x00 : 0x100;
			ymf262_set_emu_core(vgmInfo.ChipOpts[0x00].YMF262.EmuCore);
			vgmInfo.ChipOpts[0x01].YMF262.EmuCore = vgmInfo.ChipOpts[0x00].YMF262.EmuCore;

			ChipCnt = (vgmInfo.VGMHead.lngHzYMF262 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YMF262;
				CAA->ChipType = 0x0C;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				if (!vgmInfo.UseFM)
				{
					CAA->SmpRate = device_start_ymf262(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
					CAA->StreamUpdate = &ymf262_stream_update;

					CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
					AbsVol += CAA->Volume * 2;
				}
				else
				{
					setup_real_fm(CAA->ChipType, CurChip);

					CAA->SmpRate = 0x00000000;
					CAA->Volume = 0x0000;
				}
			}
		}
		if (vgmInfo.VGMHead.lngHzYMF278B)
		{
			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzYMF278B & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YMF278B;
				CAA->ChipType = 0x0D;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_ymf278b(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &ymf278b_pcm_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;	//good as long as it only uses WaveTable Synth
			}
		}
		if (vgmInfo.VGMHead.lngHzYMF271)
		{
			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzYMF271 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YMF271;
				CAA->ChipType = 0x0E;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_ymf271(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &ymf271_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzYMZ280B)
		{
			//ChipVol = 0x98;
			ChipCnt = (vgmInfo.VGMHead.lngHzYMZ280B & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].YMZ280B;
				CAA->ChipType = 0x0F;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_ymz280b(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &ymz280b_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += (CAA->Volume * 0x20 / 0x13);
			}
		}
		if (vgmInfo.VGMHead.lngHzRF5C164)
		{
			//ChipVol = 0x80;
			ChipCnt = 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].RF5C164;
				CAA->ChipType = 0x10;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_rf5c164(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &rf5c164_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 2;
			}
		}
		if (vgmInfo.VGMHead.lngHzPWM)
		{
			//ChipVol = 0xE0;	// 0xCD
			ChipCnt = 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].PWM;
				CAA->ChipType = 0x11;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_pwm(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &pwm_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzAY8910)
		{
			//ChipVol = 0x100;
			ayxx_set_emu_core(vgmInfo.ChipOpts[0x00].AY8910.EmuCore);
			vgmInfo.ChipOpts[0x01].AY8910.EmuCore = vgmInfo.ChipOpts[0x00].AY8910.EmuCore;

			ChipCnt = (vgmInfo.VGMHead.lngHzAY8910 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].AY8910;
				CAA->ChipType = 0x12;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				if (!vgmInfo.UseFM)
				{
					CAA->SmpRate = device_start_ayxx(CurChip, ChipClk,
						vgmInfo.VGMHead.bytAYType, vgmInfo.VGMHead.bytAYFlag,
						vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
					CAA->StreamUpdate = &ayxx_stream_update;

					CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
					AbsVol += CAA->Volume * 2;
				}
				else
				{
					open_fm_option(CAA->ChipType, 0x00, ChipClk);
					open_fm_option(CAA->ChipType, 0x01, vgmInfo.VGMHead.bytAYType);
					open_fm_option(CAA->ChipType, 0x02, vgmInfo.VGMHead.bytAYFlag);
					setup_real_fm(CAA->ChipType, CurChip);

					CAA->SmpRate = 0x00000000;
					CAA->Volume = 0x0000;
				}
			}
		}
		if (vgmInfo.VGMHead.lngHzGBDMG)
		{
			//ChipVol = 0xC0;
			gameboy_sound_set_options((UINT8)vgmInfo.ChipOpts[0x00].GameBoy.SpecialFlags);
			vgmInfo.ChipOpts[0x01].GameBoy.SpecialFlags = vgmInfo.ChipOpts[0x00].GameBoy.SpecialFlags;

			ChipCnt = (vgmInfo.VGMHead.lngHzGBDMG & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].GameBoy;
				CAA->ChipType = 0x13;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_gameboy_sound(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &gameboy_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 2;
			}
		}
		if (vgmInfo.VGMHead.lngHzNESAPU)
		{
			//ChipVol = 0x100;
			nes_set_emu_core(vgmInfo.ChipOpts[0x00].NES.EmuCore);
			nes_set_options(vgmInfo.ChipOpts[0x00].NES.SpecialFlags);
			vgmInfo.ChipOpts[0x01].NES.EmuCore = vgmInfo.ChipOpts[0x00].NES.EmuCore;
			vgmInfo.ChipOpts[0x01].NES.SpecialFlags = vgmInfo.ChipOpts[0x00].NES.SpecialFlags;

			ChipCnt = (vgmInfo.VGMHead.lngHzNESAPU & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].NES;
				CAA->ChipType = 0x14;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_nes(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &nes_stream_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 2;
			}
		}
		if (vgmInfo.VGMHead.lngHzMultiPCM)
		{
			//ChipVol = 0x40;
			ChipCnt = (vgmInfo.VGMHead.lngHzMultiPCM & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].MultiPCM;
				CAA->ChipType = 0x15;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_multipcm(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &MultiPCM_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 4;
			}
		}
		if (vgmInfo.VGMHead.lngHzUPD7759)
		{
			//ChipVol = 0x11E;
			ChipCnt = (vgmInfo.VGMHead.lngHzUPD7759 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].UPD7759;
				CAA->ChipType = 0x16;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_upd7759(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &upd7759_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzOKIM6258)
		{
			//ChipVol = 0x1C0;
			okim6258_set_options(vgmInfo.ChipOpts[0x00].OKIM6258.SpecialFlags);
			vgmInfo.ChipOpts[0x01].OKIM6258.SpecialFlags = vgmInfo.ChipOpts[0x00].OKIM6258.SpecialFlags;

			ChipCnt = (vgmInfo.VGMHead.lngHzOKIM6258 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].OKIM6258;
				CAA->ChipType = 0x17;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_okim6258(CurChip, ChipClk,
					(vgmInfo.VGMHead.bytOKI6258Flags & 0x03) >> 0,
					(vgmInfo.VGMHead.bytOKI6258Flags & 0x04) >> 2,
					(vgmInfo.VGMHead.bytOKI6258Flags & 0x08) >> 3,
					vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &okim6258_update;
				okim6258_set_srchg_cb(CurChip, &ChangeChipSampleRate, CAA);

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 2;
			}
		}
		if (vgmInfo.VGMHead.lngHzOKIM6295)
		{
			/*// Use the System Tag to decide between normal and CP System volume level.
			// I know, this is very hackish, but ATM there's no better solution.
			if (VGMTag.strSystemNameE != NULL && ! wcsncmp(VGMTag.strSystemNameE, L"CP", 0x02))
				ChipVol = 110;
			else
				ChipVol = 0x100;*/
			ChipCnt = (vgmInfo.VGMHead.lngHzOKIM6295 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].OKIM6295;
				CAA->ChipType = 0x18;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_okim6295(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &okim6295_update;
				okim6295_set_srchg_cb(CurChip, &ChangeChipSampleRate, CAA);

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 2;
			}
		}
		if (vgmInfo.VGMHead.lngHzK051649)
		{
			//ChipVol = 0xA0;
			ChipCnt = (vgmInfo.VGMHead.lngHzK051649 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].K051649;
				CAA->ChipType = 0x19;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_k051649(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &k051649_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzK054539)
		{
			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzK054539 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].K054539;
				CAA->ChipType = 0x1A;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_k054539(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &k054539_update;
				k054539_init_flags(CurChip, vgmInfo.VGMHead.bytK054539Flags);

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzHuC6280)
		{
			//ChipVol = 0x100;
			c6280_set_emu_core(vgmInfo.ChipOpts[0x00].HuC6280.EmuCore);
			vgmInfo.ChipOpts[0x01].HuC6280.EmuCore = vgmInfo.ChipOpts[0x00].HuC6280.EmuCore;

			ChipCnt = (vgmInfo.VGMHead.lngHzHuC6280 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].HuC6280;
				CAA->ChipType = 0x1B;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_c6280(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &c6280_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzC140)
		{
			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzC140 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].C140;
				CAA->ChipType = 0x1C;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_c140(CurChip, ChipClk, vgmInfo.VGMHead.bytC140Type, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &c140_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzK053260)
		{
			//ChipVol = 0xB3;
			ChipCnt = (vgmInfo.VGMHead.lngHzK053260 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].K053260;
				CAA->ChipType = 0x1D;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_k053260(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &k053260_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzPokey)
		{
			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzPokey & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].Pokey;
				CAA->ChipType = 0x1E;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_pokey(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &pokey_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzQSound)
		{
			qsound_set_emu_core(vgmInfo.ChipOpts[0x00].QSound.EmuCore);
			vgmInfo.ChipOpts[0x01].QSound.EmuCore = vgmInfo.ChipOpts[0x00].QSound.EmuCore;

			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzQSound & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].QSound;
				CAA->ChipType = 0x1F;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_qsound(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &qsound_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzSCSP)
		{
			scsp_set_options((UINT8)vgmInfo.ChipOpts[0x00].SCSP.SpecialFlags);
			vgmInfo.ChipOpts[0x01].SCSP.SpecialFlags = vgmInfo.ChipOpts[0x00].SCSP.SpecialFlags;

			//ChipVol = 0x20;
			ChipCnt = (vgmInfo.VGMHead.lngHzSCSP & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].SCSP;
				CAA->ChipType = 0x20;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_scsp(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &SCSP_Update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 8;
			}
		}
		if (vgmInfo.VGMHead.lngHzWSwan)
		{
			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzWSwan & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].WSwan;
				CAA->ChipType = 0x21;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = ws_audio_init(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &ws_audio_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzVSU)
		{
			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzVSU & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].VSU;
				CAA->ChipType = 0x22;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_vsu(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &vsu_stream_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzSAA1099)
		{
			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzSAA1099 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].SAA1099;
				CAA->ChipType = 0x23;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_saa1099(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &saa1099_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzES5503)
		{
			//ChipVol = 0x40;
			ChipCnt = (vgmInfo.VGMHead.lngHzES5503 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].ES5503;
				CAA->ChipType = 0x24;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_es5503(CurChip, ChipClk, vgmInfo.VGMHead.bytES5503Chns, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &es5503_pcm_update;
				es5503_set_srchg_cb(CurChip, &ChangeChipSampleRate, CAA);

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 8;
			}
		}
		if (vgmInfo.VGMHead.lngHzES5506)
		{
			//ChipVol = 0x20;
			ChipCnt = (vgmInfo.VGMHead.lngHzES5506 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].ES5506;
				CAA->ChipType = 0x25;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_es5506(CurChip, ChipClk, vgmInfo.VGMHead.bytES5506Chns, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &es5506_update;
				es5506_set_srchg_cb(CurChip, &ChangeChipSampleRate, CAA);

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 16;
			}
		}
		if (vgmInfo.VGMHead.lngHzX1_010)
		{
			//ChipVol = 0x100;
			ChipCnt = (vgmInfo.VGMHead.lngHzX1_010 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].X1_010;
				CAA->ChipType = 0x26;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_x1_010(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &seta_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}
		if (vgmInfo.VGMHead.lngHzC352)
		{
			c352_set_options((UINT8)vgmInfo.ChipOpts[0x00].C352.SpecialFlags);
			vgmInfo.ChipOpts[0x01].C352.SpecialFlags = vgmInfo.ChipOpts[0x00].C352.SpecialFlags;

			//ChipVol = 0x40;
			ChipCnt = (vgmInfo.VGMHead.lngHzC352 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].C352;
				CAA->ChipType = 0x27;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_c352(CurChip, ChipClk, vgmInfo.VGMHead.bytC352ClkDiv * 4, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &c352_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume * 8;
			}
		}
		if (vgmInfo.VGMHead.lngHzGA20)
		{
			//ChipVol = 0x280;
			ChipCnt = (vgmInfo.VGMHead.lngHzGA20 & 0x40000000) ? 0x02 : 0x01;
			for (CurChip = 0x00; CurChip < ChipCnt; CurChip++)
			{
				CAA = &vgmInfo.ChipAudio[CurChip].GA20;
				CAA->ChipType = 0x28;

				ChipClk = GetChipClock(&vgmInfo.VGMHead, (CurChip << 7) | CAA->ChipType, NULL);
				CAA->SmpRate = device_start_iremga20(CurChip, ChipClk, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
				CAA->StreamUpdate = &IremGA20_update;

				CAA->Volume = GetChipVolume(&vgmInfo.VGMHead, CAA->ChipType, CurChip, ChipCnt);
				AbsVol += CAA->Volume;
			}
		}

		// Initialize DAC Control and PCM Bank
		vgmInfo.DacCtrlUsed = 0x00;
		//memset(DacCtrlUsg, 0x00, 0x01 * 0xFF);
		for (CurChip = 0x00; CurChip < 0xFF; CurChip++)
		{
			vgmInfo.DacCtrl[CurChip].Enable = false;
		}
		//memset(vgmInfo.DacCtrl, 0x00, sizeof(DACCTRL_DATA) * 0xFF);

		memset(vgmInfo.PCMBank, 0x00, sizeof(VGM_PCM_BANK) * PCM_BANK_COUNT);
		memset(&vgmInfo.PCMTbl, 0x00, sizeof(PCMBANK_TBL));

		// Reset chips
		Chips_GeneralActions(0x01);

		while (AbsVol < 0x200 && AbsVol)
		{
			for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
			{
				CAA = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet];
				for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, CAA++)
					CAA->Volume *= 2;
				CAA = vgmInfo.CA_Paired[CurCSet];
				for (CurChip = 0x00; CurChip < 0x03; CurChip++, CAA++)
					CAA->Volume *= 2;
			}
			AbsVol *= 2;
		}
		while (AbsVol > 0x300)
		{
			for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
			{
				CAA = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet];
				for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, CAA++)
					CAA->Volume /= 2;
				CAA = vgmInfo.CA_Paired[CurCSet];
				for (CurChip = 0x00; CurChip < 0x03; CurChip++, CAA++)
					CAA->Volume /= 2;
			}
			AbsVol /= 2;
		}

		// Initialize Resampler
		for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
		{
			for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, CAA++)
			{
				SetupResampler(CurCSet, CurChip, BaseChannelIdx, false);
			}

			for (CurChip = 0x00; CurChip < 0x03; CurChip++, CAA++)
			{
				SetupResampler(CurCSet, CurChip, BaseChannelIdx, true);
			}
		}

		GeneralChipLists();
		break;
	case 0x01:	// Reset chips
		for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
		{

			CAA = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet];
			for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, CAA++)
			{
				if (CAA->ChipType == 0xFF)	// chip unused
					continue;
				else if (CAA->ChipType == 0x00 && !vgmInfo.UseFM)
					device_reset_sn764xx(CurCSet);
				else if (CAA->ChipType == 0x01 && !vgmInfo.UseFM)
					device_reset_ym2413(CurCSet);
				else if (CAA->ChipType == 0x02)
					device_reset_ym2612(CurCSet);
				else if (CAA->ChipType == 0x03)
					device_reset_ym2151(CurCSet);
				else if (CAA->ChipType == 0x04)
					device_reset_segapcm(CurCSet);
				else if (CAA->ChipType == 0x05)
					device_reset_rf5c68(CurCSet);
				else if (CAA->ChipType == 0x06)
					device_reset_ym2203(CurCSet);
				else if (CAA->ChipType == 0x07)
					device_reset_ym2608(CurCSet);
				else if (CAA->ChipType == 0x08)
					device_reset_ym2610(CurCSet);
				else if (CAA->ChipType == 0x09)
				{
					if (!vgmInfo.UseFM)
					{
						device_reset_ym3812(CurCSet);
					}
					if (vgmInfo.FileMode == 0x01)
					{
						chip_reg_write(0x09, CurCSet, 0x00, 0x01, 0x20);	// Enable Waveform Select
						chip_reg_write(0x09, CurCSet, 0x00, 0xBD, 0xC0);	// Disable Rhythm Mode
					}
				}
				else if (CAA->ChipType == 0x0A && !vgmInfo.UseFM)
					device_reset_ym3526(CurCSet);
				else if (CAA->ChipType == 0x0B && !vgmInfo.UseFM)
					device_reset_y8950(CurCSet);
				else if (CAA->ChipType == 0x0C)
				{
					if (!vgmInfo.UseFM)
					{
						device_reset_ymf262(CurCSet);
					}
					if (vgmInfo.FileMode >= 0x01)
					{
						chip_reg_write(0x0C, CurCSet, 0x01, 0x05, 0x01);	// Enable OPL3-Mode
						chip_reg_write(0x0C, CurCSet, 0x00, 0xBD, 0xC0);	// Disable Rhythm Mode
						chip_reg_write(0x0C, CurCSet, 0x01, 0x04, 0x00);	// Disable 4-Op-Mode
					}
				}
				else if (CAA->ChipType == 0x0D)
					device_reset_ymf278b(CurCSet);
				else if (CAA->ChipType == 0x0E)
					device_reset_ymf271(CurCSet);
				else if (CAA->ChipType == 0x0F)
					device_reset_ymz280b(CurCSet);
				else if (CAA->ChipType == 0x10)
					device_reset_rf5c164(CurCSet);
				else if (CAA->ChipType == 0x11)
					device_reset_pwm(CurCSet);
				else if (CAA->ChipType == 0x12 && !vgmInfo.UseFM)
					device_reset_ayxx(CurCSet);
				else if (CAA->ChipType == 0x13)
					device_reset_gameboy_sound(CurCSet);
				else if (CAA->ChipType == 0x14)
					device_reset_nes(CurCSet);
				else if (CAA->ChipType == 0x15)
					device_reset_multipcm(CurCSet);
				else if (CAA->ChipType == 0x16)
					device_reset_upd7759(CurCSet);
				else if (CAA->ChipType == 0x17)
					device_reset_okim6258(CurCSet);
				else if (CAA->ChipType == 0x18)
					device_reset_okim6295(CurCSet);
				else if (CAA->ChipType == 0x19)
					device_reset_k051649(CurCSet);
				else if (CAA->ChipType == 0x1A)
					device_reset_k054539(CurCSet);
				else if (CAA->ChipType == 0x1B)
					device_reset_c6280(CurCSet);
				else if (CAA->ChipType == 0x1C)
					device_reset_c140(CurCSet);
				else if (CAA->ChipType == 0x1D)
					device_reset_k053260(CurCSet);
				else if (CAA->ChipType == 0x1E)
					device_reset_pokey(CurCSet);
				else if (CAA->ChipType == 0x1F)
					device_reset_qsound(CurCSet);
				else if (CAA->ChipType == 0x20)
					device_reset_scsp(CurCSet);
				else if (CAA->ChipType == 0x21)
					ws_audio_reset(CurCSet);
				else if (CAA->ChipType == 0x22)
					device_reset_vsu(CurCSet);
				else if (CAA->ChipType == 0x23)
					device_reset_saa1099(CurCSet);
				else if (CAA->ChipType == 0x24)
					device_reset_es5503(CurCSet);
				else if (CAA->ChipType == 0x25)
					device_reset_es5506(CurCSet);
				else if (CAA->ChipType == 0x26)
					device_reset_x1_010(CurCSet);
				else if (CAA->ChipType == 0x27)
					device_reset_c352(CurCSet);
				else if (CAA->ChipType == 0x28)
					device_reset_iremga20(CurCSet);
			}	// end for CurChip

		}	// end for CurCSet

		Chips_GeneralActions(0x10);	// set muting mask
		Chips_GeneralActions(0x20);	// set panning

		for (CurChip = 0x00; CurChip < vgmInfo.DacCtrlUsed; CurChip++)
		{
			CurCSet = vgmInfo.DacCtrlUsg[CurChip];
			device_reset_daccontrol(CurCSet);
			//vgmInfo.DacCtrl[CurCSet].Enable = false;
		}
		//vgmInfo.DacCtrlUsed = 0x00;
		//memset(DacCtrlUsg, 0x00, 0x01 * 0xFF);

		for (CurChip = 0x00; CurChip < PCM_BANK_COUNT; CurChip++)
		{
			// reset PCM Bank, but not the data
			// (this way I don't need to decompress the data again when restarting)
			vgmInfo.PCMBank[CurChip].DataPos = 0x00000000;
			vgmInfo.PCMBank[CurChip].BnkPos = 0x00000000;
		}
		vgmInfo.PCMTbl.EntryCount = 0x00;
		break;
	case 0x02:	// Stop chips
		for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
		{

			CAA = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet];
			for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, CAA++)
			{
				if (CAA->ChipType == 0xFF)	// chip unused
					continue;
				else if (CAA->ChipType == 0x00 && !vgmInfo.UseFM)
					device_stop_sn764xx(CurCSet);
				else if (CAA->ChipType == 0x01 && !vgmInfo.UseFM)
					device_stop_ym2413(CurCSet);
				else if (CAA->ChipType == 0x02)
					device_stop_ym2612(CurCSet);
				else if (CAA->ChipType == 0x03)
					device_stop_ym2151(CurCSet);
				else if (CAA->ChipType == 0x04)
					device_stop_segapcm(CurCSet);
				else if (CAA->ChipType == 0x05)
					device_stop_rf5c68(CurCSet);
				else if (CAA->ChipType == 0x06)
					device_stop_ym2203(CurCSet);
				else if (CAA->ChipType == 0x07)
					device_stop_ym2608(CurCSet);
				else if (CAA->ChipType == 0x08)
					device_stop_ym2610(CurCSet);
				else if (CAA->ChipType == 0x09 && !vgmInfo.UseFM)
					device_stop_ym3812(CurCSet);
				else if (CAA->ChipType == 0x0A && !vgmInfo.UseFM)
					device_stop_ym3526(CurCSet);
				else if (CAA->ChipType == 0x0B && !vgmInfo.UseFM)
					device_stop_y8950(CurCSet);
				else if (CAA->ChipType == 0x0C && !vgmInfo.UseFM)
					device_stop_ymf262(CurCSet);
				else if (CAA->ChipType == 0x0D)
					device_stop_ymf278b(CurCSet);
				else if (CAA->ChipType == 0x0E)
					device_stop_ymf271(CurCSet);
				else if (CAA->ChipType == 0x0F)
					device_stop_ymz280b(CurCSet);
				else if (CAA->ChipType == 0x10)
					device_stop_rf5c164(CurCSet);
				else if (CAA->ChipType == 0x11)
					device_stop_pwm(CurCSet);
				else if (CAA->ChipType == 0x12 && !vgmInfo.UseFM)
					device_stop_ayxx(CurCSet);
				else if (CAA->ChipType == 0x13)
					device_stop_gameboy_sound(CurCSet);
				else if (CAA->ChipType == 0x14)
					device_stop_nes(CurCSet);
				else if (CAA->ChipType == 0x15)
					device_stop_multipcm(CurCSet);
				else if (CAA->ChipType == 0x16)
					device_stop_upd7759(CurCSet);
				else if (CAA->ChipType == 0x17)
					device_stop_okim6258(CurCSet);
				else if (CAA->ChipType == 0x18)
					device_stop_okim6295(CurCSet);
				else if (CAA->ChipType == 0x19)
					device_stop_k051649(CurCSet);
				else if (CAA->ChipType == 0x1A)
					device_stop_k054539(CurCSet);
				else if (CAA->ChipType == 0x1B)
					device_stop_c6280(CurCSet);
				else if (CAA->ChipType == 0x1C)
					device_stop_c140(CurCSet);
				else if (CAA->ChipType == 0x1D)
					device_stop_k053260(CurCSet);
				else if (CAA->ChipType == 0x1E)
					device_stop_pokey(CurCSet);
				else if (CAA->ChipType == 0x1F)
					device_stop_qsound(CurCSet);
				else if (CAA->ChipType == 0x20)
					device_stop_scsp(CurCSet);
				else if (CAA->ChipType == 0x21)
					ws_audio_done(CurCSet);
				else if (CAA->ChipType == 0x22)
					device_stop_vsu(CurCSet);
				else if (CAA->ChipType == 0x23)
					device_stop_saa1099(CurCSet);
				else if (CAA->ChipType == 0x24)
					device_stop_es5503(CurCSet);
				else if (CAA->ChipType == 0x25)
					device_stop_es5506(CurCSet);
				else if (CAA->ChipType == 0x26)
					device_stop_x1_010(CurCSet);
				else if (CAA->ChipType == 0x27)
					device_stop_c352(CurCSet);
				else if (CAA->ChipType == 0x28)
					device_stop_iremga20(CurCSet);

				CAA->ChipType = 0xFF;	// mark as "unused"
			}	// end for CurChip

		}	// end for CurCSet

		for (CurChip = 0x00; CurChip < vgmInfo.DacCtrlUsed; CurChip++)
		{
			CurCSet = vgmInfo.DacCtrlUsg[CurChip];
			device_stop_daccontrol(CurCSet);
			vgmInfo.DacCtrl[CurCSet].Enable = false;
		}
		vgmInfo.DacCtrlUsed = 0x00;

		for (CurChip = 0x00; CurChip < PCM_BANK_COUNT; CurChip++)
		{
			::free(vgmInfo.PCMBank[CurChip].Bank);
			::free(vgmInfo.PCMBank[CurChip].Data);
		}
		//memset(vgmInfo.PCMBank, 0x00, sizeof(VGM_PCM_BANK) * PCM_BANK_COUNT);
		::free(vgmInfo.PCMTbl.Entries);
		//memset(&vgmInfo.PCMTbl, 0x00, sizeof(PCMBANK_TBL));
		break;
	case 0x10:	// Set Muting Mask
		for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
		{

			CAA = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet];
			for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, CAA++)
			{
				if (CAA->ChipType == 0xFF)	// chip unused
					continue;
				else if (CAA->ChipType == 0x00 && !vgmInfo.UseFM)
					sn764xx_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].SN76496.ChnMute1);
				else if (CAA->ChipType == 0x01 && !vgmInfo.UseFM)
					ym2413_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YM2413.ChnMute1);
				else if (CAA->ChipType == 0x02)
					ym2612_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YM2612.ChnMute1);
				else if (CAA->ChipType == 0x03)
					ym2151_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YM2151.ChnMute1);
				else if (CAA->ChipType == 0x04)
					segapcm_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].SegaPCM.ChnMute1);
				else if (CAA->ChipType == 0x05)
					rf5c68_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].RF5C68.ChnMute1);
				else if (CAA->ChipType == 0x06)
					ym2203_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YM2203.ChnMute1,
						vgmInfo.ChipOpts[CurCSet].YM2203.ChnMute3);
				else if (CAA->ChipType == 0x07)
				{
					MaskVal = (vgmInfo.ChipOpts[CurCSet].YM2608.ChnMute1 & 0x3F) << 0;
					MaskVal |= (vgmInfo.ChipOpts[CurCSet].YM2608.ChnMute2 & 0x7F) << 6;
					ym2608_set_mute_mask(CurCSet, MaskVal, vgmInfo.ChipOpts[CurCSet].YM2608.ChnMute3);
				}
				else if (CAA->ChipType == 0x08)
				{
					MaskVal = (vgmInfo.ChipOpts[CurCSet].YM2610.ChnMute1 & 0x3F) << 0;
					MaskVal |= (vgmInfo.ChipOpts[CurCSet].YM2610.ChnMute2 & 0x7F) << 6;
					ym2610_set_mute_mask(CurCSet, MaskVal, vgmInfo.ChipOpts[CurCSet].YM2610.ChnMute3);
				}
				else if (CAA->ChipType == 0x09 && !vgmInfo.UseFM)
					ym3812_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YM3812.ChnMute1);
				else if (CAA->ChipType == 0x0A && !vgmInfo.UseFM)
					ym3526_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YM3526.ChnMute1);
				else if (CAA->ChipType == 0x0B && !vgmInfo.UseFM)
					y8950_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].Y8950.ChnMute1);
				else if (CAA->ChipType == 0x0C && !vgmInfo.UseFM)
					ymf262_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YMF262.ChnMute1);
				else if (CAA->ChipType == 0x0D)
					ymf278b_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YMF278B.ChnMute1,
						vgmInfo.ChipOpts[CurCSet].YMF278B.ChnMute2);
				else if (CAA->ChipType == 0x0E)
					ymf271_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YMF271.ChnMute1);
				else if (CAA->ChipType == 0x0F)
					ymz280b_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].YMZ280B.ChnMute1);
				else if (CAA->ChipType == 0x10)
					rf5c164_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].RF5C164.ChnMute1);
				else if (CAA->ChipType == 0x11)
					;	// PWM - nothing to mute
				else if (CAA->ChipType == 0x12 && !vgmInfo.UseFM)
					ayxx_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].AY8910.ChnMute1);
				else if (CAA->ChipType == 0x13)
					gameboy_sound_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].GameBoy.ChnMute1);
				else if (CAA->ChipType == 0x14)
					nes_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].NES.ChnMute1);
				else if (CAA->ChipType == 0x15)
					multipcm_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].MultiPCM.ChnMute1);
				else if (CAA->ChipType == 0x16)
					;	// UPD7759 - nothing to mute
				else if (CAA->ChipType == 0x17)
					;	// OKIM6258 - nothing to mute
				else if (CAA->ChipType == 0x18)
					okim6295_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].OKIM6295.ChnMute1);
				else if (CAA->ChipType == 0x19)
					k051649_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].K051649.ChnMute1);
				else if (CAA->ChipType == 0x1A)
					k054539_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].K054539.ChnMute1);
				else if (CAA->ChipType == 0x1B)
					c6280_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].HuC6280.ChnMute1);
				else if (CAA->ChipType == 0x1C)
					c140_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].C140.ChnMute1);
				else if (CAA->ChipType == 0x1D)
					k053260_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].K053260.ChnMute1);
				else if (CAA->ChipType == 0x1E)
					pokey_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].Pokey.ChnMute1);
				else if (CAA->ChipType == 0x1F)
					qsound_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].QSound.ChnMute1);
				else if (CAA->ChipType == 0x20)
					scsp_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].SCSP.ChnMute1);
				else if (CAA->ChipType == 0x21)
					ws_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].WSwan.ChnMute1);
				else if (CAA->ChipType == 0x22)
					vsu_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].VSU.ChnMute1);
				else if (CAA->ChipType == 0x23)
					saa1099_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].SAA1099.ChnMute1);
				else if (CAA->ChipType == 0x24)
					es5503_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].ES5503.ChnMute1);
				else if (CAA->ChipType == 0x25)
					es5506_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].ES5506.ChnMute1);
				else if (CAA->ChipType == 0x26)
					x1_010_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].X1_010.ChnMute1);
				else if (CAA->ChipType == 0x27)
					c352_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].C352.ChnMute1);
				else if (CAA->ChipType == 0x28)
					iremga20_set_mute_mask(CurCSet, vgmInfo.ChipOpts[CurCSet].GA20.ChnMute1);
			}	// end for CurChip

		}	// end for CurCSet
		break;
	case 0x20:	// Set Panning
		for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
		{

			CAA = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet];
			for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++, CAA++)
			{
				if (CAA->ChipType == 0xFF)	// chip unused
					continue;
				else if (CAA->ChipType == 0x00 && !vgmInfo.UseFM)
					sn764xx_set_panning(CurCSet, vgmInfo.ChipOpts[CurCSet].SN76496.Panning);
				else if (CAA->ChipType == 0x01 && !vgmInfo.UseFM)
					ym2413_set_panning(CurCSet, vgmInfo.ChipOpts[CurCSet].YM2413.Panning);
			}	// end for CurChip

		}	// end for CurCSet
		break;
	}

	return;
}

UINT16 VGMData::GetChipVolume(VGM_HEADER* FileHead, UINT8 ChipID, UINT8 ChipNum, UINT8 ChipCnt)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	// ChipID: ID of Chip
	//		Bit 7 - Is Paired Chip
	// ChipNum: chip number (0 - first chip, 1 - second chip)
	// ChipCnt: chip volume divider (number of used chips)
	const UINT16 CHIP_VOLS[CHIP_COUNT] =
	{ 0x80, 0x200/*0x155*/, 0x100, 0x100, 0x180, 0xB0, 0x100, 0x80,	// 00-07
		0x80, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x98,			// 08-0F
		0x80, 0xE0/*0xCD*/, 0x100, 0xC0, 0x100, 0x40, 0x11E, 0x1C0,		// 10-17
		0x100/*110*/, 0xA0, 0x100, 0x100, 0x100, 0xB3, 0x100, 0x100,	// 18-1F
		0x20, 0x100, 0x100, 0x100, 0x40, 0x20, 0x100, 0x40,			// 20-27
		0x280 };
	UINT16 Volume;
	UINT8 CurChp;
	VGMX_CHP_EXTRA16* TempCX;
	VGMX_CHIP_DATA16* TempCD;

	Volume = CHIP_VOLS[ChipID & 0x7F];
	switch (ChipID)
	{
	case 0x00:	// SN76496
		// if T6W28, set Volume Divider to 01
		if (GetChipClock(&vgmInfo.VGMHead, (ChipID << 7) | ChipID, NULL) & 0x80000000)
		{
			// The T6W28 consists of 2 "half" chips.
			ChipNum = 0x01;
			ChipCnt = 0x01;
		}
		break;
	case 0x18:	// OKIM6295
		// CP System 1 patch
		if (vgmInfo.VGMTag.strSystemNameE != NULL && !wcsncmp(vgmInfo.VGMTag.strSystemNameE, L"CP", 0x02))
			Volume = 110;
		break;
	case 0x86:	// YM2203's AY
		Volume /= 2;
		break;
	case 0x87:	// YM2608's AY
		// The YM2608 outputs twice as loud as the YM2203 here.
		//Volume *= 1;
		break;
	case 0x88:	// YM2610's AY
		//Volume *= 1;
		break;
	}
	if (ChipCnt > 1)
		Volume /= ChipCnt;

	TempCX = &vgmInfo.VGMH_Extra.Volumes;
	TempCD = TempCX->CCData;
	for (CurChp = 0x00; CurChp < TempCX->ChipCnt; CurChp++, TempCD++)
	{
		if (TempCD->Type == ChipID && (TempCD->Flags & 0x01) == ChipNum)
		{
			// Bit 15 - absolute/relative volume
			//	0 - absolute
			//	1 - relative (0x0100 = 1.0, 0x80 = 0.5, etc.)
			if (TempCD->Data & 0x8000)
				Volume = (Volume * (TempCD->Data & 0x7FFF) + 0x80) >> 8;
			else
			{
				Volume = TempCD->Data;
				if ((ChipID & 0x80) && vgmInfo.DoubleSSGVol)
					Volume *= 2;
			}
			break;
		}
	}

	return Volume;
}

void VGMData::GeneralChipLists(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	// Generate Chip List for playback loop
	UINT16 CurBufIdx;
	CA_LIST* CLstOld;
	CA_LIST* CLst;
	CA_LIST* CurLst;
	UINT8 CurChip;
	UINT8 CurCSet;
	CAUD_ATTR* CAA;

	vgmInfo.ChipListAll = NULL;
	vgmInfo.ChipListPause = NULL;
	//ChipListOpt = NULL;

	// generate list of all chips that are used in the current VGM
	CurBufIdx = 0x00;
	CLstOld = NULL;
	for (CurChip = 0x00; CurChip < CHIP_COUNT; CurChip++)
	{
		for (CurCSet = 0x00; CurCSet < 0x02; CurCSet++)
		{
			CAA = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet] + CurChip;
			if (CAA->ChipType != 0xFF)
			{
				CLst = &vgmInfo.ChipListBuffer[CurBufIdx];
				CurBufIdx++;
				if (CLstOld == NULL)
					vgmInfo.ChipListAll = CLst;
				else
					CLstOld->next = CLst;

				CLst->CAud = CAA;
				CLst->COpts = (CHIP_OPTS*)&vgmInfo.ChipOpts[CurCSet] + CurChip;
				CLstOld = CLst;
			}
		}
	}
	if (CLstOld != NULL)
		CLstOld->next = NULL;

	// Go through the chip list and copy all chips to a new list, except for a few
	// selected ones.
	CLstOld = NULL;
	CurLst = vgmInfo.ChipListAll;
	while (CurLst != NULL)
	{
		// don't emulate the RF5Cxx chips when paused+emulated
		if (CurLst->CAud->ChipType != 0x05 && CurLst->CAud->ChipType != 0x10)
		{
			CLst = &vgmInfo.ChipListBuffer[CurBufIdx];
			CurBufIdx++;
			if (CLstOld == NULL)
				vgmInfo.ChipListPause = CLst;
			else
				CLstOld->next = CLst;

			*CLst = *CurLst;
			CLstOld = CLst;
		}
		CurLst = CurLst->next;
	}
	if (CLstOld != NULL)
		CLstOld->next = NULL;

	return;
}

void VGMData::SetupResampler(UINT32 CurCSet, UINT32 CurChip, UINT32& BaseChannelIdx, bool UsePairedChip)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	CAUD_ATTR* CAA;
	
	if (!UsePairedChip)
		CAA = (CAUD_ATTR*)&vgmInfo.ChipAudio[CurCSet];
	else
		CAA = vgmInfo.CA_Paired[CurCSet];

	CAA += CurChip;

	if (!CAA->SmpRate)
	{
		CAA->Resampler = 0xFF;
		return;
	}


	CHIP_OPTS* COpt = (CHIP_OPTS*)(&vgmInfo.ChipOpts[CAA->ChipType]);
	int ChnCnt = CHN_COUNT[CAA->ChipType];
	//int ChnCnt = COpt->ChnCnt;

	if (CAA->SmpRate < vgmInfo.SampleRate)
		CAA->Resampler = 0x01;
	else if (CAA->SmpRate == vgmInfo.SampleRate)
		CAA->Resampler = 0x02;
	else if (CAA->SmpRate > vgmInfo.SampleRate)
		CAA->Resampler = 0x03;
	if (CAA->Resampler == 0x01 || CAA->Resampler == 0x03)
	{
		if (vgmInfo.ResampleMode == 0x02 || (vgmInfo.ResampleMode == 0x01 && CAA->Resampler == 0x03))
			CAA->Resampler = 0x00;
	}

	CAA->SmpP = 0x00;
	CAA->SmpLast = 0x00;
	CAA->SmpNext = 0x00;
	CAA->LSmpl.Left = 0x00;
	CAA->LSmpl.Right = 0x00;
	if (CAA->Resampler == 0x01)
	{
		// Pregenerate first Sample (the upsampler is always one too late)
		CAA->StreamUpdate(CAA->ChipID, vgmInfo.StreamBufs, 1, &vgmInfo.ChannelStreamBufs[BaseChannelIdx], ChnCnt);
		CAA->NSmpl.Left = vgmInfo.StreamBufs[0x00][0x00];
		CAA->NSmpl.Right = vgmInfo.StreamBufs[0x01][0x00];
	}
	else
	{
		CAA->NSmpl.Left = 0x00;
		CAA->NSmpl.Right = 0x00;
	}

	BaseChannelIdx += ChnCnt;

	return;
}

#define CHIP_CHECK(name)	(vgmInfo.ChipAudio[CurChip].name.ChipType != 0xFF)
void VGMData::InterpretVGM(UINT32 SampleCount)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	INT32 SmplPlayed;
	UINT8 Command;
	UINT8 TempByt;
	UINT16 TempSht;
	UINT32 TempLng;
	VGM_PCM_BANK* TempPCM;
	VGM_PCM_DATA* TempBnk;
	UINT32 ROMSize;
	UINT32 DataStart;
	UINT32 DataLen;
	const UINT8* ROMData;
	UINT8 CurChip;
	const UINT8* VGMPnt;

	if (vgmInfo.VGMEnd)
		return;
	if (vgmInfo.PausePlay && !vgmInfo.ForceVGMExec)
		return;

	SmplPlayed = SamplePbk2VGM_I(vgmInfo.VGMSmplPlayed + SampleCount);
	while (vgmInfo.VGMSmplPos <= SmplPlayed)
	{
		Command = vgmInfo.VGMData[vgmInfo.VGMPos + 0x00];
		if (Command >= 0x70 && Command <= 0x8F)
		{
			switch (Command & 0xF0)
			{
			case 0x70:
				vgmInfo.VGMSmplPos += (Command & 0x0F) + 0x01;
				break;
			case 0x80:
				TempByt = GetDACFromPCMBank();
				if (vgmInfo.VGMHead.lngHzYM2612)
				{
					chip_reg_write(0x02, 0x00, 0x00, 0x2A, TempByt);
				}
				vgmInfo.VGMSmplPos += (Command & 0x0F);
				break;
			}
			vgmInfo.VGMPos += 0x01;
		}
		else
		{
			VGMPnt = &vgmInfo.VGMData[vgmInfo.VGMPos];

			// Cheat Mode (to use 2 instances of 1 chip)
			CurChip = 0x00;
			switch (Command)
			{
			case 0x30:
				if (vgmInfo.VGMHead.lngHzPSG & 0x40000000)
				{
					Command += 0x20;
					CurChip = 0x01;
				}
				break;
			case 0x3F:
				if (vgmInfo.VGMHead.lngHzPSG & 0x40000000)
				{
					Command += 0x10;
					CurChip = 0x01;
				}
				break;
			case 0xA1:
				if (vgmInfo.VGMHead.lngHzYM2413 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xA2:
			case 0xA3:
				if (vgmInfo.VGMHead.lngHzYM2612 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xA4:
				if (vgmInfo.VGMHead.lngHzYM2151 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xA5:
				if (vgmInfo.VGMHead.lngHzYM2203 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xA6:
			case 0xA7:
				if (vgmInfo.VGMHead.lngHzYM2608 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xA8:
			case 0xA9:
				if (vgmInfo.VGMHead.lngHzYM2610 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xAA:
				if (vgmInfo.VGMHead.lngHzYM3812 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xAB:
				if (vgmInfo.VGMHead.lngHzYM3526 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xAC:
				if (vgmInfo.VGMHead.lngHzY8950 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xAE:
			case 0xAF:
				if (vgmInfo.VGMHead.lngHzYMF262 & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			case 0xAD:
				if (vgmInfo.VGMHead.lngHzYMZ280B & 0x40000000)
				{
					Command -= 0x50;
					CurChip = 0x01;
				}
				break;
			}

			switch (Command)
			{
			case 0x66:	// End Of File
				if (vgmInfo.VGMHead.lngLoopOffset)
				{
					vgmInfo.VGMPos = vgmInfo.VGMHead.lngLoopOffset;
					vgmInfo.VGMSmplPos -= vgmInfo.VGMHead.lngLoopSamples;
					vgmInfo.VGMSmplPlayed -= SampleVGM2Pbk_I(vgmInfo.VGMHead.lngLoopSamples);
					SmplPlayed = SamplePbk2VGM_I(vgmInfo.VGMSmplPlayed + SampleCount);
					vgmInfo.VGMCurLoop++;

					if (vgmInfo.VGMMaxLoopM && vgmInfo.VGMCurLoop >= vgmInfo.VGMMaxLoopM)
					{
#ifndef CONSOLE_MODE
						if (!vgmInfo.FadePlay)
						{
							vgmInfo.FadeStart = SampleVGM2Pbk_I(vgmInfo.VGMHead.lngTotalSamples +
								(vgmInfo.VGMCurLoop - 1) * vgmInfo.VGMHead.lngLoopSamples);
						}
#endif
						vgmInfo.FadePlay = true;
					}
					if (vgmInfo.FadePlay && !vgmInfo.FadeTime)
						vgmInfo.VGMEnd = true;
			}
				else
				{
					if (vgmInfo.VGMHead.lngTotalSamples != (UINT32)vgmInfo.VGMSmplPos)
					{
#ifdef CONSOLE_MODE
						fprintf(stderr, "Warning! Header Samples: %u\t Counted Samples: %u\n",
							vgmInfo.VGMHead.lngTotalSamples, vgmInfo.VGMSmplPos);
						vgmInfo.ErrorHappened = true;
#endif
						vgmInfo.VGMHead.lngTotalSamples = vgmInfo.VGMSmplPos;
					}

					if (vgmInfo.HardStopOldVGMs)
					{
						if (vgmInfo.VGMHead.lngVersion < 0x150 ||
							(vgmInfo.VGMHead.lngVersion == 0x150 && vgmInfo.HardStopOldVGMs == 0x02))
							Chips_GeneralActions(0x01); // reset all chips, for instant silence
					}
					vgmInfo.VGMEnd = true;
					break;
				}
				break;
			case 0x62:	// 1/60s delay
				vgmInfo.VGMSmplPos += 735;
				vgmInfo.VGMPos += 0x01;
				break;
			case 0x63:	// 1/50s delay
				vgmInfo.VGMSmplPos += 882;
				vgmInfo.VGMPos += 0x01;
				break;
			case 0x61:	// xx Sample Delay
				TempSht = ReadLE16(&VGMPnt[0x01]);
				vgmInfo.VGMSmplPos += TempSht;
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x50:	// SN76496 write
				if (CHIP_CHECK(SN76496))
				{
					chip_reg_write(0x00, CurChip, 0x00, 0x00, VGMPnt[0x01]);
				}
				vgmInfo.VGMPos += 0x02;
				break;
			case 0x51:	// YM2413 write
				if (CHIP_CHECK(YM2413))
				{
					chip_reg_write(0x01, CurChip, 0x00, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x52:	// YM2612 write port 0
			case 0x53:	// YM2612 write port 1
				if (CHIP_CHECK(YM2612))
				{
					chip_reg_write(0x02, CurChip, Command & 0x01, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x67:	// PCM Data Stream
				TempByt = VGMPnt[0x02];
				TempLng = ReadLE32(&VGMPnt[0x03]);
				if (TempLng & 0x80000000)
				{
					TempLng &= 0x7FFFFFFF;
					CurChip = 0x01;
				}

				switch (TempByt & 0xC0)
				{
				case 0x00:	// Database Block
				case 0x40:
					AddPCMData(TempByt, TempLng, &VGMPnt[0x07]);
					/*switch(TempByt)
					{
					case 0x00:	// YM2612 PCM Database
						break;
					case 0x01:	// RF5C68 PCM Database
						break;
					case 0x02:	// RF5C164 PCM Database
						break;
					}*/
					break;
				case 0x80:	// ROM/RAM Dump
					if (vgmInfo.VGMCurLoop)
						break;

					ROMSize = ReadLE32(&VGMPnt[0x07]);
					DataStart = ReadLE32(&VGMPnt[0x0B]);
					DataLen = TempLng - 0x08;
					ROMData = &VGMPnt[0x0F];
					switch (TempByt)
					{
					case 0x80:	// SegaPCM ROM
						if (!CHIP_CHECK(SegaPCM))
							break;
						sega_pcm_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x81:	// YM2608 DELTA-T ROM Image
						if (!CHIP_CHECK(YM2608))
							break;
						ym2608_write_data_pcmrom(CurChip, 0x02, ROMSize, DataStart, DataLen,
							ROMData);
						break;
					case 0x82:	// YM2610 ADPCM ROM Image
					case 0x83:	// YM2610 DELTA-T ROM Image
						if (!CHIP_CHECK(YM2610))
							break;
						TempByt = 0x01 + (TempByt - 0x82);
						ym2610_write_data_pcmrom(CurChip, TempByt, ROMSize, DataStart, DataLen,
							ROMData);
						break;
					case 0x84:	// YMF278B ROM Image
						if (!CHIP_CHECK(YMF278B))
							break;
						ymf278b_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x85:	// YMF271 ROM Image
						if (!CHIP_CHECK(YMF271))
							break;
						ymf271_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x86:	// YMZ280B ROM Image
						if (!CHIP_CHECK(YMZ280B))
							break;
						ymz280b_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x87:	// YMF278B RAM Image
						if (!CHIP_CHECK(YMF278B))
							break;
						ymf278b_write_ram(CurChip, DataStart, DataLen, ROMData);
						break;
					case 0x88:	// Y8950 DELTA-T ROM Image
						if (!CHIP_CHECK(Y8950) || vgmInfo.PlayingMode == 0x01)
							break;
						y8950_write_data_pcmrom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x89:	// MultiPCM ROM Image
						if (!CHIP_CHECK(MultiPCM))
							break;
						multipcm_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x8A:	// UPD7759 ROM Image
						if (!CHIP_CHECK(UPD7759))
							break;
						upd7759_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x8B:	// OKIM6295 ROM Image
						if (!CHIP_CHECK(OKIM6295))
							break;
						okim6295_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x8C:	// K054539 ROM Image
						if (!CHIP_CHECK(K054539))
							break;
						k054539_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x8D:	// C140 ROM Image
						if (!CHIP_CHECK(C140))
							break;
						c140_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x8E:	// K053260 ROM Image
						if (!CHIP_CHECK(K053260))
							break;
						k053260_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x8F:	// QSound ROM Image
						if (!CHIP_CHECK(QSound))
							break;
						qsound_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x90:	// ES5506 ROM Image
						if (!CHIP_CHECK(ES5506))
							break;
						es5506_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x91:	// X1-010 ROM Image
						if (!CHIP_CHECK(X1_010))
							break;
						x1_010_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x92:	// C352 ROM Image
						if (!CHIP_CHECK(C352))
							break;
						c352_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
					case 0x93:	// GA20 ROM Image
						if (!CHIP_CHECK(GA20))
							break;
						iremga20_write_rom(CurChip, ROMSize, DataStart, DataLen, ROMData);
						break;
						//	case 0x8C:	// OKIM6376 ROM Image
						//		if (! CHIP_CHECK(OKIM6376))
						//			break;
						//		break;
					}
					break;
				case 0xC0:	// RAM Write
					if (!(TempByt & 0x20))
					{
						DataStart = ReadLE16(&VGMPnt[0x07]);
						DataLen = TempLng - 0x02;
						ROMData = &VGMPnt[0x09];
					}
					else
					{
						DataStart = ReadLE32(&VGMPnt[0x07]);
						DataLen = TempLng - 0x04;
						ROMData = &VGMPnt[0x0B];
					}
					switch (TempByt)
					{
					case 0xC0:	// RF5C68 RAM Database
						if (!CHIP_CHECK(RF5C68))
							break;
						rf5c68_write_ram(CurChip, DataStart, DataLen, ROMData);
						break;
					case 0xC1:	// RF5C164 RAM Database
						if (!CHIP_CHECK(RF5C164))
							break;
						rf5c164_write_ram(CurChip, DataStart, DataLen, ROMData);
						break;
					case 0xC2:	// NES APU RAM
						if (!CHIP_CHECK(NES))
							break;
						nes_write_ram(CurChip, DataStart, DataLen, ROMData);
						break;
					case 0xE0:	// SCSP RAM
						if (!CHIP_CHECK(SCSP))
							break;
						scsp_write_ram(CurChip, DataStart, DataLen, ROMData);
						break;
					case 0xE1:	// ES5503 RAM
						if (!CHIP_CHECK(ES5503))
							break;
						es5503_write_ram(CurChip, DataStart, DataLen, ROMData);
						break;
					}
					break;
				}
				vgmInfo.VGMPos += 0x07 + TempLng;
				break;
			case 0xE0:	// Seek to PCM Data Bank Pos
				vgmInfo.PCMBank[0x00].DataPos = ReadLE32(&VGMPnt[0x01]);
				vgmInfo.VGMPos += 0x05;
				break;
			case 0x31:	// Set AY8910 stereo mask
				TempByt = VGMPnt[0x01];
				TempLng = TempByt & 0x3F;
				CurChip = (TempByt & 0x80) ? 1 : 0;
				if (TempByt & 0x40)
					ym2203_set_stereo_mask_ay(CurChip, TempLng);
				else
					ayxx_set_stereo_mask(CurChip, TempLng);
				vgmInfo.VGMPos += 0x02;
				break;
			case 0x4F:	// GG Stereo
				if (CHIP_CHECK(SN76496))
				{
					chip_reg_write(0x00, CurChip, 0x01, 0x00, VGMPnt[0x01]);
				}
				vgmInfo.VGMPos += 0x02;
				break;
			case 0x54:	// YM2151 write
				if (CHIP_CHECK(YM2151))
				{
					chip_reg_write(0x03, CurChip, 0x01, VGMPnt[0x01], VGMPnt[0x02]);
				}
				//VGMSmplPos += 80;
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xC0:	// Sega PCM memory write
				TempSht = ReadLE16(&VGMPnt[0x01]);
				CurChip = (TempSht & 0x8000) >> 15;
				if (CHIP_CHECK(SegaPCM))
				{
					sega_pcm_w(CurChip, TempSht & 0x7FFF, VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xB0:	// RF5C68 register write
				if (CHIP_CHECK(RF5C68))
				{
					chip_reg_write(0x05, CurChip, 0x00, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xC1:	// RF5C164 memory write
				if (CHIP_CHECK(RF5C68))
				{
					TempSht = ReadLE16(&VGMPnt[0x01]);
					rf5c68_mem_w(CurChip, TempSht, VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0x55:	// YM2203
				if (CHIP_CHECK(YM2203))
				{
					chip_reg_write(0x06, CurChip, 0x00, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x56:	// YM2608 write port 0
			case 0x57:	// YM2608 write port 1
				if (CHIP_CHECK(YM2608))
				{
					chip_reg_write(0x07, CurChip, Command & 0x01, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x58:	// YM2610 write port 0
			case 0x59:	// YM2610 write port 1
				if (CHIP_CHECK(YM2610))
				{
					chip_reg_write(0x08, CurChip, Command & 0x01, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x5A:	// YM3812 write
				if (CHIP_CHECK(YM3812))
				{
					chip_reg_write(0x09, CurChip, 0x00, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x5B:	// YM3526 write
				if (CHIP_CHECK(YM3526))
				{
					chip_reg_write(0x0A, CurChip, 0x00, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x5C:	// Y8950 write
				if (CHIP_CHECK(Y8950))
				{
					chip_reg_write(0x0B, CurChip, 0x00, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x5E:	// YMF262 write port 0
			case 0x5F:	// YMF262 write port 1
				if (CHIP_CHECK(YMF262))
				{
					chip_reg_write(0x0C, CurChip, Command & 0x01, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x5D:	// YMZ280B write
				if (CHIP_CHECK(YMZ280B))
				{
					chip_reg_write(0x0F, CurChip, 0x00, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xD0:	// YMF278B write
				if (CHIP_CHECK(YMF278B))
				{
					CurChip = (VGMPnt[0x01] & 0x80) >> 7;
					chip_reg_write(0x0D, CurChip, VGMPnt[0x01] & 0x7F, VGMPnt[0x02], VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xD1:	// YMF271 write
				if (CHIP_CHECK(YMF271))
				{
					CurChip = (VGMPnt[0x01] & 0x80) >> 7;
					chip_reg_write(0x0E, CurChip, VGMPnt[0x01] & 0x7F, VGMPnt[0x02], VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xB1:	// RF5C164 register write
				if (CHIP_CHECK(RF5C164))
				{
					chip_reg_write(0x10, CurChip, 0x00, VGMPnt[0x01], VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xC2:	// RF5C164 memory write
				if (CHIP_CHECK(RF5C164))
				{
					TempSht = ReadLE16(&VGMPnt[0x01]);
					rf5c164_mem_w(CurChip, TempSht, VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xB2:	// PWM channel write
				if (CHIP_CHECK(PWM))
				{
					chip_reg_write(0x11, CurChip, (VGMPnt[0x01] & 0xF0) >> 4,
						VGMPnt[0x01] & 0x0F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x68:	// PCM RAM write
				CurChip = (VGMPnt[0x02] & 0x80) >> 7;
				TempByt = VGMPnt[0x02] & 0x7F;

				DataStart = ReadLE24(&VGMPnt[0x03]);
				TempLng = ReadLE24(&VGMPnt[0x06]);
				DataLen = ReadLE24(&VGMPnt[0x09]);
				if (!DataLen)
					DataLen += 0x01000000;
				ROMData = GetPointerFromPCMBank(TempByt, DataStart);
				if (ROMData == NULL)
				{
					vgmInfo.VGMPos += 0x0C;
					break;
				}

				switch (TempByt)
				{
				case 0x01:
					if (!CHIP_CHECK(RF5C68))
						break;
					rf5c68_write_ram(CurChip, TempLng, DataLen, ROMData);
					break;
				case 0x02:
					if (!CHIP_CHECK(RF5C164))
						break;
					rf5c164_write_ram(CurChip, TempLng, DataLen, ROMData);
					break;
				case 0x06:
					if (!CHIP_CHECK(SCSP))
						break;
					scsp_write_ram(CurChip, TempLng, DataLen, ROMData);
					break;
				case 0x07:
					if (!CHIP_CHECK(NES))
						break;
					vgmInfo.Last95Drum = DataStart / DataLen - 1;
					vgmInfo.Last95Max = vgmInfo.PCMBank[TempByt].DataSize / DataLen;
					nes_write_ram(CurChip, TempLng, DataLen, ROMData);
					break;
				}
				vgmInfo.VGMPos += 0x0C;
				break;
			case 0xA0:	// AY8910 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(AY8910))
				{
					chip_reg_write(0x12, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xB3:	// GameBoy DMG write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(GameBoy))
				{
					chip_reg_write(0x13, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xB4:	// NES APU write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(NES))
				{
					chip_reg_write(0x14, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xB5:	// MultiPCM write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(MultiPCM))
				{
					chip_reg_write(0x15, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xC3:	// MultiPCM memory write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(MultiPCM))
				{
					TempSht = ReadLE16(&VGMPnt[0x02]);
					multipcm_bank_write(CurChip, VGMPnt[0x01] & 0x7F, TempSht);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xB6:	// UPD7759 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(UPD7759))
				{
					chip_reg_write(0x16, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xB7:	// OKIM6258 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(OKIM6258))
				{
					chip_reg_write(0x17, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xB8:	// OKIM6295 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(OKIM6295))
				{
					chip_reg_write(0x18, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xD2:	// SCC1 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(K051649))
				{
					chip_reg_write(0x19, CurChip, VGMPnt[0x01] & 0x7F, VGMPnt[0x02],
						VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xD3:	// K054539 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(K054539))
				{
					chip_reg_write(0x1A, CurChip, VGMPnt[0x01] & 0x7F, VGMPnt[0x02],
						VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xB9:	// HuC6280 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(HuC6280))
				{
					chip_reg_write(0x1B, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xD4:	// C140 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(C140))
				{
					chip_reg_write(0x1C, CurChip, VGMPnt[0x01] & 0x7F, VGMPnt[0x02],
						VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xBA:	// K053260 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(K053260))
				{
					chip_reg_write(0x1D, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xBB:	// Pokey write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(Pokey))
				{
					chip_reg_write(0x1E, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xC4:	// QSound write
				if (CHIP_CHECK(QSound))
				{
					chip_reg_write(0x1F, CurChip, VGMPnt[0x01], VGMPnt[0x02], VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xC5:	// YMF292/SCSP write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(SCSP))
				{
					chip_reg_write(0x20, CurChip, VGMPnt[0x01] & 0x7F, VGMPnt[0x02],
						VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xBC:	// WonderSwan write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(WSwan))
				{
					chip_reg_write(0x21, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xC6:	// WonderSwan memory write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(WSwan))
				{
					TempSht = ReadBE16(&VGMPnt[0x01]) & 0x7FFF;
					ws_write_ram(CurChip, TempSht, VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xC7:	// VSU write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(VSU))
				{
					chip_reg_write(0x22, CurChip, VGMPnt[0x01] & 0x7F, VGMPnt[0x02],
						VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xBD:	// SAA1099 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(SAA1099))
				{
					chip_reg_write(0x23, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xD5:	// ES5503 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(ES5503))
				{
					chip_reg_write(0x24, CurChip, VGMPnt[0x01] & 0x7F, VGMPnt[0x02],
						VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xBE:	// ES5506 write (8-bit data)
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(ES5506))
				{
					chip_reg_write(0x25, CurChip, VGMPnt[0x01] & 0x7F, 0x00, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0xD6:	// ES5506 write (16-bit data)
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(ES5506))
				{
					chip_reg_write(0x25, CurChip, 0x80 | (VGMPnt[0x01] & 0x7F),
						VGMPnt[0x02], VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
			case 0xC8:	// X1-010 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(X1_010))
				{
					chip_reg_write(0x26, CurChip, VGMPnt[0x01] & 0x7F, VGMPnt[0x02],
						VGMPnt[0x03]);
				}
				vgmInfo.VGMPos += 0x04;
				break;
#if 0	// for ctr's WIP rips
			case 0xC9:	// C352 write
				CurChip = 0x00;
				if (CHIP_CHECK(C352))
				{
					if (VGMPnt[0x01] == 0x03 && VGMPnt[0x02] == 0xFF && VGMPnt[0x03] == 0xFF)
						c352_w(CurChip, 0x202, 0x0020);
					else
						chip_reg_write(0x27, CurChip, VGMPnt[0x01], VGMPnt[0x02],
							VGMPnt[0x03]);
				}
				VGMPos += 0x04;
				break;
#endif
			case 0xE1:	// C352 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(C352))
				{
					TempSht = ((VGMPnt[0x01] & 0x7F) << 8) | (VGMPnt[0x02] << 0);
					c352_w(CurChip, TempSht, (VGMPnt[0x03] << 8) | VGMPnt[0x04]);
				}
				vgmInfo.VGMPos += 0x05;
				break;
			case 0xBF:	// GA20 write
				CurChip = (VGMPnt[0x01] & 0x80) >> 7;
				if (CHIP_CHECK(GA20))
				{
					chip_reg_write(0x28, CurChip, 0x00, VGMPnt[0x01] & 0x7F, VGMPnt[0x02]);
				}
				vgmInfo.VGMPos += 0x03;
				break;
			case 0x90:	// DAC Ctrl: Setup Chip
				CurChip = VGMPnt[0x01];
				if (CurChip == 0xFF)
				{
					vgmInfo.VGMPos += 0x05;
					break;
				}
				if (!vgmInfo.DacCtrl[CurChip].Enable)
				{
					device_start_daccontrol(CurChip, vgmInfo.CHIP_SAMPLING_MODE, vgmInfo.CHIP_SAMPLE_RATE, vgmInfo.SampleRate);
					device_reset_daccontrol(CurChip);
					vgmInfo.DacCtrl[CurChip].Enable = true;
					vgmInfo.DacCtrlUsg[vgmInfo.DacCtrlUsed] = CurChip;
					vgmInfo.DacCtrlUsed++;
				}
				TempByt = VGMPnt[0x02];	// Chip Type
				TempSht = ReadBE16(&VGMPnt[0x03]);
				daccontrol_setup_chip(CurChip, TempByt & 0x7F, (TempByt & 0x80) >> 7, TempSht);
				vgmInfo.VGMPos += 0x05;
				break;
			case 0x91:	// DAC Ctrl: Set Data
				CurChip = VGMPnt[0x01];
				if (CurChip == 0xFF || !vgmInfo.DacCtrl[CurChip].Enable)
				{
					vgmInfo.VGMPos += 0x05;
					break;
				}
				vgmInfo.DacCtrl[CurChip].Bank = VGMPnt[0x02];
				if (vgmInfo.DacCtrl[CurChip].Bank >= PCM_BANK_COUNT)
					vgmInfo.DacCtrl[CurChip].Bank = 0x00;

				TempPCM = &vgmInfo.PCMBank[vgmInfo.DacCtrl[CurChip].Bank];
				vgmInfo.Last95Max = TempPCM->BankCount;
				daccontrol_set_data(CurChip, TempPCM->Data, TempPCM->DataSize,
					VGMPnt[0x03], VGMPnt[0x04]);
				vgmInfo.VGMPos += 0x05;
				break;
			case 0x92:	// DAC Ctrl: Set Freq
				CurChip = VGMPnt[0x01];
				if (CurChip == 0xFF || !vgmInfo.DacCtrl[CurChip].Enable)
				{
					vgmInfo.VGMPos += 0x06;
					break;
				}
				TempLng = ReadLE32(&VGMPnt[0x02]);
				vgmInfo.Last95Freq = TempLng;
				daccontrol_set_frequency(CurChip, TempLng);
				vgmInfo.VGMPos += 0x06;
				break;
			case 0x93:	// DAC Ctrl: Play from Start Pos
				CurChip = VGMPnt[0x01];
				if (CurChip == 0xFF || !vgmInfo.DacCtrl[CurChip].Enable ||
					!vgmInfo.PCMBank[vgmInfo.DacCtrl[CurChip].Bank].BankCount)
				{
					vgmInfo.VGMPos += 0x0B;
					break;
				}
				DataStart = ReadLE32(&VGMPnt[0x02]);
				vgmInfo.Last95Drum = 0xFFFF;
				TempByt = VGMPnt[0x06];
				DataLen = ReadLE32(&VGMPnt[0x07]);
				daccontrol_start(CurChip, DataStart, TempByt, DataLen);
				vgmInfo.VGMPos += 0x0B;
				break;
			case 0x94:	// DAC Ctrl: Stop immediately
				CurChip = VGMPnt[0x01];
				if (!vgmInfo.DacCtrl[CurChip].Enable)
				{
					vgmInfo.VGMPos += 0x02;
					break;
				}
				vgmInfo.Last95Drum = 0xFFFF;
				if (CurChip < 0xFF)
				{
					daccontrol_stop(CurChip);
				}
				else
				{
					for (CurChip = 0x00; CurChip < 0xFF; CurChip++)
						daccontrol_stop(CurChip);
				}
				vgmInfo.VGMPos += 0x02;
				break;
			case 0x95:	// DAC Ctrl: Play Block (small)
				CurChip = VGMPnt[0x01];
				if (CurChip == 0xFF || !vgmInfo.DacCtrl[CurChip].Enable ||
					!vgmInfo.PCMBank[vgmInfo.DacCtrl[CurChip].Bank].BankCount)
				{
					vgmInfo.VGMPos += 0x05;
					break;
				}
				TempPCM = &vgmInfo.PCMBank[vgmInfo.DacCtrl[CurChip].Bank];
				TempSht = ReadLE16(&VGMPnt[0x02]);
				vgmInfo.Last95Drum = TempSht;
				vgmInfo.Last95Max = TempPCM->BankCount;
				if (TempSht >= TempPCM->BankCount)
					TempSht = 0x00;
				TempBnk = &TempPCM->Bank[TempSht];

				TempByt = DCTRL_LMODE_BYTES |
					(VGMPnt[0x04] & 0x10) |			// Reverse Mode
					((VGMPnt[0x04] & 0x01) << 7);	// Looping
				daccontrol_start(CurChip, TempBnk->DataStart, TempByt, TempBnk->DataSize);
				vgmInfo.VGMPos += 0x05;
				break;
			default:
#ifdef CONSOLE_MODE
				if (!vgmInfo.CmdList[Command])
				{
					fprintf(stderr, "Unknown command: %02hhX\n", Command);
					vgmInfo.CmdList[Command] = true;
				}
#endif

				switch (Command & 0xF0)
				{
				case 0x00:
				case 0x10:
				case 0x20:
					vgmInfo.VGMPos += 0x01;
					break;
				case 0x30:
					vgmInfo.VGMPos += 0x02;
					break;
				case 0x40:
				case 0x50:
				case 0xA0:
				case 0xB0:
					vgmInfo.VGMPos += 0x03;
					break;
				case 0xC0:
				case 0xD0:
					vgmInfo.VGMPos += 0x04;
					break;
				case 0xE0:
				case 0xF0:
					vgmInfo.VGMPos += 0x05;
					break;
				default:
					vgmInfo.VGMEnd = true;
					vgmInfo.EndPlay = true;
					break;
				}
				break;
		}
	}

		if (vgmInfo.VGMPos >= vgmInfo.VGMHead.lngEOFOffset)
			vgmInfo.VGMEnd = true;

		if (vgmInfo.VGMEnd)
			break;
	}

	return;
}

void VGMData::ChangeChipSampleRate(void* DataPtr, UINT32 NewSmplRate)
{
	assert(0);
#if 0
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	CAUD_ATTR* CAA = (CAUD_ATTR*)DataPtr;

	if (CAA->SmpRate == NewSmplRate)
		return;

	// quick and dirty hack to make sample rate changes work
	CAA->SmpRate = NewSmplRate;
	if (CAA->SmpRate < vgmInfo.SampleRate)
		CAA->Resampler = 0x01;
	else if (CAA->SmpRate == vgmInfo.SampleRate)
		CAA->Resampler = 0x02;
	else if (CAA->SmpRate > vgmInfo.SampleRate)
		CAA->Resampler = 0x03;
	CAA->SmpP = 1;
	CAA->SmpNext -= CAA->SmpLast;
	CAA->SmpLast = 0x00;

	return;
#endif
}

UINT8 VGMData::GetDACFromPCMBank(void)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	// for YM2612 DAC data only
	/*VGM_PCM_BANK* TempPCM;
	UINT32 CurBnk;*/
	UINT32 DataPos;

	/*TempPCM = &PCMBank[0x00];
	DataPos = TempPCM->DataPos;
	for (CurBnk = 0x00; CurBnk < TempPCM->BankCount; CurBnk ++)
	{
		if (DataPos < TempPCM->Bank[CurBnk].DataSize)
		{
			if (TempPCM->DataPos < TempPCM->DataSize)
				TempPCM->DataPos ++;
			return TempPCM->Bank[CurBnk].Data[DataPos];
		}
		DataPos -= TempPCM->Bank[CurBnk].DataSize;
	}
	return 0x80;*/

	DataPos = vgmInfo.PCMBank[0x00].DataPos;
	if (DataPos >= vgmInfo.PCMBank[0x00].DataSize)
		return 0x80;

	vgmInfo.PCMBank[0x00].DataPos++;
	return vgmInfo.PCMBank[0x00].Data[DataPos];
}

UINT8* VGMData::GetPointerFromPCMBank(UINT8 Type, UINT32 DataPos)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	if (Type >= PCM_BANK_COUNT)
		return NULL;

	if (DataPos >= vgmInfo.PCMBank[Type].DataSize)
		return NULL;

	return &vgmInfo.PCMBank[Type].Data[DataPos];
}

void VGMData::ReadPCMTable(UINT32 DataSize, const UINT8* Data)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT8 ValSize;
	UINT32 TblSize;

	vgmInfo.PCMTbl.ComprType = Data[0x00];
	vgmInfo.PCMTbl.CmpSubType = Data[0x01];
	vgmInfo.PCMTbl.BitDec = Data[0x02];
	vgmInfo.PCMTbl.BitCmp = Data[0x03];
	vgmInfo.PCMTbl.EntryCount = ReadLE16(&Data[0x04]);

	ValSize = (vgmInfo.PCMTbl.BitDec + 7) / 8;
	TblSize = vgmInfo.PCMTbl.EntryCount * ValSize;

	vgmInfo.PCMTbl.Entries = realloc(vgmInfo.PCMTbl.Entries, TblSize);
	::memcpy(vgmInfo.PCMTbl.Entries, &Data[0x06], TblSize);

	if (DataSize < 0x06 + TblSize)
		fprintf(stderr, "Warning! Bad PCM Table Length!\n");

	return;
}

void VGMData::AddPCMData(UINT8 Type, UINT32 DataSize, const UINT8* Data)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT32 CurBnk;
	VGM_PCM_BANK* TempPCM;
	VGM_PCM_DATA* TempBnk;
	UINT32 BankSize;
	bool RetVal;
	UINT8 BnkType;
	UINT8 CurDAC;

	BnkType = Type & 0x3F;
	if (BnkType >= PCM_BANK_COUNT || vgmInfo.VGMCurLoop)
		return;

	if (Type == 0x7F)
	{
		ReadPCMTable(DataSize, Data);
		return;
	}

	TempPCM = &vgmInfo.PCMBank[BnkType];
	TempPCM->BnkPos++;
	if (TempPCM->BnkPos <= TempPCM->BankCount)
		return;	// Speed hack for restarting playback (skip already loaded blocks)
	CurBnk = TempPCM->BankCount;
	TempPCM->BankCount++;
	if (vgmInfo.Last95Max != 0xFFFF)
		vgmInfo.Last95Max = TempPCM->BankCount;
	TempPCM->Bank = (VGM_PCM_DATA*)realloc(TempPCM->Bank,
		sizeof(VGM_PCM_DATA) * TempPCM->BankCount);

	if (!(Type & 0x40))
		BankSize = DataSize;
	else
		BankSize = ReadLE32(&Data[0x01]);
	TempPCM->Data = (UINT8*)realloc(TempPCM->Data, TempPCM->DataSize + BankSize);
	TempBnk = &TempPCM->Bank[CurBnk];
	TempBnk->DataStart = TempPCM->DataSize;
	if (!(Type & 0x40))
	{
		TempBnk->DataSize = DataSize;
		TempBnk->Data = TempPCM->Data + TempBnk->DataStart;
		::memcpy(TempBnk->Data, Data, DataSize);
	}
	else
	{
		TempBnk->Data = TempPCM->Data + TempBnk->DataStart;
		RetVal = DecompressDataBlk(TempBnk, DataSize, Data);
		if (!RetVal)
		{
			TempBnk->Data = NULL;
			TempBnk->DataSize = 0x00;
			//return;
			goto RefreshDACStrm;	// sorry for the goto, but I don't want to copy-paste the code
		}
	}
	if (BankSize != TempBnk->DataSize)
		fprintf(stderr, "Error reading Data Block! Data Size conflict!\n");
	TempPCM->DataSize += BankSize;

	// realloc may've moved the Bank block, so refresh all DAC Streams
RefreshDACStrm:
	for (CurDAC = 0x00; CurDAC < vgmInfo.DacCtrlUsed; CurDAC++)
	{
		if (vgmInfo.DacCtrl[vgmInfo.DacCtrlUsg[CurDAC]].Bank == BnkType)
			daccontrol_refresh_data(vgmInfo.DacCtrlUsg[CurDAC], TempPCM->Data, TempPCM->DataSize);
	}

	return;
}

bool VGMData::DecompressDataBlk(VGM_PCM_DATA* Bank, UINT32 DataSize, const UINT8* Data)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT8 ComprType;
	UINT8 BitDec;
	FUINT8 BitCmp;
	UINT8 CmpSubType;
	UINT16 AddVal;
	const UINT8* InPos;
	const UINT8* InDataEnd;
	UINT8* OutPos;
	const UINT8* OutDataEnd;
	FUINT16 InVal;
	FUINT16 OutVal;
	FUINT8 ValSize;
	FUINT8 InShift;
	FUINT8 OutShift;
	UINT8* Ent1B;
	UINT16* Ent2B;
#if defined(_DEBUG) && defined(WIN32)
	UINT32 Time;
#endif

	// ReadBits Variables
	FUINT8 BitsToRead;
	FUINT8 BitReadVal;
	FUINT8 InValB;
	FUINT8 BitMask;
	FUINT8 OutBit;

	// Variables for DPCM
	UINT16 OutMask;

#if defined(_DEBUG) && defined(WIN32)
	Time = GetTickCount();
#endif
	ComprType = Data[0x00];
	Bank->DataSize = ReadLE32(&Data[0x01]);

	switch (ComprType)
	{
	case 0x00:	// n-Bit compression
		BitDec = Data[0x05];
		BitCmp = Data[0x06];
		CmpSubType = Data[0x07];
		AddVal = ReadLE16(&Data[0x08]);
		Ent1B = NULL;
		Ent2B = NULL;

		if (CmpSubType == 0x02)
		{
			Ent1B = (UINT8*)vgmInfo.PCMTbl.Entries;	// Big Endian note: Those are stored in LE and converted when reading.
			Ent2B = (UINT16*)vgmInfo.PCMTbl.Entries;
			if (!vgmInfo.PCMTbl.EntryCount)
			{
				Bank->DataSize = 0x00;
				fprintf(stderr, "Error loading table-compressed data block! No table loaded!\n");
				return false;
			}
			else if (BitDec != vgmInfo.PCMTbl.BitDec || BitCmp != vgmInfo.PCMTbl.BitCmp)
			{
				Bank->DataSize = 0x00;
				fprintf(stderr, "Warning! Data block and loaded value table incompatible!\n");
				return false;
			}
		}

		ValSize = (BitDec + 7) / 8;
		InPos = Data + 0x0A;
		InDataEnd = Data + DataSize;
		InShift = 0;
		OutShift = BitDec - BitCmp;
		OutDataEnd = Bank->Data + Bank->DataSize;
		OutVal = 0x0000;

		for (OutPos = Bank->Data; OutPos < OutDataEnd && InPos < InDataEnd; OutPos += ValSize)
		{
			//InVal = ReadBits(Data, InPos, &InShift, BitCmp);
			// inlined - is 30% faster
			OutBit = 0x00;
			InVal = 0x0000;
			BitsToRead = BitCmp;
			while (BitsToRead)
			{
				BitReadVal = (BitsToRead >= 8) ? 8 : BitsToRead;
				BitsToRead -= BitReadVal;
				BitMask = (1 << BitReadVal) - 1;

				InShift += BitReadVal;
				InValB = (*InPos << InShift >> 8) & BitMask;
				if (InShift >= 8)
				{
					InShift -= 8;
					InPos++;
					if (InShift)
						InValB |= (*InPos << InShift >> 8) & BitMask;
				}

				InVal |= InValB << OutBit;
				OutBit += BitReadVal;
			}

			switch (CmpSubType)
			{
			case 0x00:	// Copy
				OutVal = InVal + AddVal;
				break;
			case 0x01:	// Shift Left
				OutVal = (InVal << OutShift) + AddVal;
				break;
			case 0x02:	// Table
				switch (ValSize)
				{
				case 0x01:
					OutVal = Ent1B[InVal];
					break;
				case 0x02:
#ifdef VGM_LITTLE_ENDIAN
					OutVal = Ent2B[InVal];
#else
					OutVal = ReadLE16((UINT8*)&Ent2B[InVal]);
#endif
					break;
				}
				break;
			}

#ifdef VGM_LITTLE_ENDIAN
			//memcpy(OutPos, &OutVal, ValSize);
			if (ValSize == 0x01)
				*((UINT8*)OutPos) = (UINT8)OutVal;
			else //if (ValSize == 0x02)
				*((UINT16*)OutPos) = (UINT16)OutVal;
#else
			if (ValSize == 0x01)
			{
				*OutPos = (UINT8)OutVal;
			}
			else //if (ValSize == 0x02)
			{
				OutPos[0x00] = (UINT8)((OutVal & 0x00FF) >> 0);
				OutPos[0x01] = (UINT8)((OutVal & 0xFF00) >> 8);
			}
#endif
			}
		break;
	case 0x01:	// Delta-PCM
		BitDec = Data[0x05];
		BitCmp = Data[0x06];
		OutVal = ReadLE16(&Data[0x08]);

		Ent1B = (UINT8*)vgmInfo.PCMTbl.Entries;
		Ent2B = (UINT16*)vgmInfo.PCMTbl.Entries;
		if (!vgmInfo.PCMTbl.EntryCount)
		{
			Bank->DataSize = 0x00;
			fprintf(stderr, "Error loading table-compressed data block! No table loaded!\n");
			return false;
		}
		else if (BitDec != vgmInfo.PCMTbl.BitDec || BitCmp != vgmInfo.PCMTbl.BitCmp)
		{
			Bank->DataSize = 0x00;
			fprintf(stderr, "Warning! Data block and loaded value table incompatible!\n");
			return false;
		}

		ValSize = (BitDec + 7) / 8;
		OutMask = (1 << BitDec) - 1;
		InPos = Data + 0x0A;
		InDataEnd = Data + DataSize;
		InShift = 0;
		OutShift = BitDec - BitCmp;
		OutDataEnd = Bank->Data + Bank->DataSize;
		AddVal = 0x0000;

		for (OutPos = Bank->Data; OutPos < OutDataEnd && InPos < InDataEnd; OutPos += ValSize)
		{
			//InVal = ReadBits(Data, InPos, &InShift, BitCmp);
			// inlined - is 30% faster
			OutBit = 0x00;
			InVal = 0x0000;
			BitsToRead = BitCmp;
			while (BitsToRead)
			{
				BitReadVal = (BitsToRead >= 8) ? 8 : BitsToRead;
				BitsToRead -= BitReadVal;
				BitMask = (1 << BitReadVal) - 1;

				InShift += BitReadVal;
				InValB = (*InPos << InShift >> 8) & BitMask;
				if (InShift >= 8)
				{
					InShift -= 8;
					InPos++;
					if (InShift)
						InValB |= (*InPos << InShift >> 8) & BitMask;
				}

				InVal |= InValB << OutBit;
				OutBit += BitReadVal;
			}

			switch (ValSize)
			{
			case 0x01:
				AddVal = Ent1B[InVal];
				OutVal += AddVal;
				OutVal &= OutMask;
				*((UINT8*)OutPos) = (UINT8)OutVal;
				break;
			case 0x02:
#ifdef VGM_LITTLE_ENDIAN
				AddVal = Ent2B[InVal];
				OutVal += AddVal;
				OutVal &= OutMask;
				*((UINT16*)OutPos) = (UINT16)OutVal;
#else
				AddVal = ReadLE16((UINT8*)&Ent2B[InVal]);
				OutVal += AddVal;
				OutVal &= OutMask;
				OutPos[0x00] = (UINT8)((OutVal & 0x00FF) >> 0);
				OutPos[0x01] = (UINT8)((OutVal & 0xFF00) >> 8);
#endif
				break;
			}
		}
		break;
	default:
		fprintf(stderr, "Error: Unknown data block compression!\n");
		return false;
		}

#if defined(_DEBUG) && defined(WIN32)
	Time = GetTickCount() - Time;
	fprintf(stderr, "Decompression Time: %u\n", Time);
#endif

	return true;
	}

void VGMData::ReadVGMHeader(VGM_HEADER* RetVGMHead)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile& gzfile = impl->gzfile;

	VGM_HEADER CurHead;
	UINT32 CurPos;
	UINT32 HdrLimit;

	gzread(gzfile, &CurHead, sizeof(VGM_HEADER));
#ifndef VGM_LITTLE_ENDIAN
	{
		UINT8* TempPtr;

		// Warning: Lots of pointer casting ahead!
		for (CurPos = 0x00; CurPos < sizeof(VGM_HEADER); CurPos += 0x04)
		{
			TempPtr = (UINT8*)&CurHead + CurPos;
			switch (CurPos)
			{
			case 0x28:
				// 0x28	[16-bit] SN76496 Feedback Mask
				// 0x2A	[ 8-bit] SN76496 Shift Register Width
				// 0x2B	[ 8-bit] SN76496 Flags
				*(UINT16*)TempPtr = ReadLE16(TempPtr);
				break;
			case 0x78:	// 78-7B [8-bit] AY8910 Type/Flags
			case 0x7C:	// 7C-7F [8-bit] Volume/Loop Modifiers
			case 0x94:	// 94-97 [8-bit] various flags
				break;
			default:
				// everything else is 32-bit
				*(UINT32*)TempPtr = ReadLE32(TempPtr);
				break;
}
}
	}
#endif

	// Header preperations
	if (CurHead.lngVersion < 0x00000101)
	{
		CurHead.lngRate = 0;
	}
	if (CurHead.lngVersion < 0x00000110)
	{
		CurHead.shtPSG_Feedback = 0x0000;
		CurHead.bytPSG_SRWidth = 0x00;
		CurHead.lngHzYM2612 = CurHead.lngHzYM2413;
		CurHead.lngHzYM2151 = CurHead.lngHzYM2413;
	}
	if (CurHead.lngVersion < 0x00000150)
	{
		CurHead.lngDataOffset = 0x00000000;
		// If I would aim to be very strict, I would uncomment these few lines,
		// but I sometimes use v1.51 Flags with v1.50 for better compatibility.
		// (Some hyper-strict players refuse to play v1.51 files, even if there's
		//  no new chip used.)
		//}
		//if (CurHead.lngVersion < 0x00000151)
		//{
		CurHead.bytPSG_Flags = 0x00;
		CurHead.lngHzSPCM = 0x0000;
		CurHead.lngSPCMIntf = 0x00000000;
		// all others are zeroed by memset
	}

	if (CurHead.lngHzPSG)
	{
		if (!CurHead.shtPSG_Feedback)
			CurHead.shtPSG_Feedback = 0x0009;
		if (!CurHead.bytPSG_SRWidth)
			CurHead.bytPSG_SRWidth = 0x10;
	}

	// relative -> absolute addresses
	if (CurHead.lngEOFOffset)
		CurHead.lngEOFOffset += 0x00000004;
	if (CurHead.lngGD3Offset)
		CurHead.lngGD3Offset += 0x00000014;
	if (CurHead.lngLoopOffset)
		CurHead.lngLoopOffset += 0x0000001C;

	if (CurHead.lngVersion < 0x00000150)
		CurHead.lngDataOffset = 0x0000000C;
	//if (CurHead.lngDataOffset < 0x0000000C)
	//	CurHead.lngDataOffset = 0x0000000C;
	if (CurHead.lngDataOffset)
		CurHead.lngDataOffset += 0x00000034;

	CurPos = CurHead.lngDataOffset;
	// should actually check v1.51 (first real usage of DataOffset)
	// v1.50 is checked to support things like the Volume Modifiers in v1.50 files
	if (CurHead.lngVersion < 0x00000150 /*0x00000151*/)
		CurPos = 0x40;
	if (!CurPos)
		CurPos = 0x40;
	HdrLimit = sizeof(VGM_HEADER);
	if (HdrLimit > CurPos)
		memset((UINT8*)&CurHead + CurPos, 0x00, HdrLimit - CurPos);

	if (!CurHead.bytLoopModifier)
		CurHead.bytLoopModifier = 0x10;

	if (CurHead.lngExtraOffset)
	{
		CurHead.lngExtraOffset += 0xBC;

		CurPos = CurHead.lngExtraOffset;
		if (CurPos < HdrLimit)
			memset((UINT8*)&CurHead + CurPos, 0x00, HdrLimit - CurPos);
	}

	if (CurHead.lngGD3Offset >= CurHead.lngEOFOffset)
		CurHead.lngGD3Offset = 0x00;
	if (CurHead.lngLoopOffset >= CurHead.lngEOFOffset)
		CurHead.lngLoopOffset = 0x00;
	if (CurHead.lngDataOffset >= CurHead.lngEOFOffset)
		CurHead.lngDataOffset = 0x40;
	if (CurHead.lngExtraOffset >= CurHead.lngEOFOffset)
		CurHead.lngExtraOffset = 0x00;

	*RetVGMHead = CurHead;
	return;
}

UINT8 VGMData::ReadGD3Tag(UINT32 GD3Offset, GD3_TAG* RetGD3Tag)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile& gzfile = impl->gzfile;


	UINT32 CurPos;
	UINT32 TempLng;
	UINT8 ResVal;

	ResVal = 0x00;

	// Read GD3 Tag
	if (GD3Offset)
	{
		gzseek(gzfile, GD3Offset, SEEK_SET);
		gzread(gzfile, &TempLng, 4);
		if (TempLng != FCC_GD3)
		{
			GD3Offset = 0x00000000;
			ResVal = 0x10;	// invalid GD3 offset
		}
	}

	if (RetGD3Tag == NULL)
		return ResVal;

	if (!GD3Offset)
	{
		RetGD3Tag->fccGD3 = 0x00000000;
		RetGD3Tag->lngVersion = 0x00000000;
		RetGD3Tag->lngTagLength = 0x00000000;
		RetGD3Tag->strTrackNameE = NULL;
		RetGD3Tag->strTrackNameJ = NULL;
		RetGD3Tag->strGameNameE = NULL;
		RetGD3Tag->strGameNameJ = NULL;
		RetGD3Tag->strSystemNameE = NULL;
		RetGD3Tag->strSystemNameJ = NULL;
		RetGD3Tag->strAuthorNameE = NULL;
		RetGD3Tag->strAuthorNameJ = NULL;
		RetGD3Tag->strReleaseDate = NULL;
		RetGD3Tag->strCreator = NULL;
		RetGD3Tag->strNotes = NULL;
	}
	else
	{
		//CurPos = GD3Offset;
		//gzseek(hFile, CurPos, SEEK_SET);
		//CurPos += gzgetLE32(hFile, &RetGD3Tag->fccGD3);

		CurPos = GD3Offset + 0x04;		// Save some back seeking, yay!
		RetGD3Tag->fccGD3 = TempLng;	// (That costs lots of CPU in .gz files.)
		CurPos += gzread(gzfile, &RetGD3Tag->lngVersion, 4);
		CurPos += gzread(gzfile, &RetGD3Tag->lngTagLength, 4);

		TempLng = CurPos + RetGD3Tag->lngTagLength;
		RetGD3Tag->strTrackNameE = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strTrackNameJ = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strGameNameE = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strGameNameJ = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strSystemNameE = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strSystemNameJ = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strAuthorNameE = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strAuthorNameJ = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strReleaseDate = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strCreator = ReadWStrFromFile(&CurPos, TempLng);
		RetGD3Tag->strNotes = ReadWStrFromFile(&CurPos, TempLng);
	}

	return ResVal;
}

void VGMData::ReadChipExtraData32(UINT32 StartOffset, VGMX_CHP_EXTRA32* ChpExtra)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile& gzfile = impl->gzfile;

	UINT32 CurPos;
	UINT8 CurChp;
	VGMX_CHIP_DATA32* TempCD;

	if (!StartOffset || StartOffset >= vgmInfo.VGMDataLen)
	{
		ChpExtra->ChipCnt = 0x00;
		ChpExtra->CCData = NULL;
		return;
	}

	CurPos = StartOffset;
	ChpExtra->ChipCnt = vgmInfo.VGMData[CurPos];
	if (ChpExtra->ChipCnt)
		ChpExtra->CCData = (VGMX_CHIP_DATA32*)malloc(sizeof(VGMX_CHIP_DATA32) *
			ChpExtra->ChipCnt);
	else
		ChpExtra->CCData = NULL;
	CurPos++;

	for (CurChp = 0x00; CurChp < ChpExtra->ChipCnt; CurChp++)
	{
		TempCD = &ChpExtra->CCData[CurChp];
		TempCD->Type = vgmInfo.VGMData[CurPos + 0x00];
		TempCD->Data = ReadLE32(&vgmInfo.VGMData[CurPos + 0x01]);
		CurPos += 0x05;
	}

	return;
}

void VGMData::ReadChipExtraData16(UINT32 StartOffset, VGMX_CHP_EXTRA16* ChpExtra)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile& gzfile = impl->gzfile;

	UINT32 CurPos;
	UINT8 CurChp;
	VGMX_CHIP_DATA16* TempCD;

	if (!StartOffset || StartOffset >= vgmInfo.VGMDataLen)
	{
		ChpExtra->ChipCnt = 0x00;
		ChpExtra->CCData = NULL;
		return;
	}

	CurPos = StartOffset;
	ChpExtra->ChipCnt = vgmInfo.VGMData[CurPos];
	if (ChpExtra->ChipCnt)
		ChpExtra->CCData = (VGMX_CHIP_DATA16*)malloc(sizeof(VGMX_CHIP_DATA16) *
			ChpExtra->ChipCnt);
	else
		ChpExtra->CCData = NULL;
	CurPos++;

	for (CurChp = 0x00; CurChp < ChpExtra->ChipCnt; CurChp++)
	{
		TempCD = &ChpExtra->CCData[CurChp];
		TempCD->Type = vgmInfo.VGMData[CurPos + 0x00];
		TempCD->Flags = vgmInfo.VGMData[CurPos + 0x01];
		TempCD->Data = ReadLE16(&vgmInfo.VGMData[CurPos + 0x02]);
		CurPos += 0x04;
	}

	return;
}

wchar_t* VGMData::ReadWStrFromFile(UINT32* FilePos, UINT32 EOFPos)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile& gzfile = impl->gzfile;

	// Note: Works with Windows (16-bit wchar_t) as well as Linux (32-bit wchar_t)
	UINT32 CurPos;
	wchar_t* TextStr;
	wchar_t* TempStr;
	UINT32 StrLen;
	UINT16 UnicodeChr;

	CurPos = *FilePos;
	if (CurPos >= EOFPos)
		return NULL;
	TextStr = (wchar_t*)malloc((EOFPos - CurPos) / 0x02 * sizeof(wchar_t));
	if (TextStr == NULL)
		return NULL;

	if ((UINT32)gztell(gzfile) != CurPos)
		gzseek(gzfile, CurPos, SEEK_SET);
	TempStr = TextStr - 1;
	StrLen = 0x00;
	do
	{
		TempStr++;
		gzread(gzfile, &UnicodeChr, 2);
		*TempStr = (wchar_t)UnicodeChr;
		CurPos += 0x02;
		StrLen++;
		if (CurPos >= EOFPos)
		{
			*TempStr = L'\0';
			break;
		}
	} while (*TempStr != L'\0');

	TextStr = (wchar_t*)realloc(TextStr, StrLen * sizeof(wchar_t));
	*FilePos = CurPos;

	return TextStr;
}

wchar_t* VGMData::MakeEmptyWStr(void)
{
	wchar_t* Str;

	Str = (wchar_t*)malloc(0x01 * sizeof(wchar_t));
	Str[0x00] = L'\0';

	return Str;
}

void VGMData::PauseStream(bool PauseOn)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile& gzfile = impl->gzfile;

	UINT32 RetVal;

	if (!vgmInfo.WaveOutOpen)
		return;	// Thread is not active

#ifdef WIN32
	switch (PauseOn)
	{
	case true:
		RetVal = waveOutPause(vgmInfo.hWaveOut);
		break;
	case false:
		RetVal = waveOutRestart(vgmInfo.hWaveOut);
		break;
}
	vgmInfo.StreamPause = PauseOn;
#else
	vgmInfo.PauseThread = PauseOn;
#endif

	return;
}

void VGMData::InterpretOther(UINT32 SampleCount)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile& gzfile = impl->gzfile;

	static UINT8 LastCmd = 0x90;
	static UINT8 DrumReg[0x02] = { 0x00, 0x00 };
	static UINT8 ChnIns[0x10];
	static UINT8 ChnNote[0x20];
	static INT8 ChnPitch[0x10];
	INT32 SmplPlayed;
	UINT8 Command;
	UINT8 Channel;
	UINT8 TempByt;
	UINT16 TempSht;
	UINT32 TempLng;
	UINT32 DataLen;
	static UINT8 CurChip = 0x00;
	UINT8 OpBase;	// Operator Base
	CMF_INSTRUMENT* TempIns;
	bool RhythmOn;
	bool NoteOn;
	UINT8 OpMask;

	if (vgmInfo.VGMEnd)
		return;
	if (vgmInfo.PausePlay && !vgmInfo.ForceVGMExec)
		return;

	switch (vgmInfo.FileMode)
	{
	case 0x01:	// CMF File Mode
		if (!SampleCount)
		{
			memset(ChnIns, 0xFF, 0x10);
			memset(ChnNote, 0xFF, 0x20);
			memset(ChnPitch, 0x00, 0x10);

			TempLng = vgmInfo.VGMPos;
			SmplPlayed = vgmInfo.VGMSmplPos;
			vgmInfo.VGMPos = vgmInfo.VGMHead.lngDataOffset;
			RhythmOn = false;
			while (!RhythmOn)
			{
				vgmInfo.VGMSmplPos += GetMIDIDelay(&DataLen);
				vgmInfo.VGMPos += DataLen;

				Command = vgmInfo.VGMData[vgmInfo.VGMPos];
				if (Command & 0x80)
					vgmInfo.VGMPos++;
				else
					Command = LastCmd;
				Channel = Command & 0x0F;

				switch (Command & 0xF0)
				{
				case 0xF0:	// End Of File
					switch (Command)
					{
					case 0xFF:
						switch (vgmInfo.VGMData[vgmInfo.VGMPos + 0x00])
						{
						case 0x2F:
							vgmInfo.VGMHead.lngTotalSamples = vgmInfo.VGMSmplPos;
							vgmInfo.VGMHead.lngLoopSamples = vgmInfo.VGMHead.lngTotalSamples;
							RhythmOn = true;
							break;
						}
						break;
					default:
						vgmInfo.VGMPos += 0x01;
					}
					break;
				case 0x80:
				case 0x90:
				case 0xA0:
				case 0xB0:
				case 0xE0:
					vgmInfo.VGMPos += 0x02;
					break;
				case 0xC0:
				case 0xD0:
					vgmInfo.VGMPos += 0x01;
					break;
				}
				if (Command < 0xF0)
					LastCmd = Command;
			}

			vgmInfo.VGMPos = TempLng;
			vgmInfo.VGMSmplPos = SmplPlayed;
		}

		SmplPlayed = SamplePlayback2VGM(vgmInfo.VGMSmplPlayed + SampleCount);
		while (true)
		{
			TempLng = GetMIDIDelay(&DataLen);
			if (vgmInfo.VGMSmplPos + (INT32)TempLng > SmplPlayed)
				break;
			vgmInfo.VGMSmplPos += TempLng;
			vgmInfo.VGMPos += DataLen;

			Command = vgmInfo.VGMData[vgmInfo.VGMPos];
			if (Command & 0x80)
				vgmInfo.VGMPos++;
			else
				Command = LastCmd;
			Channel = Command & 0x0F;

			if (DrumReg[0x00] & 0x20)
			{
				if (Channel < 0x0B)
				{
					CurChip = Channel / 0x06;
					Channel = Channel % 0x06;
				}
				else
				{
					Channel -= 0x0B;
					CurChip = Channel / 0x03;
					Channel = Channel % 0x03;
					if (CurChip == 0x01 && Channel == 0x00)
						Channel = 0x02;
					Channel += 0x06;
					// Map all drums to one chip
					CurChip = 0x00;
				}
			}
			else
			{
				CurChip = Channel / 0x09;
				Channel = Channel % 0x09;
			}
			CurChip = 0x00;

			RhythmOn = (Channel >= 0x06) && (DrumReg[CurChip] & 0x20);
			switch (Command & 0xF0)
			{
			case 0xF0:	// End Of File
				switch (Command)
				{
				case 0xFF:
					switch (vgmInfo.VGMData[vgmInfo.VGMPos + 0x00])
					{
					case 0x2F:
						if (vgmInfo.CMFMaxLoop != 0x01)
						{
							vgmInfo.VGMPos = vgmInfo.VGMHead.lngDataOffset;
							vgmInfo.VGMSmplPos -= vgmInfo.VGMHead.lngLoopSamples;
							vgmInfo.VGMSmplPlayed -= SampleVGM2Playback(vgmInfo.VGMHead.lngLoopSamples);
							SmplPlayed = SamplePlayback2VGM(vgmInfo.VGMSmplPlayed + SampleCount);
							vgmInfo.VGMCurLoop++;

							if (vgmInfo.CMFMaxLoop && vgmInfo.VGMCurLoop >= vgmInfo.CMFMaxLoop)
								vgmInfo.FadePlay = true;
							if (vgmInfo.FadePlay && !vgmInfo.FadeTime)
								vgmInfo.VGMEnd = true;
						}
						else
						{
							vgmInfo.VGMEnd = true;
							break;
						}
						break;
					}
					break;
				default:
					vgmInfo.VGMPos += 0x01;
				}
				break;
			case 0x80:
			case 0x90:
				TempSht = MIDINote2FNum(vgmInfo.VGMData[vgmInfo.VGMPos + 0x00], ChnPitch[Channel]);
				if ((Command & 0xF0) == 0x80)
					NoteOn = false;
				else
					NoteOn = vgmInfo.VGMData[vgmInfo.VGMPos + 0x01] ? true : false;

				if (!RhythmOn)	// Set "Key On"
					TempSht |= (UINT8)NoteOn << 13;	// << (8+5)

				if (NoteOn)
				{
					for (CurChip = 0x00; CurChip < 0x02; CurChip++)
					{
						if (ChnNote[(CurChip << 4) | Channel] == 0xFF)
						{
							ChnNote[(CurChip << 4) | Channel] = vgmInfo.VGMData[vgmInfo.VGMPos + 0x00];
							break;
						}
					}
					if (CurChip >= 0x02)
					{
						CurChip = 0x00;
						ChnNote[(CurChip << 4) | Channel] = vgmInfo.VGMData[vgmInfo.VGMPos + 0x00];
					}
				}
				else
				{
					for (CurChip = 0x00; CurChip < 0x02; CurChip++)
					{
						if (ChnNote[(CurChip << 4) | Channel] != 0xFF)
						{
							ChnNote[(CurChip << 4) | Channel] = 0xFF;
							break;
						}
					}
				}
				if (CurChip >= 0x02)
					CurChip = 0xFF;

				if (CurChip != 0xFF)
				{
					if (NoteOn)
					{
						if (!RhythmOn)
							SendMIDIVolume(CurChip, Channel | (RhythmOn << 7), Command,
								ChnIns[Command & 0x0F], vgmInfo.VGMData[vgmInfo.VGMPos + 0x01]);
					}
					if (NoteOn || !RhythmOn)
					{
						chip_reg_write(0x09, CurChip, 0x00, 0xA0 | Channel, TempSht & 0xFF);
						chip_reg_write(0x09, CurChip, 0x00, 0xB0 | Channel, TempSht >> 8);
					}

					if (RhythmOn)
					{
						TempByt = 0x0F - (Command & 0x0F);
						DrumReg[CurChip] &= ~(0x01 << TempByt);
						if (NoteOn)
							chip_reg_write(0x09, CurChip, 0x00, 0xBD, DrumReg[CurChip]);
						DrumReg[CurChip] |= (UINT8)NoteOn << TempByt;
						chip_reg_write(0x09, CurChip, 0x00, 0xBD, DrumReg[CurChip]);
					}
				}
				vgmInfo.VGMPos += 0x02;
				break;
			case 0xB0:
				NoteOn = false;
				switch (vgmInfo.VGMData[vgmInfo.VGMPos + 0x00])
				{
				case 0x66:	// Marker
					break;
				case 0x67:	// Rhythm Mode
					if (!vgmInfo.VGMData[vgmInfo.VGMPos + 0x01])
					{	// Set Rhythm Mode off
						DrumReg[0x00] = 0xC0;
						DrumReg[0x01] = 0xC0;
					}
					else
					{	// Set Rhythm Mode on
						DrumReg[0x00] = 0xE0;
						DrumReg[0x01] = 0xE0;
					}
					chip_reg_write(0x09, CurChip, 0x00, 0xBD, DrumReg[0x00]);
					chip_reg_write(0x09, CurChip, 0x00, 0xBD, DrumReg[0x01]);
					break;
				case 0x68:	// Pitch Upward
					ChnPitch[Channel] = +vgmInfo.VGMData[vgmInfo.VGMPos + 0x01];
					NoteOn = true;
					break;
				case 0x69:	// Pitch Downward
					ChnPitch[Channel] = -vgmInfo.VGMData[vgmInfo.VGMPos + 0x01];
					NoteOn = true;
					break;
				}
				if (NoteOn)
				{
					for (CurChip = 0x00; CurChip < 0x02; CurChip++)
					{
						TempByt = ChnNote[(CurChip << 4) | Channel];
						if (!RhythmOn && TempByt != 0xFF)
						{
							TempSht = MIDINote2FNum(TempByt, ChnPitch[Channel]);
							TempSht |= 0x01 << 13;	// << (8+5)

							chip_reg_write(0x09, CurChip, 0x00, 0xA0 | Channel, TempSht & 0xFF);
							chip_reg_write(0x09, CurChip, 0x00, 0xB0 | Channel, TempSht >> 8);
						}
					}
				}
				vgmInfo.VGMPos += 0x02;
				break;
			case 0xC0:
				TempByt = vgmInfo.VGMData[vgmInfo.VGMPos + 0x00];
				if (TempByt >= vgmInfo.CMFInsCount)
				{
					//VGMPos += 0x01;
					//break;
					TempByt %= vgmInfo.CMFInsCount;
				}

				TempIns = vgmInfo.CMFIns + TempByt;
				ChnIns[Command & 0x0F] = TempByt;

				OpBase = (Channel / 0x03) * 0x08 + (Channel % 0x03);
				if (!RhythmOn)
				{
					OpMask = 0x03;
				}
				else
				{
					switch (Command & 0x0F)
					{
					case 0x0B:	// Bass Drum
						OpMask = 0x03;
						break;
					case 0x0F:	// Hi Hat
						OpMask = 0x01;
						//Channel = 0x01;	// PLAY.EXE really does this sometimes
						//OpBase = 0x01;
						break;
					case 0x0C:	// Snare Drum
						OpMask = 0x02;
						break;
					case 0x0D:	// Tom Tom
						OpMask = 0x01;
						break;
					case 0x0E:	// Cymbal
						OpMask = 0x02;
						break;
					default:
						OpMask = 0x03;
						break;
					}
				}

				for (CurChip = 0x00; CurChip < 0x02; CurChip++)
				{
					TempByt = 0x00;
					if (OpMask & 0x01)
					{
						// Write Operator 1
						chip_reg_write(0x09, CurChip, 0x00,
							0x20 | (OpBase + 0x00), TempIns->Character[TempByt]);
						chip_reg_write(0x09, CurChip, 0x00,
							0x40 | (OpBase + 0x00), TempIns->ScaleLevel[TempByt]);
						chip_reg_write(0x09, CurChip, 0x00,
							0x60 | (OpBase + 0x00), TempIns->AttackDelay[TempByt]);
						chip_reg_write(0x09, CurChip, 0x00,
							0x80 | (OpBase + 0x00), TempIns->SustnRelease[TempByt]);
						chip_reg_write(0x09, CurChip, 0x00,
							0xE0 | (OpBase + 0x00), TempIns->WaveSelect[TempByt]);
						TempByt++;
					}
					if (OpMask & 0x02)
					{
						// Write Operator 2
						chip_reg_write(0x09, CurChip, 0x00,
							0x20 | (OpBase + 0x03), TempIns->Character[TempByt]);
						chip_reg_write(0x09, CurChip, 0x00,
							0x40 | (OpBase + 0x03), TempIns->ScaleLevel[TempByt]);
						chip_reg_write(0x09, CurChip, 0x00,
							0x60 | (OpBase + 0x03), TempIns->AttackDelay[TempByt]);
						chip_reg_write(0x09, CurChip, 0x00,
							0x80 | (OpBase + 0x03), TempIns->SustnRelease[TempByt]);
						chip_reg_write(0x09, CurChip, 0x00,
							0xE0 | (OpBase + 0x03), TempIns->WaveSelect[TempByt]);
						TempByt++;
					}

					chip_reg_write(0x09, CurChip, 0x00, 0xC0 | Channel, TempIns->FeedbConnect);
				}

				vgmInfo.VGMPos += 0x01;
				break;
			case 0xA0:
			case 0xE0:
				vgmInfo.VGMPos += 0x02;
				break;
			case 0xD0:
				vgmInfo.VGMPos += 0x01;
				break;
			}
			if (Command < 0xF0)
				LastCmd = Command;

			if (vgmInfo.VGMEnd)
				break;
		}
		break;
	case 0x02:	// DosBox RAW OPL File Mode
		NoteOn = false;
		if (!SampleCount)
		{
			// was done during Init (Emulation Core / Chip Mapper for real FM),
			// but Chip Mapper's init-routine now works different
			switch (vgmInfo.DROInf.iHardwareType)
			{
			case 0x00:	// OPL 2
				for (TempByt = 0xFF; TempByt >= 0x20; TempByt--)
					chip_reg_write(0x09, 0x00, 0x00, TempByt, 0x00);
				chip_reg_write(0x09, 0x00, 0x00, 0x08, 0x00);
				chip_reg_write(0x09, 0x00, 0x00, 0x01, 0x00);
				break;
			case 0x01:	// Dual OPL 2
				for (TempByt = 0xFF; TempByt >= 0x20; TempByt--)
					chip_reg_write(0x09, 0x00, 0x00, TempByt, 0x00);
				chip_reg_write(0x09, 0x00, 0x00, 0x08, 0x00);
				chip_reg_write(0x09, 0x00, 0x00, 0x01, 0x00);
				//Sleep(1);
				for (TempByt = 0xFF; TempByt >= 0x20; TempByt--)
					chip_reg_write(0x09, 0x01, 0x00, TempByt, 0x00);
				chip_reg_write(0x09, 0x01, 0x00, 0x08, 0x00);
				chip_reg_write(0x09, 0x01, 0x00, 0x01, 0x00);
				break;
			case 0x02:	// OPL 3
				for (TempByt = 0xFF; TempByt >= 0x20; TempByt--)
					chip_reg_write(0x0C, 0x00, 0x00, TempByt, 0x00);
				chip_reg_write(0x0C, 0x00, 0x00, 0x08, 0x00);
				chip_reg_write(0x0C, 0x00, 0x00, 0x01, 0x00);
				//Sleep(1);
				for (TempByt = 0xFF; TempByt >= 0x20; TempByt--)
					chip_reg_write(0x0C, 0x00, 0x01, TempByt, 0x00);
				//chip_reg_write(0x0C, 0x00, 0x01, 0x05, 0x00);
				chip_reg_write(0x0C, 0x00, 0x01, 0x04, 0x00);
				break;
			default:
				for (TempByt = 0xFF; TempByt >= 0x20; TempByt--)
					chip_reg_write(0x09, 0x00, 0x00, TempByt, 0x00);
				break;
			}
			Sleep(1);
			NoteOn = true && (vgmInfo.DROHead.iVersionMajor < 2);
			OpBase = 0x00;
		}

		SmplPlayed = SamplePlayback2VGM(vgmInfo.VGMSmplPlayed + SampleCount);
		while (vgmInfo.VGMSmplPos <= SmplPlayed)
		{
			Command = vgmInfo.VGMData[vgmInfo.VGMPos + 0x00];

			if (Command == vgmInfo.DROInf.iShortDelayCode)
				Command = 0x00;
			else if (Command == vgmInfo.DROInf.iLongDelayCode)
				Command = 0x01;
			else
			{
				switch (vgmInfo.DROHead.iVersionMajor)
				{
				case 0:
				case 1:
					if (Command <= 0x01)
						Command = 0xFF;
					break;
				case 2:
					Command = 0xFF;
					break;
				}
			}

			// DRO v0/v1 only: "Delay-Command" fix
			if (NoteOn)	// "Delay"-Command during init-phase?
			{
				if (Command < OpBase)	// out of operator range?
				{
					NoteOn = false;	// it's a delay
				}
				else
				{
					OpBase = Command;	// it's a command
					Command = 0xFF;
				}
			}
		DRO_CommandSwitch:
			switch (Command)
			{
			case 0x00:	// 1-Byte Delay
				vgmInfo.VGMSmplPos += 0x01 + vgmInfo.VGMData[vgmInfo.VGMPos + 0x01];
				vgmInfo.VGMPos += 0x02;
				break;
			case 0x01:	// 2-Byte Delay
				switch (vgmInfo.DROHead.iVersionMajor)
				{
				case 0:
				case 1:
					::memcpy(&TempSht, &vgmInfo.VGMData[vgmInfo.VGMPos + 0x01], 0x02);
					if ((TempSht & 0xFF00) == 0xBD00)
					{
						Command = 0xFF;
						goto DRO_CommandSwitch;
					}
					vgmInfo.VGMSmplPos += 0x01 + TempSht;
					vgmInfo.VGMPos += 0x03;
					break;
				case 2:
					vgmInfo.VGMSmplPos += (0x01 + vgmInfo.VGMData[vgmInfo.VGMPos + 0x01]) << 8;
					vgmInfo.VGMPos += 0x02;
					break;
				}
				break;
			case 0x02:	// Use 1st OPL Chip
			case 0x03:	// Use 2nd OPL Chip
				CurChip = Command & 0x01;
				if (CurChip > 0x00 && vgmInfo.DROInf.iHardwareType == 0x00)
				{
					//CurChip = 0x00;
					if (!vgmInfo.CmdList[0x02])
					{
						printf("More chips used than defined in header!\n");
						vgmInfo.CmdList[0x02] = true;
					}
				}
				vgmInfo.VGMPos += 0x01;
				break;
			case 0x04:	// Escape
				vgmInfo.VGMPos += 0x01;
				// No break (execute following Register)
			default:
				Command = vgmInfo.VGMData[vgmInfo.VGMPos + 0x00];
				if (vgmInfo.DROCodemap)
				{
					CurChip = (Command & 0x80) ? 0x01 : 0x00;
					Command &= 0x7F;
					if (Command < vgmInfo.DROInf.iCodemapLength)
						Command = vgmInfo.DROCodemap[Command];
					else
						Command = Command;
					switch (vgmInfo.DROInf.iHardwareType)
					{
					case 0x00:
						if (CurChip)
						{
							if (!vgmInfo.CmdList[0x02])
							{
								printf("More chips used than defined in header!\n");
								vgmInfo.CmdList[0x02] = true;
							}
							//CurChip = 0x00;
							//Command = 0x00;
						}
						break;
					case 0x01:
					case 0x02:
						break;
					}
				}
				switch (vgmInfo.DROInf.iHardwareType)
				{
				case 0x00:	// OPL 2
					if (CurChip > 0x00)
						break;
					chip_reg_write(0x09, 0x00, 0x00, Command, vgmInfo.VGMData[vgmInfo.VGMPos + 0x01]);
					break;
				case 0x01:
					chip_reg_write(0x09, CurChip, 0x00, Command, vgmInfo.VGMData[vgmInfo.VGMPos + 0x01]);
					break;
				case 0x02:	// OPL 3
					chip_reg_write(0x0C, 0x00, CurChip, Command, vgmInfo.VGMData[vgmInfo.VGMPos + 0x01]);
					break;
				default:
					chip_reg_write(0x09, CurChip, 0x00, Command, vgmInfo.VGMData[vgmInfo.VGMPos + 0x01]);
					break;
				}
				vgmInfo.VGMPos += 0x02;
				break;
			}

			if (vgmInfo.VGMPos >= vgmInfo.VGMDataLen)
			{
				if (vgmInfo.VGMHead.lngTotalSamples != (UINT32)vgmInfo.VGMSmplPos)
				{
					printf("Warning! Header Samples: %u\t Counted Samples: %u\n", vgmInfo.VGMHead.lngTotalSamples, vgmInfo.VGMSmplPos);
					vgmInfo.VGMHead.lngTotalSamples = vgmInfo.VGMSmplPos;
					vgmInfo.ErrorHappened = true;
				}
				vgmInfo.VGMEnd = true;
			}
			if (vgmInfo.VGMEnd)
				break;
		}
		break;
	}

	return;
}

UINT32 VGMData::GetMIDIDelay(UINT32* DelayLen)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	UINT32 CurPos;
	UINT32 DelayVal;

	CurPos = vgmInfo.VGMPos;
	DelayVal = 0x00;
	do
	{
		DelayVal = (DelayVal << 7) | (vgmInfo.VGMData[CurPos] & 0x7F);
	} while (vgmInfo.VGMData[CurPos++] & 0x80);

	if (DelayLen != NULL)
		*DelayLen = CurPos - vgmInfo.VGMPos;
	return DelayVal;
}

UINT16 VGMData::MIDINote2FNum(UINT8 Note, INT8 Pitch)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	const double CHIP_RATE = 3579545.0 / 72.0;	// ~49716
	double FreqVal;
	INT8 BlockVal;
	UINT16 KeyVal;

	FreqVal = 440.0 * pow(2, (Note - 69 + Pitch / 256.0) / 12.0);
	BlockVal = (Note / 12) - 1;
	if (BlockVal < 0x00)
		BlockVal = 0x00;
	else if (BlockVal > 0x07)
		BlockVal = 0x07;
	KeyVal = (UINT16)(FreqVal * pow(2, 20 - BlockVal) / CHIP_RATE + 0.5);

	return (BlockVal << 10) | KeyVal;	// << (8+2)
}

void VGMData::SendMIDIVolume(UINT8 ChipID, UINT8 Channel, UINT8 Command, UINT8 ChnIns, UINT8 Volume)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;

	bool RhythmOn;
	UINT8 TempByt;
	//UINT16 TempSht;
	UINT32 TempLng;
	UINT8 OpBase;	// Operator Base
	CMF_INSTRUMENT* TempIns;
	UINT8 OpMask;
	INT8 OpVol;
	INT8 NoteVol;

	RhythmOn = (Channel >> 7) & 0x01;
	Channel &= 0x7F;

	// Refresh Total Level (Volume)
	TempIns = vgmInfo.CMFIns + ChnIns;
	OpBase = (Channel / 0x03) * 0x08 + (Channel % 0x03);

	if (!RhythmOn)
	{
		TempLng = 0x01;
		OpMask = 0x03;
	}
	else
	{
		//TempLng = 0x01;
		switch (Command & 0x0F)
		{
		case 0x0B:	// Bass Drum
			OpMask = 0x00;
			break;
		case 0x0F:	// Hi Hat
			OpMask = 0x00;
			break;
		case 0x0C:	// Snare Drum
			OpMask = 0x01;
			break;
		case 0x0D:	// Tom Tom
			OpMask = 0x00;
			break;
		case 0x0E:	// Cymbal
			OpMask = 0x01;
			break;
		default:
			OpMask = 0x00;
			break;
		}
		TempLng = OpMask;
		OpMask *= 0x03;
	}

	// Verified with PLAY.EXE
	OpVol = (Volume + 0x04) >> 3;
	OpVol = 0x10 - (OpVol << 1);
	if (OpVol < 0x00)
		OpVol >>= 1;
	NoteVol = (TempIns->ScaleLevel[TempLng] & 0x3F) + OpVol;
	if (NoteVol < 0x00)
		NoteVol = 0x00;

	TempByt = NoteVol | (TempIns->ScaleLevel[TempLng] & 0xC0);
	chip_reg_write(0x09, ChipID, 0x00, 0x40 | (OpBase + OpMask), TempByt);

	return;
}

bool VGMData::OpenOtherFile(const char* FileName)
{
	assert(impl);
	VGMInfo& vgmInfo = impl->vgmInfo;
	gzFile hFile = impl->gzfile;

	UINT32 FileSize;
	UINT32 fccHeader;
	UINT32 CurPos;
	UINT32 TempLng;
	UINT16 FileVer;
	const char* TempStr;
	DRO_VER_HEADER_1 DRO_V1;

	FileSize = GetGZFileLength(FileName);

	vgmInfo.FileMode = 0x00;
	hFile = gzopen(FileName, "rb");
	if (hFile == NULL)
		return false;

	gzseek(hFile, 0x00, SEEK_SET);
	gzread(hFile, &fccHeader, 4);
	switch (fccHeader)
	{
	case FCC_VGM:
		vgmInfo.FileMode = 0xFF;	// should already be opened
		break;
	case FCC_CMF:
		vgmInfo.FileMode = 0x01;
		break;
	case FCC_DRO1:
		gzread(hFile, &fccHeader, 4);
		if (fccHeader == FCC_DRO2)
			vgmInfo.FileMode = 0x02;
		else
			vgmInfo.FileMode = 0xFF;
		break;
	default:
		vgmInfo.FileMode = 0xFF;
		break;
	}
	if (vgmInfo.FileMode == 0xFF)
		goto OpenErr;

	vgmInfo.VGMTag.strTrackNameE  = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strTrackNameJ  = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strGameNameE   = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strGameNameJ   = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strSystemNameE = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strSystemNameJ = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strAuthorNameE = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strAuthorNameJ = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strReleaseDate = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strCreator     = const_cast<wchar_t*>(L"");
	vgmInfo.VGMTag.strNotes       = const_cast<wchar_t*>(L"");

	switch (vgmInfo.FileMode)
	{
	case 0x00:	// VGM File
		break;
	case 0x01:	// CMF File
	case 0x02:	// DosBox RAW OPL
		vgmInfo.VGMTag.strGameNameE = (wchar_t*)malloc(0x10 * sizeof(wchar_t*));
		wcscpy(vgmInfo.VGMTag.strGameNameE, L"    Player");
		vgmInfo.VGMTag.strSystemNameE = const_cast<wchar_t*>(L"PC / MS-DOS");
		break;
	}

	vgmInfo.VGMDataLen = FileSize;

	switch (vgmInfo.FileMode)
	{
	case 0x00:	// VGM File
		// already done by OpenVGMFile
		break;
	case 0x01:	// CMF File
		// Read Data
		vgmInfo.VGMData = (UINT8*)malloc(vgmInfo.VGMDataLen);
		if (vgmInfo.VGMData == NULL)
			goto OpenErr;
		gzseek(hFile, 0x00, SEEK_SET);
		gzread(hFile, vgmInfo.VGMData, vgmInfo.VGMDataLen);

#ifdef VGM_LITTLE_ENDIAN
		::memcpy(&vgmInfo.CMFHead, &vgmInfo.VGMData[0x00], sizeof(CMF_HEADER));
#else
		vgmInfo.CMFHead.fccCMF = ReadLE32(&vgmInfo.VGMData[0x00]);
		vgmInfo.CMFHead.shtVersion = ReadLE16(&vgmInfo.VGMData[0x04]);
		vgmInfo.CMFHead.shtOffsetInsData = ReadLE16(&vgmInfo.VGMData[0x06]);
		vgmInfo.CMFHead.shtOffsetMusData = ReadLE16(&vgmInfo.VGMData[0x08]);
		vgmInfo.CMFHead.shtTickspQuarter = ReadLE16(&vgmInfo.VGMData[0x0A]);
		vgmInfo.CMFHead.shtTickspSecond = ReadLE16(&vgmInfo.VGMData[0x0C]);
		vgmInfo.CMFHead.shtOffsetTitle = ReadLE16(&vgmInfo.VGMData[0x0E]);
		vgmInfo.CMFHead.shtOffsetAuthor = ReadLE16(&vgmInfo.VGMData[0x10]);
		vgmInfo.CMFHead.shtOffsetComments = ReadLE16(&vgmInfo.VGMData[0x12]);
		memcpy(vgmInfo.CMFHead.bytChnUsed, &vgmInfo.VGMData[0x14], 0x10);
		vgmInfo.CMFHead.shtInstrumentCount = ReadLE16(&vgmInfo.VGMData[0x24]);
		vgmInfo.CMFHead.shtTempo = ReadLE16(&vgmInfo.VGMData[0x26]);
#endif

		if (vgmInfo.CMFHead.shtVersion == 0x0100)
		{
			vgmInfo.CMFHead.shtInstrumentCount &= 0x00FF;
			vgmInfo.CMFHead.shtTempo = (UINT16)(60.0 *
				vgmInfo.CMFHead.shtTickspQuarter / vgmInfo.CMFHead.shtTickspSecond + 0.5);
		}

		if (vgmInfo.CMFHead.shtOffsetTitle)
		{
			TempStr = (char*)&vgmInfo.VGMData[vgmInfo.CMFHead.shtOffsetTitle];
			TempLng = strlen(TempStr) + 0x01;
			vgmInfo.VGMTag.strTrackNameE = (wchar_t*)malloc(TempLng * sizeof(wchar_t));
			mbstowcs(vgmInfo.VGMTag.strTrackNameE, TempStr, TempLng);
		}
		vgmInfo.VGMTag.strGameNameE[0x00] = 'C';
		vgmInfo.VGMTag.strGameNameE[0x01] = 'M';
		vgmInfo.VGMTag.strGameNameE[0x02] = 'F';
		if (vgmInfo.CMFHead.shtOffsetAuthor)
		{
			TempStr = (char*)&vgmInfo.VGMData[vgmInfo.CMFHead.shtOffsetAuthor];
			TempLng = strlen(TempStr) + 0x01;
			vgmInfo.VGMTag.strAuthorNameE = (wchar_t*)malloc(TempLng * sizeof(wchar_t));
			mbstowcs(vgmInfo.VGMTag.strAuthorNameE, TempStr, TempLng);
		}
		if (vgmInfo.CMFHead.shtOffsetComments)
		{
			TempStr = (char*)&vgmInfo.VGMData[vgmInfo.CMFHead.shtOffsetComments];
			TempLng = strlen(TempStr) + 0x01;
			vgmInfo.VGMTag.strNotes = (wchar_t*)malloc(TempLng * sizeof(wchar_t));
			mbstowcs(vgmInfo.VGMTag.strNotes, TempStr, TempLng);
		}

		vgmInfo.CMFInsCount = vgmInfo.CMFHead.shtInstrumentCount;
		TempLng = vgmInfo.CMFInsCount * sizeof(CMF_INSTRUMENT);
		vgmInfo.CMFIns = (CMF_INSTRUMENT*)malloc(TempLng);
		::memcpy(vgmInfo.CMFIns, &vgmInfo.VGMData[vgmInfo.CMFHead.shtOffsetInsData], TempLng);

		memset(&vgmInfo.VGMHead, 0x00, sizeof(VGM_HEADER));
		vgmInfo.VGMHead.lngEOFOffset = vgmInfo.VGMDataLen;
		vgmInfo.VGMHead.lngVersion = vgmInfo.CMFHead.shtVersion;
		vgmInfo.VGMHead.lngDataOffset = vgmInfo.CMFHead.shtOffsetMusData;
		vgmInfo.VGMSampleRate = vgmInfo.CMFHead.shtTickspSecond;
		vgmInfo.VGMHead.lngTotalSamples = 0;
		vgmInfo.VGMHead.lngHzYM3812 = 3579545 | 0x40000000;

		break;
	case 0x02:	// DosBox RAW OPL
		// Read Data
		vgmInfo.VGMData = (UINT8*)malloc(vgmInfo.VGMDataLen);
		if (vgmInfo.VGMData == NULL)
			goto OpenErr;
		gzseek(hFile, 0x00, SEEK_SET);
		vgmInfo.VGMDataLen = gzread(hFile, vgmInfo.VGMData, vgmInfo.VGMDataLen);

		vgmInfo.VGMTag.strGameNameE[0x00] = 'D';
		vgmInfo.VGMTag.strGameNameE[0x01] = 'R';
		vgmInfo.VGMTag.strGameNameE[0x02] = 'O';

		memset(&vgmInfo.VGMHead, 0x00, sizeof(VGM_HEADER));
		CurPos = 0x00;
#ifdef VGM_LITTLE_ENDIAN
		::memcpy(&vgmInfo.DROHead, &vgmInfo.VGMData[CurPos], sizeof(DRO_HEADER));
#else
		::memcpy(vgmInfo.DROHead.cSignature, &vgmInfo.VGMData[CurPos + 0x00], 0x08);
		vgmInfo.DROHead.iVersionMajor = ReadLE16(&vgmInfo.VGMData[CurPos + 0x08]);
		vgmInfo.DROHead.iVersionMinor = ReadLE16(&vgmInfo.VGMData[CurPos + 0x0A]);
#endif
		CurPos += sizeof(DRO_HEADER);

		::memcpy(&TempLng, &vgmInfo.VGMData[0x08], sizeof(UINT32));
		if (TempLng & 0xFF00FF00)
		{
			// DosBox Version 0.61
			// this version didn't contain Version Bytes
			CurPos = 0x08;
			vgmInfo.DROHead.iVersionMajor = 0x00;
			vgmInfo.DROHead.iVersionMinor = 0x00;
		}
		else if (!(TempLng & 0x0000FFFF))
		{
			// DosBox Version 0.63
			// the order of the Version Bytes is swapped in this version
			FileVer = vgmInfo.DROHead.iVersionMinor;
			if (FileVer == 0x01)
			{
				vgmInfo.DROHead.iVersionMinor = vgmInfo.DROHead.iVersionMajor;
				vgmInfo.DROHead.iVersionMajor = FileVer;
			}
		}
		vgmInfo.VGMHead.lngEOFOffset = vgmInfo.VGMDataLen;
		vgmInfo.VGMHead.lngVersion = (vgmInfo.DROHead.iVersionMajor << 8) |
			((vgmInfo.DROHead.iVersionMinor & 0xFF) << 0);
		vgmInfo.VGMSampleRate = 1000;

		if (vgmInfo.DROHead.iVersionMajor > 2)
			vgmInfo.DROHead.iVersionMajor = 2;
		switch (vgmInfo.DROHead.iVersionMajor)
		{
		case 0:	// Version 0 (DosBox Version 0.61)
		case 1:	// Version 1 (DosBox Version 0.63)
			switch (vgmInfo.DROHead.iVersionMajor)
			{
			case 0:	// Version 0
				DRO_V1.iLengthMS = ReadLE32(&vgmInfo.VGMData[CurPos + 0x00]);
				DRO_V1.iLengthBytes = ReadLE32(&vgmInfo.VGMData[CurPos + 0x04]);
				DRO_V1.iHardwareType = vgmInfo.VGMData[CurPos + 0x08];
				CurPos += 0x09;
				break;
			case 1:	// Version 1
				DRO_V1.iLengthMS = ReadLE32(&vgmInfo.VGMData[CurPos + 0x00]);
				DRO_V1.iLengthBytes = ReadLE32(&vgmInfo.VGMData[CurPos + 0x04]);
				DRO_V1.iHardwareType = ReadLE32(&vgmInfo.VGMData[CurPos + 0x08]);
				CurPos += 0x0C;
				break;
			}

			vgmInfo.DROInf.iLengthPairs = DRO_V1.iLengthBytes >> 1;
			vgmInfo.DROInf.iLengthMS = DRO_V1.iLengthMS;
			switch (DRO_V1.iHardwareType)
			{
			case 0x01:	// Single OPL3
				vgmInfo.DROInf.iHardwareType = 0x02;
				break;
			case 0x02:	// Dual OPL2
				vgmInfo.DROInf.iHardwareType = 0x01;
				break;
			default:
				vgmInfo.DROInf.iHardwareType = (UINT8)DRO_V1.iHardwareType;
				break;
			}
			vgmInfo.DROInf.iFormat = 0x00;
			vgmInfo.DROInf.iCompression = 0x00;
			vgmInfo.DROInf.iShortDelayCode = 0x00;
			vgmInfo.DROInf.iLongDelayCode = 0x01;
			vgmInfo.DROInf.iCodemapLength = 0x00;

			break;
		case 2:	// Version 2 (DosBox Version 0.73)
			// sizeof(DRO_VER_HEADER_2) returns 0x10, but the exact size is 0x0E
			//memcpy(&DROInf, &VGMData[CurPos], 0x0E);
			vgmInfo.DROInf.iLengthPairs = ReadLE32(&vgmInfo.VGMData[CurPos + 0x00]);
			vgmInfo.DROInf.iLengthMS = ReadLE32(&vgmInfo.VGMData[CurPos + 0x04]);
			vgmInfo.DROInf.iHardwareType = vgmInfo.VGMData[CurPos + 0x08];
			vgmInfo.DROInf.iFormat = vgmInfo.VGMData[CurPos + 0x09];
			vgmInfo.DROInf.iCompression = vgmInfo.VGMData[CurPos + 0x0A];
			vgmInfo.DROInf.iShortDelayCode = vgmInfo.VGMData[CurPos + 0x0B];
			vgmInfo.DROInf.iLongDelayCode = vgmInfo.VGMData[CurPos + 0x0C];
			vgmInfo.DROInf.iCodemapLength = vgmInfo.VGMData[CurPos + 0x0D];
			CurPos += 0x0E;

			break;
		}

		if (vgmInfo.DROInf.iCodemapLength)
		{
			vgmInfo.DROCodemap = (UINT8*)malloc(vgmInfo.DROInf.iCodemapLength * sizeof(UINT8));
			::memcpy(vgmInfo.DROCodemap, &vgmInfo.VGMData[CurPos], vgmInfo.DROInf.iCodemapLength);
			CurPos += vgmInfo.DROInf.iCodemapLength;
		}
		else
		{
			vgmInfo.DROCodemap = NULL;
		}

		vgmInfo.VGMHead.lngDataOffset = CurPos;
		vgmInfo.VGMHead.lngTotalSamples = vgmInfo.DROInf.iLengthMS;
		switch (vgmInfo.DROInf.iHardwareType)
		{
		case 0x00:	// Single OPL2 Chip
			vgmInfo.VGMHead.lngHzYM3812 = 3579545;
			break;
		case 0x01:	// Dual OPL2 Chip
			vgmInfo.VGMHead.lngHzYM3812 = 3579545 | 0xC0000000;
			break;
		case 0x02:	// Single OPL3 Chip
			vgmInfo.VGMHead.lngHzYMF262 = 14318180;
			break;
		default:
			vgmInfo.VGMHead.lngHzYM3812 = 3579545 | 0x40000000;
			break;
		}

		break;
	}

	gzclose(hFile);
	return true;

OpenErr:

	gzclose(hFile);
	return false;
}