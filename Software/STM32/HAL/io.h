#ifndef _8501_h_
#define _8501_h_
	
#include "delay.h"
#include "stm32f10x.h"

#ifdef __cplusplus 
extern "C"
{
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef struct{
    GPIO_TypeDef* type;
    uint16_t pin;
    uint32_t rcc;
}GPIO;

void pinMode(const GPIO* gpio, int mode);
#define IO_MODE(n, v) pinMode((&(n)), (v))
#define IO_WRITE(n, v) (((v)==0) ? GPIO_ResetBits(n.type, n.pin) : GPIO_SetBits(n.type, n.pin))
#define IO_READ(n) GPIO_ReadInputDataBit(n.type, n.pin)
#define IO_DELAY_MS(ms) delay_ms((ms))
#define IO_DELAY_US(us) delay_us((us))

// emulate 8051
extern const GPIO PORT_A0;
extern const GPIO PORT_A1;
extern const GPIO PORT_A2;
extern const GPIO PORT_A3;
extern const GPIO PORT_A4;
extern const GPIO PORT_A5;
extern const GPIO PORT_A6;
extern const GPIO PORT_A7;

extern const GPIO PORT_A8 ;
extern const GPIO PORT_A9 ;
extern const GPIO PORT_A10;
extern const GPIO PORT_A11;
extern const GPIO PORT_A12;
extern const GPIO PORT_A13;
extern const GPIO PORT_A14;
extern const GPIO PORT_A15;

extern const GPIO PORT_B0 ;
extern const GPIO PORT_B1 ;
extern const GPIO PORT_B2 ;
extern const GPIO PORT_B3 ;
extern const GPIO PORT_B4 ;
extern const GPIO PORT_B5 ;
extern const GPIO PORT_B6 ;
extern const GPIO PORT_B7 ;

extern const GPIO PORT_B8 ;
extern const GPIO PORT_B9 ;
extern const GPIO PORT_B10;
extern const GPIO PORT_B11;
extern const GPIO PORT_B12;
extern const GPIO PORT_B13;
extern const GPIO PORT_B14;
extern const GPIO PORT_B15;

#define TBD1				PORT_C13
#define TBD2				PORT_C14
#define TBD3				PORT_C15
#define D0					PORT_A0		// P1.0
#define D1					PORT_A1		// P1.1
#define D2					PORT_A2		// P1.2
#define D3					PORT_A3		// P1.3
#define D4					PORT_A4		// P1.4
#define D5					PORT_A5		// P1.5
#define D6					PORT_A6		// P1.6
#define D7					PORT_A7		// P1.7
#define YM_CS				PORT_B0		// P3.2
#define YM_WR				PORT_B1		// P3.3
#define YM_A0				PORT_B10	// P3.5
#define YM_A1				PORT_B11	// P3.6

#define SN_CLK_CS		PORT_B9  //  P0.4
#define SN_WR				PORT_B8  //  P0.2
#define I2C_DAT			PORT_B7  /// P2.0
#define I2C_CLK			PORT_B6  /// P2.1
#define SD_CS				PORT_B5  /// P0.6
#define YM_CLK_CS		PORT_B4  //  P0.3 // 
#define PWR_HOLD		PORT_B3  /// P0.7 //
#define STOP				PORT_A15 /// P2.2 //
#define NEXT_PAUSE	PORT_A12 /// P2.3
#define PREV_RESUME	PORT_A11 /// P2.4
#define RXD					PORT_A10 //  P3.0
#define TXD					PORT_A9  //  P3.1
#define PLAY				PORT_A8  /// P2.5
#define SPI_MOSI		PORT_B15 //  P0.1
#define SPI_MISO		PORT_B14 /// P0.5
#define SPI_CLK			PORT_B13 //  P0.0
#define YM_IC				PORT_B12 //  P3.7

#ifdef __cplusplus 
};
#endif
	
#endif
