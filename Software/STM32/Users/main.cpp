#include <Array.h>
#include "Platform.h"
#include "VGMFile.h"
#include "VGMAudioPlayer.h"
#include "VGMWaveFormViewer.h"
#ifdef STM32
#include <lcd2.h>
#else
#endif
#include "AudioDevice.h"

/*
#include <timer.h>

void initAudio(f32 freq1, f32 freq2)
{

}

void filloutputBuffer(AudioDevice* audioDevice)
{
	u16 testAudio[VGM_SAMPLE_COUNT*2];
	u32 i;
	float ratio1 = 1.0f / VGM_SAMPLE_COUNT * 1000 / 60.0f * 2.0 * 3.1416;
	float ratio2 = 1.0f / VGM_SAMPLE_COUNT * 2000 / 60.0f * 2.0 * 3.1416;
	float ang1 = 0;
	float ang2 = 0;
	for(i=0; i<VGM_SAMPLE_COUNT; i+=1)
	{
		ang1 += ratio1;
		ang2 += ratio2;
		testAudio[(i<<1) + 0] = 16384 * sin(ang1) + 16384;
		testAudio[(i<<1) + 1] = 16384 * sin(ang2) + 16384;
	}

	//memset(&testAudio[0], 0, VGM_SAMPLE_COUNT * 2);
	if (!audioDevice->queue(&testAudio[0], VGM_SAMPLE_COUNT * 2 * 2))
		return;

	if (audioDevice->getDeviceState() != 3)
	{
		audioDevice->play();
		//outputDevice.setVolume(1.0);
		//outputDevice.setPlayRate(1.0);
	}
}
*/

void LCDTest()
{
	//LCD_Initialize();
	//LCD_Test();	
}

void PWMTest()
{
	/*
	u32 bufferCount = 4;
	initAudio(1, 2);

	AudioDevice audioDevice;
	boolean ok = audioDevice.open(2, 16, 22050, bufferCount);
	if(ok)
	{
		//audioDevice.play();
		while(1)
		{
			audioDevice.setVolume(1.0f);
			float volume = audioDevice.getVolume();
			audioDevice.setPlayRate(1.09);
			float playrate = audioDevice.getPlayRate();

			if(audioDevice.getQueued()<bufferCount/2)
			{
				filloutputBuffer(&audioDevice);
			}

			audioDevice.update();
		};

		audioDevice.stop();
		audioDevice.close();
	}

	/*
	//u32 sampleRate = 44100;
	u32 sampleRate = 22050;
	//u32 sampleRate = 11025;
	u32 bpp = 10;
	//u32 bpp = 11;
	//u32 bpp = 12;
	u32 max = 1024;//512//pow(2, bpp);
	//u32 max = 2048;//1024//pow(2, bpp);
	//u32 max = 4096;//2048//pow(2, bpp);
	u32 interruptsPerUpdate = 3.18877551; // 1.594387755 // 72*1000*1000 / max / sampleRate;

	TIMER_Initialize(max, 0); 	// Frequency is 720kHz: Tout = (arr+1)(psc+1)/TClk, assume APB1 no prescale, TClk 72Mhz
								// 72000000/(99+1) = 720KHZ, no prescale clock
	GPIOB->BRR = GPIO_Pin_1;
	//TIMER_PWM_Initialize(1);
	//TIMER_PWM_Initialize(2);
	TIMER_PWM_Initialize(3);
	//TIMER_PWM_Initialize(4);
	//DMA_Initialize();

	TIMER_Start();
	TIMER_Stop();
	TIMER_Start();

	//NVIC_Initialize();
	//while(1)
	{
	}

	TIMER_Test();
	*/
}

#ifdef STM32
#include <ff.h>
enum SearchResult
{
	SR_OK = 0,
	SR_ERROR = 1,
	SR_NO_FILE = 2,
};

