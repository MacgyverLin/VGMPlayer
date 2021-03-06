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
	"ThunderForce3/Test1.vgm",
	"ThunderForce3/Test2.vgm",
	"ThunderForce3/Test3.vgm",
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
	"Falsion_(Family_Computer_Disk_System)/01 The First Space Fight (Stage 1 BGM).vgz",
	"Falsion_(Family_Computer_Disk_System)/02 Boss.vgz",
	"Falsion_(Family_Computer_Disk_System)/03 The Evening Voyage (Stage 2 BGM).vgz",
	"Falsion_(Family_Computer_Disk_System)/04 Narrow Road (Stage 3 BGM).vgz",
	"Falsion_(Family_Computer_Disk_System)/05 Careful with the Walls!! (Stage 4 BGM).vgz",
	"Falsion_(Family_Computer_Disk_System)/06 Flying High (Stage 5 BGM).vgz",
	"Falsion_(Family_Computer_Disk_System)/07 The Final Space Fight (Stage 6 BGM).vgz",
	"Falsion_(Family_Computer_Disk_System)/08 Last Boss.vgz",
	"Falsion_(Family_Computer_Disk_System)/09 Farewell to Journey (Ending).vgz",
	"Falsion_(Family_Computer_Disk_System)/10 Game Over.vgz"
};

const char* file18s[] =
{
	"Gradius_(TG-16)/01 Konami Logo.vgz",
	"Gradius_(TG-16)/02 Coin.vgz",
	"Gradius_(TG-16)/03 Beginning of the History.vgz",
	"Gradius_(TG-16)/04 Challenger 1985.vgz",
	"Gradius_(TG-16)/05 Beat Back.vgz",
	"Gradius_(TG-16)/06 Blank Mask.vgz",
	"Gradius_(TG-16)/07 Free Flyer.vgz",
	"Gradius_(TG-16)/08 Mazed Music.vgz",
	"Gradius_(TG-16)/09 Mechanical Globule.vgz",
	"Gradius_(TG-16)/10 Final Attack.vgz",
	"Gradius_(TG-16)/11 Aircraft Carrier.vgz",
	"Gradius_(TG-16)/12 Staff Roll.vgz",
	"Gradius_(TG-16)/13 Game Over.vgz",
	"Gradius_(TG-16)/14 Historic Soldier.vgz",
	"Gradius_(TG-16)/15 Unused 1.vgz",
	"Gradius_(TG-16)/16 Unused 2.vgz"
};

const char* file19s[] =
{
"Detana!!_TwinBee_(TG-16)/01 Konami Logo.vgz",
"Detana!!_TwinBee_(TG-16)/02 Detana TwinBee (Title).vgz",
"Detana!!_TwinBee_(TG-16)/03 Message from Queen Melora (Start Demo).vgz",
"Detana!!_TwinBee_(TG-16)/04 Gift of The Wind (Stage 1 BGM).vgz",
"Detana!!_TwinBee_(TG-16)/05 Attacked by The Forces of Iva (Stage 1, 2, 5 Boss BGM).vgz",
"Detana!!_TwinBee_(TG-16)/06 Thank You! TwinBee (Stage Clear).vgz",
"Detana!!_TwinBee_(TG-16)/07 Crossing The Sea of Clouds (Stage 2 BGM).vgz",
"Detana!!_TwinBee_(TG-16)/08 Sky Fortress Laputa (Stage 3 BGM).vgz",
"Detana!!_TwinBee_(TG-16)/09 Bring it on!! (Stage 3, 4 Boss BGM).vgz",
"Detana!!_TwinBee_(TG-16)/10 World of The Mold (Stage 4 BGM).vgz",
"Detana!!_TwinBee_(TG-16)/11 Aquatic Parade (Stage 5 BGM).vgz",
"Detana!!_TwinBee_(TG-16)/12 Paradise of Beyond The Fog (Stage 6 BGM).vgz",
"Detana!!_TwinBee_(TG-16)/13 Emperor Iva Brain Insect (Last Boss BGM).vgz",
"Detana!!_TwinBee_(TG-16)/14 The Day Peace Returns to Planet Meru (Ending).vgz",
"Detana!!_TwinBee_(TG-16)/15 Let's Try Again (Game Over).vgz",
"Detana!!_TwinBee_(TG-16)/16 Congratulations! (Ranking).vgz",
"Detana!!_TwinBee_(TG-16)/17 Detana TwinBee (Title, With Voice).vgz"
};

