/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-09					�ĵ���д
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "key_led.h"
#include "delay.h"
/* �궨��	--------------------------------------------------------------------*/
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
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
static uint8_t s_open_flag 					= 1;										//LED���Ʊ�־λ     bit0,0,����ָʾ�ƹ��� 1,����ָʾ�ƽ�ֹ
																														//								 bit1,0,�ź�ָʾ�Ʋ����� 1,�ź�ָʾ�ƹ���ָʾ
uint8_t		g_sys_error_flag					=	0;										//ϵͳ���ϱ�־λ    0,�޹���,LED������˸ 1,�й���,LED���ֵ�ǰ״̬
uint8_t		g_reset_flag 							= 0;										//��λ��־
uint16_t	g_sys_operation_msg	      = 0;									  //bit0 �豸��λ bit1 �豸���� bit2 �ֶ����� bit3 �������� bit4 �Լ� bit5 ȷ�� bit6 ���� bit7 SIM�������� bit8 �豸���� bit9 ģ�鱻����
uint32_t  g_alarm_flag 							= 0;										//������־λ
/*
  bit0					bit1					bit2					bit3					bit4					bit5					bit6					bit7					bit8					bit9
��ѹ�� 			����1��			����2��			����3��			�¶�1��			�¶�2��			�¶�3��			©������	  		Ԥ��	    		Ԥ��
  bit10					bit11					bit12					bit13					bit14					bit15					bit16					bit17					bit18					bit19
��ѹ������	����1������	����2������	����3������	�¶�1������	�¶�2������	�¶�3������	©����������	    Ԥ��	    		Ԥ��
  bit20					bit21					bit22					bit23					bit24					bit25					bit26					bit27					bit28					bit29
��ѹ���ϱ�	����1���ϱ�	����2���ϱ�	����3���ϱ�	�¶�1���ϱ�	�¶�2���ϱ�	�¶�3���ϱ�	©�������ϱ�	    Ԥ��	    		Ԥ��
  bit30	        bit31							
 �𾯻ָ�	     ������								
*/
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		������LED �����ʼ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-09				������д
							2018-04-04				�����˷�������ش���
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
	/*1> ERROR LED �Ƶĳ�ʼ��*/
	newGPIO_Init.GPIO_Pin		=	LED_ERROR_PIN;
	RCC_ERROR_LED_CMD(	RCC_ERROR_LED_CLK , ENABLE );
	GPIO_Init(	LED_ERROR_PORT , &newGPIO_Init );
	/*2> ALARM LED �Ƶĳ�ʼ��*/
	newGPIO_Init.GPIO_Pin		=	LED_ALARM_PIN;
	RCC_ALARM_LED_CMD(	RCC_ALARM_LED_CLK , ENABLE );
	GPIO_Init( LED_ALARM_PORT , &newGPIO_Init );
	/*3> WORK LED �Ƶĳ�ʼ��*/
	newGPIO_Init.GPIO_Pin		=	LED_WORK_PIN;
	RCC_WORK_LED_CMD(	RCC_WORK_LED_CLK , ENABLE );
	GPIO_Init( LED_WORK_PORT , &newGPIO_Init );
	/*4> CLEAR LED �Ƶĳ�ʼ��*/
	newGPIO_Init.GPIO_Pin		=	LED_CLEAR_PIN;
	RCC_CLEAR_LED_CMD(	RCC_CLEAR_LED_CLK , ENABLE );
	GPIO_Init( LED_CLEAR_PORT , &newGPIO_Init );
	/*5> SIGNAL LED �Ƶĳ�ʼ��*/
	newGPIO_Init.GPIO_Pin		=	LED_SIGNAL_PIN;
	RCC_SIGNAL_LED_CMD(	RCC_SIGNAL_LED_CLK , ENABLE );
	GPIO_Init( LED_SIGNAL_PORT , &newGPIO_Init );
	/*6> CLEAR KEY �����ĳ�ʼ��*/
	newGPIO_Init.GPIO_Mode	=	GPIO_Mode_IN;
	newGPIO_Init.GPIO_Pin		=	KEY_CLEAR_PIN;
	newGPIO_Init.GPIO_PuPd	=	GPIO_PuPd_DOWN;
	RCC_CLEAR_KEY_CMD(	RCC_CLEAR_KEY_CLK , ENABLE );
	GPIO_Init( KEY_CLEAR_PORT , &newGPIO_Init );
	/*7> RESET KEY �����ĳ�ʼ��*/
	newGPIO_Init.GPIO_Pin		=	KEY_RESET_PIN;
	newGPIO_Init.GPIO_PuPd	=	GPIO_PuPd_DOWN;
	RCC_RESET_KEY_CMD(	RCC_RESET_KEY_CLK , ENABLE );
	GPIO_Init( KEY_RESET_PORT , &newGPIO_Init );
	/*8> ��ʱ����ʼ��*/
	RCC_LED_TIME_CMD( RCC_LED_TIME_CLK , ENABLE );
	TIM_DeInit( LED_TIMER );
	newTim_Init.TIM_Prescaler     = 999;									//��ʱ�����ֵ ���ü��������С��ÿ��1�����Ͳ���һ�������¼�10us
	newTim_Init.TIM_Period        = 96000-1;							//38-96Ԥ��Ƶϵ��Ϊ480-1������������ʱ��Ϊ48MHz/480 = 100kHz
	newTim_Init.TIM_ClockDivision =TIM_CKD_DIV1;
	newTim_Init.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInit( LED_TIMER , &newTim_Init );				//20us�ж�һ��
	TIM_ITConfig( LED_TIMER , TIM_IT_Update,ENABLE );		//��������ж�
	TIM_ClearFlag( LED_TIMER , TIM_FLAG_Update );				//����жϱ�־
	TIM_ARRPreloadConfig( LED_TIMER , DISABLE );				//��ֹԤװ������
	/*9> ��ʱ���ж�����*/
	NVIC_SetPriority( LED_TIMER_IRQ , 1);										//�������
	newNVIC_Init.NVIC_IRQChannel         = LED_TIMER_IRQ;		//TIM3�ж�
	newNVIC_Init.NVIC_IRQChannelPriority = 5;						//��ռ���ȼ�3
	newNVIC_Init.NVIC_IRQChannelCmd      = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&newNVIC_Init);
	TIM_Cmd( LED_TIMER , ENABLE );											//��ʱ��ʹ��
	/*10> �������ĳ�ʼ��*/
	RCC_BEEP_CMD( RCC_BEEP_CLK , ENABLE);
	newGPIO_Init.GPIO_Mode  = GPIO_Mode_OUT;
	newGPIO_Init.GPIO_Pin		=	BEEP_PIN;
  GPIO_Init( BEEP_PORT , &newGPIO_Init );
		
	BEEP_OPEN;
	delay_ms(100);
	BEEP_CLOS;
}
/*****************************************************************************
 * ��������:		��ʱ���жϷ�����
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-09				������д
 ****************************************************************************/ 
