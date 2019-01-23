#include <stm32f10x.h>
#include "io.h"
#include "LCD2.h"

#define CLK_E

#define BLACK         		 	0x0000
#define DARK_BLUE           	0x0000007F
#define DARK_GREEN          	0x00007F00
#define DARK_CYAN           	(DARK_GREEN | DARK_BLUE)
#define DARK_RED            	0x007F0000
#define DARK_MANGENTA       	(DARK_RED | DARK_BLUE)
#define DARK_YELLOW		      	(DARK_RED | DARK_GREEN)
#define GREY 			     	(DARK_RED | DARK_GREEN | DARK_BLUE)
#define LIGHT_BLUE           	0x000000FF
#define LIGHT_GREEN          	0x0000FF00
#define LIGHT_CYAN           	(LIGHT_GREEN | LIGHT_BLUE)
#define LIGHT_RED            	0x00FF0000
#define LIGHT_MANGENTA       	(LIGHT_RED | LIGHT_BLUE)
#define LIGHT_YELLOW		    (LIGHT_RED | LIGHT_GREEN)
#define WHITE         			(LIGHT_RED | LIGHT_GREEN | LIGHT_BLUE)

#ifdef LCD_TK022F2218
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#else
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 800
#endif

#ifdef CLK_E
#define TOGGLE_WR() WR.type->BRR = WR.pin; RD.type->BSRR = RD.pin;	RD.type->BRR = RD.pin
#define	SET_DATA(data) D0.type->ODR = data
#define	GET_DATA() ((u16)D0.type->ODR)
#else
#define TOGGLE_WR() WR.type->BRR = WR.pin;	WR.type->BSRR = WR.pin
#define	SET_DATA(data) D0.type->ODR = data
#define	GET_DATA() ((u16)D0.type->ODR)
#endif

#define RGB888(r, g, b) ((r<<16) | (g<<8) | (b))
#define RGB565(r, g, b) (((((u32)r)<<8)&0xf800) | ((((u32)g)<<3)&0x07e0) | ((((u32)b)>>3)&0x001f))
#define RGB444(r, g, b) (((r&0x0f)<<8)  | ((g&0x0f)<<4) | (b&0x0f))


typedef enum
{
	PF_RGB444 = 0,  // 12 bit format
	PF_RGB565,		 // 16 bit format
	PF_RGB666,		 // 18 bit format
	PF_RGB888,		 // 24 bit format
}PixelFormat;

void LCD_Config_GPIO(void)
{
}

void LCD_Reset(void)
{
#ifdef CLK_E	
	RD.type->BRR   = RD.pin; // WR inactive low
	WR.type->BRR   = WR.pin; // RD inactive low
#endif
}

u8 LCD_ReadData(void)
{
	u8 d;

#ifdef CLK_E
	LCD_CS.type->BRR = LCD_CS.pin;

	RD.type->BSRR = WR.pin;
	A0.type->BSRR = A0.pin;
	
	RD.type->BSRR = RD.pin;

	d = D0.type->IDR;

	RD.type->BRR = RD.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;
#else		
	LCD_CS.type->BRR = LCD_CS.pin;

	A0.type->BSRR = A0.pin;
	RD.type->BRR = RD.pin;

	d = D0.type->IDR;

	RD.type->BSRR = RD.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;
#endif
	/*
	IO_WRITE(A0, 1);

	IO_WRITE(LCD_CS, 0);

	IO_WRITE(RD, 0);

	d = GPIO_ReadOutputData(D0.type) & 0xff;

	IO_WRITE(RD, 1);

	IO_WRITE(LCD_CS, 1);
	*/

	return d;
}

void LCD_WriteData(u8 data)
{
#ifdef CLK_E
	LCD_CS.type->BRR = LCD_CS.pin;

	RD.type->BRR = WR.pin;
	A0.type->BSRR = A0.pin;
	D0.type->ODR = (D0.type->ODR & 0x0000ff00) | data;
	
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;
#else	
	LCD_CS.type->BRR = LCD_CS.pin;

	A0.type->BSRR = A0.pin;
	D0.type->ODR = (D0.type->ODR & 0x0000ff00) | data;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;
#endif
	/*
	u16 d = GPIO_ReadOutputData(D0.type);
	GPIO_Write(D0.type, (d & 0xff00) | data);

	IO_WRITE(A0, 1);

	IO_WRITE(LCD_CS, 0);

	IO_WRITE(WR, 0);

	IO_WRITE(WR, 1);

	IO_WRITE(LCD_CS, 1);
	*/
}

