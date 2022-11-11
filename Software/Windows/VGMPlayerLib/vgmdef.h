#ifndef _vgmdef_h_
#define _vgmdef_h_

#include <windows.h>
#include <assert.h>

//typedef unsigned char boolean;
typedef signed long long INT64;
typedef signed int INT32;
typedef signed short INT16;
typedef signed char INT8;
typedef unsigned long long UINT64;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef float FLOAT32;
typedef double FLOAT64;

#include "../stdbool.h"

typedef struct waveform_16bit_stereo
{
	INT16 Left;
	INT16 Right;
} WAVE_16BS;

typedef struct waveform_32bit_stereo
{
	INT32 Left;
	INT32 Right;
} WAVE_32BS;

/* offsets and addresses are 32-bit (for now...) */
typedef UINT32	offs_t;

/* stream_sample_t is used to represent a single sample in a sound stream */
typedef INT32 stream_sample_t;
typedef void (*strm_func)(UINT8 ChipID, stream_sample_t** outputs, int samples, WAVE_32BS** channeloutputs, int channelcount);

extern stream_sample_t* DUMMYBUF[];
typedef void (*SRATE_CALLBACK)(void*, UINT32);

extern void chip_reg_write(UINT8 ChipType, UINT8 ChipID, UINT8 Port, UINT8 Offset, UINT8 Data);

#if defined(_MSC_VER)
//#define INLINE	static __forceinline
#define INLINE	static __inline
#elif defined(__GNUC__)
#define INLINE	static __inline__
#else
#define INLINE	static inline
#endif

// #define VGM_LITTLE_ENDIAN

#define DISABLE_HW_SUPPORT	// disable support for OPL hardware
#define FUINT8	unsigned int
#define FUINT16	unsigned int

#define logerror printf

extern char* FindFile(const char* FileName);

#define NO_CLAMP
#define M_PI 3.141592654f

#define vgm_log printf
#define VGM_SAMPLE_BUFFER_SIZE		1024 // (44100 / VGM_FRAME_PER_SECOND)
#define VGM_FRAME_PER_SECOND		(44100 / VGM_SAMPLE_BUFFER_SIZE)
#define VGM_OUTPUT_BUFFER_COUNT		4   // 64 Frames

#define VGM_NOTE_BUFFER_SIZE		(64)

#define CHANNEL_BUFFER_COUNT		(32)

#define VGMPlayer_MIN(a, b) ((a)<(b)) ? (a) : (b)
#define VGMPlayer_MAX(a, b) ((a)>(b)) ? (a) : (b)



#endif