const char* file20s[] =
{
"Fantasy_Zone_(TG-16)/01 Start ~ Opa-Opa! [Round 1].vgz",
"Fantasy_Zone_(TG-16)/02 Shop.vgz",
"Fantasy_Zone_(TG-16)/03 Boss [Round 1~7 Boss].vgz",
"Fantasy_Zone_(TG-16)/04 Round Up [Round Clear].vgz",
"Fantasy_Zone_(TG-16)/05 Keep on the Beat [Round 2].vgz",
"Fantasy_Zone_(TG-16)/06 Saari [Round 3].vgz",
"Fantasy_Zone_(TG-16)/07 Prome [Round 4].vgz",
"Fantasy_Zone_(TG-16)/08 Hot Snow [Round 5].vgz",
"Fantasy_Zone_(TG-16)/09 Don't Stop [Round 6].vgz",
"Fantasy_Zone_(TG-16)/10 Dreaming Tomorrow [Round 7].vgz",
"Fantasy_Zone_(TG-16)/11 Ya-Da-Yo [Round 8 Boss].vgz",
"Fantasy_Zone_(TG-16)/12 Victory Way [Ending].vgz",
"Fantasy_Zone_(TG-16)/13 Miss.vgz",
"Fantasy_Zone_(TG-16)/14 Game Over.vgz"
};

const char* file21s[] =
{
"Salamander_(TG-16)/01 Konami Logo.vgz",
"Salamander_(TG-16)/02 Title Screen.vgz",
"Salamander_(TG-16)/03 Attract Mode.vgz",
"Salamander_(TG-16)/04 Power of Anger.vgz",
"Salamander_(TG-16)/05 Poison of Snake.vgz",
"Salamander_(TG-16)/06 Fly High.vgz",
"Salamander_(TG-16)/07 Planet Ratis.vgz",
"Salamander_(TG-16)/08 Starfield.vgz",
"Salamander_(TG-16)/09 Burn the Wind.vgz",
"Salamander_(TG-16)/10 Destroy Them All.vgz",
"Salamander_(TG-16)/11 Aircraft Carrier.vgz",
"Salamander_(TG-16)/12 Destroy Them All -Type 2- ~ Poison of Snake.vgz",
"Salamander_(TG-16)/13 Peace Again.vgz",
"Salamander_(TG-16)/14 Crystal Forever.vgz"
};

const char* file22s[] =
{
"Columns_(TG-16)/01 Filthy(Select).vgz",
"Columns_(TG-16)/02 Clotho(BGM 1).vgz",
"Columns_(TG-16)/03 Lathesis(BGM 2).vgz",
"Columns_(TG-16)/04 Atropos(BGM 3).vgz",
"Columns_(TG-16)/05 Boy Wonder(Name Entry).vgz",
"Columns_(TG-16)/06 Concilation(Game Over).vgz",
};

const char* file23s[] =
{
"OutRun_(TG-16)/01 Magical Sound Shower.vgz",
"OutRun_(TG-16)/02 Passing Breeze.vgz",
"OutRun_(TG-16)/03 Splash Wave.vgz",
"OutRun_(TG-16)/04 Last Wave.vgz",
};

const char* file24s[] =
{
"Youkai_Douchuuki_(TG-16)/01 Start.vgz",
"Youkai_Douchuuki_(TG-16)/02 BGM.vgz",
"Youkai_Douchuuki_(TG-16)/03 Oni.vgz",
"Youkai_Douchuuki_(TG-16)/04 Round Clear.vgz",
"Youkai_Douchuuki_(TG-16)/05 Opening the Parlor.vgz",
"Youkai_Douchuuki_(TG-16)/06 Upper Region ~Elysium.vgz",
"Youkai_Douchuuki_(TG-16)/07 Onigashima ~BGM.vgz",
"Youkai_Douchuuki_(TG-16)/08 Ryuuguujou.vgz",
"Youkai_Douchuuki_(TG-16)/09 Mambo Ryuuguu.vgz",
"Youkai_Douchuuki_(TG-16)/10 Tamatebako.vgz",
"Youkai_Douchuuki_(TG-16)/11 Acheron ~Enma Daioh.vgz",
"Youkai_Douchuuki_(TG-16)/12 Buddha.vgz",
"Youkai_Douchuuki_(TG-16)/13 Ending ~Credits.vgz",
"Youkai_Douchuuki_(TG-16)/14 Game Over.vgz",
"Youkai_Douchuuki_(TG-16)/15 Unknown 1.vgz",
"Youkai_Douchuuki_(TG-16)/16 Unknown 2.vgz",
};

