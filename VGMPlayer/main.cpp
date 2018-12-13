#include <windows.h>

const char* file1s[] =
{
	"ThunderCross/01 The Thunder Fighters (Title).vgz",
	"ThunderCross/02 Coin.vgz",
	"ThunderCross/03 First Attack (1st.BGM).vgz",
	"ThunderCross/04 Skywalker (2st.BGM).vgz",
	"ThunderCross/05 Machine Graveyard (3st.BGM).vgz",
	"ThunderCross/06 Great Battleship (4st.BGM).vgz",
	"ThunderCross/07 Endless Labyrinth (5st.BGM).vgz",
	"ThunderCross/08 Fire Cavern (6st.BGM).vgz",
	"ThunderCross/09 Final Base (7st.BGM).vgz",
	"ThunderCross/10 Gloidential Mechanism (Boss BGM).vgz",
	"ThunderCross/11 Stage Clear.vgz",
	"ThunderCross/12 Ranking.vgz",
	"ThunderCross/13 Game Over.vgz",
	"ThunderCross/14 A Long Way (Ending).vgz",
	"ThunderCross/15 Start Demo - First Attack.vgz",
	"ThunderCross/16 Appear Demo - Great Battleship.vgz",
	"ThunderCross/17 Stage Clear (with Effect).vgz"
};

// test ym2612, k053260
const char* file2s[] =
{
	"ThunderCross2/01 Credit.vgz",
	"ThunderCross2/02 Rise in Arms (Opening Demo).vgz",
	"ThunderCross2/03 Air Battle = Thunder Cross II (Stage 1 BGM).vgz",
	"ThunderCross2/04 Approach the Boss (Boss Approach BGM).vgz",
	"ThunderCross2/05 He is not dead = Evil Eye (Stage 1, 5 Boss BGM).vgz",
	"ThunderCross2/06 Clear Demo (Stage Clear).vgz",
	"ThunderCross2/07 Kartus-Part 2 (Stage 2 BGM).vgz",
	"ThunderCross2/08 Theme of Ragamuffin (Stage 2 Boss BGM).vgz",
	"ThunderCross2/09 Heavy Metal Bomber (Stage 3 BGM).vgz",
	"ThunderCross2/10 Dark Force (Stage 3, 6 Boss BGM).vgz",
	"ThunderCross2/11 Dog Fight III (Stage 4 BGM).vgz",
	"ThunderCross2/12 Theme of Dinosaur Jr. (Stage 4 Boss BGM).vgz",
	"ThunderCross2/13 Battles of Battleship (Stage 5 BGM).vgz",
	"ThunderCross2/14 Star Light (Stage 6 BGM).vgz",
	"ThunderCross2/15 At The Front (Stage 7 BGM).vgz",
	"ThunderCross2/16 An Ominous Silence (Boss Approach BGM).vgz",
	"ThunderCross2/17 Theme of Sprouts Layber (Stage 7 Boss BGM).vgz",
	"ThunderCross2/18 A Shooting Star (Ending).vgz",
	"ThunderCross2/19 Game Over.vgz",
	"ThunderCross2/20 A Victory March (Ranking).vgz"
};

// test ym2151, SN76489
const char* file3s[] =
{
	"ThunderForce3/01 - The Wind Blew All Day Long (Opening Theme).vgz",
	"ThunderForce3/02 - Beyond the Peace (Stage Select).vgz",
	"ThunderForce3/03 - Back to the Fire (Stage 1 - Hydra).vgz",
	"ThunderForce3/04 - Gargoyle (Stage 1 Boss).vgz",
	"ThunderForce3/05 - Venus Fire (Stage 2 - Gorgon).vgz",
	"ThunderForce3/06 - Twin Vulcan (Stage 2 Boss).vgz",
	"ThunderForce3/07 - The Grubby Dark Blue (Stage 3 - Seiren).vgz",
	"ThunderForce3/08 - King Fish (Stage 3 Boss).vgz",
	"ThunderForce3/09 - Truth (Stage 4 - Haides).vgz",
	"ThunderForce3/10 - G Lobster (Stage 4 Boss).vgz",
	"ThunderForce3/11 - Final Take a Chance (Stage 5 - Ellis).vgz",
	"ThunderForce3/12 - Mobile Fort (Stage 5 Boss).vgz",
	"ThunderForce3/13 - His Behavior Inspired Us With Distrust (Stage 6 - Cerberus).vgz",
	"ThunderForce3/14 - Hunger Made Them Desperate (Stage 7 - Orn Base).vgz",
	"ThunderForce3/15 - Off Luck (Stage 7 Boss).vgz",
	"ThunderForce3/16 - Final Moment (Stage 8 - Orn Core).vgz",
	"ThunderForce3/17 - Be Menaced by Orn (Stage 8 Boss).vgz",
	"ThunderForce3/18 - Stage Clear.vgz",
	"ThunderForce3/19 - A War Without the End (Ending).vgz",
	"ThunderForce3/20 - Present (Staff Roll).vgz",
	"ThunderForce3/21 - Continue.vgz",
	"ThunderForce3/22 - Game Over.vgz"
};

