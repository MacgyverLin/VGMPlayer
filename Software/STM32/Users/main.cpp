#include <Array.h>
#include "Platform.h"
#include "VGMFile.h"
#include "VGMAudioPlayer.h"
#include "VGMWaveFormViewer.h"

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
*/

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

#include <LCD.h>

int main()
{
	if (!Platform::initialize())
		return false;
	
	LCD_Initialize();

	bool quit = false;
	while (!quit)
	{
		const char* filename = getNextMusic();

		VGMFile vgmFile(filename, 2, 16, 44100);
		VGMAudioPlayer vgmAudioPlayer;
		vgmFile.attach(vgmAudioPlayer);
#ifdef STM32
#else
		VGMWaveFormViewer vgmWaveFormViewer1("Waveform Viewer", 40, 40, 640, 480, VGMWaveFormViewer::Skin());
		VGMSpectrumViewer vgmWaveFormViewer2("Frequency Viewer", 40 + 640, 40, 640, 480, VGMSpectrumViewer::Skin());
		//VGMFireSpectrumViewer vgmWaveFormViewer3("Frequency Viewer", 40 + 640 * 2, 40, 320, 240, VGMFireSpectrumViewer::Skin());\
		vgmFile.attach(vgmWaveFormViewer1);
		vgmFile.attach(vgmWaveFormViewer2);
		//vgmFile.attach(vgmWaveFormViewer3);
#endif
		if (!vgmFile.open())
		{
			return -1;
		}

#if 0
		const VGMHeader& header = vgmFile.getHeader();
		printf("Now Playing <<%s>>\n", filename);
		printf("EOFOffset=%08x\n", header.EOFOffset);
		printf("version=%08x\n", header.version);
		printf("GD3Offset=%08x\n", header.GD3Offset);
		printf("totalSamples=%08x\n", header.totalSamples);
		printf("loopOffset=%08x\n", header.loopOffset);
		printf("loopSamples=%08x\n", header.loopSamples);
		printf("Rate=%08x\n", header.Rate);
#endif

		while (1)
		{
			quit = !Platform::update();

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

	Platform::terminate();

	return 0;
}