const char* file25s[] =
{
"Detana!!_TwinBee_(Sharp_X68000)/01 Morning Music (Load BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/02 Konami Logo.vgz",
"Detana!!_TwinBee_(Sharp_X68000)/03 Detana TwinBee (Title).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/04 Credit.vgz",
"Detana!!_TwinBee_(Sharp_X68000)/05 Message from Queen Melora (Start Demo).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/06 Gift of The Wind (Stage 1 BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/07 Attacked by The Forces of Iva (Stage 1, 2, 5, 6 Boss BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/08 Thank You! TwinBee (Stage Clear).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/09 Crossing The Sea of Clouds (Stage 2 BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/10 Sky Fortress Laputa (Stage 3 BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/11 Bring it on!! (Stage 3, 4 Boss BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/12 World of The Mold (Stage 4 BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/13 Aquatic Parade (Stage 5 BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/14 Iva Temple (Stage 6 BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/15 Paradise of Beyond The Fog (Stage 7 BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/16 Emperor Iva Brain Insect (Last Boss BGM).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/17 The Day Peace Returns to Planet Meru (Ending).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/18 Let's Try Again (Game Over).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/19 Congratulations! (Ranking).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/20 Detana TwinBee (Title, With SE).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/21 Opening (Shop Demo).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/22 Message from Queen Melora (Shop Demo).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/23 It is Dispatched to The Planet Meru (Shop Demo).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/24 Detana TwinBee (Shop Demo).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/25 Twinbee's Home Town Song (Shop Demo).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/26 Shop Demo 1.vgz",
"Detana!!_TwinBee_(Sharp_X68000)/27 Shop Demo 2.vgz",
"Detana!!_TwinBee_(Sharp_X68000)/28 Shop Demo 3 (Pre-Release).vgz",
"Detana!!_TwinBee_(Sharp_X68000)/29 Shop Demo 3 (Now on Sale).vgz"
};

const char* file26s[] =
{
"Bells_&_Whistles_(Konami_Sunset_Riders)/01 Bells & Whistles (Title).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/02 Credit.vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/03 Message from Queen Melora (Start Demo).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/04 Gift of The Wind (Stage 1 BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/05 Attacked by The Forces of Iva (Stage 1, 2, 5, 6 Boss BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/06 Thank You! TwinBee (Stage Clear).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/07 Crossing The Sea of Clouds (Stage 2 BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/08 Sky Fortress Laputa (Stage 3 BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/09 Bring it on!! (Stage 3, 4 Boss BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/10 World of The Mold (Stage 4 BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/11 Aquatic Parade (Stage 5 BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/12 Iva Temple (Stage 6 BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/13 Paradise of Beyond The Fog (Stage 7 BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/14 Emperor Iva Brain Insect (Last Boss BGM).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/15 The Day Peace Returns to Planet Meru (Ending).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/16 Let's Try Again (Game Over).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/17 Congratulations! (Ranking).vgz",
"Bells_&_Whistles_(Konami_Sunset_Riders)/18 Bells & Whistles (Title, With SE).vgz",
};

const char* file27s[] =
{
"Street_Fighter_II_Champion_Edition_(CP_System)/01 Title.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/02 Credit.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/03 Player Select.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/04 VS.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/05 Japan (Ryu) I.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/06 Japan (Ryu) II.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/07 Stage End.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/08 Brazil (Blanka) I.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/09 Brazil (Blanka) II.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/12 China (Chun Li) I.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/13 China (Chun Li) II.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/14 Bonus Stage.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/15 Here Comes A New Challenger.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/22 India (Dhalsim) I.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/23 India (Dhalsim) II.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/26 Spain (Vega) I.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/27 Spain (Vega) II.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/28 Thailand (Sagat) I.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/29 Thailand (Sagat) II.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/32 Ending (Ryu).vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/33 Ending (Blanka).vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/34 Ending (Guile).vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/35 Ending (Chun Li) I.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/36 Ending (Chun Li) II.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/38 Ending (Ken) I.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/39 Ending (Ken) II.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/40 Ending (Zangief).vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/41 Ending (Dhalsim).vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/42 Ending (four bosses).vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/43 Credits Roll.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/44 Continue.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/45 Game Over.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/46 Ranking Display.vgm",
"Street_Fighter_II_Champion_Edition_(CP_System)/47 Unused.vgm"
};

