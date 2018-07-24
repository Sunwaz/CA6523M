/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-09					文档编写
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "key_led.h"
#include "delay.h"
/* 宏定义	--------------------------------------------------------------------*/
#define LED_ERROR_PORT							GPIOB
#define LED_ERROR_PIN								GPIO_Pin_14
#define RCC_ERROR_LED_CMD						RCC_AHBPeriphClockCmd
#define RCC_ERROR_LED_CLK						RCC_AHBPeriph_GPIOB

#define LED_ALARM_PORT							GPIOB
#define LED_ALARM_PIN								GPIO_Pin_13
#define RCC_ALARM_LED_CMD						RCC_AHBPeriphClockCmd
#define RCC_ALARM_LED_CLK						RCC_AHBPeriph_GPIOB

#define LED_WORK_PORT								GPIOB
#define LED_WORK_PIN								GPIO_Pin_12
#define RCC_WORK_LED_CMD						RCC_AHBPeriphClockCmd
#define RCC_WORK_LED_CLK						RCC_AHBPeriph_GPIOB

#define LED_CLEAR_PORT							GPIOA
#define LED_CLEAR_PIN								GPIO_Pin_12
#define RCC_CLEAR_LED_CMD						RCC_AHBPeriphClockCmd
#define RCC_CLEAR_LED_CLK						RCC_AHBPeriph_GPIOA

#define LED_SIGNAL_PORT							GPIOB
#define LED_SIGNAL_PIN							GPIO_Pin_15
#define RCC_SIGNAL_LED_CMD					RCC_AHBPeriphClockCmd
#define RCC_SIGNAL_LED_CLK					RCC_AHBPeriph_GPIOB

#define KEY_CLEAR_PORT							GPIOF
#define KEY_CLEAR_PIN								GPIO_Pin_7
#define RCC_CLEAR_KEY_CMD						RCC_AHBPeriphClockCmd
#define RCC_CLEAR_KEY_CLK						RCC_AHBPeriph_GPIOF

#define KEY_RESET_PORT							GPIOF
#define KEY_RESET_PIN								GPIO_Pin_6
#define RCC_RESET_KEY_CMD						RCC_AHBPeriphClockCmd
#define RCC_RESET_KEY_CLK						RCC_AHBPeriph_GPIOF

#define BEEP_PORT                   GPIOB
#define BEEP_PIN										GPIO_Pin_8
#define RCC_BEEP_CMD								RCC_AHBPeriphClockCmd
#define RCC_BEEP_CLK						    RCC_AHBPeriph_GPIOB

#define RCC_LED_TIME_CMD						RCC_APB1PeriphClockCmd
#define RCC_LED_TIME_CLK						RCC_APB1Periph_TIM14
#define LED_TIMER										TIM14
#define LED_TIMER_IRQ								TIM14_IRQn

#define READ_RESET_KEY							GPIO_ReadInputDataBit( KEY_RESET_PORT , KEY_RESET_PIN)
#define READ_CLEAR_KEY							GPIO_ReadInputDataBit( KEY_CLEAR_PORT , KEY_CLEAR_PIN)

#define BEEP_CLOS                   GPIO_ResetBits(BEEP_PORT  , BEEP_PIN);
#define BEEP_OPEN										GPIO_SetBits( BEEP_PORT , BEEP_PIN );
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
static uint8_t s_open_flag 					= 1;										//LED控制标志位     bit0,0,工作指示灯工作 1,工作指示灯禁止
																														//								 bit1,0,信号指示灯不工作 1,信号指示灯故障指示
uint8_t		g_sys_error_flag					=	0;										//系统故障标志位    0,无故障,LED正常闪烁 1,有故障,LED保持当前状态
uint8_t		g_reset_flag 							= 0;										//复位标志
uint16_t	g_sys_operation_msg	      = 0;									  //bit0 设备复位 bit1 设备消音 bit2 手动报警 bit3 警情消除 bit4 自检 bit5 确认 bit6 测试 bit7 SIM卡被更改 bit8 设备掉电 bit9 模块被更换
uint32_t  g_alarm_flag 							= 0;										//报警标志位
/*
  bit0					bit1					bit2					bit3					bit4					bit5					bit6					bit7					bit8					bit9
电压火警 			电流1火警			电流2火警			电流3火警			温度1火警			温度2火警			温度3火警			漏电流火警	  		预留	    		预留
  bit10					bit11					bit12					bit13					bit14					bit15					bit16					bit17					bit18					bit19
电压火警消音	电流1火警消音	电流2火警消音	电流3火警消音	温度1火警消音	温度2火警消音	温度3火警消音	漏电流火警消音	    预留	    		预留
  bit20					bit21					bit22					bit23					bit24					bit25					bit26					bit27					bit28					bit29
电压火警上报	电流1火警上报	电流2火警上报	电流3火警上报	温度1火警上报	温度2火警上报	温度3火警上报	漏电流火警上报	    预留	    		预留
  bit30	        bit31							
 火警恢复	     火警消除								
*/
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		按键和LED 外设初始化
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-09				函数编写
							2018-04-04				增加了蜂鸣器相关代码
 ****************************************************************************/ 
