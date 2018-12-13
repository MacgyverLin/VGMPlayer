#ifndef _VGMDef_h_
#define _VGMDef_h_

#include <windows.h>

//typedef char BOOL;
typedef char CHAR;
//typedef void VOID;
typedef signed long long INT64;
typedef signed int INT32;
typedef signed short INT16;
typedef signed char INT8;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef unsigned long long UINT64;
typedef float FLOAT32;
typedef double FLOAT64;
//#define TRUE -1
//#define FALSE 0

#define vgm_log printf
#define VGM_SAMPLE_COUNT		882 // NTSC FRAME
#define VGM_OUTPUT_BUFFER_COUNT 64   // 64 Frames
#define NO_CLAMP

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)

#define logerror printf

#define BURN_SND_CLIP(A) ((A) < -0x8000 ? -0x8000 : (A) > 0x7fff ? 0x7fff : (A))

// sound routes
#define BURN_SND_ROUTE_LEFT			1
#define BURN_SND_ROUTE_RIGHT		2
#define BURN_SND_ROUTE_BOTH			(BURN_SND_ROUTE_LEFT | BURN_SND_ROUTE_RIGHT)

#endif