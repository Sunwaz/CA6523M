#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f0xx.h"

extern __IO u32 s_wTimeCnt;
void SysTick_Init(void);
void TimingDelay_Decrement(void);
void Delay_us(__IO u32 nTime);         // 单位1ms
#define Delay_ms(x) Delay_us(x)	 //单位ms
void Delay_nop_nus(u16 nus);
void Delay_nop_nms(u16 nms);
#endif /* __ISO_SYSTICK_H */