void KeyLED_Config( void )
{
	GPIO_InitTypeDef  				newGPIO_Init;
	TIM_TimeBaseInitTypeDef		newTim_Init;
	NVIC_InitTypeDef					newNVIC_Init;
	
	newGPIO_Init.GPIO_Mode	= GPIO_Mode_OUT;
	newGPIO_Init.GPIO_OType	=	GPIO_OType_PP;
	newGPIO_Init.GPIO_PuPd	=	GPIO_PuPd_UP;
	newGPIO_Init.GPIO_Speed	=	GPIO_Speed_50MHz;
	/*1> ERROR LED 灯的初始化*/
	newGPIO_Init.GPIO_Pin		=	LED_ERROR_PIN;
	RCC_ERROR_LED_CMD(	RCC_ERROR_LED_CLK , ENABLE );
	GPIO_Init(	LED_ERROR_PORT , &newGPIO_Init );
	/*2> ALARM LED 灯的初始化*/
	newGPIO_Init.GPIO_Pin		=	LED_ALARM_PIN;
	RCC_ALARM_LED_CMD(	RCC_ALARM_LED_CLK , ENABLE );
	GPIO_Init( LED_ALARM_PORT , &newGPIO_Init );
	/*3> WORK LED 灯的初始化*/
	newGPIO_Init.GPIO_Pin		=	LED_WORK_PIN;
	RCC_WORK_LED_CMD(	RCC_WORK_LED_CLK , ENABLE );
	GPIO_Init( LED_WORK_PORT , &newGPIO_Init );
	/*4> CLEAR LED 灯的初始化*/
	newGPIO_Init.GPIO_Pin		=	LED_CLEAR_PIN;
	RCC_CLEAR_LED_CMD(	RCC_CLEAR_LED_CLK , ENABLE );
	GPIO_Init( LED_CLEAR_PORT , &newGPIO_Init );
	/*5> SIGNAL LED 灯的初始化*/
	newGPIO_Init.GPIO_Pin		=	LED_SIGNAL_PIN;
	RCC_SIGNAL_LED_CMD(	RCC_SIGNAL_LED_CLK , ENABLE );
	GPIO_Init( LED_SIGNAL_PORT , &newGPIO_Init );
	/*6> CLEAR KEY 按键的初始化*/
	newGPIO_Init.GPIO_Mode	=	GPIO_Mode_IN;
	newGPIO_Init.GPIO_Pin		=	KEY_CLEAR_PIN;
	newGPIO_Init.GPIO_PuPd	=	GPIO_PuPd_DOWN;
	RCC_CLEAR_KEY_CMD(	RCC_CLEAR_KEY_CLK , ENABLE );
	GPIO_Init( KEY_CLEAR_PORT , &newGPIO_Init );
	/*7> RESET KEY 按键的初始化*/
	newGPIO_Init.GPIO_Pin		=	KEY_RESET_PIN;
	newGPIO_Init.GPIO_PuPd	=	GPIO_PuPd_DOWN;
	RCC_RESET_KEY_CMD(	RCC_RESET_KEY_CLK , ENABLE );
	GPIO_Init( KEY_RESET_PORT , &newGPIO_Init );
	/*8> 定时器初始化*/
	RCC_LED_TIME_CMD( RCC_LED_TIME_CLK , ENABLE );
	TIM_DeInit( LED_TIMER );
	newTim_Init.TIM_Prescaler     = 999;									//定时器溢出值 设置计数溢出大小，每计1个数就产生一个更新事件10us
	newTim_Init.TIM_Period        = 96000-1;							//38-96预分频系数为480-1，这样计数器时钟为48MHz/480 = 100kHz
	newTim_Init.TIM_ClockDivision =TIM_CKD_DIV1;
	newTim_Init.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInit( LED_TIMER , &newTim_Init );				//20us中断一次
	TIM_ITConfig( LED_TIMER , TIM_IT_Update,ENABLE );		//允许更新中断
	TIM_ClearFlag( LED_TIMER , TIM_FLAG_Update );				//清除中断标志
	TIM_ARRPreloadConfig( LED_TIMER , DISABLE );				//禁止预装缓存器
	/*9> 定时器中断配置*/
	NVIC_SetPriority( LED_TIMER_IRQ , 1);										//优先组别
	newNVIC_Init.NVIC_IRQChannel         = LED_TIMER_IRQ;		//TIM3中断
	newNVIC_Init.NVIC_IRQChannelPriority = 5;						//抢占优先级3
	newNVIC_Init.NVIC_IRQChannelCmd      = ENABLE;			//IRQ通道使能
	NVIC_Init(&newNVIC_Init);
	TIM_Cmd( LED_TIMER , ENABLE );											//定时器使能
	/*10> 蜂鸣器的初始化*/
	RCC_BEEP_CMD( RCC_BEEP_CLK , ENABLE);
	newGPIO_Init.GPIO_Mode  = GPIO_Mode_OUT;
	newGPIO_Init.GPIO_Pin		=	BEEP_PIN;
  GPIO_Init( BEEP_PORT , &newGPIO_Init );
		
	BEEP_OPEN;
	delay_ms(100);
	BEEP_CLOS;
}
/*****************************************************************************
 * 函数功能:		定时器中断服务函数
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-09				函数编写
 ****************************************************************************/ 
