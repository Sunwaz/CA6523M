/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-08					�ĵ���ֲ
 *˵    ��:	
*******************************************************************************************/
#ifndef __usart_H
#define __usart_H
/* ͷ�ļ� ------------------------------------------------------------------*/
#include <stm32f0xx.h>
#include "stdio.h"
/* �궨�� ------------------------------------------------------------------*/
#define RX_485_LEN         						0xFF
#define USART_REC_LEN  								256*2+50  	//�����NB�������ֽ���[ʵ�����ݳ���=256�ֽ�] ("0,192.53.100.53,5683,4,60A041C7,0")
#define CRT_FUN_CNT                   30

/* �ṹ������	--------------------------------------------------------------*/
/* ö������	----------------------------------------------------------------*/
/* ȫ�ֱ���	----------------------------------------------------------------*/
extern uint16_t USART_RX1_CNT; 
extern uint8_t  USART_RX1_BUF[RX_485_LEN];
extern uint8_t	g_nb_rx_buff[USART_REC_LEN];	//���յ�������
extern void (*crt_fun[CRT_FUN_CNT])(void);//����ָ������
extern uint8_t g_crt_run_cnt;
extern uint8_t g_crt_fun_cnt;
extern uint8_t g_ack_flag;
extern uint8_t g_re_cnt;
extern uint8_t g_cail_type;
extern uint8_t g_usart_errortype;
extern uint8_t g_usart_errorvalu;
extern uint8_t g_monitor_flag;
/* �ӿ��ṩ	----------------------------------------------------------------*/
void USART_485_Config(void);//���ڳ�ʼ��
void USART_DataHandle(void);
int fputc(int ch, FILE *f);
void USART_CONFIG_SEND(uint8_t *p,uint16_t len);
void USART_485_IRQnHandle(void);
void USART_SendOK(void);
void NB_Config(void);
void NB_SendData( uint8_t *p , uint16_t len );
void NB_ReceiveData( void );
void NB_IRQHandle( void );
void NB_HardwareReset( void );							//BC95Ӳ����λ
void NB_OpenPower( void );
void USART_SendCail(void);
void USART_CRT_FunAdd( void fun(void) );
void USART_SendError(void);
void USART_SendMonitorData(void);
void USART_SendSenserSta(void);
void CRT_FunMove(void);
#endif       /*__usart_H end*/

/*------------------------------File----------End------------------------------*/
