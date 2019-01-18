#include "stm32f10x.h"
#include "stm32_m.h"
#include "dzy.h"   
#include "myRCC.h"   
//******************************************************************************
#define TIM1_CCR3_Address    0x40012C3C

bit f_tb;			// ������ʱ��־
bit f_100ms;
bit f_1000ms;
INT16U cnt_test;	// �������������������ʹ��
float clk_sys;		// �����������ʹ��

#if 1
uint16_t SRC_Buffer[] =
{ 72 * 5 };   // �����ز�Ƶ��ȡ20kHZ��������������Ȳ�Ҫ��50us, ��������Ҫ����72*50

#else

uint16_t SRC_Buffer[] =
{ 72 * 2,72 * 5,72 * 10,72 * 20,72 * 40,72 * 10 }; // �����ز�Ƶ��ȡ20kHZ��������������Ȳ�Ҫ��50us,��������Ҫ����72*50
#endif

//******************************************************************************
// ��ʱ���򣬵�λΪ*1ms
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
// pcb�ϵ�ָʾ��
//******************************************************************************
static void led_toggle(void)
{
	GPIOC->ODR ^= GPIO_Pin_7;  // led2 toogle
	GPIOC->ODR ^= GPIO_Pin_6;  // led3 toogle
}

//******************************************************************************
// ʱ�����ó�ʼ��
//******************************************************************************
static void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;
	/*
	 RCC_AdjustHSICalibrationValue �����ڲ����پ���HSI��У׼ֵ
	 RCC_ITConfig ʹ�ܻ���ʧ��ָ����RCC�ж�
	 RCC_ClearFlag ���RCC�ĸ�λ��־λ
	 RCC_GetITStatus ���ָ����RCC�жϷ������
	 RCC_ClearITPendingBit ���RCC���жϴ�����λ
	 */
	 /* RCC system reset(for debug purpose) */
	 // ʱ��ϵͳ��λ
	RCC_DeInit();

	// ʹ���ⲿ��8M����
	// �����ⲿ���پ���HSE��
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	// ʹ�ܻ���ʧ���ڲ����پ���HSI��
	RCC_HSICmd(DISABLE);

	// �ȴ�HSE����
	// �ú������ȴ�ֱ��HSE�����������ڳ�ʱ��������˳�
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		// ����AHBʱ�ӣ�HCLK��
		RCC_HCLKConfig(RCC_HCLK_Div_); // 36 MHz

		// ���õ���AHBʱ�ӣ�PCLK1��
		RCC_PCLK1Config(RCC_PCLK1_Div_); // 2.25 MHz

		// ���ø���AHBʱ�ӣ�PCLK2��
		RCC_PCLK2Config(RCC_PCLK2_Div_); // 2.25 MHz

		/* ADCCLK = PCLK2/8 */
		// ����ADCʱ�ӣ�ADCCLK��
		RCC_ADCCLKConfig(RCC_ADC_DIV_); // 0.281Mhz

		// ����USBʱ�ӣ�USBCLK��
		// USBʱ�� = PLLʱ�ӳ���1.5
		//RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

		// �����ⲿ���پ���LSE��
		RCC_LSEConfig(RCC_LSE_OFF);

		// ʹ�ܻ���ʧ���ڲ����پ���LSI��
		// LSE����OFF
		RCC_LSICmd(DISABLE);

		// ����RTCʱ�ӣ�RTCCLK��
		// ѡ��HSEʱ��Ƶ�ʳ���128��ΪRTCʱ��
		//RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);

		// ʹ�ܻ���ʧ��RTCʱ��
		// RTCʱ�ӵ���״̬
		RCC_RTCCLKCmd(DISABLE);

		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);

		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		/* PLLCLK = 8MHz * 9 = 72 MHz */
		// ����PLLʱ��Դ����Ƶϵ��
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_);

		/* Enable PLL */
		// ʹ�ܻ���ʧ��PLL
		RCC_PLLCmd(ENABLE);

		/* Wait till PLL is ready */
		// ���ָ����RCC��־λ�������
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		/* Select PLL as system clock source */
		// ����ϵͳʱ�ӣ�SYSCLK��
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Wait till PLL is used as system clock source */
		// ��������ϵͳʱ�ӵ�ʱ��Դ
		while (RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}

	// ʹ�ܻ���ʧ��AHB����ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2 | RCC_AHBPeriph_SRAM 
		| RCC_AHBPeriph_FLITF | RCC_AHBPeriph_CRC | RCC_AHBPeriph_FSMC
		| RCC_AHBPeriph_SDIO, DISABLE);
	// ʹ�ܻ���ʧ��APB1����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL, DISABLE);

	// ǿ�ƻ����ͷŸ���APB��APB2�����踴λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ALL, ENABLE);
	// �˳���λ״̬
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ALL, DISABLE);

	// ǿ�ƻ����ͷŵ���APB��APB1�����踴λ
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_ALL, ENABLE);

	// ǿ�ƻ����ͷź���λ
	RCC_BackupResetCmd(ENABLE);

	// ʹ�ܻ���ʧ��ʱ�Ӱ�ȫϵͳ
	RCC_ClockSecuritySystemCmd(DISABLE);
}

