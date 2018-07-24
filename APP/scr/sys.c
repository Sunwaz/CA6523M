/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-12					�ĵ���д
						2018-03-13					BC95�������
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "sys.h"
#include "usart.h"
#include "BC95.h"
#include "delay.h"
#include "rtc.h"
#include "nbiot.h"
#include "adc_get.h"
#include "key_led.h"
#include "timer.h"
#include "app.h"
#include "configure.h"
/* �궨��	--------------------------------------------------------------------*/
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		�����ʼ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-14				������д
 ****************************************************************************/ 
void Network_Config( void )
{
	network_parameter_flashRead();		 //��ȡ��������
  NB_Config();
	while(RTC_Config())
	{
		printf("RTC ERROR!\r\n ");	
		delay_ms(500);
	}
	Nbiot_Start();
	delay_ms(500);
	Init_app();
}
/*****************************************************************************
 * ��������:		�������Ź���ʼ��
 * ��ʽ����:		��
 * ���ز���:		��
 ****************************************************************************/
static void IWDG_Config(void)
{
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
			RCC_ClearFlag();                             //�����־
	}
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //��������IWDG_PR��IWDG_RLR�Ĵ���
	IWDG_SetPrescaler(IWDG_Prescaler_256);         //��Ƶֵ
	IWDG_SetReload(0x0fff);                        //
	IWDG_ReloadCounter();                          //ʹ������ֵo
	IWDG_Enable();                                 //ʹ�ܿ��Ź�
}
/*****************************************************************************
 * ��������:		ϵͳ��ʼ������
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-12				������д
							2018-03-14				��������
							2018-04-18				ʱ���л��������ǰ��(ADC��ʼ�����棬�ر�HSE��LSE)
							2018-06-11				�����Ż�;��Timer_Config�ƶ���Network_Config֮ǰ
 ****************************************************************************/ 
void Sys_Init(void) 
{
	ADC_GetConfig();
	delay_init();
	USART_485_Config();
	delay_ms(200);
	printf("�����汾��:%s\r\n " , SYS_VER);
	Timer_Config();
	Network_Config();
	KeyLED_Config();
	IWDG_Config();
	IWDG_ReloadCounter();                //ι��
}
/*------------------------------File----------End------------------------------*/