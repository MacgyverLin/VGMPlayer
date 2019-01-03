#ifndef _VGMDef_h_
#define _VGMDef_h_

#include <windows.h>

#ifdef WIN32
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
#else
typedef bool BOOL;
typedef char CHAR;
typedef void VOID;
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
#define TRUE true
#define FALSE false
#endif

#define vgm_log printf
#define VGM_SAMPLE_COUNT		882 // NTSC FRAME
#define VGM_OUTPUT_BUFFER_COUNT 4   // 64 Frames
#define NO_CLAMP
#define PI 3.141592654f

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)

#endif