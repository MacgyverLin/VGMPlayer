#include "timer3.h" 
#include <stm32f10x_tim.h>
  
/////////////////////////////////////////////////////////////
#define TIMER3_PWM_PORT             GPIOB  
#define TIMER3_PWM_PIN              GPIO_Pin_0  
#define TIMER3_PWM_MODE             GPIO_Mode_AF_PP 
#define TIMER3_PWM_GPIO_CLOCK				RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO 
#define TIMER3_PWM_PERIPH_CLOCK			RCC_APB1Periph_TIM3  
 
#define TIMER3_PWM_PERIOD						(72 - 1)  
 
/////////////////////////////////////////////////////////////
static void Timer3PWM_InitIO(void);
static void Timer3PWM_InitConfig(u16 prescaler);
 
static void Timer3PWM_InitIO(void)  
{      
	GPIO_InitTypeDef GPIO_InitStructure;  
 
	//???? 
	RCC_APB2PeriphClockCmd(TIMER3_PWM_GPIO_CLOCK, ENABLE);	
 
	//IO???
	GPIO_InitStructure.GPIO_Pin = TIMER3_PWM_PIN;
	GPIO_InitStructure.GPIO_Mode = TIMER3_PWM_MODE;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIMER3_PWM_PORT, &GPIO_InitStructure);				
}

/////////////////////////////////////////////////////////////             
static void Timer3PWM_InitConfig(u16 prescaler)  
{      
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_OCInitTypeDef TIM_OCInitStructure;  
  
	//????
	RCC_APB1PeriphClockCmd(TIMER3_PWM_PERIPH_CLOCK, ENABLE);  
	
	//?????
	TIM_TimeBaseStructure.TIM_Period = TIMER3_PWM_PERIOD;								//?????  
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;		//????? 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;														//????????:???  
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;							//????????  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
	
	//PWM??
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;												//???PWM??1(???????????)  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;						//??????                
	TIM_OCInitStructure.TIM_Pulse = 0;																			//????????????????  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;								//????????  
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);																//????3      
 
	//????PWM		
	Timer3PWMSetStatus(TIMER3_PWM_STATUS_OFF);	
	
	//??	
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);												//CH3?????  
	TIM_ARRPreloadConfig(TIM3, ENABLE);																			//??TIM3?????ARR  	
	TIM_Cmd(TIM3, ENABLE);																									//?????3  
}

/////////////////////////////////////////////////////////////
void Timer3PWMSetStatus(u8 status)  
{  
  //??PWM  
  if(status == TIMER3_PWM_STATUS_ON)  
  {   
		TIM_SetCounter(TIM3, 0);		
		TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Enable);		
  }  
  //??PWM
  else  
  {  
		TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Disable);			 	
  }  
}  
  
/////////////////////////////////////////////////////////////   
void Timer3PWM_SetDutyCycle(u8 dutyCycle)  
{  
  TIM_SetCompare3(TIM3, TIMER3_PWM_PERIOD*dutyCycle/100);   
}  

/////////////////////////////////////////////////////////////
void Timer3PWM_Init(u16 prescaler)   
{  	
	Timer3PWM_InitIO();  
 
	Timer3PWM_InitConfig(prescaler);
}

void test()
{
	Timer3PWM_Init(26);													//??PWM???1000000/26=38000Hz=38KHz
	Timer3PWM_SetDutyCycle(50);									//??PWM????50%
	Timer3PWM_Status(GUA_TIMER3_PWM_STATUS_ON);	//??PWM	
	
	Timer3PWM_SetDutyCycle(30);									//??PWM????30%  
	Timer3PWM_SetDutyCycle(60);									//??PWM????60%  
	Timer3PWM_SetDutyCycle(0);										//??PWM????0%  
	Timer3PWM_SetDutyCycle(100);									//??PWM????100% 	
	
	Timer3PWM_Status(GUA_TIMER3_PWM_STATUS_OFF);	//??PWM		
	Timer3PWM_SetDutyCycle(50);									//??PWM????50%
	Timer3PWM_Status(GUA_TIMER3_PWM_STATUS_ON);	//??PWM
}