void LED_TimerIrqHandle( void )
{
	static uint32_t s_tim_cnt = 0;					//闪烁计数
	
	if( TIM_GetITStatus( LED_TIMER , TIM_IT_Update ) != RESET )
	{
		if( !g_sys_error_flag )
		{
			s_tim_cnt++;
			GPIO_WriteBit( LED_WORK_PORT	, LED_WORK_PIN   , (BitAction)((!(((s_open_flag & 0x01) << 1) & (s_tim_cnt & 0xFF)))&0x01));
			GPIO_WriteBit( BEEP_PORT			, BEEP_PIN       , (BitAction)( (((s_open_flag & 0x04) >> 0x02) & (s_tim_cnt & 0xFF)) & 0x01));
		}else
		{
			GPIO_WriteBit( BEEP_PORT , BEEP_PIN , Bit_RESET );											//如果死机,关掉蜂鸣器
		}
		TIM_ClearITPendingBit( LED_TIMER , TIM_IT_Update );
	}
}

/*****************************************************************************
 * 函数功能:		打开蜂鸣器
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-04-08				函数编写
 ****************************************************************************/ 
void Alarm_SetBit( void )
{
	s_open_flag |= 0x04;
}

/*****************************************************************************
 * 函数功能:		关闭蜂鸣器
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-04-08				函数编写
 ****************************************************************************/ 
void Alarm_ResetBit( void )
{
	s_open_flag &=~ 0x04;
}
/*****************************************************************************
 * 函数功能:		LED灯控制函数
 * 形式参数:		Led 被控制的LED State 控制的状态
 * 返回参数:		无
 * 更改日期;		2018-03-09				函数编写
 ****************************************************************************/ 
void LED_Control( LED_Typedef Led , LED_Ctl_Typedef State )
{
	BitAction led_stat = Bit_RESET;					//LED的控制状态

	if( State == OPEN )
	{
		led_stat = Bit_SET;
	}
	switch( Led )
	{
		case L_WORK:
			GPIO_WriteBit( LED_WORK_PORT , LED_WORK_PIN , led_stat );break;
		case L_ERROR:
			GPIO_WriteBit( LED_ERROR_PORT , LED_ERROR_PIN , led_stat );break;
		case L_CLEAR:
			GPIO_WriteBit( LED_CLEAR_PORT , LED_CLEAR_PIN , led_stat );break;
		case L_ALARM:
			GPIO_WriteBit( LED_ALARM_PORT , LED_ALARM_PIN , led_stat );break;
		case L_SIGNAL:
			GPIO_WriteBit( LED_SIGNAL_PORT , LED_SIGNAL_PIN , led_stat );break;
		case L_ALL:
			GPIO_WriteBit( LED_ALARM_PORT  , LED_ALARM_PIN  , led_stat );
			GPIO_WriteBit( LED_CLEAR_PORT  , LED_CLEAR_PIN  , led_stat );
			GPIO_WriteBit( LED_ERROR_PORT  , LED_ERROR_PIN  , led_stat );
			GPIO_WriteBit( LED_WORK_PORT   , LED_WORK_PIN   , led_stat );
			GPIO_WriteBit( LED_SIGNAL_PORT , LED_SIGNAL_PIN , led_stat );break;                
		default:break;
	}
}

