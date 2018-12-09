/////////////////////////////////////////////////////////////////////////////////////////////
// SOUND.H
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_H
#define SOUND_H

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

	extern int CPU_Mode;
	extern int VDP_Current_Line;
	extern int VDP_Num_Lines;
	extern int VDP_Status;

	extern int Sound_Rate;
	extern int Sound_Segs;
	extern int Sound_Enable;
	extern int Sound_Stereo;
	extern int Sound_Is_Playing;
	extern int Sound_Initialised;
	extern int WAV_Dumping;
	extern int GYM_Playing;
	extern int GYM_Dumping;
	extern int Seg_L[882], Seg_R[882];
	extern int Seg_Lenght;
	extern int WP, RP;

	extern unsigned int Sound_Interpol[882];
	extern unsigned int Sound_Extrapol[312][2];

	extern char Dump_Dir[1024];
	extern char Dump_GYM_Dir[1024];

	int Init_Sound();
	void End_Sound(void);
	void Update_Sound(float time);	
	int Write_Sound_Buffer(void *Dump_Buf);
	int Clear_Sound_Buffer(void);
	int Play_Sound(void);
	int Stop_Sound(void);

	int Start_WAV_Dump(const char* filename);
	int Update_WAV_Dump(void);
	int Stop_WAV_Dump(void);
	
	int Start_GYM_Dump(const char* filename);
	int Update_GYM_Dump(int v0, int v1, unsigned char v2);
	int Stop_GYM_Dump(void);
	
	int Start_Play_GYM(const char* filename);
	int Play_GYM();
	int Stop_Play_GYM(void);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif