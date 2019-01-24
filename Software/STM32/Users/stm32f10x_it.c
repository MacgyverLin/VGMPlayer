/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include <stm32f10x_dma.h>
#include <stm32f10x_tim.h>

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
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
  * @retval None
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
  * @retval None
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
  * @retval None
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
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

u32 counter = 0;
u8 led = 0;

//u32 sampleRate = 44100;
u32 sampleRate = 22050;
//u32 sampleRate = 11025;
u32 bpp = 10;//9
//u32 bpp = 11;//10
//u32 bpp = 12;//11
u32 max = 1024;//512//pow(2, bpp);
//u32 max = 2048;//1024//pow(2, bpp);
//u32 max = 4096;//2048//pow(2, bpp);
u32 interruptsPerUpdate = 3.18877551; // 1.594387755 // 72*1000*1000 / max / sampleRate;

void TIM3_IRQHandler( void )   // ????,???50US
{
	TIM_ClearITPendingBit( TIM3, TIM_IT_CC3 );
	// #define TIM_IT_CC1                         ((uint16_t)0x0002)
	// #define TIM_IT_CC2                         ((uint16_t)0x0004)
	// #define TIM_IT_CC3                         ((uint16_t)0x0008)
	// #define TIM_IT_CC4                         ((uint16_t)0x0010)
	
	if(counter++>=interruptsPerUpdate)
	{
		counter = 0;
		led = 1 - led;
		if(led)
			GPIOB->BSRR = GPIO_Pin_1;
		else
			GPIOB->BRR = GPIO_Pin_1;
	}
}

/******************************************************************************/
void DMA1_Channel2_IRQHandler( void )
{
	DMA_ClearITPendingBit( DMA1_IT_TC2 );

	GPIOB->BSRR = GPIO_Pin_1;
}  

void DMA1_Channel3_IRQHandler( void )
{
	DMA_ClearITPendingBit( DMA1_IT_TC3 );

	//GPIOB->BSRR = GPIO_Pin_1;
}  

void DMA1_Channel6_IRQHandler( void )
{
	DMA_ClearITPendingBit( DMA1_IT_TC6 );

	//GPIOB->BSRR = GPIO_Pin_1;
}  

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
