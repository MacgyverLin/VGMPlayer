#ifndef _vgmdef_h_
#define _vgmdef_h_

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_NTSC 53693175
#define CLOCK_PAL  53203424

#define CLOCK_NTSC 53693175
#define CLOCK_PAL  53203424

	
typedef signed long long INT64;
typedef signed int INT32;
typedef signed short INT16;
typedef signed char INT8;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef unsigned long long UINT64;

#define logerror printf

#define BURN_SND_CLIP(A) ((A) < -0x8000 ? -0x8000 : (A) > 0x7fff ? 0x7fff : (A))

// sound routes
#define BURN_SND_ROUTE_LEFT			1
#define BURN_SND_ROUTE_RIGHT		2
#define BURN_SND_ROUTE_BOTH			(BURN_SND_ROUTE_LEFT | BURN_SND_ROUTE_RIGHT)


#ifdef __cplusplus
};
#endif

#endif