DIR dirSearchHandle;
FILINFO dirSearchInfo;
SearchResult beginSearchDirectory(char* path)
{
	FRESULT res = f_opendir(&dirSearchHandle, path);			/* Open the directory */
	if (res == FR_OK)
	{
		return SR_OK;
	}
	else
	{
		return SR_ERROR;
	}
}

SearchResult getNextDirectory(char* directory, int size)
{
	FRESULT res;

	dirSearchInfo.lfname = directory;
	dirSearchInfo.lfsize = size;
	res = f_readdir(&dirSearchHandle, &dirSearchInfo);			/* Read a directory item */
	if (res != FR_OK)
		return SR_ERROR;

	if (!(dirSearchInfo.fattrib & AM_DIR)) // skip if is not directory
		return getNextDirectory(directory, size);

	if (dirSearchInfo.lfname[0] == 0)
	{
		return SR_NO_FILE;
	}

	return SR_OK;
}

SearchResult rewindDirectory()
{
	f_readdir(&dirSearchHandle, 0); // rewind
}

void endSearchDirectory()
{
	f_closedir(&dirSearchHandle);
}

////////////////////////////////////////////////
DIR fileSearchHandle;
FILINFO fileSearchInfo;

SearchResult beginSearchFile(char* path)
{
	FRESULT res = f_opendir(&fileSearchHandle, path);			/* Open the directory */
	if (res == FR_OK)
	{
		return SR_OK;
	}
	else
	{
		return SR_ERROR;
	}
}

bool isExt(const char* path, const char* ext)
{
	return (strstr(path, ext) != 0);
}

SearchResult getNextFile(char* directory, int size, const char* ext)
{
	FRESULT res;

	fileSearchInfo.lfname = directory;
	fileSearchInfo.lfsize = size;
	res = f_readdir(&fileSearchHandle, &fileSearchInfo);			/* Read a directory item */
	if (res != FR_OK)
		return SR_ERROR;

	if (fileSearchInfo.lfname[0] == 0)
		return SR_NO_FILE;

	if ((fileSearchInfo.fattrib & AM_DIR)) // skip if is directory
		return getNextFile(directory, size, ext);

	if (!isExt(fileSearchInfo.lfname, ext))
		return getNextFile(directory, size, ext);

	return SR_OK;
}

void endSearchFile()
{
	f_closedir(&fileSearchHandle);
}

void loadBmp(const char* directory)
{
	char path[256];

	strncpy(path, directory, 256);
	strcat(path, "/Title.bmp");

	LCD_DrawImage(0, 0, path);
}

#else

enum SearchResult
{
	SR_OK = 0,
	SR_ERROR = 1,
	SR_NO_FILE = 2,
};

HANDLE dirSearchHandle;
SearchResult beginSearchDirectory(char* path)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE dirSearchHandle = FindFirstFile(path, &FindFileData);
	if(dirSearchHandle != INVALID_HANDLE_VALUE)
	{
		return SR_OK;
	}
	else
	{
		return SR_ERROR;
	}
}

SearchResult getNextDirectory(char* directory, int size)
{
	WIN32_FIND_DATA ffd;
	HRESULT res = FindNextFile(dirSearchHandle, &ffd);
	if(!res)
	{
		if(GetLastError() == ERROR_NO_MORE_FILES)
			return SR_NO_FILE;
		else
			return SR_ERROR;
	}
	
	if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		return getNextDirectory(directory, size);

	return SR_OK;
}

SearchResult rewindDirectory()
{
	//f_readdir(&dirSearchHandle, 0); // rewind
	return SR_OK;
}

void endSearchDirectory()
{
	FindClose(dirSearchHandle);
}

////////////////////////////////////////////////
HANDLE fileSearchHandle;
SearchResult beginSearchFile(char* path)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE fileSearchHandle = FindFirstFile(path, &FindFileData);
	if (fileSearchHandle != INVALID_HANDLE_VALUE)
	{
		return SR_OK;
	}
	else
	{
		return SR_ERROR;
	}
}