void LCD_WriteCommand0(u8 command)
{
#ifdef CLK_E
	LCD_CS.type->BRR = LCD_CS.pin;

	RD.type->BRR = WR.pin;
	A0.type->BRR = A0.pin;
	D0.type->ODR = (D0.type->ODR & 0x0000ff00) | command;
	
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;
#else
	LCD_CS.type->BRR = LCD_CS.pin;

	A0.type->BRR = A0.pin;
	D0.type->ODR = (D0.type->ODR & 0x0000ff00) | command;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;
#endif
	/*
	u16 d = GPIO_ReadOutputData(D0.type);
	GPIO_Write(D0.type, (d & 0xff00) | command);

	IO_WRITE(A0, 0);

	IO_WRITE(LCD_CS, 0);

	IO_WRITE(WR, 0);

	IO_WRITE(WR, 1);

	IO_WRITE(LCD_CS, 1);
	*/
}

void LCD_WriteCommand1(u8 command, u8 d0)
{
	u16 d = (D0.type->ODR & 0x0000ff00);
	LCD_CS.type->BRR = LCD_CS.pin;

#ifdef CLK_E
	WR.type->BRR = WR.pin;
	
	A0.type->BRR = A0.pin;
	D0.type->ODR = d | command;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;
#else	
	A0.type->BRR = A0.pin;
	D0.type->ODR = d | command;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;
#endif

#ifdef CLK_E
	WR.type->BRR = WR.pin;
	
	A0.type->BSRR = A0.pin;
	D0.type->ODR = d | d0;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;
#else		
	A0.type->BSRR = A0.pin;
	D0.type->ODR = d | d0;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;
#endif

	LCD_CS.type->BSRR = LCD_CS.pin;
}

void LCD_WriteCommand2(u8 command, u8 d0, u8 d1)
{
	u16 d = (D0.type->ODR & 0x0000ff00);

	/////////////////////////////////////
	LCD_CS.type->BRR = LCD_CS.pin;

	/////////////////////////////////////
#ifdef CLK_E
	WR.type->BRR = WR.pin;

	A0.type->BRR = A0.pin;	
	D0.type->ODR = d | command;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;
#else		
	A0.type->BRR = A0.pin;
	D0.type->ODR = d | command;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;
#endif

	/////////////////////////////////////
	A0.type->BSRR = A0.pin;
	
#ifdef CLK_E	
	WR.type->BRR = WR.pin;
	
	D0.type->ODR = d | d0;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;

	D0.type->ODR = d | d1;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;
#else
	D0.type->ODR = d | d0;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | d1;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;
#endif

	/////////////////////////////////////
	LCD_CS.type->BSRR = LCD_CS.pin;
}

void LCD_WriteCommand3(u8 command, u8 d0, u8 d1, u8 d2)
{
	u16 d = (D0.type->ODR & 0x0000ff00);

	/////////////////////////////////////
	LCD_CS.type->BRR = LCD_CS.pin;

	/////////////////////////////////////
#ifdef CLK_E
	WR.type->BRR = WR.pin;

	A0.type->BRR = A0.pin;	
	D0.type->ODR = d | command;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;
#else		
	A0.type->BRR = A0.pin;
	D0.type->ODR = d | command;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;
#endif
	/////////////////////////////////////
	A0.type->BSRR = A0.pin;

#ifdef CLK_E	
	WR.type->BRR = WR.pin;
	
	D0.type->ODR = d | d0;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;

	D0.type->ODR = d | d1;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;

	D0.type->ODR = d | d2;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;
#else	
	D0.type->ODR = d | d0;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | d1;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | d2;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;
#endif	

	/////////////////////////////////////
	LCD_CS.type->BSRR = LCD_CS.pin;
}

