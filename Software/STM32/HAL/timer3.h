#ifndef _TIMER3_H_  
#define _TIMER3_H_  

#include "vgmdef.h"
 
#define TIMER3_PWM_STATUS_ON                0  
#define TIMER3_PWM_STATUS_OFF               1
 
/*********************??????************************/   
void Timer3PWMSetStatus(u8 status);  
void Timer3PWMSetDutyCycle(u8 dutyCycle);  
void Timer3PWMInit(u16 prescaler);  

#endif