const char* file4s[] =
{
	"Life_Force(NES)/01 Coin.vgz",
	"Life_Force(NES)/02 Power of Anger.vgz",
	"Life_Force(NES)/03 Poison of Snake.vgz",
	"Life_Force(NES)/04 Thunderbolt.vgz",
	"Life_Force(NES)/05 Planet Ratis.vgz",
	"Life_Force(NES)/06 Starfield.vgz",
	"Life_Force(NES)/07 Burn the Wind.vgz",
	"Life_Force(NES)/08 Destroy Them All.vgz",
	"Life_Force(NES)/09 Aircraft Carrier.vgz",
	"Life_Force(NES)/10 Peace Again.vgz",
	"Life_Force(NES)/11 Crystal Forever.vgz"
};

const char* file5s[] =
{
	"Excitebike(NES)/01 Title BGM 1.vgz",
	"Excitebike(NES)/02 Title BGM 2.vgz",
	"Excitebike(NES)/03 Title BGM 3.vgz",
	"Excitebike(NES)/04 Title BGM 4.vgz",
	"Excitebike(NES)/05 Start BGM.vgz",
	"Excitebike(NES)/06 Victory Stand BGM.vgz"
};

const char* file6s[] =
{
	"Super_Mario_Bros._(NES)/01 Running About.vgz",
	"Super_Mario_Bros._(NES)/02 Underground.vgz",
	"Super_Mario_Bros._(NES)/03 Swimming Around.vgz",
	"Super_Mario_Bros._(NES)/04 Bowser's Castle.vgz",
	"Super_Mario_Bros._(NES)/05 Invincible.vgz",
	"Super_Mario_Bros._(NES)/06 Level Complete.vgz",
	"Super_Mario_Bros._(NES)/07 Bowser's Castle Complete.vgz",
	"Super_Mario_Bros._(NES)/08 Miss.vgz",
	"Super_Mario_Bros._(NES)/09 Game Over.vgz",
	"Super_Mario_Bros._(NES)/10 Into the Pipe.vgz",
	"Super_Mario_Bros._(NES)/11 Saved the Princess.vgz",
	"Super_Mario_Bros._(NES)/12 Running About (Hurry!).vgz",
	"Super_Mario_Bros._(NES)/13 Underground (Hurry!).vgz",
	"Super_Mario_Bros._(NES)/14 Swimming Around (Hurry!).vgz",
	"Super_Mario_Bros._(NES)/15 Bowser's Castle (Hurry!).vgz",
	"Super_Mario_Bros._(NES)/16 Invincible (Hurry!).vgz",
	"Super_Mario_Bros._(NES)/17 Into the Pipe (Hurry!).vgz",
	"Super_Mario_Bros._(NES)/18 Saved the Princess.vgz",
	"Super_Mario_Bros._(NES)/19 Name Entry.vgz",
	"Super_Mario_Bros._(NES)/20 Unused.vgz"
};

const char* file7s[] =
{
	"Contra_Force_(NES)/01 Opening.vgz",
	"Contra_Force_(NES)/02 Character Selection.vgz",
	"Contra_Force_(NES)/03 Stage 1.vgz",
	"Contra_Force_(NES)/04 Boss.vgz",
	"Contra_Force_(NES)/05 Stage Clear.vgz",
	"Contra_Force_(NES)/06 Intermission.vgz",
	"Contra_Force_(NES)/07 Stage 2.vgz",
	"Contra_Force_(NES)/08 Stage 3.vgz",
	"Contra_Force_(NES)/09 Stage 4.vgz",
	"Contra_Force_(NES)/10 Stage 5.vgz",
	"Contra_Force_(NES)/11 Final Boss.vgz",
	"Contra_Force_(NES)/12 Ending.vgz",
	"Contra_Force_(NES)/13 Game Over.vgz",
	"Contra_Force_(NES)/14 Character Selection (Pause).vgz"
};