//******************************************************************************
// NVIC����
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
// SysTick���ó�ʼ��
//******************************************************************************
static void SysTick_Config1(void)
{
#if 1
#define SystemFreq  (FCLK*1000000.0)    // ��λΪHz
#define TB_SysTick  (TIME_TB*1000)  // ��λΪuS,��ʾ����ʵ��һ��

	static INT32U ticks;

	ticks = (INT32U)((TB_SysTick / 1000000.0) * SystemFreq);
	SysTick_Config(ticks);
#endif 
}
//******************************************************************************
// GPIO����
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

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // dma1ʱ��ʹ��

	DMA_DeInit(DMA1_Channel5);   // DMA��λ
	DMA_StructInit(&DMA_InitStructure);   // DMAȱʡ�Ĳ���

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM1_CCR3_Address; //�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SRC_Buffer;  //�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //dma���䷽��,����
	DMA_InitStructure.DMA_BufferSize = sizeof(SRC_Buffer) / 2; //����DMA�ڴ���ʱ�������ĳ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //����DMA���������ģʽ��һ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //����DMA���ڴ����ģʽ��
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //���������ֳ�
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;   //�ڴ������ֳ�
	
	//ѭ��ģʽ������Bufferд�����Զ��ص���ʼ��ַ��ʼ����
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //����DMA�Ĵ���ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //����DMA�����ȼ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //����DMA��2��memory�еı����������
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
	++TIM_Prescaler;                       // ע����仰��һ��Ҫ++

	utemp = (INT32U)(TIM1CLK * 1000000.0) / TIM_Prescaler;  // ��Ƶ��Ķ�ʱ������Ƶ��
	TIM_Period = utemp / Freq_PWM;               // ���ڳ���

	TIM_TimeBaseStructure.TIM_Period = TIM_Period - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM_Prescaler - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 1 - 1;               // ÿ��ֱ�Ӹ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	//------------------------------------------------------------------------------
	  /* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // ʹ������Ƚ�״̬
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; // ʧ������Ƚ�N״̬
	TIM_OCInitStructure.TIM_Pulse = 72;
	/*
	 TIM_OCMode_PWM2  TIM_OCPolarity_Low    ������ģʽ
	 TIM_OCMode_PWM2  TIM_OCPolarity_High   ������ģʽ

	 TIM_OCMode_PWM1  TIM_OCPolarity_Low    ������ģʽ
	 TIM_OCMode_PWM1  TIM_OCPolarity_High   ������ģʽ

	 TIM1_OCPolarity�������---TIM_OCPolarity_High������Ƚϼ��Ը�,�������������
	 TIM1_OCPolarity�������---TIM_OCPolarity_Low�� ����Ƚϼ��Ե�,������Ǹ�����
	 */
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low; // �����������
	// ѡ�����״̬�µķǹ���״̬
	// ��MOE=0����TIM1����ȽϿ���״̬       
	// Ĭ�����λ�͵�ƽ                
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	// ѡ�����״̬�µķǹ���״̬
	// ��MOE=0����TIM1����Ƚ�N����״̬
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);   // ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���

	/* TIM1 Update DMA Request enable */
	TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);

	//TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE); // ��ʱ�ж�ʹ��
	//TIM_ITConfig(TIM1,TIM_IT_CC3,ENABLE); // ��ʱ�ж�ʹ��
	TIM_Cmd(TIM1, ENABLE);      // ��ʱ����ʼ����

	// ����������Ҫ��!!!
	TIM_CtrlPWMOutputs(TIM1, ENABLE); /* Main Output Enable */
}
//******************************************************************************
// ������ʱ����,����Ϊ TIME_TB
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
// ����˯��ģʽ
// ���б�д
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
// ������
//******************************************************************************
void main(void)
{
	int i;

	RCC_Configuration();
	GPIO_Configuration();
	delayms(100);   // ��ʱ���ȴ���ѹ�ȶ�
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
		//myPWR_EnterSleepMode(); // ���ߣ����͹���

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
extern bit f_tb;        // ������ʱ��־
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
void TIM1_UP_IRQHandler(void)   // ʵ�ʲ���������Ϊ50US
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