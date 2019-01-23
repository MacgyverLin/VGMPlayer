#include "stm32f10x.h"
#include "LCD.h"
#include "io.h"
// #include "MM1_240.h"
// #include "MM3_240.h"

void Lcd_Light_ON(void)
{
}

void LCD_GPIO_Config(void)
{
#if 0
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* config lcd gpio clock base on FSMC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    
    /* config tft rst gpio */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    //LED 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    /* config tft back_light gpio base on the PT4101 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 
    GPIO_Init(GPIOA, &GPIO_InitStructure);  		   
    
    /* config tft data lines base on FSMC
	 * data lines,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	 */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* config tft control lines base on FSMC
	 * PD4-FSMC_NOE  :LCD-RD
   * PD5-FSMC_NWE  :LCD-WR
	 * PD7-FSMC_NE1  :LCD-CS
   * PD11-FSMC_A16 :LCD-DC
	 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    /* tft control gpio init */	 

		GPIO_SetBits(GPIOD, GPIO_Pin_13);		 // RST = 1 
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1 
#endif
}

static void Delay(__IO u32 nCount)
{	
	volatile int i;
	for(i=0; i<0XFFFF; i++)
		for(; nCount != 0; nCount--);
}  
  
void LCD_Rst(void)
{			
/*
    Clr_Rst;
    Delay(1000);					   

		Set_Rst;		 	 
    Delay(1000);	
*/
}

void SetData(u8 data)
{			
	IO_WRITE(D0, data & 0x01);
	IO_WRITE(D1, data & 0x02);
	IO_WRITE(D2, data & 0x04);
	IO_WRITE(D3, data & 0x08);
	
	IO_WRITE(D4, data & 0x10);
	IO_WRITE(D5, data & 0x20);
	IO_WRITE(D6, data & 0x40);
	IO_WRITE(D7, data & 0x80);	
	// *(__IO u16 *) (Bank1_LCD_C) = CMD;
} 

void WriteComm(u8 command)
{
	SetData(command);
	
	IO_WRITE(A0, 0);
	
	IO_WRITE(RD, 1);
	
	IO_WRITE(LCD_CS, 0);
	
	IO_WRITE(WR, 0);

	IO_WRITE(WR, 1);
	
	IO_WRITE(LCD_CS, 1);
	// *(__IO u16 *) (Bank1_LCD_C) = CMD;
}

void WriteData(u8 data)
{			
	SetData(data);
	
	IO_WRITE(A0, 1);
	
	IO_WRITE(RD, 1);
	
	IO_WRITE(LCD_CS, 0);
	
	IO_WRITE(WR, 0);

	IO_WRITE(WR, 1);
	
	IO_WRITE(LCD_CS, 1);	
	// *(__IO u16 *) (Bank1_LCD_D) = tem_data;	
}

