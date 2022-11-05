#ifndef _VGMDef_h_
#define _VGMDef_h_


#include <windows.h>
#include <assert.h>
//typedef unsigned char boolean;
typedef signed long long s64;
typedef signed int s32;
typedef signed short s16;
typedef signed char s8;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef float f32;
typedef double f64;

#define vgm_log printf
//#define VGM_FRAME_PER_SECOND		60	// NTSC
#define VGM_FRAME_PER_SECOND		50	// PAL
#define VGM_SAMPLE_BUFFER_SIZE		1024 // (44100 / VGM_FRAME_PER_SECOND)
#define VGM_OUTPUT_BUFFER_COUNT		4   // 64 Frames

#define VGM_NOTE_BUFFER_SIZE		(64)


#define NO_CLAMP
#define PI 3.141592654f

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)

typedef void* (*RomReadCallBack)(u8 chipID, u32 address);
typedef void(*RomLoadCallBack)(u8 chipID, u32 startAddress, u8 *data, u32 length, u32 totalROMLength);

#endif