const char* file28s[] =
{
"Street_Fighter_(Arcade)/01 Game Start(Credit).vgz",
"Street_Fighter_(Arcade)/02 Stage Select..vgz",
"Street_Fighter_(Arcade)/03 VS.vgz",
"Street_Fighter_(Arcade)/04 Round.vgz",
"Street_Fighter_(Arcade)/05 Retsu(Japan, Shaolin Temple).vgz",
"Street_Fighter_(Arcade)/06 Geki(Japan, Ninja).vgz",
"Street_Fighter_(Arcade)/07 Stage Clear.vgz",
"Street_Fighter_(Arcade)/08 Joe(USA, Kickboxer).vgz",
"Street_Fighter_(Arcade)/09 Mike(USA, Boxer).vgz",
"Street_Fighter_(Arcade)/10 Bonus Stage(Breaking Bricks).vgz",
"Street_Fighter_(Arcade)/11 Birdie(England, Mohawk).vgz",
"Street_Fighter_(Arcade)/12 Eagle(England, Ancient Castle).vgz",
"Street_Fighter_(Arcade)/13 Breaking In.vgz",
"Street_Fighter_(Arcade)/14 Lee(China, Great Wall).vgz",
"Street_Fighter_(Arcade)/15 Gen(China, Town).vgz",
"Street_Fighter_(Arcade)/16 Bonus Stage(Breaking Boards).vgz",
"Street_Fighter_(Arcade)/17 Adon(Thailand, Sunset).vgz",
"Street_Fighter_(Arcade)/18 Sagat(Thailand, Final Match).vgz",
"Street_Fighter_(Arcade)/19 Final Demo.vgz",
"Street_Fighter_(Arcade)/20 Credits Roll.vgz",
"Street_Fighter_(Arcade)/21 Name Entry.vgz",
"Street_Fighter_(Arcade)/22 Ranking Display.vgz",
"Street_Fighter_(Arcade)/23 Continue.vgz"
};

const char* file29s[] =
{
"Commando_(Arcade)/01 Credit.vgz",
"Commando_(Arcade)/02 Start Demo ~BGM 1.vgz",
"Commando_(Arcade)/03 Player Out.vgz",
"Commando_(Arcade)/04 Restart ~BGM 1.vgz",
"Commando_(Arcade)/05 Fortress.vgz",
"Commando_(Arcade)/06 Stage Clear Demo.vgz",
"Commando_(Arcade)/07 BGM 2.vgz",
"Commando_(Arcade)/08 All Clear.vgz",
"Commando_(Arcade)/09 Game Over.vgz",
"Commando_(Arcade)/10 Name Regist.vgz",
"Commando_(Arcade)/11 1st Place Name Entry End.vgz",
"Commando_(Arcade)/12 Below 2nd Place Name Entry End.vgz",
"Commando_(Arcade)/13 BGM 1.vgz",
"Commando_(Arcade)/14 Restart ~BGM 2.vgz"
};

const char* file30s[] =
{
"1943_The_Battle_of_Midway_(1943)/01 Credit.vgz",
"1943_The_Battle_of_Midway_(1943)/02 Air Battle A (Round 1, 2, 7, 10, 13, 16).vgz",
"1943_The_Battle_of_Midway_(1943)/03 Antiship Battle A (Round 1-Tone, 2-Kaga, 7-Ise, 10-Mutsu, 13-Souryu).vgz",
"1943_The_Battle_of_Midway_(1943)/04 Ayako (Round 3, 9).vgz",
"1943_The_Battle_of_Midway_(1943)/05 Mission Completed I (Hit Ratio Result 1).vgz",
"1943_The_Battle_of_Midway_(1943)/06 Refuel (Counting Bonus Points).vgz",
"1943_The_Battle_of_Midway_(1943)/07 Air Battle B (Round 3, 4, 8, 14).vgz",
"1943_The_Battle_of_Midway_(1943)/08 Antiship Battle B (Round 4-Fuso, 8-Hiryu).vgz",
"1943_The_Battle_of_Midway_(1943)/09 Failed in the Attack (Hit Ratio Result 2).vgz",
"1943_The_Battle_of_Midway_(1943)/10 Air Battle C (Round 5, 9, 12, 15).vgz",
"1943_The_Battle_of_Midway_(1943)/11 Antiship Battle C (Round 5-Akagi, 12-Yamashiro, 15-Nagato, 16-Yamato).vgz",
"1943_The_Battle_of_Midway_(1943)/12 Daihiryu (Round 6, 11).vgz",
"1943_The_Battle_of_Midway_(1943)/13 Mission Completed II (Hit Ratio Result 3).vgz",
"1943_The_Battle_of_Midway_(1943)/14 Air Battle D (Round 6, 11).vgz",
"1943_The_Battle_of_Midway_(1943)/15 Final Ayako (Round 14).vgz",
"1943_The_Battle_of_Midway_(1943)/16 Final Stage Cleared.vgz",
"1943_The_Battle_of_Midway_(1943)/17 Star-spangled Banner.vgz",
"1943_The_Battle_of_Midway_(1943)/18 Credits Roll.vgz",
"1943_The_Battle_of_Midway_(1943)/19 Continue.vgz",
"1943_The_Battle_of_Midway_(1943)/20 Game Over.vgz"
};