void Lcd_Initialize(void)
{	
	int i;
	LCD_GPIO_Config();
	LCD_Rst();

	WriteComm(0xB9);   // Set EXTC 
	WriteData(0xFF); 
	WriteData(0x83); 
	WriteData(0x68); 
	Delay(20);
	 
	WriteComm(0xBB);   // Set OTP 
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0x80);   //OTP load disable 
	Delay(20); 
	 
	WriteComm(0xC2);  // For Himax internel use
	WriteData(0x00); 
	WriteData(0x30); 
	Delay(20);
	   
	WriteComm(0xC0);  // For Himax internel use
	WriteData(0x1B); 
	WriteData(0x05); 
	WriteData(0x08); 
	WriteData(0xEC); 
	WriteData(0x00); 
	WriteData(0x01); 
	Delay(20);
	  
	WriteComm(0xE3);   // For Himax internel use
	WriteData(0x00); 
	WriteData(0x4F); 
	WriteData(0x00); 
	WriteData(0x4F); 
	Delay(20);
	  
	WriteComm(0xBD);  // For Himax internel use
	WriteData(0x00); 
	WriteData(0x20); 
	WriteData(0x52); 
	Delay(20);
	   
	WriteComm(0xBF);  // For Himax internel use
	WriteData(0x01); 
	WriteData(0x04); 
	WriteData(0x2C); 
	Delay(20);
	   
	WriteComm(0xB1);   // Set Power 
	WriteData(0x00); 
	WriteData(0x02); 
	WriteData(0x1E); 
	WriteData(0x00); 
	WriteData(0x22); 
	WriteData(0x11); 	
	WriteData(0x8D); 
	Delay(20);
	  
	WriteComm(0xB6);   // Set VCOM 
	WriteData(0xc0);   // VMF 95
	WriteData(0x55);   // VMH 64
	WriteData(0x42);   // VML 42
	Delay(20);
	 
	WriteComm(0xE0);   // Set Gamma 
	WriteData(0x13); 
	WriteData(0x24); 
	WriteData(0x23); 
	WriteData(0x21); 
	WriteData(0x21); 
	WriteData(0x3F); 
	WriteData(0x16); 
	WriteData(0x5D); 
	WriteData(0x07); 
	WriteData(0x06); 
	WriteData(0x09); 
	WriteData(0x10); 
	WriteData(0x17); 
	WriteData(0x00); 
	WriteData(0x1E); 
	WriteData(0x1E); 
	WriteData(0x1C); 
	WriteData(0x1B); 
	WriteData(0x2C); 
	WriteData(0x22); 
	WriteData(0x69); 
	WriteData(0x07); 
	WriteData(0x0F); 
	WriteData(0x16); 
	WriteData(0x19); 
	WriteData(0x18); 
	WriteData(0xFF); 
	Delay(20); 
	 
	WriteComm(0xB0);   // Set OSC 
	WriteData(0x0D); 
	WriteData(0x01); 
	Delay(20); 
	 
	WriteComm(0x36);   // Set BGR 
	WriteData(0x08);

	WriteComm(0x3a);  
	WriteData(0x55);

	WriteComm(0x2D);   // Look up table for 65K color 
	for(i=0;i<32;i++)  
	{ 
		if(i<16) 
			WriteData(2*i);  // RED 
		else 
			WriteData(2*i+1); 
	} 
	for(i=0;i<64;i++) 
		WriteData(1*i);   // Green  
	for(i=0;i<32;i++) 
	{ 
		if(i<16) 
			WriteData(2*i);  // Blue  
		else 
			WriteData(2*i+1); 
	} 

	WriteComm(0x21);		// inv On

	WriteComm(0x11);    //Sleep Out 
	Delay(20);
	  
	WriteComm(0x29);    //Display On 
	Delay(20);
	 
	WriteComm(0x2C);  //Write data to SRAM 

	Delay(10);
	
	Lcd_Light_ON();

	WriteComm(0x16);   // Set BGR 
	WriteData(0x50);

	while(1)
	{
		Lcd_ColorBox(0,0,320,240,Yellow);Delay(1000000);
		//Lcd_ColorBox(0,0,320,240,Blue);Delay(1000000);
		//Lcd_ColorBox(0,0,320,240,Red);Delay(1000000);
		//Lcd_ColorBox(0,0,320,240,Green);Delay(1000000);
		//Lcd_ColorBox(0,0,320,240,Magenta);Delay(1000000);
		//Lcd_ColorBox(0,0,320,240,Cyan);Delay(1000000);
		//Lcd_ColorBox(0,0,320,240,White);Delay(1000000);
		//LCD_Fill_Pic(0,0,320,240, gImage_MM1_240);Delay(20000000);
		//LCD_Fill_Pic(0,0,320,240, gImage_MM3_240);Delay(20000000);
	}
}

/*
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	*(__IO u16 *) (Bank1_LCD_C) = Index;	
	*(__IO u16 *) (Bank1_LCD_D) = CongfigTemp;
}

void Lcd_WR_Start(void)
{
	*(__IO u16 *) (Bank1_LCD_C) = 0x2C;
}
*/

void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
	WriteComm(0x2A);
	WriteData(Xstart>>8); 
	WriteData(Xstart&0xff);     //Column Start 
	WriteData(Xend>>8); 
	WriteData(Xend&0xff);     //Column End 

	WriteComm(0x2B);
	WriteData(Ystart>>8); 
	WriteData(Ystart&0xff);     //Column Start 
	WriteData(Yend>>8); 
	WriteData(Yend&0xff);     //Column End 

	/*
	LCD_WR_REG(0x02,Xstart>>8); 
	LCD_WR_REG(0x03,Xstart&0xff);     //Column Start 
	LCD_WR_REG(0x04,Xend>>8); 
	LCD_WR_REG(0x05,Xend&0xff);     //Column End 
	 
	LCD_WR_REG(0x06,Ystart>>8); 
	LCD_WR_REG(0x07,Ystart&0xff);     //Row Start 
	LCD_WR_REG(0x08,Yend>>8); 
	LCD_WR_REG(0x09,Yend&0xff);     //Row End 
	*/
	  
	WriteComm(0x2C);
}

