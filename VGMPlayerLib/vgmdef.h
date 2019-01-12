#ifndef _VGMDef_h_
#define _VGMDef_h_

#ifdef STM32
#include <stm32f10x.h>
#include <assert.h>
#include <string.h>
typedef unsigned char boolean;
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

#define True 1
#define False 0


#define vgm_log printf
#define VGM_SAMPLE_COUNT		882 // NTSC FRAME
#define VGM_OUTPUT_BUFFER_COUNT 4   // 64 Frames
#define NO_CLAMP
#define PI 3.141592654f

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)

#endif