const char* file31s[] =
{
	"Famicom Disk System BIOS/Famicom Disk System BIOS - 01 - BIOS.vgm"
};

const char* file32s[] =
{
"Dracula_II_-_Noroi_no_Fuuin_(Family_Computer_Disk_System)/01 Message of Darkness (Naming BGM).vgz",
"Dracula_II_-_Noroi_no_Fuuin_(Family_Computer_Disk_System)/02 Within these Castle Walls (Castle BGM).vgz",
"Dracula_II_-_Noroi_no_Fuuin_(Family_Computer_Disk_System)/03 Bloody Tears (Roads by Day BGM).vgz",
"Dracula_II_-_Noroi_no_Fuuin_(Family_Computer_Disk_System)/04 The Silence of the Daylight (Town by Day BGM).vgz",
"Dracula_II_-_Noroi_no_Fuuin_(Family_Computer_Disk_System)/05 Dwelling of Doom (Mansion BGM).vgz",
"Dracula_II_-_Noroi_no_Fuuin_(Family_Computer_Disk_System)/06 Monster Dance (By Night BGM).vgz",
"Dracula_II_-_Noroi_no_Fuuin_(Family_Computer_Disk_System)/07 Last Boss (Last Boss BGM).vgz",
"Dracula_II_-_Noroi_no_Fuuin_(Family_Computer_Disk_System)/08 Game Over (Game Over).vgz",
"Dracula_II_-_Noroi_no_Fuuin_(Family_Computer_Disk_System)/09 A Requiem (Ending).vgz",
};

const char* file33s[] =
{
"Green_Beret_(NES)/01 Prologue.vgz",
"Green_Beret_(NES)/02 Stages 1, 3, 5 (with intro).vgz",
"Green_Beret_(NES)/03 Stages 1, 3, 5 (without intro).vgz",
"Green_Beret_(NES)/04 Boss Battle (with intro).vgz",
"Green_Beret_(NES)/05 Boss Battle (without intro).vgz",
"Green_Beret_(NES)/06 Stage 1 Finished.vgz",
"Green_Beret_(NES)/07 Stages 2, 4 (with intro).vgz",
"Green_Beret_(NES)/08 Stages 2, 4 (without intro).vgz",
"Green_Beret_(NES)/09 Underground.vgz",
"Green_Beret_(NES)/10 Stage 2 Finished.vgz",
"Green_Beret_(NES)/11 Stage 3 Finished.vgz",
"Green_Beret_(NES)/12 Stage 4 Finished.vgz",
"Green_Beret_(NES)/13 Stage 5 Finished.vgz",
"Green_Beret_(NES)/14 Stage 6 (with intro).vgz",
"Green_Beret_(NES)/15 Stage 6 (without intro).vgz",
"Green_Beret_(NES)/16 Last Boss Battle (with intro).vgz",
"Green_Beret_(NES)/17 Last Boss Battle (without intro).vgz",
"Green_Beret_(NES)/18 Unlimited Ammo.vgz",
"Green_Beret_(NES)/19 Invincibility.vgz",
"Green_Beret_(NES)/20 Losing a Life.vgz",
"Green_Beret_(NES)/21 Game Over.vgz",
"Green_Beret_(NES)/22 Ending.vgz"
};