void LED_TimerIrqHandle( void )
{
	static uint32_t s_tim_cnt = 0;					//��˸����
	
	if( TIM_GetITStatus( LED_TIMER , TIM_IT_Update ) != RESET )
	{
		if( !g_sys_error_flag )
		{
			s_tim_cnt++;
			GPIO_WriteBit( LED_WORK_PORT	, LED_WORK_PIN   , (BitAction)((!(((s_open_flag & 0x01) << 1) & (s_tim_cnt & 0xFF)))&0x01));
			GPIO_WriteBit( BEEP_PORT			, BEEP_PIN       , (BitAction)( (((s_open_flag & 0x04) >> 0x02) & (s_tim_cnt & 0xFF)) & 0x01));
		}else
		{
			GPIO_WriteBit( BEEP_PORT , BEEP_PIN , Bit_RESET );											//�������,�ص�������
		}
		TIM_ClearITPendingBit( LED_TIMER , TIM_IT_Update );
	}
}

/*****************************************************************************
 * ��������:		�򿪷�����
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-08				������д
 ****************************************************************************/ 
void Alarm_SetBit( void )
{
	s_open_flag |= 0x04;
}

/*****************************************************************************
 * ��������:		�رշ�����
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-08				������д
 ****************************************************************************/ 
void Alarm_ResetBit( void )
{
	s_open_flag &=~ 0x04;
}
/*****************************************************************************
 * ��������:		LED�ƿ��ƺ���
 * ��ʽ����:		Led �����Ƶ�LED State ���Ƶ�״̬
 * ���ز���:		��
 * ��������;		2018-03-09				������д
 ****************************************************************************/ 
void LED_Control( LED_Typedef Led , LED_Ctl_Typedef State )
{
	BitAction led_stat = Bit_RESET;					//LED�Ŀ���״̬

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
 * ��������:		ͨѶ���Ͽ���
 * ��ʽ����:		State ���Ƶ�״̬
 * ���ز���:		��
 * ��������;		2018-03-09				������д
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
 * ��������:		��������
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-12				������д
 ****************************************************************************/
void Alarm_Set( void )
{
	Alarm_SetBit();
	LED_Control( L_ALARM , OPEN );
}
/*****************************************************************************
 * ��������:		��������
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-12				������д
 ****************************************************************************/
void Alarm_Reset( void )
{
	Alarm_ResetBit();
	LED_Control( L_ALARM , CLOS );
	LED_Control( L_CLEAR , CLOS );
	BEEP_CLOS;								     //�ص�������
}
/*****************************************************************************
 * ��������:		��������
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-12				������д
 ****************************************************************************/
void Key_Scan( void )
{
	uint16_t temp = 0;						//�м����
	uint8_t  flag = 0;						//��־      �����λ��������������λ
	
	if(1){											//��λ����
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
			if((temp > 0) && (temp < 0x50))		//�̰�,��λ�𾯹���
			{
				for(temp = 1;temp < 0x400;temp <<= 1)//��������
				{
					if((g_alarm_flag & (temp << LOCAL_ALARM)) || (g_alarm_flag & (temp << ALARM_UPLOAD)))
					{
						flag = 1;
						break;
					}
				}
				if(flag)//�л𾯲Ž��л𾯸�λ
				{
					g_alarm_flag = 0;               //״̬��λ
					g_sys_operation_msg |= 0x08;    //�������
					Alarm_Reset();							    //��ֹ��������������
				}
			}else if( (temp >= 0x50) && (temp <= 0xD0) )//����,��λ�豸
			{
				g_reset_flag = 0x0F;
				g_sys_operation_msg |= 0x01;
			}
		}
  }
	if(1){											//��������
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
			if((temp > 0) && (temp < 0x50))		//�̰�,��������
			{
				if(s_open_flag & 0x04)				//����������
				{
					for(temp = 1;temp < 0x400;temp <<= 1)//��������
					{
						if((g_alarm_flag & (temp << LOCAL_ALARM)) || (g_alarm_flag & (temp << ALARM_UPLOAD)))
						{
							g_alarm_flag |= (temp << ALARM_SILENCE);
						}
					}
					g_sys_operation_msg |= 0x02;  //����
					Alarm_ResetBit();							//��ֹ��������������
					BEEP_CLOS;								    //�ص�������
					LED_Control( L_CLEAR , OPEN );
				}
			}else if( (temp >= 0x50) && (temp <= 0xD0) )//����
			{
			}
		}
	}
}
/*------------------------------File----------End------------------------------*/
