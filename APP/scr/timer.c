/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-14					文档编写
						2018-03-29					定时器函数的编写
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "timer.h"
/* 宏定义	--------------------------------------------------------------------*/
#define RCC_TIM_CMD																RCC_APB1PeriphClockCmd
#define RCC_TIM_CLK																RCC_APB1Periph_TIM3
#define TIM_COM																		TIM3
#define TIM_IRQn																	TIM3_IRQn

#define TIM_FREQ_MS																1000												//1KHz ms
#define TIM_COUNT																	500													//500ms
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
uint32_t g_sys_tim_s = 0;																		 //定时器计时
uint8_t g_adc_get_flag = 0;																	 //ADC采集标志
uint8_t NET_outtime = 0;																	  //服务器网络超时
uint8_t g_adc_time   = 0;																		//adc的采样时间
uint8_t g_crt_flag 		= 0;																	//CRT接收标志
uint8_t g_crt_time    = 0;																	//CRT接收超时时间
uint8_t	g_wait_flag  = 0;
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		定时器初始化(单位 500ms)
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-29				函数编写
 ****************************************************************************/
void Timer_Config( void )
{
	TIM_TimeBaseInitTypeDef		newTIM_Init;
	NVIC_InitTypeDef					newNVIC_Init;
	
	newTIM_Init.TIM_ClockDivision	=	TIM_CKD_DIV1;
	newTIM_Init.TIM_CounterMode		=	TIM_CounterMode_Up;
	newTIM_Init.TIM_Period				=	SystemCoreClock / TIM_FREQ_MS -1;	//1KHz  48000分频
	newTIM_Init.TIM_Prescaler			=	TIM_COUNT-1;										//100ms
	
	/*1> 定时器 配置*/	
	TIM_DeInit( TIM_COM );
	RCC_TIM_CMD( RCC_TIM_CLK , ENABLE );
	TIM_TimeBaseInit(TIM_COM , &newTIM_Init );
	TIM_ITConfig(TIM_COM , TIM_IT_Update , ENABLE );	//允许更新中断
	TIM_ClearFlag(TIM_COM , TIM_FLAG_Update );				//清除中断标志
	TIM_ARRPreloadConfig(TIM_COM , DISABLE );				  //禁止预装缓存器
	
	newNVIC_Init.NVIC_IRQChannelCmd      = ENABLE;
	/*5> 中断 配置*/	
	newNVIC_Init.NVIC_IRQChannel				 = TIM_IRQn;
	newNVIC_Init.NVIC_IRQChannelPriority = 2;
	NVIC_SetPriority( TIM_IRQn	, 1);
	NVIC_Init( &newNVIC_Init );
	
	TIM_Cmd(TIM_COM , ENABLE);											//定时器使能
}
/*****************************************************************************
 * 函数功能:		采样时间更新
 * 形式参数:		time 采样时间 单位s
 * 返回参数:		无
 * 更改日期;		2018-07-24				函数编写
 ****************************************************************************/
void CampTime_Updeat( uint16_t *time )
{
	if(*time > 65)*time=65;
	g_adc_time = g_sys_tim_s + (*time);
}

/*****************************************************************************
 * 函数功能:		采样时间更新
 * 形式参数:		time 采样时间 单位s
 * 返回参数:		无
 * 更改日期;		2018-07-24				函数编写
 ****************************************************************************/
void SysTimeReset( void )
{
	TIM_COM->CNT = 0;
}
/*****************************************************************************
 * 函数功能:		定时器中断服务函数(500ms)
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-29				函数编写
 ****************************************************************************/
void Sys_TimerIRQnHandle( void )
{
	static uint8_t time_num =0;

	if( TIM_GetITStatus( TIM_COM , TIM_IT_Update ) != RESET )
	{
		if(g_wait_flag==1)g_wait_flag = 2;
		if(time_num<240)time_num++;
		else time_num=0;
		if((time_num %2)==0)  
		{
			if(NET_outtime>0)NET_outtime--;//服务器等待超时时间
			if(g_crt_time>0)g_crt_time--;
			else g_crt_flag = 0;
			g_sys_tim_s++;
			if((g_adc_time == (g_sys_tim_s&0xFF)) && (!g_adc_get_flag))
				g_adc_get_flag = 1;
		}
		TIM_ClearITPendingBit( TIM_COM , TIM_IT_Update );
	}
}
/*------------------------------File----------End------------------------------*/