/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart和 yStart随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的终止点
          xLong 要选定矩形的x方向长度
          yLong  要选定矩形的y方向长度
返回值：无
***********************************************/
void Lcd_ColorBox(u16 xStart,u16 yStart,u16 xLong,u16 yLong,u16 Color)
{
	u32 temp;

	BlockWrite(xStart,xStart+xLong-1,yStart,yStart+yLong-1);
	for (temp=0; temp<xLong*yLong; temp++)
	{
		//*(__IO u16 *) (Bank1_LCD_D) = Color;
		WriteData(Color);
	}
}

#if 0
/******************************************
函数名：Lcd图像填充100*100
功能：向Lcd指定位置填充图像
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
******************************************/
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
{
  unsigned long i;
	unsigned int j;

// 	WriteComm(0x36); //Set_address_mode
// 	WriteData(0x0a); //横屏，从左下角开始，从左到右，从下到上
	BlockWrite(x,x+pic_H-1,y,y+pic_V-1);
	for (i = 0; i < pic_H*pic_V*2; i+=2)
	{
		j=pic[i];
		j=j<<8;
		j=j+pic[i+1];
		*(__IO u16 *) (Bank1_LCD_D) = j;
	}
// 	WriteComm(0x36); //Set_address_mode
// 	WriteData(0xaa);
}

void DrawPixel(u16 x, u16 y, u16 Color)
{
	BlockWrite(x,x+1,y,y+1);
	*(__IO u16 *) (Bank1_LCD_D) = Color;
}

