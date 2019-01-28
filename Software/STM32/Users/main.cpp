#include <Array.h>
#include "Platform.h"
#include "VGMFile.h"
#include "VGMAudioPlayer.h"
#include "VGMWaveFormViewer.h"
#include <lcd2.h>
#include "AudioDevice.h"

/*
const char* getNextMusic()
{
	static u32 currentMusic = 6;
	if(currentMusic>=22)
	{
		currentMusic=0;
	}

	switch (currentMusic++)
	{
	case 0:
		return "0:/ThunderForce3/01 - The Wind Blew All Day Long (Opening Theme).vgm";
	case 1:
		return "0:/ThunderForce3/02 - Beyond the Peace (Stage Select).vgm";
	case 2:
		return "0:/ThunderForce3/03 - Back to the Fire (Stage 1 - Hydra).vgm";
	case 3:
		return "0:/ThunderForce3/04 - Gargoyle (Stage 1 Boss).vgm";
	case 4:
		return "0:/ThunderForce3/05 - Venus Fire (Stage 2 - Gorgon).vgm";
	case 5:
		return "0:/ThunderForce3/06 - Twin Vulcan (Stage 2 Boss).vgm";
	case 6:
		return "0:/ThunderForce3/07 - The Grubby Dark Blue (Stage 3 - Seiren).vgm";
	case 7:
		return "0:/ThunderForce3/08 - King Fish (Stage 3 Boss).vgm";
	case 8:
		return "0:/ThunderForce3/09 - Truth (Stage 4 - Haides).vgm";
	case 9:
		return "0:/ThunderForce3/10 - G Lobster (Stage 4 Boss).vgm";
	case 10:
		return "0:/ThunderForce3/11 - Final Take a Chance (Stage 5 - Ellis).vgm";
	case 11:
		return "0:/ThunderForce3/12 - Mobile Fort (Stage 5 Boss).vgm";
	case 12:
		return "0:/ThunderForce3/13 - His Behavior Inspired Us With Distrust (Stage 6 - Cerberus).vgm";
	case 13:
		return "0:/ThunderForce3/14 - Hunger Made Them Desperate (Stage 7 - Orn Base).vgm";
	case 14:
		return "0:/ThunderForce3/15 - Off Luck (Stage 7 Boss).vgm";
	case 15:
		return "0:/ThunderForce3/16 - Final Moment (Stage 8 - Orn Core).vgm";
	case 16:
		return "0:/ThunderForce3/17 - Be Menaced by Orn (Stage 8 Boss).vgm";
	case 17:
		return "0:/ThunderForce3/18 - Stage Clear.vgm";
	case 18:
		return "0:/ThunderForce3/19 - A War Without the End (Ending).vgm";
	case 19:
		return "0:/ThunderForce3/20 - Present (Staff Roll).vgm";
	case 20:
		return "0:/ThunderForce3/21 - Continue.vgm";
	case 21:
	default:
		return "0:/ThunderForce3/22 - Game Over.vgm";
	};
}

const char* getNextMusic()
{
	static u32 currentMusic = 0;
	if(currentMusic>=34)
	{
		currentMusic=0;
	}

	switch (currentMusic++)
	{
	case 0:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/01 Title.vgm";
	case 1:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/02 Credit.vgm";
	case 2:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/03 Player Select.vgm";
	case 3:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/04 VS.vgm";
	case 4:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/05 Japan (Ryu) I.vgm";
	case 5:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/06 Japan (Ryu) II.vgm";
	case 6:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/07 Stage End.vgm";
	case 7:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/08 Brazil (Blanka) I.vgm";
	case 8:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/09 Brazil (Blanka) II.vgm";
	case 9:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/12 China (Chun Li) I.vgm";
	case 10:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/13 China (Chun Li) II.vgm";
	case 11:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/14 Bonus Stage.vgm";
	case 12:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/15 Here Comes A New Challenger.vgm";
	case 13:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/22 India (Dhalsim) I.vgm";
	case 14:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/23 India (Dhalsim) II.vgm";
	case 15:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/26 Spain (Vega) I.vgm";
	case 16:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/27 Spain (Vega) II.vgm";
	case 17:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/28 Thailand (Sagat) I.vgm";
	case 18:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/29 Thailand (Sagat) II.vgm";
	case 19:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/32 Ending (Ryu).vgm";
	case 20:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/33 Ending (Blanka).vgm";
	case 21:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/34 Ending (Guile).vgm";
	case 22:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/35 Ending (Chun Li) I.vgm";
	case 23:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/36 Ending (Chun Li) II.vgm";
	case 24:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/38 Ending (Ken) I.vgm";
	case 25:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/39 Ending (Ken) II.vgm";
	case 26:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/40 Ending (Zangief).vgm";
	case 27:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/41 Ending (Dhalsim).vgm";
	case 28:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/42 Ending (four bosses).vgm";
	case 29:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/43 Credits Roll.vgm";
	case 30:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/44 Continue.vgm";
	case 31:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/45 Game Over.vgm";
	case 32:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/46 Ranking Display.vgm";
	default:
	case 33:
		return "Street_Fighter_II_Champion_Edition_(CP_System)/47 Unused.vgm";
	};
};
*/

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

	if(fileSearchInfo.lfname[0] == 0)
		return SR_NO_FILE;
	
	if ((fileSearchInfo.fattrib & AM_DIR)) // skip if is directory
		return getNextFile(directory, size, ext);
	
	if(!isExt(fileSearchInfo.lfname, ext))
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
#endif

int main()
{
	char path[256];
	int len;
	SearchResult sr;

	if (!Platform::initialize())
		return false;

	LCD_Initialize();
	LCD_DrawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00, 0x00, 0x00);	
	//LCD_Test();		
	
	// LCDTest();
	// PWMTest();	

	
	strcpy(path, "YM2151");
	if (beginSearchDirectory(path) == SR_OK)
	{
		while (1)
		{
			strcpy(path, "YM2151");
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