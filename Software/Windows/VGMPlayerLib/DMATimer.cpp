#include "stm32f10x.h"
#include "stm32_m.h"
#include "dzy.h"   
#include "myRCC.h"   
//******************************************************************************
#define TIM1_CCR3_Address    0x40012C3C

bit f_tb;			// 基本定时标志
bit f_100ms;
bit f_1000ms;
INT16U cnt_test;	// 计数器，仅供软件调试使用
float clk_sys;		// 仅供软件调试使用

#if 1
uint16_t SRC_Buffer[] =
{ 72 * 5 };   // 由于载波频率取20kHZ，所以最大脉冲宽度不要超50us, 即常数不要超过72*50

#else

uint16_t SRC_Buffer[] =
{ 72 * 2,72 * 5,72 * 10,72 * 20,72 * 40,72 * 10 }; // 由于载波频率取20kHZ，所以最大脉冲宽度不要超50us,即常数不要超过72*50
#endif

//******************************************************************************
// 延时程序，单位为*1ms
//******************************************************************************
void delayms(INT16U cnt)
{
	//#define   CONST_1MS  7333   // 72MhZ
	//#define   CONST_1MS 3588   // 32MhZ
	#define   CONST_1MS (105*FCLK) 

	INT16U i;

	__no_operation();
	while (cnt--)
		for (i = 0; i < CONST_1MS; i++)
			;
}

//******************************************************************************
// pcb上的指示灯
//******************************************************************************
static void led_toggle(void)
{
	GPIOC->ODR ^= GPIO_Pin_7;  // led2 toogle
	GPIOC->ODR ^= GPIO_Pin_6;  // led3 toogle
}

//******************************************************************************
// 时钟设置初始化
//******************************************************************************
static void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;
	/*
	 RCC_AdjustHSICalibrationValue 调整内部高速晶振（HSI）校准值
	 RCC_ITConfig 使能或者失能指定的RCC中断
	 RCC_ClearFlag 清除RCC的复位标志位
	 RCC_GetITStatus 检查指定的RCC中断发生与否
	 RCC_ClearITPendingBit 清除RCC的中断待处理位
	 */
	 /* RCC system reset(for debug purpose) */
	 // 时钟系统复位
	RCC_DeInit();

	// 使能外部的8M晶振
	// 设置外部高速晶振（HSE）
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	// 使能或者失能内部高速晶振（HSI）
	RCC_HSICmd(DISABLE);

	// 等待HSE起振
	// 该函数将等待直到HSE就绪，或者在超时的情况下退出
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		// 设置AHB时钟（HCLK）
		RCC_HCLKConfig(RCC_HCLK_Div_); // 36 MHz

		// 设置低速AHB时钟（PCLK1）
		RCC_PCLK1Config(RCC_PCLK1_Div_); // 2.25 MHz

		// 设置高速AHB时钟（PCLK2）
		RCC_PCLK2Config(RCC_PCLK2_Div_); // 2.25 MHz

		/* ADCCLK = PCLK2/8 */
		// 设置ADC时钟（ADCCLK）
		RCC_ADCCLKConfig(RCC_ADC_DIV_); // 0.281Mhz

		// 设置USB时钟（USBCLK）
		// USB时钟 = PLL时钟除以1.5
		//RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

		// 设置外部低速晶振（LSE）
		RCC_LSEConfig(RCC_LSE_OFF);

		// 使能或者失能内部低速晶振（LSI）
		// LSE晶振OFF
		RCC_LSICmd(DISABLE);

		// 设置RTC时钟（RTCCLK）
		// 选择HSE时钟频率除以128作为RTC时钟
		//RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);

		// 使能或者失能RTC时钟
		// RTC时钟的新状态
		RCC_RTCCLKCmd(DISABLE);

		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);

		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		/* PLLCLK = 8MHz * 9 = 72 MHz */
		// 设置PLL时钟源及倍频系数
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_);

		/* Enable PLL */
		// 使能或者失能PLL
		RCC_PLLCmd(ENABLE);

		/* Wait till PLL is ready */
		// 检查指定的RCC标志位设置与否
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		/* Select PLL as system clock source */
		// 设置系统时钟（SYSCLK）
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Wait till PLL is used as system clock source */
		// 返回用作系统时钟的时钟源
		while (RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}

	// 使能或者失能AHB外设时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2 | RCC_AHBPeriph_SRAM 
		| RCC_AHBPeriph_FLITF | RCC_AHBPeriph_CRC | RCC_AHBPeriph_FSMC
		| RCC_AHBPeriph_SDIO, DISABLE);
	// 使能或者失能APB1外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL, DISABLE);

	// 强制或者释放高速APB（APB2）外设复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ALL, ENABLE);
	// 退出复位状态
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ALL, DISABLE);

	// 强制或者释放低速APB（APB1）外设复位
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_ALL, ENABLE);

	// 强制或者释放后备域复位
	RCC_BackupResetCmd(ENABLE);

	// 使能或者失能时钟安全系统
	RCC_ClockSecuritySystemCmd(DISABLE);
}