const char* file9s[] =
{
	"Gradius_II_(Family_Computer)/01 Equipment (Selection).vgz",
	"Gradius_II_(Family_Computer)/02 Tabidachi (Air Battle).vgz",
	"Gradius_II_(Family_Computer)/03 Burning Heat (Stage 1).vgz",
	"Gradius_II_(Family_Computer)/04 The Final Enemy (Boss).vgz",
	"Gradius_II_(Family_Computer)/05 Fortress (Stage 2).vgz",
	"Gradius_II_(Family_Computer)/06 Heavy Blow (Stage 3-1).vgz",
	"Gradius_II_(Family_Computer)/07 Dead End (Stage 3-2).vgz",
	"Gradius_II_(Family_Computer)/08 The Old Stone Age (Stage 4).vgz",
	"Gradius_II_(Family_Computer)/09 Gradius I -Boss Theme- (Stage 5-1).vgz",
	"Gradius_II_(Family_Computer)/10 Salamander -Boss Theme- (Stage 5-2).vgz",
	"Gradius_II_(Family_Computer)/11 Over Heat (Stage 6).vgz",
	"Gradius_II_(Family_Computer)/12 Something Ghostly (Stage 7).vgz",
	"Gradius_II_(Family_Computer)/13 Take Care! (Final Boss).vgz",
	"Gradius_II_(Family_Computer)/14 Farewell (Ending).vgz",
	"Gradius_II_(Family_Computer)/15 Game Over.vgz"
};

const char* file10s[] =
{
	"Konami_Wai_Wai_World_(Family_Computer)/01 Garage BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/02 Stage Select BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/03 Konami Man BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/04 Konami Lady BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/05 Goemon BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/06 Simon BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/07 Getsu Fuuma BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/08 Mikey BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/09 Moai BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/10 Kong BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/11 Rescue Jingle.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/12 Dracula Stage Boss BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/13 Getsu Fuuma Stage Boss BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/14 Kong Stage Boss BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/15 Moai Stage Boss BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/16 Shooting Stage BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/17 Shooting Stage Boss BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/18 Final Stage BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/19 Final Stage Boss BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/20 Final Stage Escape BGM.vgz",
	"Konami_Wai_Wai_World_(Family_Computer)/21 Ending BGM.vgz"
};

const char* file11s[] =
{
	"Super_Mario_Bros._3_(NES)/01 World 1 Map.vgz",
	"Super_Mario_Bros._3_(NES)/02 Overworld BGM.vgz",
	"Super_Mario_Bros._3_(NES)/03 Invincible BGM.vgz",
	"Super_Mario_Bros._3_(NES)/04 Miss.vgz",
	"Super_Mario_Bros._3_(NES)/05 World 2 Map.vgz",
	"Super_Mario_Bros._3_(NES)/06 Athletic BGM.vgz",
	"Super_Mario_Bros._3_(NES)/07 Course Clear Fanfare.vgz",
	"Super_Mario_Bros._3_(NES)/08 World 3 Map.vgz",
	"Super_Mario_Bros._3_(NES)/09 Underwater BGM.vgz",
	"Super_Mario_Bros._3_(NES)/10 Music Box.vgz",
	"Super_Mario_Bros._3_(NES)/11 Slot BGM ~ Nervous Breakdown BGM.vgz",
	"Super_Mario_Bros._3_(NES)/12 World 4 Map.vgz",
	"Super_Mario_Bros._3_(NES)/13 Underground BGM.vgz",
	"Super_Mario_Bros._3_(NES)/14 World 5 Map.vgz",
	"Super_Mario_Bros._3_(NES)/15 Enemy Battle.vgz",
	"Super_Mario_Bros._3_(NES)/16 Toad's House.vgz",
	"Super_Mario_Bros._3_(NES)/17 World 6 Map.vgz",
	"Super_Mario_Bros._3_(NES)/18 Fortress BGM.vgz",
	"Super_Mario_Bros._3_(NES)/19 Fortress Boss.vgz",
	"Super_Mario_Bros._3_(NES)/20 Fireworks Fanfare.vgz",
	"Super_Mario_Bros._3_(NES)/21 Whistle Sound ~ Warp Island.vgz",
	"Super_Mario_Bros._3_(NES)/22 World 7 Map.vgz",
	"Super_Mario_Bros._3_(NES)/23 King's Room.vgz",
	"Super_Mario_Bros._3_(NES)/24 Airship BGM.vgz",
	"Super_Mario_Bros._3_(NES)/25 World 8 Map.vgz",
	"Super_Mario_Bros._3_(NES)/26 King Koopa.vgz",
	"Super_Mario_Bros._3_(NES)/27 King Koopa Defeated.vgz",
	"Super_Mario_Bros._3_(NES)/28 World Clear Fanfare.vgz",
	"Super_Mario_Bros._3_(NES)/29 Ending.vgz",
	"Super_Mario_Bros._3_(NES)/30 Game Over.vgz",
	"Super_Mario_Bros._3_(NES)/31 World 5 Sky Map.vgz",
	"Super_Mario_Bros._3_(NES)/32 Hurry!.vgz"
};

