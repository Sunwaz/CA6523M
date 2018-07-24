/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-14					�ĵ���д
						2018-03-29					��ʱ�������ı�д
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "timer.h"
/* �궨��	--------------------------------------------------------------------*/
#define RCC_TIM_CMD																RCC_APB1PeriphClockCmd
#define RCC_TIM_CLK																RCC_APB1Periph_TIM3
#define TIM_COM																		TIM3
#define TIM_IRQn																	TIM3_IRQn

#define RCC_TIM_SAMP_CMD													RCC_APB2PeriphClockCmd
#define RCC_TIM_SAMP_CLK													RCC_APB2Periph_TIM15
#define TIM_SAMP_COM															TIM15
#define TIM_SAMP_IRQn															TIM15_IRQn


#define TIM_FREQ_MS																1000												//1KHz ms
#define TIM_COUNT																	500													//500ms
#define TIM_FREQ_S																1													  //1Hz s
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
uint32_t g_sys_tim_s = 0;																		 //��ʱ����ʱ
uint8_t g_adc_get_flag = 0;																	 //ADC�ɼ���־
uint8_t NET_outtime = 0;																	  //���������糬ʱ
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		��ʱ����ʼ��(��λ 500ms)
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-29				������д
 ****************************************************************************/
void Timer_Config( void )
{
	TIM_TimeBaseInitTypeDef		newTIM_Init;
	NVIC_InitTypeDef					newNVIC_Init;
	
	newTIM_Init.TIM_ClockDivision	=	TIM_CKD_DIV1;
	newTIM_Init.TIM_CounterMode		=	TIM_CounterMode_Up;
	newTIM_Init.TIM_Period				=	SystemCoreClock / TIM_FREQ_MS -1;	//1KHz  48000��Ƶ
	newTIM_Init.TIM_Prescaler			=	TIM_COUNT-1;										//100ms
	
	/*1> ��ʱ�� ����*/	
	TIM_DeInit( TIM_COM );
	RCC_TIM_CMD( RCC_TIM_CLK , ENABLE );
	TIM_TimeBaseInit(TIM_COM , &newTIM_Init );
	TIM_ITConfig(TIM_COM , TIM_IT_Update , ENABLE );	//��������ж�
	TIM_ClearFlag(TIM_COM , TIM_FLAG_Update );				//����жϱ�־
	TIM_ARRPreloadConfig(TIM_COM , DISABLE );				  //��ֹԤװ������
	
	newNVIC_Init.NVIC_IRQChannelCmd      = ENABLE;
	/*5> �ж� ����*/	
	newNVIC_Init.NVIC_IRQChannel				 = TIM_IRQn;
	newNVIC_Init.NVIC_IRQChannelPriority = 2;
	NVIC_SetPriority( TIM_IRQn	, 1);
	NVIC_Init( &newNVIC_Init );
	
	TIM_Cmd(TIM_COM , ENABLE);											//��ʱ��ʹ��
}

/*****************************************************************************
 * ��������:		��ʱ����(��λ s)
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-10				������д
 ****************************************************************************/
void Sampling_TimerConfig( uint16_t time )
{
	TIM_TimeBaseInitTypeDef		newTIM_Init;
	NVIC_InitTypeDef					newNVIC_Init;
	
	newTIM_Init.TIM_ClockDivision	=	TIM_CKD_DIV1;
	newTIM_Init.TIM_CounterMode		=	TIM_CounterMode_Up;
	newTIM_Init.TIM_Period				=	SystemCoreClock / TIM_FREQ_S -1;	//1KHz  48000��Ƶ
	newTIM_Init.TIM_Prescaler			=	time - 1;										//100ms
	
	/*1> ��ʱ�� ����*/	
	TIM_DeInit( TIM_SAMP_COM );
	RCC_TIM_SAMP_CMD( RCC_TIM_SAMP_CLK , ENABLE );
	TIM_TimeBaseInit(TIM_SAMP_COM , &newTIM_Init );
	TIM_ITConfig(TIM_SAMP_COM , TIM_IT_Update , ENABLE );	//��������ж�
	TIM_ClearFlag(TIM_SAMP_COM , TIM_FLAG_Update );				//����жϱ�־
	TIM_ARRPreloadConfig(TIM_SAMP_COM , DISABLE );				  //��ֹԤװ������
	
	newNVIC_Init.NVIC_IRQChannelCmd      = ENABLE;
	/*5> �ж� ����*/	
	newNVIC_Init.NVIC_IRQChannel				 = TIM_SAMP_IRQn;
	newNVIC_Init.NVIC_IRQChannelPriority = 2;
	NVIC_SetPriority( TIM_SAMP_IRQn	, 1);
	NVIC_Init( &newNVIC_Init );
	
	TIM_Cmd(TIM_SAMP_COM , ENABLE);											//��ʱ��ʹ��
}
/*****************************************************************************
 * ��������:		��ʱ���жϷ�����(500ms)
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-29				������д
 ****************************************************************************/
void Sys_TimerIRQnHandle( void )
{
	static uint8_t time_num =0;

	if( TIM_GetITStatus( TIM_COM , TIM_IT_Update ) != RESET )
	{
		if(time_num<240)time_num++;
		else time_num=0;
		if((time_num %2)==0)  
		{
			if(NET_outtime>0)NET_outtime--;//�������ȴ���ʱʱ��
			g_sys_tim_s++;
		}
		TIM_ClearITPendingBit( TIM_COM , TIM_IT_Update );
	}
}

/*****************************************************************************
 * ��������:		��ʱ���жϷ�����(500ms)
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-10				������д
 ****************************************************************************/
void ADC_TimerIRQnHandle( void )
{
	if( TIM_GetITStatus( TIM_SAMP_COM , TIM_IT_Update ) != RESET )
	{
		if(!g_adc_get_flag)g_adc_get_flag = 1;														//����adc�ɼ�
		TIM_ClearITPendingBit( TIM_SAMP_COM , TIM_IT_Update );
	}
}
/*------------------------------File----------End------------------------------*/