void LCD_WriteCommand4(u8 command, u8 d0, u8 d1, u8 d2, u8 d3)
{
	u16 d = (D0.type->ODR & 0x0000ff00);

	/////////////////////////////////////
	LCD_CS.type->BRR = LCD_CS.pin;

	/////////////////////////////////////
#ifdef CLK_E
	WR.type->BRR = WR.pin;

	A0.type->BRR = A0.pin;	
	D0.type->ODR = d | command;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;
#else		
	A0.type->BRR = A0.pin;
	D0.type->ODR = d | command;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;
#endif
	/////////////////////////////////////
	A0.type->BSRR = A0.pin;

#ifdef CLK_E	
	WR.type->BRR = WR.pin;
	
	D0.type->ODR = d | d0;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;

	D0.type->ODR = d | d1;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;

	D0.type->ODR = d | d2;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;

	D0.type->ODR = d | d3;
	RD.type->BSRR = RD.pin;
	RD.type->BRR = RD.pin;
#else	
	D0.type->ODR = d | d0;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | d1;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | d2;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | d3;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;
#endif	

	/////////////////////////////////////
	LCD_CS.type->BSRR = LCD_CS.pin;
}

u8 LCD_ReadU8(u8 command)
{
	LCD_WriteCommand0(command);

	LCD_ReadData(); // dummy

	return LCD_ReadData();
}

u16 LCD_ReadU16(u8 command)
{
	u16 data;
	LCD_WriteCommand0(command);

	LCD_ReadData(); // dummy

	data = LCD_ReadData(); // dummy
	data <<= 8;
	data += LCD_ReadData(); // dummy

	return data;
}

u32 LCD_ReadU32(u8 command)
{
	u32 data;
	LCD_WriteCommand0(command);

	LCD_ReadData(); // dummy

	data = LCD_ReadData(); // dummy
	data <<= 8;

	data += LCD_ReadData(); // dummy
	data <<= 8;

	data += LCD_ReadData(); // dummy
	data <<= 8;

	data += LCD_ReadData(); // dummy
	data <<= 8;

	return data;
}

#define LCD_NOP()											LCD_WriteCommand0(0x00)
#define LCD_SWReset()										LCD_WriteCommand0(0x01)
#define LCD_DummyRead()										LCD_ReadU32(0x04)
#define LCD_ReadRedColor() 									LCD_ReadU8(0x06)
#define LCD_ReadGreenColor() 								LCD_ReadU8(0x07)
#define LCD_ReadBlueColor()	 								LCD_ReadU8(0x08)
#define LCD_ReadStatus() 									LCD_ReadU32(0x09)
#define LCD_ReadDisplayPowerMode()	 						LCD_ReadU8(0x0a)
#define LCD_ReadDisplayMADCtrl()						 	LCD_ReadU8(0x0b)
#define LCD_ReadDisplayPixelFormat()						LCD_ReadU8(0x0c)
#define LCD_ReadDisplayImageMode()							LCD_ReadU8(0x0d)
#define LCD_ReadDisplaySignalMode()							LCD_ReadU8(0x0e)
#define LCD_ReadDisplayDiagnoseResult()						LCD_ReadU8(0x0f)

#define LCD_Sleep()											LCD_WriteCommand0(0x10)
#define LCD_Wake()											LCD_WriteCommand0(0x11)
#define LCD_SetPartialMode()								LCD_WriteCommand0(0x12)
#define LCD_SetNormalMode()									LCD_WriteCommand0(0x13)

#define LCD_SetInvertMode()									LCD_WriteCommand0(0x20)
#define LCD_ResetInvertMode()								LCD_WriteCommand0(0x21)

#define LCD_SetGamma(gamma)									LCD_WriteU8(0x26, gamma)