const char* file34s[] =
{
"Stinger_(NES)/01 Introduction.vgz",
"Stinger_(NES)/02 Stage Begin 1.vgz",
"Stinger_(NES)/03 Stage Begin 2.vgz",
"Stinger_(NES)/04 Main BGM.vgz",
"Stinger_(NES)/05 Power-Up Acquired.vgz",
"Stinger_(NES)/06 Power-Up Theme.vgz",
"Stinger_(NES)/07 Boss Approaching.vgz",
"Stinger_(NES)/08 Boss Theme.vgz",
"Stinger_(NES)/09 Bonus Game.vgz",
"Stinger_(NES)/10 Invincibility.vgz",
"Stinger_(NES)/11 Final BGM.vgz",
"Stinger_(NES)/12 Final Power-Up.vgz",
"Stinger_(NES)/13 Final Boss Approach.vgz",
"Stinger_(NES)/14 Final Boss.vgz",
"Stinger_(NES)/15 Game Clear.vgz",
"Stinger_(NES)/16 Epilogue.vgz",
"Stinger_(NES)/17 Staff Roll.vgz",
"Stinger_(NES)/18 Game Over.vgz",
};

const char* file35s[] =
{
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/01 Title BGM.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/02 Overworld BGM.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/03 Battle BGM.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/04 Flute BGM 1.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/05 Town BGM.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/06 Inside A House BGM.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/07 Flute BGM 2.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/08 Temple BGM.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/09 Boss.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/10 Temple Clear.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/11 Level Up.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/12 Game Over.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/13 Great Temple.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/14 Last Boss.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/15 Ending.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/16 Princess Zelda.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/17 Fanfare.vgz",
"The_Legend_of_Zelda_2_-_Link_no_Bouken_(Family_Computer_Disk_System)/18 Item Jingle.vgz",
};

