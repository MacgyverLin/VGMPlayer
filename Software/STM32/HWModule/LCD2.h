#ifndef _LCD_h_
#define _LCD_h_

#include "stm32f10x.h"
#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

void LCD_Initialize(void);
void LCD_DrawBMP(u32 x, u32 y, const u8* filename);
void LCD_DrawRect888(u32 x, u32 y, u32 w, u32 h, u8 r, u8 g, u8 b);
void LCD_DrawRect565(u32 x, u32 y, u32 w, u32 h, u8 r, u8 g, u8 b);

//#define LCD_TK022F2218
	
//#define COLOR565
#ifdef COLOR565
#define LCD_DrawRect LCD_DrawRect565
#else
#define LCD_DrawRect LCD_DrawRect888
#endif
	
#ifdef __cplusplus
};
#endif

#endif