#define LCD_SetDisplayOff()									LCD_WriteCommand0(0x28)
#define LCD_SetDisplayOn()									LCD_WriteCommand0(0x29)
#define LCD_SetColumnAddress(start, end)					LCD_WriteCommand4(0x2a, start>>8, start, end>>8, end)
#define LCD_SetRowAddress(start, end)						LCD_WriteCommand4(0x2b, start>>8, start, end>>8, end)
#define LCD_StartWriteMemory()								LCD_WriteCommand0(0x2c)
#define LCD_EndWriteMemory()								LCD_NOP()
#define LCD_StartWritePalette()								LCD_WriteCommand0(0x2d)
#define LCD_EndWritePalette()								LCD_NOP()
#define LCD_StartReadMemory()								LCD_ReadU8(0x2e)
#define LCD_EndReadMemory()									LCD_NOP()
#define LCD_SetPartialArea(start, end) 						LCD_WriteCommand4(0x30, start>>8, start, end>>8, end)
#define LCD_SetScrollArea(tfa, vsa, bfa)					LCD_WriteCommand4(0x33, tfa>>8, tfa, vsa>>8, vsa); LCD_WriteData(bfa>>8) LCD_WriteData(bfa)
#define LCD_SetTearingLineOff()								LCD_WriteCommand0(0x34)
#define LCD_SetTearingLineOn(vblank)						LCD_WriteCommand1(0x35, vblank?1:0)
#define LCD_SetAddressMode(ml, bgr, ss)						LCD_WriteCommand1(0x36, ((ml?1:0)<<4) | ((bgr?1:0)<<3) | ((ss?1:0)<<2)) // 0x08
#define LCD_SetScrollStart(vsp)								LCD_WriteCommand4(0x37, vsp>>8, vsp)
#define LCD_ExitIdleMode()									LCD_WriteCommand0(0x38)
#define LCD_EnterIdleMode()									LCD_WriteCommand0(0x39)
#define LCD_SetDisplayPixelFormat(format)					LCD_WriteCommand1(0x3A, format)
#define LCD_WriteMemoryContinue()							LCD_WriteCommand0(0x3C)
#define LCD_ReadMemoryContinue()							LCD_ReadU8(0x3D)
#define LCD_SetTearingLine(line)							LCD_WriteCommand2(0x44, line>>8, line)
#define LCD_GetCurrentScanLine()							LCD_ReadU16(0x45)
#define LCD_SetBrightness(brightness)						LCD_WriteU8(0x51, brightness)
#define LCD_GetBrightness()									LCD_ReadU8(0x52)
#define LCD_SetDisplayControl(bctrl, dimming, bl)			LCD_WriteU8(0x53, ((bctrl?1:0)<<5) | ((dd?1:0)<<3) | ((bl?1:0)<<2))
#define LCD_GetDisplayControl(bctrl, dimming, bl)			LCD_ReadU8(0x54)
#define LCD_SetContentAdaptiveBrightnessControl(ctrl)		LCD_WriteU8(0x55, ctrl)
#define LCD_GetContentAdaptiveBrightnessControl()			(LCD_ReadU16(0x56)>>8)

void LCD_LoadPalette(PixelFormat format)
{
	u32 i;
	switch(format)
	{
		case PF_RGB666:
		case PF_RGB888:
		{
			LCD_WriteCommand0(0x2D);
			for (i = 0; i <64; i++)
				LCD_WriteData(i * 4);

			for (i = 0; i <64; i++)
				LCD_WriteData(i * 4);

			for (i = 0; i <64; i++)
				LCD_WriteData(i * 4);
		}
		break;
		
		case PF_RGB565:
		{
			LCD_WriteCommand0(0x2D);
			for (i = 0; i <64; i++)
			{
				if(i<32)
					LCD_WriteData(i * 8);
				else
					LCD_WriteData(0);
			}

			for (i = 0; i <64; i++)
				LCD_WriteData(i * 4);

			for (i = 0; i <64; i++)
			{
				if(i<32)
					LCD_WriteData(i * 8);
				else
					LCD_WriteData(0);
			}
		}
		break;
		
		case PF_RGB444:
		{
		}
		break;			
	}
}

