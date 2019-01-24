#ifndef _LCD_h_
#define _LCD_h_

#include "stm32f10x.h"
#include "vgmdef.h"

#ifdef __cplusplus
extern "C" {
#endif

void LCD_Initialize(void);

void LCD_DrawRect888(u32 x, u32 y, u32 w, u32 h, u8 r, u8 g, u8 b);
void LCD_DrawRect565(u32 x, u32 y, u32 w, u32 h, u8 r, u8 g, u8 b);
void LCD_DrawRect444(u32 x, u32 y, u32 w, u32 h, u8 r, u8 g, u8 b);
void LCD_DrawJPEG888(u32 x, u32 y, const u8* filename);
void LCD_DrawJPEG565(u32 x, u32 y, const u8* filename);
void LCD_DrawJPEG444(u32 x, u32 y, const u8* filename);
	
#define LCD_TK022F2218

//#define COLOR888	
//#define COLOR565
#define COLOR444

#ifdef COLOR888
#define LCD_DrawRect LCD_DrawRect888
#define LCD_DrawJPEG LCD_DrawImage888
#endif
#ifdef COLOR565
#define LCD_DrawRect LCD_DrawRect565
#define LCD_DrawJPEG LCD_DrawImage565
#endif
#ifdef COLOR444
#define LCD_DrawRect LCD_DrawRect444
#define LCD_DrawJPEG LCD_DrawImage444
#endif

#ifdef __cplusplus
};
#endif

#endif