u16 ReadPixel(u16 x,u8 y)
{
	WriteComm(0x2a);   
	WriteData(x>>8);
	WriteData(x&0xff);
	WriteData(x>>8);
	WriteData(x&0xff);
	WriteComm(0x2b);   
	WriteData(y>>8);
	WriteData(y&0xff);
	WriteData(y>>8);
	WriteData(y&0xff);
	WriteComm(0x2e);
	x = *(__IO u16 *) (Bank1_LCD_D);
	x = *(__IO u16 *) (Bank1_LCD_D);
	y = *(__IO u16 *) (Bank1_LCD_D);
	y = *(__IO u16 *) (Bank1_LCD_D);
	x = ((x&0xf800))|((x&0x00FC)<<3)|((y&0xf8)>>3);
	return x;
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

BMP_HEAD bmp;

 


//任意屏大小范围内显示图片
/*
x，y像素起点坐标
*/
char display_picture(char *filename)
{
	u16 ReadValue;
	FATFS fs;            // Work area (file system object) for logical drive
	FIL fsrc;      			// file objects
	u8 buffer[2048]; 		// file copy buffer
	FRESULT res;         // FatFs function common result code
	UINT br;         		// File R/W count
	u16 r_data,g_data,b_data;	
	u32	 tx,ty,temp;
	
	
  f_mount(0, &fs);

  res = f_open(&fsrc, filename, FA_OPEN_EXISTING | FA_READ);	 //打上图片文件名
  if(res==FR_NO_FILE||res==FR_INVALID_NAME){
     f_mount(0, NULL);
	 return 0;
  }

  if(res!=FR_OK){
     f_mount(0, NULL);
	 SD_Init();//重新初始化SD卡 
	 return 0;
  }

  res = f_read(&fsrc, &bmp, sizeof(bmp), &br);

  if(br!=sizeof(bmp))
	{
		f_close(&fsrc);
		f_mount(0, NULL);
	 	return 0;
  }

  if((bmp.pic_head[0]=='B')&&(bmp.pic_head[1]=='M'))
  {
	res = f_lseek(&fsrc, ((bmp.pic_data_address_h<<16)|bmp.pic_data_address_l));
	if(res!=FR_OK){
     f_close(&fsrc);
     f_mount(0, NULL);
	 return 0;
    }
	if(bmp.pic_dip==16)
	{
		temp=bmp.pic_w_l*2;	
		if(bmp.pic_w_l>bmp.pic_h_l)
		{
		WriteComm(0x36); //Set_address_mode
		WriteData(0x20); //横屏，从左下角开始，从左到右，从下到上
		if(bmp.pic_w_l<480||bmp.pic_h_l<320)
			{
				Lcd_ColorBox(0,0,480,320,0x0000);
				BlockWrite((480-bmp.pic_w_l)/2,(480-bmp.pic_w_l)/2+bmp.pic_w_l-1,(320-bmp.pic_h_l)/2,(320-bmp.pic_h_l)/2+bmp.pic_h_l-1);
			}
			else 	BlockWrite(0,479,0,319);
		}
		else
		{
			WriteComm(0x36); //Set_address_mode
			WriteData(0x80); //竖屏，从左下角开始，从左到右，从下到上
			if(bmp.pic_w_l<320||bmp.pic_h_l<480)
				{
					Lcd_ColorBox(0,0,320,480,0x0000);
					BlockWrite((320-bmp.pic_w_l)/2,(320-bmp.pic_w_l)/2+bmp.pic_w_l-1,(480-bmp.pic_h_l)/2,(480-bmp.pic_h_l)/2+bmp.pic_h_l-1);
				}
				else BlockWrite(0,319,0,479);	
		}
		
		for (tx = 0; tx < bmp.pic_h_l; tx++)
		{
		f_read(&fsrc, buffer, (bmp.pic_w_l)*2, &br);
		for(ty=0;ty<temp;ty+=2)
			{
// 				r_data = *(ty +2+buffer);
				g_data = *(ty +1+buffer);
				b_data = *(ty +  buffer);			
				ReadValue= (g_data<<8) | b_data;
				g_data = ReadValue & 0x1f;  //保留低5位
				b_data = ReadValue & 0xffe0;    //保留高11位
				if(b_data & 0x20 == 0x20)
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
	temp=bmp.pic_w_l*3;	
	if(bmp.pic_w_l>bmp.pic_h_l)
	{
 	WriteComm(0x36); //Set_address_mode
 	WriteData(0x20); //横屏，从左下角开始，从左到右，从下到上
	if(bmp.pic_w_l<480||bmp.pic_h_l<320)
		{
 			Lcd_ColorBox(0,0,480,320,0x0000);
 			BlockWrite((480-bmp.pic_w_l)/2,(480-bmp.pic_w_l)/2+bmp.pic_w_l-1,(320-bmp.pic_h_l)/2,(320-bmp.pic_h_l)/2+bmp.pic_h_l-1);
		}
		else 	BlockWrite(0,479,0,319);
	}
	else
	{
		WriteComm(0x36); //Set_address_mode
		WriteData(0x80); //竖屏，从左下角开始，从左到右，从下到上
		if(bmp.pic_w_l<320||bmp.pic_h_l<480)
			{
				Lcd_ColorBox(0,0,320,480,0x0000);
				BlockWrite((320-bmp.pic_w_l)/2,(320-bmp.pic_w_l)/2+bmp.pic_w_l-1,(480-bmp.pic_h_l)/2,(480-bmp.pic_h_l)/2+bmp.pic_h_l-1);
			}
			else BlockWrite(0,319,0,479);	
	}
	
	for (tx = 0; tx < bmp.pic_h_l; tx++)
	{
	f_read(&fsrc, buffer, (bmp.pic_w_l)*3, &br);
	for(ty=0;ty<temp;ty+=3)
		{
			r_data = *(ty +2+buffer);
			g_data = *(ty +1+buffer);
			b_data = *(ty +  buffer);			
			ReadValue=(r_data & 0xF8) << 8 | (g_data & 0xFC) << 3 | b_data >> 3;
			*(__IO u16 *) (Bank1_LCD_D) = ReadValue; 	
			
			
// 			*(__IO u16 *) (Bank1_LCD_D) = ((*(ty +2+buffer)<<8))|(*(ty +1+buffer)); 
// 			*(__IO u16 *) (Bank1_LCD_D) = ((*(ty +0+buffer)<<8))|(*(ty +5+buffer));  
// 			*(__IO u16 *) (Bank1_LCD_D) = ((*(ty +4+buffer)<<8))|(*(ty +3+buffer)); 
		}
	}	
		f_close(&fsrc);
   }
  f_mount(0, NULL);
// 	WriteComm(0x36); //Set_address_mode
//  	WriteData(0x08); //竖屏，从左上角开始，从左到右，从上到下
  return 1;
}

#endif