void LCD_SetRegisters()
{
#ifdef LCD_TK022F2218	
	LCD_WriteCommand(0xCB);
	LCD_WriteData(0x39);
	LCD_WriteData(0x2C);
	LCD_WriteData(0x00);
	LCD_WriteData(0x34);
	LCD_WriteData(0x02);

	LCD_WriteCommand(0xCF);
	LCD_WriteData(0x00);
	LCD_WriteData(0XC1);
	LCD_WriteData(0X30);

	LCD_WriteCommand(0xE8);
	LCD_WriteData(0x85);
	LCD_WriteData(0x00);
	LCD_WriteData(0x78);

	LCD_WriteCommand(0xEA);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);

	LCD_WriteCommand(0xED);
	LCD_WriteData(0x64);
	LCD_WriteData(0x03);
	LCD_WriteData(0X12);
	LCD_WriteData(0X81);

	LCD_WriteCommand(0xF7);
	LCD_WriteData(0x20);


	LCD_WriteCommand(0xC0);    //Power control 
	LCD_WriteData(0x23);   //VRH[5:0] 

	LCD_WriteCommand(0xC1);    //Power control 
	LCD_WriteData(0x10);   //SAP[2:0];BT[3:0] 

	LCD_WriteCommand(0xC5);    //VCM control 
	LCD_WriteData(0x3e); //对比度调节
	LCD_WriteData(0x28);

	LCD_WriteCommand(0xC7);    //VCM control2 
	LCD_WriteData(0x00);  //--

	LCD_WriteCommand(0x21);
	LCD_WriteCommand(0x36);    // Memory Access Control 
	LCD_WriteData(0xC8);
	LCD_WriteCommand(0x3A);
	LCD_WriteData(0x55);

	LCD_WriteCommand(0xB1);
	LCD_WriteData(0x00);
	LCD_WriteData(0x18);

	LCD_WriteCommand(0xB6);    // Display Function Control 
	LCD_WriteData(0x08);
	LCD_WriteData(0x82);
	LCD_WriteData(0x27);

	LCD_WriteCommand(0xF2);    // 3Gamma Function Disable 
	LCD_WriteData(0x00);

	LCD_WriteCommand(0x26);    //Gamma curve selected 
	LCD_WriteData(0x01);

	LCD_WriteCommand(0xE0);    //Set Gamma 
	LCD_WriteData(0x0F);
	LCD_WriteData(0x31);
	LCD_WriteData(0x2B);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x08);
	LCD_WriteData(0x4E);
	LCD_WriteData(0xF1);
	LCD_WriteData(0x37);
	LCD_WriteData(0x07);
	LCD_WriteData(0x10);
	LCD_WriteData(0x03);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x09);
	LCD_WriteData(0x00);

	LCD_WriteCommand(0XE1);    //Set Gamma 
	LCD_WriteData(0x00);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x14);
	LCD_WriteData(0x03);
	LCD_WriteData(0x11);
	LCD_WriteData(0x07);
	LCD_WriteData(0x31);
	LCD_WriteData(0xC1);
	LCD_WriteData(0x48);
	LCD_WriteData(0x08);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x31);
	LCD_WriteData(0x36);
	LCD_WriteData(0x0F);

	LCD_WriteCommand(0x11);    //Exit Sleep 
	LCD_WriteCommand(0x29);    //Display on 
	delay_us(120);
