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

#define NO_CLAMP
#define M_PI 3.141592654f

#define vgm_log printf
#define VGM_SAMPLE_BUFFER_SIZE		1024 // (44100 / VGM_FRAME_PER_SECOND)
#define VGM_FRAME_PER_SECOND		(44100 / VGM_SAMPLE_BUFFER_SIZE)
#define VGM_OUTPUT_BUFFER_COUNT		4   // 64 Frames

#define VGM_NOTE_BUFFER_SIZE		(64)

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)

typedef void* (*RomReadCallBack)(u8 chipID, u32 address);
typedef void(*RomLoadCallBack)(u8 chipID, u32 startAddress, u8* data, u32 length, u32 totalROMLength);




//////////////
#define INLINE

typedef s64 INT64;
typedef s32 INT32;
typedef s16 INT16;
typedef s8 INT8;
typedef u64 UINT64;
typedef u32 UINT32;
typedef u16 UINT16;
typedef u8 UINT8;
typedef f32 F32;
typedef f64 F64;
typedef INT32 stream_sample_t;
typedef UINT32 offs_t;

typedef void (*SRATE_CALLBACK)(void*, UINT32);
extern stream_sample_t* DUMMYBUF[];


#endif