const char* file12s[] =
{
	"Metal_Gear_(NES)/01 Aerial Insertion.vgz",
	"Metal_Gear_(NES)/02 Jungle Infiltration.vgz",
	"Metal_Gear_(NES)/03 Base Infiltration.vgz",
	"Metal_Gear_(NES)/04 Intruder Detected.vgz",
	"Metal_Gear_(NES)/05 Warning! ~ Intruder Detected.vgz",
	"Metal_Gear_(NES)/06 -!- ~ Intruder Detected.vgz",
	"Metal_Gear_(NES)/07 Mercenary.vgz",
	"Metal_Gear_(NES)/08 TX-55 Metal Gear.vgz",
	"Metal_Gear_(NES)/09 Escape -Beyond Big Boss-.vgz",
	"Metal_Gear_(NES)/10 Return of Fox Hounder.vgz",
	"Metal_Gear_(NES)/11 Just Another Dead Soldier.vgz",
	"Metal_Gear_(NES)/12 Password Entry.vgz"
};

const char* file13s[] =
{
	"Bubble_Bobble_(NES)/01 Introduction.vgz",
	"Bubble_Bobble_(NES)/02 Bonus Music.vgz",
	"Bubble_Bobble_(NES)/03 Secret Room.vgz",
	"Bubble_Bobble_(NES)/04 False Ending.vgz",
	"Bubble_Bobble_(NES)/05 Super Drunk.vgz",
	"Bubble_Bobble_(NES)/06 Real Ending.vgz",
	"Bubble_Bobble_(NES)/07 Name Regist.vgz",
	"Bubble_Bobble_(NES)/08 Game Over.vgz"
};

const char* file8s[] =
{
	"Castlevania_(NES)/01 Prologue.vgz",
	"Castlevania_(NES)/02 Vampire Killer.vgz",
	"Castlevania_(NES)/03 Stalker.vgz",
	"Castlevania_(NES)/04 Wicked Child.vgz",
	"Castlevania_(NES)/05 Walking On the Edge.vgz",
	"Castlevania_(NES)/06 Heart of Fire.vgz",
	"Castlevania_(NES)/07 Out of Time.vgz",
	"Castlevania_(NES)/08 Poison Mind.vgz",
	"Castlevania_(NES)/09 Player Miss.vgz",
	"Castlevania_(NES)/10 Game Over.vgz",
	"Castlevania_(NES)/11 Stage Clear.vgz",
	"Castlevania_(NES)/12 Name Entry.vgz",
	"Castlevania_(NES)/13 Nothing to Lose.vgz",
	"Castlevania_(NES)/14 Black Night.vgz",
	"Castlevania_(NES)/15 All Clear.vgz",
	"Castlevania_(NES)/16 Voyager.vgz"
};

const char* file14s[] =
{
	"The_Goonies_(NES)/01 Start.vgz",
	"The_Goonies_(NES)/02 The Goonies are Good Enough.vgz",
	"The_Goonies_(NES)/03 Round Clear.vgz",
	"The_Goonies_(NES)/04 BGM 2.vgz",
	"The_Goonies_(NES)/05 BGM 3.vgz",
	"The_Goonies_(NES)/06 Secret Cave.vgz",
	"The_Goonies_(NES)/07 Invincible.vgz",
	"The_Goonies_(NES)/08 Ending.vgz",
	"The_Goonies_(NES)/09 Miss.vgz",
	"The_Goonies_(NES)/10 Game Over.vgz"
};

const char* file15s[] =
{
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/01 Opening.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/02 Stage 1, 4, 6.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/03 Stage Exit.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/04 Boss.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/05 Stage Clear.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/06 Stage 2, 3.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/07 Stage 5.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/08 Last Boss.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/09 Last Boss Clear.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/10 Arumana Recovered.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/11 Ending.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/12 Game Over.vgz",
};