#else
	LCD_WriteCommand0(0xB9);  // SET password
	LCD_WriteData(0xFF);
	LCD_WriteData(0x83);
	LCD_WriteData(0x69);

	LCD_WriteCommand0(0xB1);  //Set Power 
	LCD_WriteData(0x85);
	LCD_WriteData(0x00);
	LCD_WriteData(0x34);
	LCD_WriteData(0x07);
	LCD_WriteData(0x00);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x2A);
	LCD_WriteData(0x32);
	LCD_WriteData(0x3F);
	LCD_WriteData(0x3F);
	LCD_WriteData(0x01); //update VBIAS
	LCD_WriteData(0x3A);
	LCD_WriteData(0x01);
	LCD_WriteData(0xE6);
	LCD_WriteData(0xE6);
	LCD_WriteData(0xE6);
	LCD_WriteData(0xE6);
	LCD_WriteData(0xE6);

	LCD_WriteCommand0(0xB2);  	// SET Display  480x800
	LCD_WriteData(0x00);
	LCD_WriteData(0x20);  		//23    
	LCD_WriteData(0x05); 		//03
	LCD_WriteData(0x05);  		//03
	LCD_WriteData(0x70);
	LCD_WriteData(0x00);
	LCD_WriteData(0xFF);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x03);
	LCD_WriteData(0x03);
	LCD_WriteData(0x00);
	LCD_WriteData(0x01);

	LCD_WriteCommand0(0xB4);  	// SET Display  480x800
	LCD_WriteData(0x00);
	LCD_WriteData(0x18);
	LCD_WriteData(0x80);
	LCD_WriteData(0x06);
	LCD_WriteData(0x02);

	LCD_WriteCommand0(0xB6);  	// SET VCOM
	LCD_WriteData(0x42);  		// Update VCOM
	LCD_WriteData(0x42);

	LCD_WriteCommand0(0xD5);
	LCD_WriteData(0x00);
	LCD_WriteData(0x04);
	LCD_WriteData(0x03);
	LCD_WriteData(0x00);
	LCD_WriteData(0x01);
	LCD_WriteData(0x04); 		//05 04
	LCD_WriteData(0x1A); 		//28 
	LCD_WriteData(0xff);  		//70
	LCD_WriteData(0x01);
	LCD_WriteData(0x13);		//03  
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x40);
	LCD_WriteData(0x06);
	LCD_WriteData(0x51);
	LCD_WriteData(0x07);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x41);
	LCD_WriteData(0x06);
	LCD_WriteData(0x50);
	LCD_WriteData(0x07);
	LCD_WriteData(0x07);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x04);
	LCD_WriteData(0x00);

	///Gamma2.2 
	LCD_WriteCommand0(0xE0);
	LCD_WriteData(0x00);
	LCD_WriteData(0x13);
	LCD_WriteData(0x19);
	LCD_WriteData(0x38);
	LCD_WriteData(0x3D);
	LCD_WriteData(0x3F);
	LCD_WriteData(0x28);
	LCD_WriteData(0x46);
	LCD_WriteData(0x07);
	LCD_WriteData(0x0D);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x12);
	LCD_WriteData(0x15);
	LCD_WriteData(0x12);
	LCD_WriteData(0x14);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x17);
	LCD_WriteData(0x00);
	LCD_WriteData(0x13);
	LCD_WriteData(0x19);
	LCD_WriteData(0x38);
	LCD_WriteData(0x3D);
	LCD_WriteData(0x3F);
	LCD_WriteData(0x28);
	LCD_WriteData(0x46);
	LCD_WriteData(0x07);
	LCD_WriteData(0x0D);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x12);
	LCD_WriteData(0x15);
	LCD_WriteData(0x12);
	LCD_WriteData(0x14);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x17);
	delay_us(10);

	///DGC Setting 
	LCD_WriteCommand0(0xC1);
	LCD_WriteData(0x01);
	//R 
	LCD_WriteData(0x04);
	LCD_WriteData(0x13);
	LCD_WriteData(0x1a);
	LCD_WriteData(0x20);
	LCD_WriteData(0x27);
	LCD_WriteData(0x2c);
	LCD_WriteData(0x32);
	LCD_WriteData(0x36);
	LCD_WriteData(0x3f);
	LCD_WriteData(0x47);
	LCD_WriteData(0x50);
	LCD_WriteData(0x59);
	LCD_WriteData(0x60);
	LCD_WriteData(0x68);
	LCD_WriteData(0x71);
	LCD_WriteData(0x7B);
	LCD_WriteData(0x82);
	LCD_WriteData(0x89);
	LCD_WriteData(0x91);
	LCD_WriteData(0x98);
	LCD_WriteData(0xA0);
	LCD_WriteData(0xA8);
	LCD_WriteData(0xB0);
	LCD_WriteData(0xB8);
	LCD_WriteData(0xC1);
	LCD_WriteData(0xC9);
	LCD_WriteData(0xD0);
	LCD_WriteData(0xD7);
	LCD_WriteData(0xE0);
	LCD_WriteData(0xE7);
	LCD_WriteData(0xEF);
	LCD_WriteData(0xF7);
	LCD_WriteData(0xFE);
	LCD_WriteData(0xCF);
	LCD_WriteData(0x52);
	LCD_WriteData(0x34);
	LCD_WriteData(0xF8);
	LCD_WriteData(0x51);
	LCD_WriteData(0xF5);
	LCD_WriteData(0x9D);
	LCD_WriteData(0x75);
	LCD_WriteData(0x00);
	//G 
	LCD_WriteData(0x04);
	LCD_WriteData(0x13);
	LCD_WriteData(0x1a);
	LCD_WriteData(0x20);
	LCD_WriteData(0x27);
	LCD_WriteData(0x2c);
	LCD_WriteData(0x32);
	LCD_WriteData(0x36);
	LCD_WriteData(0x3f);
	LCD_WriteData(0x47);
	LCD_WriteData(0x50);
	LCD_WriteData(0x59);
	LCD_WriteData(0x60);
	LCD_WriteData(0x68);
	LCD_WriteData(0x71);
	LCD_WriteData(0x7B);
	LCD_WriteData(0x82);
	LCD_WriteData(0x89);
	LCD_WriteData(0x91);
	LCD_WriteData(0x98);
	LCD_WriteData(0xA0);
	LCD_WriteData(0xA8);
	LCD_WriteData(0xB0);
	LCD_WriteData(0xB8);
	LCD_WriteData(0xC1);
	LCD_WriteData(0xC9);
	LCD_WriteData(0xD0);
	LCD_WriteData(0xD7);
	LCD_WriteData(0xE0);
	LCD_WriteData(0xE7);
	LCD_WriteData(0xEF);
	LCD_WriteData(0xF7);
	LCD_WriteData(0xFE);
	LCD_WriteData(0xCF);
	LCD_WriteData(0x52);
	LCD_WriteData(0x34);
	LCD_WriteData(0xF8);
	LCD_WriteData(0x51);
	LCD_WriteData(0xF5);
	LCD_WriteData(0x9D);
	LCD_WriteData(0x75);
	LCD_WriteData(0x00);
	//B 
	LCD_WriteData(0x04);
	LCD_WriteData(0x13);
	LCD_WriteData(0x1a);
	LCD_WriteData(0x20);
	LCD_WriteData(0x27);
	LCD_WriteData(0x2c);
	LCD_WriteData(0x32);
	LCD_WriteData(0x36);
	LCD_WriteData(0x3f);
	LCD_WriteData(0x47);
	LCD_WriteData(0x50);
	LCD_WriteData(0x59);
	LCD_WriteData(0x60);
	LCD_WriteData(0x68);
	LCD_WriteData(0x71);
	LCD_WriteData(0x7B);
	LCD_WriteData(0x82);
	LCD_WriteData(0x89);
	LCD_WriteData(0x91);
	LCD_WriteData(0x98);
	LCD_WriteData(0xA0);
	LCD_WriteData(0xA8);
	LCD_WriteData(0xB0);
	LCD_WriteData(0xB8);
	LCD_WriteData(0xC1);
	LCD_WriteData(0xC9);
	LCD_WriteData(0xD0);
	LCD_WriteData(0xD7);
	LCD_WriteData(0xE0);
	LCD_WriteData(0xE7);
	LCD_WriteData(0xEF);
	LCD_WriteData(0xF7);
	LCD_WriteData(0xFE);
	LCD_WriteData(0xCF);
	LCD_WriteData(0x52);
	LCD_WriteData(0x34);
	LCD_WriteData(0xF8);
	LCD_WriteData(0x51);
	LCD_WriteData(0xF5);
	LCD_WriteData(0x9D);
	LCD_WriteData(0x75);
	LCD_WriteData(0x00);
	delay_us(10);

	LCD_WriteCommand0(0x11);
	delay_us(120);

	LCD_WriteCommand0(0x29);

	LCD_WriteCommand0(0x2C);
	delay_us(10);
	
	LCD_WriteCommand0(0x11);
	delay_us(120);
