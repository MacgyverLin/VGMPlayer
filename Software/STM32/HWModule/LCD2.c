#include <stm32f10x.h>
#include "io.h"
#include "LCD2.h"

#define BLACK         		 	0x0000

#define DARK_BLUE           	0x000F
#define DARK_GREEN          	0x03E0
#define DARK_CYAN           	(DARK_GREEN | DARK_BLUE)
#define DARK_RED            	0x7800
#define DARK_MANGENTA       	(DARK_RED | DARK_BLUE)
#define DARK_YELLOW		      	(DARK_RED | DARK_GREEN)
#define GREY 			     	(DARK_RED | DARK_GREEN | DARK_BLUE)
#define LIGHT_BLUE           	0x001F
#define LIGHT_GREEN          	0x07E0
#define LIGHT_CYAN           	(LIGHT_GREEN | LIGHT_BLUE)
#define LIGHT_RED            	0xF800
#define LIGHT_MANGENTA       	(LIGHT_RED | LIGHT_BLUE)
#define LIGHT_YELLOW		    (LIGHT_RED | LIGHT_GREEN)
#define WHITE         			(LIGHT_RED | LIGHT_GREEN | LIGHT_BLUE)

void LCD_Config_GPIO(void);
void LCD_Reset(void);
void LCD_WriteCommand(u8 command);
void LCD_WriteData(u8 data);
u8 LCD_ReadData(void);

void LCD_Initialize(void);
void LCD_DrawRect(u32 x, u32 y, u32 w, u32 h, u32 color);
void LCD_DrawBMP(u32 x, u32 y, const u8* filename);

void LCD_Config_GPIO(void)
{
}

void LCD_Reset(void)
{
}