/*****************************************************************************
 * 函数功能:		通讯故障控制
 * 形式参数:		State 控制的状态
 * 返回参数:		无
 * 更改日期;		2018-03-09				函数编写
 ****************************************************************************/ 
void LED_SignalErrorCtl( LED_Ctl_Typedef State )
{
	GPIO_ResetBits( LED_SIGNAL_PORT , LED_SIGNAL_PIN );
	if( State == OPEN)
	{
		s_open_flag |=  0x02;
	}else
	{
		s_open_flag &=~ 0x02;
	}
}
/*****************************************************************************
 * 函数功能:		报警控制
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-04-12				函数编写
 ****************************************************************************/
void Alarm_Set( void )
{
	Alarm_SetBit();
	LED_Control( L_ALARM , OPEN );
}
/*****************************************************************************
 * 函数功能:		报警控制
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-04-12				函数编写
 ****************************************************************************/
void Alarm_Reset( void )
{
	Alarm_ResetBit();
	LED_Control( L_ALARM , CLOS );
	LED_Control( L_CLEAR , CLOS );
	BEEP_CLOS;								     //关掉蜂鸣器
}
/*****************************************************************************
 * 函数功能:		按键控制
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-04-12				函数编写
 ****************************************************************************/
void Key_Scan( void )
{
	uint16_t temp = 0;						//中间变量
	uint8_t  flag = 0;						//标志      如果复位按键被长按则置位
	
	if(1){											//复位按键
		temp = 0;
		if( READ_RESET_KEY != RESET)
		{
			delay_ms(20);
			while( READ_RESET_KEY != RESET )
			{
				temp++;
				delay_ms(10);
				if(temp > 0xD0)temp = 0xD0;
			}
			if((temp > 0) && (temp < 0x50))		//短按,复位火警功能
			{
				for(temp = 1;temp < 0x400;temp <<= 1)//消音处理
				{
					if((g_alarm_flag & (temp << LOCAL_ALARM)) || (g_alarm_flag & (temp << ALARM_UPLOAD)))
					{
						flag = 1;
						break;
					}
				}
				if(flag)//有火警才进行火警复位
				{
					g_alarm_flag = 0;               //状态复位
					g_sys_operation_msg |= 0x08;    //警情清除
					Alarm_Reset();							    //禁止蜂鸣器发出声音
				}
			}else if( (temp >= 0x50) && (temp <= 0xD0) )//长按,复位设备
			{
				g_reset_flag = 0x0F;
				g_sys_operation_msg |= 0x01;
			}
		}
  }
	if(1){											//消音按键
		temp = 0;
		if( READ_CLEAR_KEY != RESET )
		{
			delay_ms(20);
			while( READ_CLEAR_KEY != RESET )
			{
				temp++;
				delay_ms(10);
				if(temp > 0xD0)temp = 0xD0;
			}
			if((temp > 0) && (temp < 0x50))		//短按,消音功能
			{
				if(s_open_flag & 0x04)				//蜂鸣器被打开
				{
					for(temp = 1;temp < 0x400;temp <<= 1)//消音处理
					{
						if((g_alarm_flag & (temp << LOCAL_ALARM)) || (g_alarm_flag & (temp << ALARM_UPLOAD)))
						{
							g_alarm_flag |= (temp << ALARM_SILENCE);
						}
					}
					g_sys_operation_msg |= 0x02;  //消音
					Alarm_ResetBit();							//禁止蜂鸣器发出声音
					BEEP_CLOS;								    //关掉蜂鸣器
					LED_Control( L_CLEAR , OPEN );
				}
			}else if( (temp >= 0x50) && (temp <= 0xD0) )//长按
			{
			}
		}
	}
}
/*------------------------------File----------End------------------------------*/
