#include "VGMBoard.h"
#include <delay.h>
#include <io.h>
#include <spi.h>
#include <malloc.h>
#include <mmc_sd.h>
#include <ff.h>

FATFS fs;

void VGMBoard_SetData(u8 data)
{
	IO_WRITE(D0, data & 0x01);
	IO_WRITE(D1, data & 0x02);
	IO_WRITE(D2, data & 0x04);
	IO_WRITE(D3, data & 0x08);
	IO_WRITE(D4, data & 0x10);
	IO_WRITE(D5, data & 0x20);
	IO_WRITE(D6, data & 0x40);
	IO_WRITE(D7, data & 0x80);
}

void VGMBoard_InitYMClock()
{
	IO_WRITE(YM_CLK_CS, 0);
	IO_DELAY_US(10);

	SPI2_ReadWriteByte(0xCE);
	SPI2_ReadWriteByte(0x40);

	IO_WRITE(YM_CLK_CS, 1);
	IO_DELAY_US(10);
}

void VGMBoard_InitSNClock()
{
	IO_WRITE(SN_CLK_CS, 0);
	IO_DELAY_US(10);

	SPI2_ReadWriteByte(0xBC);
	SPI2_ReadWriteByte(0xFC);

	IO_WRITE(SN_CLK_CS, 1);
	IO_DELAY_US(10);
}

#define DATA_SETUP_TIME 5
#define SAMPLE_PERIOD (22.67573f * 0.98)

void VGMBoard_WriteYM2151(u8 addr, u8 data)
{
	/////////////////////////
	VGMBoard_SetData(addr);
	IO_WRITE(YM_A0, 0);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_CS, 0);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_WR, 0);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_WR, 1);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_CS, 1);
	IO_DELAY_US(DATA_SETUP_TIME);

	/////////////////////////
	VGMBoard_SetData(data);
	IO_WRITE(YM_A0, 1);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_CS, 0);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_WR, 0);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_WR, 1);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_CS, 1);
	IO_DELAY_US(DATA_SETUP_TIME);
}


void VGMBoard_WriteYM2612(u8 addr, u8 data)
{
	/////////////////////////
	VGMBoard_SetData(data);
	IO_WRITE(YM_A0, addr & 0x01);
	IO_WRITE(YM_A1, addr & 0x02);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_CS, 0);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_WR, 0);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_WR, 1);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(YM_CS, 1);
	IO_DELAY_US(DATA_SETUP_TIME);
}

void VGMBoard_WriteSN76489(u8 data)
{
	/////////////////////////
	VGMBoard_SetData(data);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(SN_WR, 0);
	IO_DELAY_US(DATA_SETUP_TIME);

	IO_WRITE(SN_WR, 1);
	IO_DELAY_US(DATA_SETUP_TIME);
}

u32 VGMBoard_UpdateSamples(u32 nnnn)
{
	u32 time = nnnn * SAMPLE_PERIOD;
	IO_DELAY_US(time);
	
	return nnnn;
}

s32 VGMBoard_Initialize(u32 clock, u32 sampleRate)
{
	//SystemInit();
	delay_init(72);    //????? 

	//delay_ms(1000);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_15;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	IO_MODE(PWR_HOLD		, 0);//
	IO_WRITE(PWR_HOLD		, 0);
	
	IO_MODE(D0		, 0);
	IO_MODE(D1		, 0);
	IO_MODE(D2		, 0);
	IO_MODE(D3		, 0);
	IO_MODE(D4		, 0);
	IO_MODE(D5		, 0);
	IO_MODE(D6		, 0);
	IO_MODE(D7		, 0);
	IO_MODE(YM_CS	, 0);
	IO_MODE(YM_WR	, 0);
	IO_MODE(YM_A0	, 0);
	IO_MODE(YM_A1	, 0);

	IO_MODE(SN_CLK_CS		, 0);
	IO_MODE(SN_WR				, 0);
	IO_MODE(I2C_DAT			, 0);
	IO_MODE(I2C_CLK			, 0);
	IO_MODE(SD_CS				, 0);
	IO_MODE(YM_CLK_CS		, 0);// 

	IO_MODE(STOP				, 1);//
	IO_MODE(NEXT_PAUSE	, 1);
	IO_MODE(PREV_RESUME	, 1);
	IO_MODE(RXD					, 0);
	IO_MODE(TXD					, 0);
	IO_MODE(PLAY				, 1);
	//IO_MODE(SPI_MOSI		, 0);
	//IO_MODE(SPI_MISO		, 0);
	//IO_MODE(SPI_CLK			, 0);
	IO_MODE(YM_IC				, 0);
	
	// IO initial state
	VGMBoard_SetData(0x00);

	// YM2612
	IO_WRITE(YM_CLK_CS, 1);
	IO_WRITE(YM_CS, 1);
	IO_WRITE(YM_WR, 1);
	IO_WRITE(YM_A0, 1);
	IO_WRITE(YM_A1, 1);
	IO_WRITE(YM_IC, 1);

	// SN76489
	IO_WRITE(SN_CLK_CS, 1);
	IO_WRITE(SN_WR, 1);
	
	IO_WRITE(I2C_DAT		, 1);
	IO_WRITE(I2C_CLK		, 1);
	IO_WRITE(SD_CS			, 1);
	IO_WRITE(STOP				, 1);
	IO_WRITE(NEXT_PAUSE	, 1);
	IO_WRITE(PREV_RESUME, 1);
	IO_WRITE(RXD				, 1);
	IO_WRITE(TXD				, 1);
	IO_WRITE(PLAY				, 1);

	IO_DELAY_US(100);
	IO_WRITE(YM_IC, 0);
	IO_DELAY_US(100);
	IO_WRITE(YM_IC, 1);
	IO_DELAY_US(100);
	
	//////////////////////////////////////////////////
	SPI2_Init();
	SPI2_SetSpeed(SPI_SPEED_256);
	
	VGMBoard_InitYMClock(); // generate 4.79Mhz
	
	VGMBoard_InitSNClock(); // generate 3.57Mhz	
	
	VGMBoard_WriteSN76489(0x9f);
	VGMBoard_WriteSN76489(0xbf);
	VGMBoard_WriteSN76489(0xdf);
	VGMBoard_WriteSN76489(0xff);

	SPI2_SetSpeed(SPI_SPEED_2);
	
 	mem_init();	// initialize malloc
 	while(SD_Initialize()) // initialize SDCard
	{
			// USART1_printf("SD Card Error!");
			// USART1_printf("Please Check!");
			delay_ms(500);
	}
	
	//exfuns_init();	
  //f_mount(fs[0], "0:", 0);
	//while(exf_getfree("0",&total,&free));
	
	f_mount(&fs, (const TCHAR*)"0:", 0);
	
	return -1;
}

void VGMBoard_Shutdown()
{
}

void VGMBoard_Reset()
{
}

void VGMBoard_Update()
{
	u8 a = IO_READ(NEXT_PAUSE);
	if(a)
	{
			//IO_WRITE(PWR_HOLD		, 1);
	}
	else
	{
			IO_MODE(PWR_HOLD		, 1);
	}
}