void LCD_WriteCommand(u8 command)
{
	LCD_CS.type->BRR = LCD_CS.pin;

	A0.type->BRR = A0.pin;
	D0.type->ODR = (D0.type->ODR & 0x0000ff00) | command;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;
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

void LCD_WriteData(u8 data)
{
	LCD_CS.type->BRR = LCD_CS.pin;

	A0.type->BSRR = A0.pin;
	D0.type->ODR = (D0.type->ODR & 0x0000ff00) | data;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;

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

u8 LCD_ReadData(void)
{
	u8 d;

	LCD_CS.type->BRR = LCD_CS.pin;

	A0.type->BSRR = A0.pin;
	RD.type->BRR = WR.pin;

	d = D0.type->IDR;

	RD.type->BSRR = WR.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;

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

void LCD_WriteU8(u8 command, u8 data)
{
	u16 d = (D0.type->ODR & 0x0000ff00);
	LCD_CS.type->BRR = LCD_CS.pin;

	A0.type->BRR = A0.pin;
	D0.type->ODR = d | command;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	A0.type->BSRR = A0.pin;
	D0.type->ODR = d | data;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	LCD_CS.type->BSRR = LCD_CS.pin;

	// LCD_WriteCommand(command);

	// LCD_WriteData(data);
}

void LCD_WriteU16(u8 command, u16 data)
{
	u16 d = (D0.type->ODR & 0x0000ff00);

	/////////////////////////////////////
	LCD_CS.type->BRR = LCD_CS.pin;

	/////////////////////////////////////
	A0.type->BRR = A0.pin;

	D0.type->ODR = d | command;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	/////////////////////////////////////
	A0.type->BSRR = A0.pin;

	D0.type->ODR = d | ((data >> 8) & 0xff);
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | ((data) & 0xff);
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	/////////////////////////////////////
	LCD_CS.type->BSRR = LCD_CS.pin;

	/*
	LCD_WriteCommand(command);
	LCD_WriteData((data >> 8) & 0xff);
	LCD_WriteData((data) & 0xff);
	*/
}

void LCD_WriteU32(u8 command, u32 data)
{
	u16 d = (D0.type->ODR & 0x0000ff00);

	/////////////////////////////////////
	LCD_CS.type->BRR = LCD_CS.pin;

	/////////////////////////////////////
	A0.type->BRR = A0.pin;

	D0.type->ODR = d | command;
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	/////////////////////////////////////
	A0.type->BSRR = A0.pin;

	D0.type->ODR = d | ((data >> 24) & 0xff);
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | ((data >> 16) & 0xff);
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | ((data >> 8) & 0xff);
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	D0.type->ODR = d | ((data) & 0xff);
	WR.type->BRR = WR.pin;
	WR.type->BSRR = WR.pin;

	/////////////////////////////////////
	LCD_CS.type->BSRR = LCD_CS.pin;
	/*
		LCD_WriteCommand(command);

		LCD_WriteData((data >> 24) & 0xff);
		LCD_WriteData((data >> 16) & 0xff);
		LCD_WriteData((data >> 8) & 0xff);
		LCD_WriteData((data) & 0xff);
	*/
}

u8 LCD_ReadU8(u8 command)
{
	LCD_WriteCommand(command);

	LCD_ReadData(); // dummy

	return LCD_ReadData();
}

u16 LCD_ReadU16(u8 command)
{
	u16 data;
	LCD_WriteCommand(command);

	LCD_ReadData(); // dummy

	data = LCD_ReadData(); // dummy
	data <<= 8;
	data += LCD_ReadData(); // dummy

	return data;
}

u32 LCD_ReadU32(u8 command)
{
	u32 data;
	LCD_WriteCommand(command);

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

#define LCD_NOP()											LCD_WriteCommand(0x00)
#define LCD_SWReset()										LCD_WriteCommand(0x01)
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

#define LCD_Sleep()											LCD_WriteCommand(0x10)
#define LCD_Wake()											LCD_WriteCommand(0x11)
#define LCD_SetPartialMode()								LCD_WriteCommand(0x12)
#define LCD_SetNormalMode()									LCD_WriteCommand(0x13)

#define LCD_SetInvertMode()									LCD_WriteCommand(0x20)
#define LCD_ResetInvertMode()								LCD_WriteCommand(0x21)

#define LCD_SetGamma(gamma)									LCD_WriteU8(0x26, (u8)(gamma))

#define LCD_SetDisplayOff()									LCD_WriteCommand(0x28)
#define LCD_SetDisplayOn()									LCD_WriteCommand(0x29)
#define LCD_SetColumnAddress(start, end)					LCD_WriteU32(0x2a, (((u32)start)<<16) | ((u32)end) )
#define LCD_SetRowAddress(start, end)						LCD_WriteU32(0x2b, (((u32)start)<<16) | ((u32)end) )
#define LCD_StartWritePixel()								LCD_WriteCommand(0x2c)
#define LCD_EndWritePixel()									LCD_NOP()
#define LCD_StartWritePalette()								LCD_WriteCommand(0x2d)
#define LCD_EndWritePalette()								LCD_NOP()
#define LCD_StartReadPixel()								LCD_ReadU8(0x2e)
#define LCD_EndReadPixel()									LCD_NOP()
#define LCD_SetPartialArea(start, end) 						LCD_WriteU32(0x30, (((u32)start)<<16) | ((u32)end) )
#define LCD_WriteDisplayPixelFormat(format)					LCD_WriteU8(0x3a, format)

void LCD_Initialize(void)
{
	u32 w = 0;
	u32 i;
	LCD_Config_GPIO();
	LCD_Reset();

	LCD_WriteCommand(0xB9);  // SET password
	LCD_WriteData(0xFF);
	LCD_WriteData(0x83);
	LCD_WriteData(0x69);

	LCD_WriteCommand(0xB1);  //Set Power 
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



	LCD_WriteCommand(0xB2);  // SET Display  480x800
	LCD_WriteData(0x00);
	LCD_WriteData(0x20);  //23    
	LCD_WriteData(0x05); //03
	LCD_WriteData(0x05);  //03
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


	LCD_WriteCommand(0xB4);  // SET Display  480x800
	LCD_WriteData(0x00);
	LCD_WriteData(0x18);
	LCD_WriteData(0x80);
	LCD_WriteData(0x06);
	LCD_WriteData(0x02);



	LCD_WriteCommand(0xB6);  // SET VCOM
	LCD_WriteData(0x42);  // Update VCOM
	LCD_WriteData(0x42);



	LCD_WriteCommand(0xD5);
	LCD_WriteData(0x00);
	LCD_WriteData(0x04);
	LCD_WriteData(0x03);
	LCD_WriteData(0x00);
	LCD_WriteData(0x01);
	LCD_WriteData(0x04); //05 04
	LCD_WriteData(0x1A); //28 
	LCD_WriteData(0xff);  //70
	LCD_WriteData(0x01);
	LCD_WriteData(0x13);//03  
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
	LCD_WriteCommand(0xE0);
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
	LCD_WriteCommand(0xC1);
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


	//LCD_WriteCommand(0x36); 
	//LCD_WriteData(0x80);   //??36H????????GATE?SOURCE???

	LCD_WriteCommand(0x3A);
	LCD_WriteData(0x77);

	LCD_WriteCommand(0x11);
	delay_us(120);

	LCD_WriteCommand(0x29);

	LCD_WriteCommand(0x2C);
	delay_us(10);
	LCD_WriteCommand(0x3A); //pixel format setting
	LCD_WriteData(0x77); //设置为1600万色模式  

	LCD_WriteCommand(0x11);
	delay_us(120);
	
	LCD_WriteCommand(0x2D);
	for (i = 0; i <= 63; i++)
	{
		LCD_WriteData(i * 4);
	}

	for (i = 0; i <= 63; i++)
	{
		LCD_WriteData(i * 4);
	}

	for (i = 0; i <= 63; i++)
	{
		LCD_WriteData(i * 4);
	}

	LCD_WriteCommand(0x29);  //Display on 


	delay_us(120);

	while (1)
	{
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, BLACK); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, DARK_BLUE); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, DARK_GREEN); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, DARK_CYAN); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, DARK_RED); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, DARK_MANGENTA); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, DARK_YELLOW); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, GREY); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, LIGHT_BLUE); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, LIGHT_GREEN); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, LIGHT_CYAN); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, LIGHT_RED); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, LIGHT_MANGENTA); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, LIGHT_YELLOW); w += 5;
		delay_ms(100);
		LCD_DrawRect(w, w, 320 - w * 2, 240 - w * 2, WHITE); w += 5;
		delay_ms(20000);
		w = 0;
	}
}