//******************************************************************************
// NVIC设置
//******************************************************************************
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//******************************************************************************
// SysTick设置初始化
//******************************************************************************
static void SysTick_Config1(void)
{
#if 1
#define SystemFreq  (FCLK*1000000.0)    // 单位为Hz
#define TB_SysTick  (TIME_TB*1000)  // 单位为uS,与示波器实测一致

	static INT32U ticks;

	ticks = (INT32U)((TB_SysTick / 1000000.0) * SystemFreq);
	SysTick_Config(ticks);
#endif 
}
//******************************************************************************
// GPIO设置
//******************************************************************************
static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(
		RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
		| RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

	//------------------------------------------------------------------------------
	GPIO_Write(GPIOA, 0xffff);

	/* GPIOA Configuration: Channel 3 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	// GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//------------------------------------------------------------------------------

	GPIO_Write(GPIOB, 0xffff); // 11111101-11111111     

	/* GPIOB Configuration: Channel 3N as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//------------------------------------------------------------------------------

	GPIO_Write(GPIOC, 0xff0f); // 11111111-00001111

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_4
		| GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//------------------------------------------------------------------------------
	GPIO_Write(GPIOD, 0xffff); // 11111111-11111111  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
//******************************************************************************
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // dma1时钟使能

	DMA_DeInit(DMA1_Channel5);   // DMA复位
	DMA_StructInit(&DMA_InitStructure);   // DMA缺省的参数

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM1_CCR3_Address; //外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SRC_Buffer;  //内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //dma传输方向,单向
	DMA_InitStructure.DMA_BufferSize = sizeof(SRC_Buffer) / 2; //设置DMA在传输时缓冲区的长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //设置DMA的外设递增模式，一个外设
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //设置DMA的内存递增模式，
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设数据字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;   //内存数据字长
	
	//循环模式开启，Buffer写满后，自动回到初始地址开始传输
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //设置DMA的传输模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //设置DMA的优先级别
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //设置DMA的2个memory中的变量互相访问
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);

	DMA_ClearFlag(DMA1_IT_TC5);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

	DMA_Cmd(DMA1_Channel5, ENABLE);
}

/* TIM1 DMA Transfer example -------------------------------------------------
 TIM1CLK = 72 MHz, Prescaler = 0, TIM1 counter clock = 72 MHz
 The TIM1 Channel3 is configured to generate a complementary PWM signal with
 a frequency equal to: TIM1 counter clock / (TIM1_Period + 1) = 17.57 KHz and
 a variable duty cycle that is changed by the DMA after a specific number of
 Update DMA request.
 The number of this repetitive requests is defined by the TIM1 Repetion counter,
 each 3 Update Requests, the TIM1 Channel 3 Duty Cycle changes to the next new
 value defined by the SRC_Buffer .
 -----------------------------------------------------------------------------*/
 //******************************************************************************
void Tim1_Configuration(void)
{
	INT16U TIM_Prescaler, TIM_Period;
	INT32U utemp;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_DeInit(TIM1);

	utemp = (INT32U)(TIM1CLK * 1000000.0) / Freq_PWM;

	TIM_Prescaler = utemp / 65536;
	++TIM_Prescaler;                       // 注意这句话，一定要++

	utemp = (INT32U)(TIM1CLK * 1000000.0) / TIM_Prescaler;  // 分频后的定时器输入频率
	TIM_Period = utemp / Freq_PWM;               // 周期常数

	TIM_TimeBaseStructure.TIM_Period = TIM_Period - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM_Prescaler - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 1 - 1;               // 每次直接更新
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	//------------------------------------------------------------------------------
	  /* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出比较状态
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; // 失能输出比较N状态
	TIM_OCInitStructure.TIM_Pulse = 72;
	/*
	 TIM_OCMode_PWM2  TIM_OCPolarity_Low    正脉冲模式
	 TIM_OCMode_PWM2  TIM_OCPolarity_High   负脉冲模式

	 TIM_OCMode_PWM1  TIM_OCPolarity_Low    负脉冲模式
	 TIM_OCMode_PWM1  TIM_OCPolarity_High   正脉冲模式

	 TIM1_OCPolarity输出极性---TIM_OCPolarity_High，输出比较极性高,输出的是正脉冲
	 TIM1_OCPolarity输出极性---TIM_OCPolarity_Low， 输出比较极性低,输出的是负脉冲
	 */
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low; // 互补输出极性
	// 选择空闲状态下的非工作状态
	// 当MOE=0设置TIM1输出比较空闲状态       
	// 默认输出位低电平                
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	// 选择空闲状态下的非工作状态
	// 当MOE=0重置TIM1输出比较N空闲状态
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);   // 使能TIMx在CCR1上的预装载寄存器

	/* TIM1 Update DMA Request enable */
	TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);

	//TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE); // 定时中断使能
	//TIM_ITConfig(TIM1,TIM_IT_CC3,ENABLE); // 定时中断使能
	TIM_Cmd(TIM1, ENABLE);      // 定时器开始运行

	// 这条语句必须要有!!!
	TIM_CtrlPWMOutputs(TIM1, ENABLE); /* Main Output Enable */
}
//******************************************************************************
// 基本定时程序,周期为 TIME_TB
//******************************************************************************
static func_tb(void)
{
	static INT16U tmr_100ms;
	static INT16U tmr_1000ms;

	if (!f_tb)
		return;
	f_tb = 0;

	if (++tmr_100ms >= (100 / TIME_TB))
	{
		tmr_100ms = 0;
		f_100ms = 1;

		GPIOC->ODR ^= GPIO_Pin_4;  // led5 toogle          
	}
	if (++tmr_1000ms >= (1000 / TIME_TB))
	{
		tmr_1000ms = 0;
		f_1000ms = 1;
		GPIOC->ODR ^= GPIO_Pin_5;  // led4 toogle      
	}
}