const char* file36s[] =
{
"Vs._Excitebike_(Family_Computer_Disk_System)/01 Title BGM.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/02 Starting Track Selection.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/03 Challenge Race.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/04 Nomination Start.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/05 Nomination BGM.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/06 Qualified BGM.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/07 Excitebike BGM.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/08 Final Selection Start.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/09 Final Selection BGM.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/10 Game Over BGM.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/11 Bonus Stage Start.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/12 Bonus Stage BGM.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/13 Results.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/14 Extra Play BGM.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/15 Excite Mode Select.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/16 Excite Mode Start.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/17 Excite Mode 1P Win.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/18 Excite Mode 2P Win.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/19 Excite Mode Draw.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/20 Edit Mode.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/21 Track Mode.vgz",
"Vs._Excitebike_(Family_Computer_Disk_System)/22 Unknown.vgz"
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
#include <GL/glu.h>
#include <GL/gl.h>
#include "Platform.h"
#include "VGMPlayerLib.h"
#include "VGMAudioPlayer.h"
#include "VGMWaveFormViewer.h"
#include "VGMSpectrumViewer.h"
#include "VGMFireSpectrumViewer.h"
using namespace std;

const char** getAlbum(int i)
{
	const char** musics[] =
	{
		file1s,
		file2s,
		file3s,
		file4s,
		file5s,
		file6s,
		file7s,
		file8s,
		file9s,
		file10s,
		file11s,
		file12s,
		file13s,
		file14s,
		file15s,
		file16s,
		file17s,
		file18s,
		file19s,
		file20s,
		file21s,
		file22s,
		file23s,
		file24s,
		file25s,
		file26s,
		file27s,
		file28s,
		file29s,
		file30s,
		file31s,
		file32s,
		file33s,
		file34s,
		file35s,
		file36s,
	};

	return musics[i];
}

int getAlbumMusicCount(int i)
{
	int musicCounts[] =
	{
		sizeof(file1s) / sizeof(file1s[0]),
		sizeof(file2s) / sizeof(file2s[0]),
		sizeof(file3s) / sizeof(file3s[0]),
		sizeof(file4s) / sizeof(file4s[0]),
		sizeof(file5s) / sizeof(file5s[0]),
		sizeof(file6s) / sizeof(file6s[0]),
		sizeof(file7s) / sizeof(file7s[0]),
		sizeof(file8s) / sizeof(file8s[0]),
		sizeof(file9s) / sizeof(file9s[0]),
		sizeof(file10s) / sizeof(file10s[0]),
		sizeof(file11s) / sizeof(file11s[0]),
		sizeof(file12s) / sizeof(file12s[0]),
		sizeof(file13s) / sizeof(file13s[0]),
		sizeof(file14s) / sizeof(file14s[0]),
		sizeof(file15s) / sizeof(file15s[0]),
		sizeof(file16s) / sizeof(file16s[0]),
		sizeof(file17s) / sizeof(file17s[0]),
		sizeof(file18s) / sizeof(file18s[0]),
		sizeof(file19s) / sizeof(file19s[0]),
		sizeof(file20s) / sizeof(file20s[0]),
		sizeof(file21s) / sizeof(file21s[0]),
		sizeof(file22s) / sizeof(file22s[0]),
		sizeof(file23s) / sizeof(file23s[0]),
		sizeof(file24s) / sizeof(file24s[0]),
		sizeof(file25s) / sizeof(file25s[0]),
		sizeof(file26s) / sizeof(file26s[0]),
		sizeof(file27s) / sizeof(file27s[0]),
		sizeof(file28s) / sizeof(file28s[0]),
		sizeof(file29s) / sizeof(file29s[0]),
		sizeof(file30s) / sizeof(file30s[0]),
		sizeof(file31s) / sizeof(file31s[0]),
		sizeof(file32s) / sizeof(file32s[0]),
		sizeof(file33s) / sizeof(file33s[0]),
		sizeof(file34s) / sizeof(file34s[0]),
		sizeof(file35s) / sizeof(file35s[0]),
		sizeof(file36s) / sizeof(file36s[0]),
	};

	return musicCounts[i];
}

/*
int main(int argc, char *argv[])
{
	if(!Platform::initialize())
		return false;

	bool quit = false;
	int currentAlbum = 4;

	int currentMusic = 1;

	while (!quit)
	{
		const char* filename = getAlbum(currentAlbum)[currentMusic];
		VGMFile vgmFile(filename, 2, 16, 44100);
		VGMAudioPlayer vgmAudioPlayer;
		VGMWaveFormViewer vgmWaveFormViewer1("Waveform Viewer", 40, 40, 640, 480, VGMWaveFormViewer::Skin());
		VGMSpectrumViewer vgmWaveFormViewer2("Frequency Viewer", 40 + 640, 40, 640, 480, VGMSpectrumViewer::Skin());
		//VGMFireSpectrumViewer vgmWaveFormViewer3("Frequency Viewer", 40 + 640 * 2, 40, 320, 240, VGMFireSpectrumViewer::Skin());
		vgmFile.attach(vgmAudioPlayer);
		vgmFile.attach(vgmWaveFormViewer1);
		vgmFile.attach(vgmWaveFormViewer2);
		//vgmFile.attach(vgmWaveFormViewer3);
		if (!vgmFile.open())
		{
			return -1;
		}

		const VGMHeader& header = vgmFile.getHeader();
		printf("Now Playing <<%s>>\n", filename);
		printf("EOFOffset=%08x\n", header.EOFOffset);
		printf("version=%08x\n", header.version);
		printf("GD3Offset=%08x\n", header.GD3Offset);
		printf("totalSamples=%08x\n", header.totalSamples);
		printf("loopOffset=%08x\n", header.loopOffset);
		printf("loopSamples=%08x\n", header.loopSamples);
		printf("Rate=%08x\n", header.Rate);

		while (!quit)
		{
			Platform::update();

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

		currentMusic++;
		if (currentMusic >= getAlbumMusicCount(currentAlbum))
		{
			currentMusic = 0;
			currentAlbum++;
			if (currentAlbum >= 26)
			{
				quit = true;
			}
		}

		quit = true;
	}

	Platform::terminate();

	return 0;
}
*/

const char* getNextMusic()
{
	static int currentAlbum = 26;
	static int currentMusic = 0;

	if (currentMusic >= getAlbumMusicCount(currentAlbum))
	{
		currentMusic = 0;
		currentAlbum++;
		if (currentAlbum >= 26)
		{
			currentAlbum = 0;
		}
	}

	return getAlbum(currentAlbum)[currentMusic++];
}

int main()
{
	if (!Platform::initialize())
		return false;

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
		//VGMFireSpectrumViewer vgmWaveFormViewer3("Frequency Viewer", 40 + 640 * 2, 40, 320, 240, VGMFireSpectrumViewer::Skin());
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