void LCD_WR_REG(u8 address, u8 data)
{
	LCD_WriteCommand(address);

	LCD_WriteData(data);
}

#define TOGGLE_WR() WR.type->BRR = WR.pin;	WR.type->BSRR = WR.pin;
#define	SET_DATA(data) D0.type->ODR = data;
#define	GET_DATA() ((u16)D0.type->ODR)

void LCD_DrawRect(u32 x, u32 y, u32 w, u32 h, u32 color)
{
	u16 d;
	u16 r;
	u16 g;
	u16 b;
	u32 count;
	u32 xEnd = x + w - 1;
	u32 yEnd = y + h - 1;

	LCD_WriteCommand(0x2A);
	LCD_WriteData(x >> 8);
	LCD_WriteData(x & 0xff);
	LCD_WriteData(xEnd >> 8);
	LCD_WriteData(xEnd & 0xff);

	LCD_WriteCommand(0x2B);
	LCD_WriteData(y >> 8);
	LCD_WriteData(y & 0xff);
	LCD_WriteData(yEnd >> 8);
	LCD_WriteData(yEnd & 0xff);

	LCD_WriteCommand(0x2C);

	count = w * h;
	d = GET_DATA() & 0xff00;
	r = d | ((color    ) & 0xff);
	g = d | ((color>> 8) & 0xff);
	b = d | ((color>>16) & 0xff);

	IO_WRITE(A0, 1);
	IO_WRITE(LCD_CS, 0);

	while (count--)
	{
		SET_DATA(r)

		TOGGLE_WR()

		SET_DATA(g)

		TOGGLE_WR()
		
		SET_DATA(b)

		TOGGLE_WR()
	}

	IO_WRITE(LCD_CS, 1);
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

void LCD_DrawBMP(u32 x, u32 y, const u8* filename)
{
	u16 d;
	u16 lower;
	u16 upper;
	u32 count;
	u32 w;
	u32 h;
	BMP_HEAD bmp;
	u16 color = 0;
	u32 xEnd = x + w - 1;
	u32 yEnd = y + h - 1;

	LCD_WriteCommand(0x2A);
	LCD_WriteData(x >> 8);
	LCD_WriteData(x & 0xff);
	LCD_WriteData(xEnd >> 8);
	LCD_WriteData(xEnd & 0xff);

	LCD_WriteCommand(0x2B);
	LCD_WriteData(y >> 8);
	LCD_WriteData(y & 0xff);
	LCD_WriteData(yEnd >> 8);
	LCD_WriteData(yEnd & 0xff);

	LCD_WriteCommand(0x2C);

	count = w * h;
	d = GET_DATA() & 0xff00;
	upper = d | (color >> 8);
	lower = d | (color & 0xff);

	IO_WRITE(A0, 1);
	IO_WRITE(LCD_CS, 0);

	while (count--)
	{
		SET_DATA(upper);

		TOGGLE_WR()

			SET_DATA(lower);

		TOGGLE_WR()
	}

	IO_WRITE(LCD_CS, 1);
}

u16 ReadPixel(u16 x, u8 y)
{
	return x;
}

#if 0
char display_picture(char *filename)
{
	u16 ReadValue;
	FATFS fs;            // Work area (file system object) for logical drive
	FIL fsrc;      			// file objects
	u8 buffer[2048]; 		// file copy buffer
	FRESULT res;         // FatFs function common result code
	UINT br;         		// File R/W count
	u16 r_data, g_data, b_data;
	u32 tx, ty, temp;

	f_mount(0, &fs);

	res = f_open(&fsrc, filename, FA_OPEN_EXISTING | FA_READ);	 //打上图片文件名
	if (res == FR_NO_FILE || res == FR_INVALID_NAME)
	{
		f_mount(0, NULL);
		return 0;
	}

	if (res != FR_OK)
	{
		f_mount(0, NULL);
		SD_Init();//重新初始化SD卡 
		return 0;
	}

	res = f_read(&fsrc, &bmp, sizeof(bmp), &br);

	if (br != sizeof(bmp))
	{
		f_close(&fsrc);
		f_mount(0, NULL);
		return 0;
	}

	if ((bmp.pic_head[0] == 'B') && (bmp.pic_head[1] == 'M'))
	{
		res = f_lseek(&fsrc, ((bmp.pic_data_address_h << 16) | bmp.pic_data_address_l));
		if (res != FR_OK) {
			f_close(&fsrc);
			f_mount(0, NULL);
			return 0;
		}
		if (bmp.pic_dip == 16)
		{
			temp = bmp.pic_w_l * 2;
			if (bmp.pic_w_l > bmp.pic_h_l)
			{
				LCD_WriteCommand(0x36); //Set_address_mode
				LCD_WriteData(0x20); //横屏，从左下角开始，从左到右，从下到上
				if (bmp.pic_w_l < 480 || bmp.pic_h_l < 320)
				{
					Lcd_ColorBox(0, 0, 480, 320, 0x0000);
					BlockWrite((480 - bmp.pic_w_l) / 2, (480 - bmp.pic_w_l) / 2 + bmp.pic_w_l - 1, (320 - bmp.pic_h_l) / 2, (320 - bmp.pic_h_l) / 2 + bmp.pic_h_l - 1);
				}
				else 	BlockWrite(0, 479, 0, 319);
			}
			else
			{
				LCD_WriteCommand(0x36); //Set_address_mode
				LCD_WriteData(0x80); //竖屏，从左下角开始，从左到右，从下到上
				if (bmp.pic_w_l < 320 || bmp.pic_h_l < 480)
				{
					Lcd_ColorBox(0, 0, 320, 480, 0x0000);
					BlockWrite((320 - bmp.pic_w_l) / 2, (320 - bmp.pic_w_l) / 2 + bmp.pic_w_l - 1, (480 - bmp.pic_h_l) / 2, (480 - bmp.pic_h_l) / 2 + bmp.pic_h_l - 1);
				}
				else BlockWrite(0, 319, 0, 479);
			}

			for (tx = 0; tx < bmp.pic_h_l; tx++)
			{
				f_read(&fsrc, buffer, (bmp.pic_w_l) * 2, &br);
				for (ty = 0; ty < temp; ty += 2)
				{
					// 				r_data = *(ty +2+buffer);
					g_data = *(ty + 1 + buffer);
					b_data = *(ty + buffer);
					ReadValue = (g_data << 8) | b_data;
					g_data = ReadValue & 0x1f;  //保留低5位
					b_data = ReadValue & 0xffe0;    //保留高11位
					if (b_data & 0x20 == 0x20)
					{
						b_data <<= 1;
					}
					else
					{
						b_data <<= 1;
						b_data |= 0x20;
					}
					*(__IO u16 *) (Bank1_LCD_D) = g_data | b_data;
				}
			}
			f_close(&fsrc);
			f_mount(0, NULL);
			return 1;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		temp = bmp.pic_w_l * 3;
		if (bmp.pic_w_l > bmp.pic_h_l)
		{
			LCD_WriteCommand(0x36); //Set_address_mode
			LCD_WriteData(0x20); //横屏，从左下角开始，从左到右，从下到上
			if (bmp.pic_w_l < 480 || bmp.pic_h_l < 320)
			{
				Lcd_ColorBox(0, 0, 480, 320, 0x0000);
				BlockWrite((480 - bmp.pic_w_l) / 2, (480 - bmp.pic_w_l) / 2 + bmp.pic_w_l - 1, (320 - bmp.pic_h_l) / 2, (320 - bmp.pic_h_l) / 2 + bmp.pic_h_l - 1);
			}
			else 	BlockWrite(0, 479, 0, 319);
		}
		else
		{
			LCD_WriteCommand(0x36); //Set_address_mode
			LCD_WriteData(0x80); //竖屏，从左下角开始，从左到右，从下到上
			if (bmp.pic_w_l < 320 || bmp.pic_h_l < 480)
			{
				Lcd_ColorBox(0, 0, 320, 480, 0x0000);
				BlockWrite((320 - bmp.pic_w_l) / 2, (320 - bmp.pic_w_l) / 2 + bmp.pic_w_l - 1, (480 - bmp.pic_h_l) / 2, (480 - bmp.pic_h_l) / 2 + bmp.pic_h_l - 1);
			}
			else BlockWrite(0, 319, 0, 479);
		}

		for (tx = 0; tx < bmp.pic_h_l; tx++)
		{
			f_read(&fsrc, buffer, (bmp.pic_w_l) * 3, &br);
			for (ty = 0; ty < temp; ty += 3)
			{
				r_data = *(ty + 2 + buffer);
				g_data = *(ty + 1 + buffer);
				b_data = *(ty + buffer);
				ReadValue = (r_data & 0xF8) << 8 | (g_data & 0xFC) << 3 | b_data >> 3;
				*(__IO u16 *) (Bank1_LCD_D) = ReadValue;


				// 			*(__IO u16 *) (Bank1_LCD_D) = ((*(ty +2+buffer)<<8))|(*(ty +1+buffer)); 
				// 			*(__IO u16 *) (Bank1_LCD_D) = ((*(ty +0+buffer)<<8))|(*(ty +5+buffer));  
				// 			*(__IO u16 *) (Bank1_LCD_D) = ((*(ty +4+buffer)<<8))|(*(ty +3+buffer)); 
			}
		}
		f_close(&fsrc);
	}
	f_mount(0, NULL);
	// 	LCD_WriteCommand(0x36); //Set_address_mode
	//  	LCD_WriteData(0x08); //竖屏，从左上角开始，从左到右，从上到下
	return 1;
}
#endif