#endif	
}

void LCD_SetPixelFormat(PixelFormat pixelFormat)
{
	u8 data;

	switch(pixelFormat)
	{
#ifdef LCD_TK022F2218
		case PF_RGB444:
			data = 0x33;
		break;	
#else
#endif
		case PF_RGB565:
			data = 0x55;
		break;
		case PF_RGB666:
			data = 0x66;
		break;
#ifdef LCD_TK022F2218		
#else
		case PF_RGB888:
		default:	
			data = 0x77;
		break;
#endif
	};

	LCD_WriteCommand0(0x3A);
	LCD_WriteData(data);
}

void LCD_TurnOnDisplay(void)
{
	LCD_WriteCommand0(0x29);  //Display on 
}

void LCD_Initialize(void)
{
	u32 w = 0;

	LCD_Config_GPIO();
	LCD_Reset();

	LCD_SetRegisters();

#ifdef COLOR565	
	LCD_LoadPalette(PF_RGB565);	
	LCD_SetPixelFormat(PF_RGB565);
	//LCD_SetPixelFormat(PF_RGB444);	
	//LCD_SetPixelFormat(PF_RGB666);
#else
	LCD_LoadPalette(PF_RGB888);
	LCD_SetPixelFormat(PF_RGB888);
#endif	
	//LCD_SetAddressMode(0, 1, 0);
	LCD_SetDisplayOn();
	//LCD_SetDisplayOff();
	delay_us(120);

	while (1)
	{
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x00, 0x00, 0x00); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x00, 0x00, 0x7f); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x00, 0x7f, 0x00); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x00, 0x7f, 0x7f); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x7f, 0x00, 0x00); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x7f, 0x00, 0x7f); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x7f, 0x7f, 0x00); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x7f, 0x7f, 0x7f); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x00, 0x00, 0xff); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x00, 0xff, 0x00); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0x00, 0xff, 0xff); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0xff, 0x00, 0x00); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0xff, 0x00, 0xff); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0xff, 0xff, 0x00); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, SCREEN_WIDTH - w * 2, SCREEN_HEIGHT - w * 2, 0xff, 0xff, 0xff); w += 5;
		delay_ms(20000);
		w = 0;
	}
}

