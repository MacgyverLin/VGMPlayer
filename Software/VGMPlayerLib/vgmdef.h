#ifndef _VGMDef_h_
#define _VGMDef_h_

//#define TEST_YM2151

#ifdef STM32
#include <stm32f10x.h>
#include <assert.h>
#include <string.h>
#ifndef FALSE
typedef int boolean;
#define FALSE 0
#define TRUE 1
#endif

typedef float f32;
typedef double f64;
#else
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
#endif

#define vgm_log printf
#ifdef STM32
#define VGM_SAMPLE_COUNT		441 // NTSC FRAME
#else
#define VGM_SAMPLE_COUNT		882 // NTSC FRAME
#endif
#define VGM_OUTPUT_BUFFER_COUNT 4   // 64 Frames
#define NO_CLAMP
#define PI 3.141592654f

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)

typedef void* (*RomReadCallBack)(u8 chipID, u32 address);
typedef void(*RomLoadCallBack)(u8 chipID, u32 startAddress, u8 *data, u32 length, u32 totalROMLength);

#endif