const char* file16s[] =
{
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/01 Opening.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/02 Stage 1, 4, 6.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/03 Stage Exit.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/04 Boss.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/05 Stage Clear.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/06 Stage 2, 3.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/07 Stage 5.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/08 Last Boss.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/09 Last Boss Clear.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/10 Arumana Recovered.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/11 Ending.vgz",
	"Arumana_no_Kiseki_(Nintendo_Famicom_Disk_System)/12 Game Over.vgz",
};

const char* file17s[] =
{
	"Falsion_(Family_Computer_Disk_System)/01 Opening.vgz",
	"Falsion_(Family_Computer_Disk_System)/02 Stage 1, 4, 6.vgz",
	"Falsion_(Family_Computer_Disk_System)/03 Stage Exit.vgz",
	"Falsion_(Family_Computer_Disk_System)/04 Boss.vgz",
	"Falsion_(Family_Computer_Disk_System)/05 Stage Clear.vgz",
	"Falsion_(Family_Computer_Disk_System)/06 Stage 2, 3.vgz",
	"Falsion_(Family_Computer_Disk_System)/07 Stage 5.vgz",
	"Falsion_(Family_Computer_Disk_System)/08 Last Boss.vgz",
	"Falsion_(Family_Computer_Disk_System)/09 Last Boss Clear.vgz",
	"Falsion_(Family_Computer_Disk_System)/10 Arumana Recovered.vgz",
};

#if 0
#include "AudioDevice.h"

bool testSoundDevice()
{
	AudioDevice soundDevice;
	int channels = 2;
	int bitPerSamples = 16;
	int sampleRate = 22050;
	int bufferSegmentCount = 8;
	int frameRate = 30;
	int bufferSegmentSize = channels * (bitPerSamples / 8) * sampleRate / frameRate; // 1 / bufferSegmentCount second
	if (!soundDevice.open(channels, bitPerSamples, sampleRate, bufferSegmentCount))
		return false;

	FILE *fp = fopen("2.raw", "rb");
	if (!fp)
		return false;

	int batchSize = bufferSegmentSize;
	vector<char> rawData;
	rawData.resize(batchSize);
	memset(&rawData[0], 0, batchSize);

	int readSize = 0;
	int WP = 0;
	do
	{
		while (soundDevice.getQueued() < 8)
		{
			readSize = fread(&rawData[0], 1, batchSize, fp);
			if (readSize == 0)
				break;

			if (!soundDevice.queue(&rawData[0], readSize))
				break;
		};

		if (soundDevice.getDeviceState() != 3)
		{
			soundDevice.play();

			soundDevice.setVolume(1.0);
			soundDevice.setPlayRate(1.0);
		}

		if (!soundDevice.update())
			break;
	} while (readSize != 0);

	soundDevice.stop();
	soundDevice.close();

	fclose(fp);

	return true;
}
#endif

#include <stdio.h>
#include <iostream>
#include <vector>
#include <SDL_main.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "VGMPlayerLib.h"
#include "VGMAudioPlayer.h"
#include "VGMChannelViewer.h"
using namespace std;

int main(int argc, char *argv[])
{
	SDL_Event sdlEvent;

	//Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Initialize video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		// Display error message
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	bool quit = false;
	int musicIdx = 0;
#define MUSIC_SELECT(n)\
	int numMusics = sizeof(file##n##s) / sizeof(file##n##s[0]);\
	const char** file = file##n##s;

	MUSIC_SELECT(2)
	while (musicIdx < numMusics && !quit)
	{
		VGMFile vgmFile(file[musicIdx], 2, 16, 44100);
		VGMAudioPlayer vgmAudioPlayer;
		VGMChannelViewer vgmChannelViewer1(40    , 40, 320, 240, Color(0.0, 0.0, 0.0, 1.0));
		//VGMChannelViewer vgmChannelViewer2(40+320, 40, 320, 240, Color(0.0, 0.0, 0.0, 1.0));
		vgmFile.attach(vgmAudioPlayer);
		vgmFile.attach(vgmChannelViewer1);
		//vgmFile.attach(vgmChannelViewer2);
		//vgmFile.attach(vgmDataWaveFormViewer);
		if (!vgmFile.open())
		{
			return -1;
		}

		while (!quit)
		{
			while (SDL_PollEvent(&sdlEvent) != 0)
			{
				// Esc button is pressed
				if (sdlEvent.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			if (vgmAudioPlayer.getQueued() < VGM_OUTPUT_BUFFER_COUNT / 2)
			{
				// printf("vgmDataOpenALPlayer.getQueued() %d\n", vgmAudioPlayer.getQueued());
				vgmFile.requestUpdateData();
			}

			if (!vgmFile.update())
				break;
		}

		vgmFile.close();

		musicIdx++;
	}

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}