void LCD_WR_REG(u8 address, u8 data)
{
	LCD_WriteCommand0(address);

	LCD_WriteData(data);
}

void LCD_DrawRect565(u32 x, u32 y, u32 w, u32 h, u8 r, u8 g, u8 b)
{
	u16 d0;
	u16 d1;
	u16 d2;
	u32 count = w * h;
	u32 xEnd = x + w - 1;
	u32 yEnd = y + h - 1;
	u32 color = RGB565(r, g, b);
	u16 d = GET_DATA() & 0xff00;
	d1 = d | (color >> 8);
	d0 = d | (color & 0xff);

	LCD_SetColumnAddress(x, xEnd);
	LCD_SetRowAddress(y, yEnd);
	LCD_StartWriteMemory();

	////////////////////////////////////////////////////
	IO_WRITE(A0, 1);
	IO_WRITE(LCD_CS, 0);
	while(count--)
	{
		SET_DATA(d1);

		TOGGLE_WR();

		SET_DATA(d0);

		TOGGLE_WR();
	}

	IO_WRITE(LCD_CS, 1);
	//////////////////////////////////////////////////////
}

void LCD_DrawRect888(u32 x, u32 y, u32 w, u32 h, u8 r, u8 g, u8 b)
{
	u16 d0;
	u16 d1;
	u16 d2;
	u32 count = w * h;
	u32 xEnd = x + w - 1;
	u32 yEnd = y + h - 1;
	u32 color = RGB888(r, g, b);
	u16 d = GET_DATA() & 0xff00;
	
	d2 = d | r;
	d1 = d | g;
	d0 = d | b;
	LCD_SetColumnAddress(x, xEnd);
	LCD_SetRowAddress(y, yEnd);
	LCD_StartWriteMemory();

	////////////////////////////////////////////////////
	IO_WRITE(A0, 1);
	IO_WRITE(LCD_CS, 0);

	while(count--)
	{
		SET_DATA(d2);

		TOGGLE_WR();

		SET_DATA(d1);

		TOGGLE_WR();
		
		SET_DATA(d0);

		TOGGLE_WR();
	}

	IO_WRITE(LCD_CS, 1);
	//////////////////////////////////////////////////////
}

typedef __packed struct
{
	u8  pic_head[2];				//1
	u16 pic_size_l;			    //2
	u16 pic_size_h;			    //3
	u16 pic_nc1;				    //4
	u16 pic_nc2;				    //5
	u16 pic_data_address_l;	    //6
	u16 pic_data_address_h;		//7	
	u16 pic_message_head_len_l;	//8
	u16 pic_message_head_len_h;	//9
	u16 pic_w_l;					//10
	u16 pic_w_h;				    //11
	u16 pic_h_l;				    //12
	u16 pic_h_h;				    //13	
	u16 pic_bit;				    //14
	u16 pic_dip;				    //15
	u16 pic_zip_l;			    //16
	u16 pic_zip_h;			    //17
	u16 pic_data_size_l;		    //18
	u16 pic_data_size_h;		    //19
	u16 pic_dipx_l;			    //20
	u16 pic_dipx_h;			    //21	
	u16 pic_dipy_l;			    //22
	u16 pic_dipy_h;			    //23
	u16 pic_color_index_l;	    //24
	u16 pic_color_index_h;	    //25
	u16 pic_other_l;			    //26
	u16 pic_other_h;			    //27
	u16 pic_color_p01;		    //28
	u16 pic_color_p02;		    //29
	u16 pic_color_p03;		    //30
	u16 pic_color_p04;		    //31
	u16 pic_color_p05;		    //32
	u16 pic_color_p06;		    //33
	u16 pic_color_p07;		    //34
	u16 pic_color_p08;			//35			
}BMP_HEAD;
