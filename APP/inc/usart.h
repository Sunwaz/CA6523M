/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-08					文档移植
 *说    明:	
*******************************************************************************************/
#ifndef __usart_H
#define __usart_H
/* 头文件 ------------------------------------------------------------------*/
#include <stm32f0xx.h>
#include "stdio.h"
/* 宏定义 ------------------------------------------------------------------*/
#define RX_485_LEN         						0xFF
#define USART_REC_LEN  								256*2+50  	//定义从NB最大接收字节数[实际数据长度=256字节] ("0,192.53.100.53,5683,4,60A041C7,0")
#define CRT_FUN_CNT                   30

/* 结构体声明	--------------------------------------------------------------*/
/* 枚举声明	----------------------------------------------------------------*/
/* 全局变量	----------------------------------------------------------------*/
extern uint16_t USART_RX1_CNT; 
extern uint8_t  USART_RX1_BUF[RX_485_LEN];
extern uint8_t	g_nb_rx_buff[USART_REC_LEN];	//接收到的数据
extern void (*crt_fun[CRT_FUN_CNT])(void);//函数指针数组
extern uint8_t g_crt_run_cnt;
extern uint8_t g_crt_fun_cnt;
extern uint8_t g_ack_flag;
extern uint8_t g_re_cnt;
extern uint8_t g_cail_type;
extern uint8_t g_usart_errortype;
extern uint8_t g_usart_errorvalu;
extern uint8_t g_monitor_flag;
/* 接口提供	----------------------------------------------------------------*/
void USART_485_Config(void);//串口初始化
void USART_DataHandle(void);
int fputc(int ch, FILE *f);
void USART_CONFIG_SEND(uint8_t *p,uint16_t len);
void USART_485_IRQnHandle(void);
void USART_SendOK(void);
void NB_Config(void);
void NB_SendData( uint8_t *p , uint16_t len );
void NB_ReceiveData( void );
void NB_IRQHandle( void );
void NB_HardwareReset( void );							//BC95硬件复位
void NB_OpenPower( void );
void USART_SendCail(void);
void USART_CRT_FunAdd( void fun(void) );
void USART_SendError(void);
void USART_SendMonitorData(void);
void USART_SendSenserSta(void);
void CRT_FunMove(void);
#endif       /*__usart_H end*/

/*------------------------------File----------End------------------------------*/
