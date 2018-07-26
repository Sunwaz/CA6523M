/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-14					文档移植
 *说    明:	
*******************************************************************************************/
#ifndef __timer_H
#define __timer_H
/* 头文件 ------------------------------------------------------------------*/
#include <stm32f0xx.h>
/* 宏定义 ------------------------------------------------------------------*/
/* 结构体声明	--------------------------------------------------------------*/
/* 枚举声明	----------------------------------------------------------------*/
/* 全局变量	----------------------------------------------------------------*/
extern uint8_t NET_outtime;										//服务器网络超时
extern uint8_t g_adc_get_flag;								//ADC采集标志
extern uint8_t g_crt_flag;										//CRT接收标志
extern uint8_t g_crt_time;										//CRT接收超时时间
extern uint8_t  g_wait_flag;
/* 接口提供	----------------------------------------------------------------*/
void Timer_Config( void );										//定时器初始化
void Sys_TimerIRQnHandle( void );							//系统定时器中断服务函数
void CampTime_Updeat( uint16_t *time );
void SysTimeReset( void );
#endif       /*__timer_H end*/
/*------------------------------File----------End------------------------------*/