bool isExt(const char* path, const char* ext)
{
	return (strstr(path, ext) != 0);
}

SearchResult getNextFile(char* directory, int size, const char* ext)
{
	WIN32_FIND_DATA ffd;
	HRESULT res = FindNextFile(fileSearchHandle, &ffd);
	if (!res)
	{
		if (GetLastError() == ERROR_NO_MORE_FILES)
			return SR_NO_FILE;
		else
			return SR_ERROR;
	}

	if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return getNextDirectory(directory, size);

	if (!isExt(ffd.cFileName, ext))
		return getNextFile(directory, size, ext);

	return SR_OK;
}

void endSearchFile()
{
	FindClose(fileSearchHandle);
}

void loadBmp(const char* directory)
{
	char path[256];

	strncpy(path, directory, 256);
	strcat(path, "/Title.bmp");

	//LCD_DrawImage(0, 0, path);
}

#endif

int main()
{
	char path[256];
	int len;
	SearchResult sr;

	if (!Platform::initialize())
		return false;

#ifdef STM32
	LCD_Initialize();
	LCD_DrawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00, 0x00, 0x00);
	// PWMTest();	
#else
#endif

#ifdef TEST_YM2151	
	strcpy(path, "YM2151");
#else
	strcpy(path, "YM2612");
#endif
	
	if (beginSearchDirectory(path) == SR_OK)
	{
		while (1)
		{
#ifdef TEST_YM2151	
			strcpy(path, "YM2151");
#else
			strcpy(path, "YM2612");
#endif		
			strcat(path, "/");
			len = strlen(path);

			SearchResult sr = getNextDirectory(&path[len], 256 - len);
			if (sr == SR_ERROR)
			{
				endSearchDirectory();
				return -1;
			}
			if (sr == SR_NO_FILE)
			{
				rewindDirectory();
				continue;
			}

			loadBmp(path);

			if (beginSearchFile(path) == SR_OK)
			{
				strcat(path, "/");
				len = strlen(path);

				while (1)
				{
					SearchResult sr = getNextFile(&path[len], 256 - len, ".vgm");
					if (sr == SR_ERROR)
					{
						endSearchFile();
						return -1;
					}
					if (sr == SR_NO_FILE)
					{
						break;
					}

					// printf("%s/%s %s\n", path, fno.fname, fno.lfname);
					VGMFile vgmFile(path, 2, 16, 44100);
					VGMAudioPlayer vgmAudioPlayer;
					vgmFile.attach(vgmAudioPlayer);
#ifdef STM32
#else
					//VGMWaveFormViewer vgmWaveFormViewer1("Waveform Viewer", 40, 40, 640, 480, VGMWaveFormViewer::Skin());
					//VGMSpectrumViewer vgmWaveFormViewer2("Frequency Viewer", 40 + 640, 40, 640, 480, VGMSpectrumViewer::Skin());
					//VGMFireSpectrumViewer vgmWaveFormViewer3("Frequency Viewer", 40 + 640 * 2, 40, 320, 240, VGMFireSpectrumViewer::Skin());\
						//vgmFile.attach(vgmWaveFormViewer1);
						//vgmFile.attach(vgmWaveFormViewer2);
						//vgmFile.attach(vgmWaveFormViewer3);
#endif
					if (!vgmFile.open())
					{
						return -1;
					}

					while (1)
					{
						if (!Platform::update())
							return -1;

						if (vgmAudioPlayer.getQueued() < VGM_OUTPUT_BUFFER_COUNT / 2)
						{
							// printf("vgmDataOpenALPlayer.getQueued() %d\n", vgmAudioPlayer.getQueued());
							vgmFile.requestUpdateData();
						}

						if (!vgmFile.update())
						{
							break;
						}
					}

					vgmFile.close();
				}


				endSearchFile();
			}
		};

		endSearchDirectory();
	}

	Platform::terminate();

	while (1)
	{
	};
}