//******************************************************************************
// 进入睡眠模式
// 自行编写
//******************************************************************************
void myPWR_EnterSleepMode(void)
{
	//PWR->CR |= CR_CWUF_Set; /* Clear Wake-up flag */

	/* Set SLEEPDEEP bit of Cortex System Control Register */
	//*(__IO uint32_t *) SCB_SysCtrl |= SysCtrl_SLEEPDEEP_Set;
	/* This option is used to ensure that store operations are completed */

	__WFI(); /* Request Wait For Interrupt */
}
//******************************************************************************
// 主程序
//******************************************************************************
void main(void)
{
	int i;

	RCC_Configuration();
	GPIO_Configuration();
	delayms(100);   // 延时，等待电压稳定
	Tim1_Configuration();
	DMA_Configuration();
	SysTick_Config1();
	NVIC_Configuration();
	//------------------------------------------------------------------------------ 
	for (i = 0; i < 6; ++i)
	{
		//i=0;
		//GPIOA->ODR ^= GPIO_Pin_10;  // led3 toogle
		//GPIOA->ODR ^= GPIO_Pin_11;  // led3 toogle

		delayms(1);
		led_toggle();
	}
	//------------------------------------------------------------------------------  
	for (;; )
	{
		//if (set_sw&0x01)
		//myPWR_EnterSleepMode(); // 休眠，降低功耗

		func_tb();
	}
}
//******************************************************************************
#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval : None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	 /* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
 ******************************************************************************
 * @file    Project/Template/stm32f10x_it.c
 * @author  MCD Application Team
 * @version V3.0.0
 * @date    04/06/2009
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @copy
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
 */

 /* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32_m.h"

int tt;
extern bit f_tb;        // 基本定时标志
//******************************************************************************
/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval : None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval : None
 */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval : None
 */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval : None
 */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval : None
 */
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval : None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval : None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval : None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval : None
 */
void SysTick_Handler(void)
{
	f_tb = 1;
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
void TIM1_UP_IRQHandler(void)   // 实际测量，周期为50US
{
	tt = 1;
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

/******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
	tt = 2;
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);
}


/******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
	tt = 3;
	DMA_ClearITPendingBit(DMA1_IT_TC5);
	GPIOA->ODR ^= GPIO_Pin_11;    // for test!
}
//******************************************************************************
/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
//******************************************************************************