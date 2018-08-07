/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-14					�ĵ���д
						2018-03-15					ADC����ĵ���(��ѹ�ɼ�,�����ɼ�)
						2018-03-16					ADC����ĵ���(�¶Ȳɼ�)
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "adc_get.h"
#include "delay.h"
#include "usart.h"
#include "configure.h"
#include "com.h"
#include "key_led.h"
#include "timer.h"

#include "string.h"
/* �궨��	--------------------------------------------------------------------*/
#define RCC_CD4051_CS_GPIO_CMD										RCC_AHBPeriphClockCmd
#define RCC_CD4051_CS_GPIO_CLK										RCC_AHBPeriph_GPIOF
#define CD4051_CS_PORT														GPIOF
#define CD4051_CS_PIN															GPIO_Pin_1
#define CD4051_CS_DISABLE													GPIO_SetBits(   CD4051_CS_PORT , CD4051_CS_PIN )
#define CD4051_CS_ENABLE													GPIO_ResetBits( CD4051_CS_PORT , CD4051_CS_PIN )

#define RCC_CD4051_C_GPIO_CMD											RCC_AHBPeriphClockCmd
#define RCC_CD4051_C_GPIO_CLK											RCC_AHBPeriph_GPIOF
#define CD4051_C_PORT															GPIOF
#define CD4051_C_PIN															GPIO_Pin_0
#define CD4051_C_H																GPIO_SetBits(   CD4051_C_PORT , CD4051_C_PIN )
#define CD4051_C_L																GPIO_ResetBits( CD4051_C_PORT , CD4051_C_PIN )

#define RCC_CD4051_B_GPIO_CMD											RCC_AHBPeriphClockCmd
#define RCC_CD4051_B_GPIO_CLK											RCC_AHBPeriph_GPIOC
#define CD4051_B_PORT															GPIOC
#define CD4051_B_PIN															GPIO_Pin_15
#define CD4051_B_H																GPIO_SetBits(   CD4051_B_PORT , CD4051_B_PIN )
#define CD4051_B_L																GPIO_ResetBits( CD4051_B_PORT , CD4051_B_PIN )

#define RCC_CD4051_A_GPIO_CMD											RCC_AHBPeriphClockCmd
#define RCC_CD4051_A_GPIO_CLK											RCC_AHBPeriph_GPIOC
#define CD4051_A_PORT															GPIOC
#define CD4051_A_PIN															GPIO_Pin_14
#define CD4051_A_H																GPIO_SetBits(   CD4051_A_PORT , CD4051_A_PIN )
#define CD4051_A_L																GPIO_ResetBits( CD4051_A_PORT , CD4051_A_PIN )

#define EN_CD4051_CH0															{CD4051_CS_ENABLE;CD4051_C_L;CD4051_B_L;CD4051_A_L;}
#define EN_CD4051_CH1															{CD4051_CS_ENABLE;CD4051_C_L;CD4051_B_L;CD4051_A_H;}
#define EN_CD4051_CH2														  {CD4051_CS_ENABLE;CD4051_C_L;CD4051_B_H;CD4051_A_L;}
#define EN_CD4051_CH3														  {CD4051_CS_ENABLE;CD4051_C_L;CD4051_B_H;CD4051_A_H;}
#define EN_CD4051_CH4														  {CD4051_CS_ENABLE;CD4051_C_H;CD4051_B_L;CD4051_A_L;}
#define EN_CD4051_CH5														  {CD4051_CS_ENABLE;CD4051_C_H;CD4051_B_L;CD4051_A_H;}
#define EN_CD4051_CH6														  {CD4051_CS_ENABLE;CD4051_C_H;CD4051_B_H;CD4051_A_L;}
#define EN_CD4051_CH7														  {CD4051_CS_ENABLE;CD4051_C_H;CD4051_B_H;CD4051_A_H;}
#define DIS_CD4051																{CD4051_CS_DISABLE;}

#define RCC_CD4051_ADC_GPIO_CMD										RCC_AHBPeriphClockCmd
#define RCC_CD4051_ADC_GPIO_CLK										RCC_AHBPeriph_GPIOA
#define CD4051_ADC_PORT														GPIOA
#define CD4051_ADC_PIN														GPIO_Pin_7

#ifndef NEW_MODEL																																//�ϰ���
#define RCC_TEMP2_ADC_GPIO_CMD										RCC_AHBPeriphClockCmd
#define RCC_TEMP2_ADC_GPIO_CLK										RCC_AHBPeriph_GPIOA
#define TEMP2_ADC_PORT														GPIOA
#define TEMP2_ADC_PIN															GPIO_Pin_6

#define RCC_TEMP1_ADC_GPIO_CMD										RCC_AHBPeriphClockCmd
#define RCC_TEMP1_ADC_GPIO_CLK										RCC_AHBPeriph_GPIOB
#define TEMP1_ADC_PORT														GPIOB
#define TEMP1_ADC_PIN															GPIO_Pin_1

#define RCC_FLOOR_ELE_GPIO_CMD										RCC_AHBPeriphClockCmd
#define RCC_FLOOR_ELE_GPIO_CLK										RCC_AHBPeriph_GPIOA
#define FLOOR_ELE1_ADC_PORT												GPIOA
#define FLOOR_ELE1_ADC_PIN												GPIO_Pin_5

#define ADC_CD4051_ADC_CH													ADC_Channel_7
#define ADC_TEMP1_ADC_CH													ADC_Channel_9
#define ADC_TEMP2_ADC_CH													ADC_Channel_6
#define ADC_FLOOR_ELE1_ADC_CH											ADC_Channel_5

#else																																					//�°���
#define RCC_TEMP1_ADC_GPIO_CMD										RCC_AHBPeriphClockCmd
#define RCC_TEMP1_ADC_GPIO_CLK										RCC_AHBPeriph_GPIOA
#define TEMP1_ADC_PORT														GPIOA
#define TEMP1_ADC_PIN															GPIO_Pin_6

#define RCC_TEMP2_ADC_GPIO_CMD										RCC_AHBPeriphClockCmd
#define RCC_TEMP2_ADC_GPIO_CLK										RCC_AHBPeriph_GPIOA
#define TEMP2_ADC_PORT														GPIOA
#define TEMP2_ADC_PIN															GPIO_Pin_5

#define RCC_TEMP3_ADC_GPIO_CMD										RCC_AHBPeriphClockCmd
#define RCC_TEMP3_ADC_GPIO_CLK										RCC_AHBPeriph_GPIOA
#define TEMP3_ADC_PORT														GPIOA
#define TEMP3_ADC_PIN															GPIO_Pin_4

#define RCC_FLOOR_ELE_GPIO_CMD										RCC_AHBPeriphClockCmd
#define RCC_FLOOR_ELE_GPIO_CLK										RCC_AHBPeriph_GPIOB
#define FLOOR_ELE1_ADC_PORT												GPIOB
#define FLOOR_ELE1_ADC_PIN												GPIO_Pin_1

#define ADC_CD4051_ADC_CH													ADC_Channel_7
#define ADC_TEMP1_ADC_CH													ADC_Channel_6
#define ADC_TEMP2_ADC_CH													ADC_Channel_5
#define ADC_TEMP3_ADC_CH													ADC_Channel_4
#define ADC_FLOOR_ELE1_ADC_CH											ADC_Channel_9
#endif



#define RCC_SENSER_CHECK_GPIO_CMD									RCC_AHBPeriphClockCmd
#define RCC_SENSER_CHECK_GPIO_CLK									RCC_AHBPeriph_GPIOB
#define SENSER_CHECK_PORT													GPIOB
#define SENSER_CHECK_PIN													GPIO_Pin_0

#define SENSER_CHECK_OPEN													GPIO_ResetBits( SENSER_CHECK_PORT , SENSER_CHECK_PIN )
#define SENSER_CHECK_CLOS													GPIO_SetBits(	  SENSER_CHECK_PORT , SENSER_CHECK_PIN )

#define RCC_ADC_CMD																RCC_APB2PeriphClockCmd
#define RCC_ADC_CLK																RCC_APB2Periph_ADC1
#define ADC_COM																		ADC1

#define RCC_ADC_DMA_CMD														RCC_AHBPeriphClockCmd
#define RCC_ADC_DMA_CLK														RCC_AHBPeriph_DMA1
#define ADC_GET_DMA_COM														DMA1_Channel1
#define ADC_DMA_IRQn															DMA1_Channel1_IRQn



#define ADC_SAMPLING_CNT													1024												//ADC��������
#define ADC_SAMPLING_MOVE													10													//2<<10=1024

#define TEMP3_ADC_NUM															0														//�¶�2��ADC��ͨ��
#define TEMP2_ADC_NUM															4														//�¶�2��ADC��ͨ��
#define TEMP1_ADC_NUM															3														//�¶�1��ADC��ͨ��
#define CD4051_ADC_NUM														2														//4051��ADC��ͨ��
#define FLOOR_ADC_NUM															1														//©������ADC��ͨ��

//s_adc_sta ��ز���
#define READ_VOLAT_STA                            0x0000000F
#define SET_VOLAT_STA                             0x00
#define READ_CURR_1_STA                           0x000000F0
#define SET_CURR_1_STA                            0x04
#define READ_CURR_2_STA                           0x00000F00									//Ԥ��
#define SET_CURR_2_STA                            0x08
#define READ_CURR_3_STA                           0x0000F000									//Ԥ��
#define SET_CURR_3_STA														0x0C
#define READ_TEMP_1_STA														0x000F0000
#define SET_TEMP_1_STA														0x10
#define READ_TEMP_2_STA														0x00F00000									//Ԥ��
#define SET_TEMP_2_STA														0x14
#define READ_TEMP_3_STA														0x0F000000									//Ԥ��
#define SET_TEMP_3_STA														0x18
#define READ_SY_CURR_STA													0xF0000000
#define SET_SY_CURR_STA														0x1C




/* �ṹ�嶨��	----------------------------------------------------------------*/
typedef enum{																																	//CD4051ͨ��
	CHANNEL_0 = 0,																															//����b ��ͨ��
	CHANNEL_1 = 1,																															//����c ��ͨ��
	CHANNEL_2 = 2,																															//��ѹa ��ͨ��
	CHANNEL_3 = 3,																															//����a ��ͨ��
	CHANNEL_4 = 4,																															//��ѹb ��ͨ��
	CHANNEL_5 = 5,																															//��ѹc ��ͨ��
	CHANNEL_6 = 6,																															//����
	CHANNEL_7 = 7,																															//����
	CHANNEL_NULL = 8,																														//��
}CD4051_CHANNEL;
typedef struct{																																//adc״̬�ṹ��
	int16_t		value;																														//ADC��ֵ
	int16_t		threa;																														//ADC����ֵ
	uint32_t	alarm_flag;																												//ADC������־
	uint32_t	read_flag;																												//ADC��ȡ����λ
	uint32_t	read_sta;																													//ADC��ȡ״̬	
	uint8_t		move_sta;																													//ADC״̬λ
}adc_sta_typedef;
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
static uint16_t s_temp_table[72] ={																						//�¶ȷ�Χ��:-50 ~ 305
//-50      -45			-40				-35			 -30				-25			-20				-15			
	8031,   8229,		8427,			8625,		 8822,		  9019,		9216,			9412,
//-10      -5			 0				 5			  10				 15			 20				 25			 
	9609,	  9804,		10000,		10195,	 10390,		  10585,	10779,		10973,	
//30       35			 40				 45			  50				 55			 60				 65			 
	11167,  11361,	11554,		11747,	 11940,		  12132,	 12324,		12516,	
//70       75			 80				 85			  90				 95			100				105			
	12708,  12899,  13090,	  13280,	 13471,	    13661,	 13851,	  14040,	
//110      115			120				125			 130				135			140				145			
	14229,  14418,	14607,		14795,	 14983,		  15171,	 15358,		15546,	
//150      155			160				165				170				175			180				185				
	15733,  15919,	16105,		16291,	 16477,		  16663,	 16848,		17033,	
//190      195     200				205				210				215			220				225			
	17217,  17402,	17586,		17769,	 17953,		  18136,	 18319,		18501,	
//230      235     240				245				250				255			260				265				
	18684,  18866,	19047,		19229,	 19410,		  19591,	 19771,		19951,	
//270      275     280				285				290				295			300				305
	20131,  20311,	20490,		20670,	 20848,		  21027,	 21205,		21383
};
static uint8_t	s_adc_num       = 0;																					//adc���
static uint8_t	s_adc_collect_flag = 0;																				//adc�ɼ���־
static uint16_t s_ADC_Value[5] = {0};																					//ADC��ֵ
static uint32_t s_adc_sta     = 0;																						//0-3bit(0-2bit����,3bit��־λ)��ѹ 4-7bit����1 8-11bit��·2 12-15bit����3 16-19bit�¶�1 20-23bit�¶�2 24-27bit�¶�3 28-31bit©����
static uint32_t s_curr1_max		= 0;																						//�������ֵ
static uint32_t s_curr1_min    = 0;																						//������Сֵ
static uint32_t s_curr2_max		= 0;																						//�������ֵ
static uint32_t s_curr2_min    = 0;																						//������Сֵ
static uint32_t s_curr3_max		= 0;																						//�������ֵ
static uint32_t s_curr3_min    = 0;																						//������Сֵ
static uint32_t s_volat_max   = 0;																						//��ѹ���ֵ
static uint32_t s_volat_min   = 0;																						//��ѹ��Сֵ
static uint16_t s_camp_cnt    = 0;																						//adc��������
static uint16_t s_old_adc     = 0;
adc_cail				g_cail_data;																							    //У׼����
Msg_data				g_coll_data[ADC_SIZE];	                                      //�ɼ�����������
uint8_t					g_early_alarm = 0;												                    //Ԥ����־λ
int32_t					g_camp_value = 0;																					    //����ֵ
uint32_t        g_senser_flag = 0;										                        //���ϱ�־λ
/*
  bit0					bit1					bit2					bit3					bit4					bit5					bit6					bit7					bit8					bit9
��ѹ����			  ����1����			����2����			����3����			�¶�1����			�¶�2����			�¶�3����			©��������	  		Ԥ��	    		Ԥ��
  bit10					bit11					bit12					bit13					bit14					bit15					bit16					bit17					bit18					bit19
��ѹ�������� 	����1��������	����2��������	����3��������	�¶�1��������	�¶�2��������	�¶�3��������	©������������	  Ԥ��	    		Ԥ��
  bit20					bit21					bit22					bit23					bit24					bit25					bit26					bit27					bit28					bit29
��ѹ�����ϱ�	  ����1�����ϱ�	����2�����ϱ�	����3�����ϱ�	�¶�1�����ϱ�	�¶�2�����ϱ�	�¶�3�����ϱ�	©���������ϱ�	  Ԥ��	    		Ԥ��
  bit30	        bit31							
   Ԥ��	        Ԥ��							
*/
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		CD4051��ʼ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-14				������д
 ****************************************************************************/
static void CD4051_Config( void )
{
	GPIO_InitTypeDef		newGPIO_Init;
	
	RCC_HSEConfig( RCC_HSE_OFF );
	RCC_LSEConfig( RCC_LSE_OFF );							//���� HSE �� LSE ���ܵ�GPIO,Ҫ��ʱ����ع��ܹر�
	
	newGPIO_Init.GPIO_Mode	=	GPIO_Mode_OUT;
	newGPIO_Init.GPIO_OType	=	GPIO_OType_OD;
	newGPIO_Init.GPIO_PuPd	=	GPIO_PuPd_DOWN;
	newGPIO_Init.GPIO_Speed	=	GPIO_Speed_50MHz;
	
	/*1> CS �˿ڳ�ʼ��*/
	RCC_CD4051_CS_GPIO_CMD( RCC_CD4051_CS_GPIO_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin		= CD4051_CS_PIN;
	GPIO_Init( CD4051_CS_PORT , &newGPIO_Init );
	
	/*2> C �˿ڳ�ʼ��*/
	RCC_CD4051_C_GPIO_CMD( RCC_CD4051_C_GPIO_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin		=	CD4051_C_PIN;
	GPIO_Init( CD4051_C_PORT , &newGPIO_Init );
	
	/*3> B �˿ڳ�ʼ��*/
	RCC_CD4051_B_GPIO_CMD( RCC_CD4051_B_GPIO_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin		=	CD4051_B_PIN;
	GPIO_Init( CD4051_B_PORT , &newGPIO_Init );
	
	/*4> A �˿ڳ�ʼ��*/
	RCC_CD4051_A_GPIO_CMD( RCC_CD4051_A_GPIO_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin		=	CD4051_A_PIN;
	GPIO_Init( CD4051_A_PORT , &newGPIO_Init );
	
}
/*****************************************************************************
 * ��������:		ADC��ʼ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-15				������д
 ****************************************************************************/
static void ADC_Config( void )
{
	ADC_InitTypeDef     ADC_InitStruct;
	DMA_InitTypeDef     DMA_InitStruct;
	GPIO_InitTypeDef    GPIO_InitStruct;
	NVIC_InitTypeDef		newNVIC_Init;

	/*1> ʱ�ӵĳ�ʼ��*/
	ADC_DeInit( ADC_COM );        
	RCC_ADC_CMD( RCC_ADC_CLK , ENABLE);  
	RCC_ADC_DMA_CMD( RCC_ADC_DMA_CLK , ENABLE ); 
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);  //48M/4=6M
 
	GPIO_InitStruct.GPIO_Mode		= GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd		= GPIO_PuPd_NOPULL ;
	
	/*2.1> CD4051 �˿ڵ�����*/
	GPIO_InitStruct.GPIO_Pin		= CD4051_ADC_PIN;
	RCC_CD4051_ADC_GPIO_CMD( RCC_CD4051_ADC_GPIO_CLK , ENABLE );
	GPIO_Init( CD4051_ADC_PORT, &GPIO_InitStruct );            
	/*2.2> Temp1	�˿�����*/
	GPIO_InitStruct.GPIO_Pin		= TEMP1_ADC_PIN;
	RCC_TEMP1_ADC_GPIO_CMD( RCC_TEMP1_ADC_GPIO_CLK , ENABLE );
	GPIO_Init( TEMP1_ADC_PORT , &GPIO_InitStruct );
	/*2.3> Floor ele2 �˿�����*/
	GPIO_InitStruct.GPIO_Pin		=	FLOOR_ELE1_ADC_PIN;
	RCC_FLOOR_ELE_GPIO_CMD( RCC_FLOOR_ELE_GPIO_CLK , ENABLE );
	GPIO_Init( FLOOR_ELE1_ADC_PORT , &GPIO_InitStruct );
	/*2.4> Floor ele1 �˿�����*/
	/*2.5> Temp2   �˿�����*/
	GPIO_InitStruct.GPIO_Pin    = TEMP2_ADC_PIN;
	RCC_TEMP2_ADC_GPIO_CMD(RCC_TEMP2_ADC_GPIO_CLK , ENABLE);
	GPIO_Init( TEMP2_ADC_PORT , &GPIO_InitStruct );
	/*2.5> Temp3   �˿�����*/
	#ifdef NEW_MODEL
	GPIO_InitStruct.GPIO_Pin    = TEMP3_ADC_PIN;
	RCC_TEMP3_ADC_GPIO_CMD(RCC_TEMP3_ADC_GPIO_CLK , ENABLE);
	GPIO_Init( TEMP3_ADC_PORT , &GPIO_InitStruct );
	
	GPIO_InitStruct.GPIO_Mode		=	GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd		= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	/*2.6> ���������˿�����*/
	GPIO_InitStruct.GPIO_Pin		=	SENSER_CHECK_PIN;
	RCC_SENSER_CHECK_GPIO_CMD( RCC_SENSER_CHECK_GPIO_CLK , ENABLE );
	GPIO_Init( SENSER_CHECK_PORT , &GPIO_InitStruct );
	SENSER_CHECK_CLOS;
	#endif
	DMA_DeInit( ADC_GET_DMA_COM );

	DMA_InitStruct.DMA_DIR							  = DMA_DIR_PeripheralSRC;                                                                                
	DMA_InitStruct.DMA_PeripheralInc			= DMA_PeripheralInc_Disable;                       
	DMA_InitStruct.DMA_MemoryInc				  = DMA_MemoryInc_Enable;                                                        
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize		  = DMA_MemoryDataSize_HalfWord;       
	DMA_InitStruct.DMA_Mode								= DMA_Mode_Circular;                                                                                       
	DMA_InitStruct.DMA_Priority						= DMA_Priority_High;                                                                       
	DMA_InitStruct.DMA_M2M							  = DMA_M2M_Disable;                                                                                                        
  
	/*3> DMA ����*/
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(ADC_COM->DR);  
	DMA_InitStruct.DMA_MemoryBaseAddr			= (uint32_t)s_ADC_Value;
	DMA_InitStruct.DMA_BufferSize					= 5;
	DMA_Init( ADC_GET_DMA_COM , &DMA_InitStruct);

	DMA_Cmd( ADC_GET_DMA_COM , ENABLE);                                                                                                                                               
	DMA_ITConfig( ADC_GET_DMA_COM ,DMA_IT_TC,ENABLE);                                                                                       
	ADC_DMARequestModeConfig( ADC_COM , ADC_DMAMode_Circular);      				
	ADC_DMACmd( ADC_COM , ENABLE);
         
	ADC_StructInit(&ADC_InitStruct);
         
	ADC_InitStruct.ADC_Resolution							= ADC_Resolution_12b;
	ADC_InitStruct.ADC_ExternalTrigConvEdge	  = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_DataAlign							= ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ScanDirection					= ADC_ScanDirection_Backward;
 	
	/*4> ADC ����*/	
	ADC_InitStruct.ADC_ContinuousConvMode  		= ENABLE;
	ADC_Init( ADC_COM, &ADC_InitStruct);  
	ADC_ChannelConfig( ADC_COM , ADC_CD4051_ADC_CH     , ADC_SampleTime_239_5Cycles );               
	ADC_ChannelConfig( ADC_COM , ADC_TEMP1_ADC_CH      , ADC_SampleTime_239_5Cycles );
  ADC_ChannelConfig( ADC_COM , ADC_TEMP2_ADC_CH      , ADC_SampleTime_239_5Cycles );	
	#ifdef NEW_MODEL
  ADC_ChannelConfig( ADC_COM , ADC_TEMP3_ADC_CH      , ADC_SampleTime_239_5Cycles );	
	#endif
	ADC_ChannelConfig( ADC_COM , ADC_FLOOR_ELE1_ADC_CH , ADC_SampleTime_239_5Cycles );

	ADC_VrefintCmd( ENABLE );																																//ʹ�ܻ�׼��ѹ���
	ADC_GetCalibrationFactor( ADC_COM ); 																										//adcУ��
	
	ADC_Cmd( ADC_COM , ENABLE);     
 
	while(!ADC_GetFlagStatus( ADC_COM , ADC_FLAG_ADRDY)){}; 	
		
	newNVIC_Init.NVIC_IRQChannelCmd      = ENABLE;
	/*5> �ж� ����*/	
	newNVIC_Init.NVIC_IRQChannel				 = ADC_DMA_IRQn;
	newNVIC_Init.NVIC_IRQChannelPriority = 1;
	NVIC_SetPriority( ADC_DMA_IRQn	, 1);
	NVIC_Init( &newNVIC_Init );
	 
  ADC_StartOfConversion( ADC_COM );		
}

/*****************************************************************************
 * ��������:		ADC��ʼ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-15				������д
 ****************************************************************************/
void ADC_GetConfig( void )
{
	CD4051_Config();
	ADC_Config();
}

/*****************************************************************************
 * ��������:		�¶�ֵ��ȡ
 * ��ʽ����:		temp_adc �¶ȵ�adcֵ
 * ���ز���:		�¶�ֵ
 * ��������;		2018-03-16				������д
							2018-03-19				�㷨�ĸ���(��ǰ�㷨������)
							2018-03-28				��������bug
 ****************************************************************************/
static float ADC_GetTemp( uint16_t temp_adc )
{
	double resis = 0.0;
	float vin   = 0.0;
	uint16_t temp = 0;
	uint8_t i   = 0;
	float temp_l;
	/*
		R1 = 2000 R2 = 51 R3 = 2000
	  Rx = (R2*R3 + R3(R1+R2)Vin/3.3v)/(R1-(R2+R1)*Vin/3.37)
	*/
	vin   = (double)(temp_adc*0.000806);
	vin   = (double)vin/4+RES_COMP;//�˷�������3��,3.3v����10��,����30��
	resis = (double)(102000+1243030.30*vin)/(2000-621.51*vin)/1.007;//*0.994;
	//printf("%.3f \r\n  ",resis);delay_ms(10);
	temp = (uint16_t)(resis * 100);				//��������100��
	for(i = 0;i < 72;i++)
	{
		if( temp < s_temp_table[i] )break;
	}
	temp_l = -50 + (i-1)*5;
	resis = temp_l+(float)(5*(temp-s_temp_table[i-1]))/(s_temp_table[i]-s_temp_table[i-1]);
	//printf("%.3f \r\n  ",resis);delay_ms(10);
	return resis;
}

/*****************************************************************************
 * ��������:		��������
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-11				������д
							2018-06-28				�Ż�����;��ǰ���ӵĶ���жϵ���ϵͳ������Ӧ����
							2018-07-20				�Ż�����;���ε������Զ���������(����Ժ���Ҫֱ�Ӵ����μ���)
 ****************************************************************************/
uint32_t ADC_Alarmjudge(adc_sta_typedef sta)
{
	if((sta.value >= (0.85*sta.threa)) && (sta.threa != 0) && (sta.value < sta.threa))//�ﵽ����ֵ��90%����Ԥ��
	{
		g_early_alarm = 1;                                                              //Ԥ��
		return 0x80000000;                                                                    //��Чֵ
	}else if((sta.value <= (0.7*sta.threa)) && (g_alarm_flag & sta.read_flag))        //�Ѿ�����,�����½����趨ֵ��70%��ȡ������
	{
//		s_adc_sta &=~ sta.read_sta;                                                     //�����־
//		return 0;                                                                       //�ָ�
		return 0x80000000;
	}else if((sta.value >= sta.threa) && (sta.threa != 0))                            //�ﵽ����ֵ,���ر���
	{
		s_adc_sta += (1 << sta.move_sta);																								//��������
		if(((s_adc_sta & sta.read_sta) >> sta.move_sta) == 0x04)												//0-4=8��,���������8�ζ�������ȷ��Ϊ��
		{
			s_adc_sta &=~ sta.read_sta;                                                   //�����־,���¼���
			return sta.alarm_flag;                                                        //����
		}else
		return 0x40000000;
	}
	return 0x80000000;                                                                      //��Чֵ
}

/*****************************************************************************
 * ��������:		����ֵ����
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-11				������д
 ****************************************************************************/
uint8_t ADC_GetADC( float dat , PART prt_type )
{
	uint8_t  dat_index   = 0;						//����λ��           ָ������ buffer ���������λ��
	uint32_t data_type   = 0xFF;				//������������        0xFF->�ޱ��� 0->�����ָ� ����->����ֵ
	uint16_t cail_value  = 0;           //У׼ֵ
	uint8_t  data_ch     = 0;						//����ͨ��
	DATA     adc_type;                  //������������
	adc_sta_typedef adc_sta;	
  uint32_t *max,*min;
	int16_t value = 0;
	
	if(1){//���ݷ���
		switch( prt_type )
		{
			case part_curr:{//��������
					switch(s_adc_num)
					{
						case 1:{//����1ͨ��
							adc_sta.read_flag = READ_CURR_1;
							adc_sta.read_sta  = READ_CURR_1_STA;
							adc_sta.alarm_flag= CURR_1_ALARM;
							adc_sta.move_sta  = SET_CURR_1_STA;
							adc_sta.threa     = g_sys_param.threa.curr1;
							adc_type    			= data_curr;
							cail_value  			= g_sys_param.cali.curr;
							dat_index					= CURR1_DATA_INDEX;
							data_ch						= 1;
							max								= &s_curr1_max;
							min               = &s_curr1_min;
							break;}
						case 6:{//����2ͨ��
							adc_sta.read_flag = READ_CURR_2;
							adc_sta.read_sta  = READ_CURR_2_STA;
							adc_sta.alarm_flag= CURR_2_ALARM;
							adc_sta.move_sta  = SET_CURR_2_STA;
							adc_sta.threa     = g_sys_param.threa.curr2;
							adc_type    			= data_curr;
							cail_value  			= g_sys_param.cali.curr;
							dat_index					= CURR2_DATA_INDEX;
							data_ch						= 2;
							max								= &s_curr2_max;
							min               = &s_curr2_min;
							break;}
						case 7:{//����3ͨ��
							adc_sta.read_flag = READ_CURR_3;
							adc_sta.read_sta  = READ_CURR_3_STA;
							adc_sta.alarm_flag= CURR_3_ALARM;
							adc_sta.move_sta  = SET_CURR_3_STA;
							adc_sta.threa     = g_sys_param.threa.curr3;
							adc_type    			= data_curr;
							cail_value  			= g_sys_param.cali.curr;
							dat_index					= CURR3_DATA_INDEX;
							data_ch						= 3;
							max								= &s_curr3_max;
							min               = &s_curr3_min;
							break;}
						default:{//����
							break;}
					}

					break;}
			case part_sycu:{//ʣ���������
					adc_sta.read_flag = READ_SY_CURR;
				  adc_sta.read_sta  = READ_SY_CURR_STA;
				  adc_sta.alarm_flag= SY_CURR_ALARM;
				  adc_sta.move_sta  = SET_SY_CURR_STA;
				  adc_sta.threa     = g_sys_param.threa.sy_curr;
				  adc_type    			= data_sy_curr;
					cail_value  			= g_sys_param.cali.sy_curr;
					dat_index   			= SY_CURR_DATA_INDEX;
					data_ch     			= 1;
				break;}
			case part_temp:{//�¶�����
				  switch(s_adc_num)
					{
						case 4:{//�¶�ͨ��1
							adc_sta.read_flag = READ_TEMP_1;
							adc_sta.read_sta  = READ_TEMP_1_STA;
							adc_sta.alarm_flag= TEMP_1_ALARM;
							adc_sta.move_sta  = SET_TEMP_1_STA;
							adc_sta.threa     = g_sys_param.threa.temp1;
							data_ch           = 1;
							dat_index         = TEMP1_DATA_INDEX;
							break;}
						case 5:{//�¶�ͨ��2
							adc_sta.read_flag = READ_TEMP_2;
							adc_sta.read_sta  = READ_TEMP_2_STA;
							adc_sta.alarm_flag= TEMP_2_ALARM;
							adc_sta.move_sta  = SET_TEMP_2_STA;
							adc_sta.threa     = g_sys_param.threa.temp2;
							data_ch           = 2;
							dat_index         = TEMP2_DATA_INDEX;
							break;}
						case 8:{//�¶�ͨ��3
							/*adc_sta.read_flag = READ_TEMP_3;
							adc_sta.read_sta  = READ_TEMP_3_STA;
							adc_sta.alarm_flag= TEMP_3_ALARM;
							adc_sta.move_sta  = SET_TEMP_3_STA;
							adc_sta.threa     = g_sys_param.threa.temp3;
							data_ch           = 3;
							dat_index         = TEMP3_DATA_INDEX;*/
							break;}
						default:{//����
							break;}
					}
					adc_type = data_temp;
					if(dat < 387)
					{
						cail_value = 1000;
					}else if((dat >= 387) && (dat < 542))
					{
						cail_value = g_sys_param.cali.temp[0];
					}else if((dat >= 542) && (dat < 775))
					{
						cail_value = g_sys_param.cali.temp[1]; 
					}else if(dat >= 775)
					{
						cail_value = g_sys_param.cali.temp[2]; 
					}
					break;}
			case part_volt:{//��ѹ����
					adc_sta.read_flag = READ_VOLAT;
				  adc_sta.read_sta  = READ_VOLAT_STA;
				  adc_sta.alarm_flag= VOLAT_ALARM;
				  adc_sta.move_sta  = SET_VOLAT_STA;
				  adc_sta.threa     = g_sys_param.threa.volat;
				  adc_type          = data_volat;
					cail_value        = g_sys_param.cali.volat;
					dat_index         = VOLAT_DATA_INDEX;
					data_ch           = 1;
				  max               = &s_volat_max;
				  min               = &s_volat_min;
					break;}
			default:return 0;//����������ֱ���˳�,������ƫ��,�´����ݹ���ֱ�Ӹ���
		}
	}
	if(2){//���ݸ�ֵ
		value = (int16_t)(dat/((float)cail_value/1000));
		switch(prt_type)
		{
			case part_curr:
			case part_volt:
				if(g_coll_data[dat_index].data_value != 0)
				{
					if(value >= g_coll_data[dat_index].data_value)*max = value;
					else *min = value;
					adc_sta.value = *max - *min;
				}else
				{
					adc_sta.value = 0;
				}
				break;
			case part_sycu:
			case part_temp:
				adc_sta.value = value;
				break;
			default:return 0;
		}
		g_coll_data[dat_index].data_value = value;//�������           �¶������и���
		g_coll_data[dat_index].data_type  = adc_type;//��������
		g_coll_data[dat_index].part_type	=	prt_type;
		g_coll_data[dat_index].part_addr  = data_ch;
		RTC_Get();																									//��ȡʱ��
		memcpy(&g_coll_data[dat_index].time , &Time , sizeof(Time));//ʱ�俽��
	}
	if(3){//��������
		data_type = ADC_Alarmjudge( adc_sta );//���ж�
		if((!(g_alarm_flag & adc_sta.read_flag)) && !(data_type&0xC0000000))//���������������
		{
			g_alarm_flag |= data_type;
		}else if((g_alarm_flag & adc_sta.read_flag) && (!data_type))//��������ָ�
		{
			g_alarm_flag |= ALARM_RECOVERY;
			g_alarm_flag &=~ adc_sta.read_flag;
		}else if (data_type & 0x40000000)
		{
			return 0xFF;
		}
	}
	return 0;
}
/*****************************************************************************
 * ��������:		����ֵ�ж�
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-07-13				������д
 ****************************************************************************/
uint32_t ADC_CollValueCheck(uint16_t CollValue)
{
	static uint16_t error_cnt = 0;                      //�������ݼ���
  uint16_t error_max = ADC_SAMPLING_CNT*2/3;          //�������ֵ	
	uint32_t value = 0;                                 //��ż���ֵ���м����
	
	if(s_old_adc>100)//��С50���������ı仯Ϊ����
	{
		if((CollValue > s_old_adc))
		{
			if((CollValue-s_old_adc)>(s_old_adc/2))
			{
				s_camp_cnt--;
				error_cnt++;
				if(error_cnt > error_max)//1�����ڵ�2/3�����ڸ���,���˳��ɼ�,�ɼ�ƽ��ֵΪǰһ���ɼ�ֵ��ƽ��ֵ
				{
          error_cnt = 0;
					value = s_old_adc*(ADC_SAMPLING_CNT-s_camp_cnt);
					s_camp_cnt = ADC_SAMPLING_CNT-1;
					return value;
				}
				return 0;
			}
		}else if(CollValue < s_old_adc)
		{
			if((s_old_adc-CollValue)>(s_old_adc/2))
			{
				s_camp_cnt--;
				error_cnt++;
				if(error_cnt > error_max)//1�����ڵ�2/3
				{
          error_cnt = 0;
					value = s_old_adc*(ADC_SAMPLING_CNT-s_camp_cnt);
					s_camp_cnt = ADC_SAMPLING_CNT-1;
					return value;
				}
				return 0;
			}
		} 
	}
 s_old_adc = CollValue;//�ɵ�ADCֵΪ���β���ֵ
 error_cnt = 0;
 return s_old_adc;
}
/*****************************************************************************
 * ��������:		DMA�жϷ�����
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-15				������д
							2018-03-29				�����Ż�,�����ڴ�ʹ��
							2018-05-03				���ݱ���ɾ��,�ƶ������� ADC_GetADC ��
							2018-05-04				�޸�bug;ĳЩ���ӵ�©�����ɼ�����adc����0x10
							2018-06-07				������д
							2018-06-11				�޸�bug;����ֵ�����,���¼��������ADCֵƫ��
              2018-07-13        �����Ż�;��Ը��Ž����˴���,ģ����ͻȻ����100���ϵ�ƫ��ʱ���²ɼ�
 ****************************************************************************/
void ADC_DMAIRQnHandle( void )
{
	static uint16_t	sy_curr_cnt	  = 0;//ADC�ɼ�����
	uint16_t        sy_curr_test  = 0;//ʣ������Ĳ���ֵ
	
	if( DMA_GetITStatus( DMA_IT_TC ) != RESET )
	{
		if(s_adc_collect_flag)//����ɼ�
		{
			switch(s_adc_num)
			{
				case 1:{//�����ɼ�
					g_camp_value += ADC_CollValueCheck(s_ADC_Value[CD4051_ADC_NUM]);
					break;}
				case 2:{//ʣ������ɼ�(���αȽ�����,�����⴦��)
					sy_curr_test = s_ADC_Value[FLOOR_ADC_NUM];
					if(sy_curr_test <= 0x10)
					{
						sy_curr_cnt++;
						if(sy_curr_cnt > 1000)
						{
							sy_curr_cnt        = 0;															//�������
							g_camp_value       = 0;
							s_adc_collect_flag = 0;															//�ɼ����
						}
						DMA_ClearITPendingBit( DMA_IT_TC );
						return;
					}else
					{
						sy_curr_cnt         = 0;															 //�������
						g_camp_value       += ADC_CollValueCheck(s_ADC_Value[FLOOR_ADC_NUM]);
					}
					break;}
				case 3:{//��ѹ�ɼ�
					g_camp_value     		 += ADC_CollValueCheck(s_ADC_Value[CD4051_ADC_NUM]);
					break;}
				case 4:{//�¶�1�ɼ�
					g_camp_value    		 += ADC_CollValueCheck(s_ADC_Value[TEMP1_ADC_NUM]);
					break;}
				case 5:{//�¶�2�ɼ�
					g_camp_value         += ADC_CollValueCheck(s_ADC_Value[TEMP2_ADC_NUM]);
					break;}
				case 6:{//����2�ɼ�
					g_camp_value         += ADC_CollValueCheck(s_ADC_Value[CD4051_ADC_NUM]);
					break;}
				case 7:{//����3�ɼ�
					g_camp_value         += ADC_CollValueCheck(s_ADC_Value[CD4051_ADC_NUM]);
					break;}
				case 8:{//�¶�3�ɼ�
//					g_camp_value       += ADC_CollValueCheck(s_ADC_Value[TEMP3_ADC_NUM]);
					break;}
				default:{//��������
					break;
				}
			}
			if(++s_camp_cnt == ADC_SAMPLING_CNT)
				s_adc_collect_flag = 0;															//�ɼ����
		}
		DMA_ClearITPendingBit( DMA_IT_TC );
	}
}

/*****************************************************************************
 * ��������:		ͨ���л�
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-20				������д
							2018-07-03				�����޸�;ɾ����� s_adc_collect_flag = 1;
 ****************************************************************************/
static void ADC_ChannelChange( CD4051_CHANNEL channel )
{
  switch( channel )
	{
		case CHANNEL_0:
			EN_CD4051_CH0;break;
		case CHANNEL_1:
			EN_CD4051_CH1;break;
		case CHANNEL_2:
			EN_CD4051_CH2;break;
		case CHANNEL_3:
			EN_CD4051_CH3;break;
		case CHANNEL_4:
			EN_CD4051_CH4;break;
		case CHANNEL_5:
			EN_CD4051_CH5;break;
		case CHANNEL_6:
			EN_CD4051_CH6;break;
		case CHANNEL_7:
			EN_CD4051_CH7;break;
		default:
			break;
	}
	g_wait_flag = 1;//�ȴ�500ms,һ����ʱ����
	SysTimeReset();
}

/*****************************************************************************
 * ��������:		���ݲɼ�
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-07-03				������д
 ****************************************************************************/
void ADC_DataRead(CD4051_CHANNEL ch , uint8_t adc_num , int32_t* camp)
{
	if(!g_wait_flag)
	{
		s_old_adc = 0;
		s_adc_num = adc_num;														//��������
		(*camp)	  = 0;																	//�����������
		if(ch != CHANNEL_NULL)ADC_ChannelChange( ch );	//CD4051ͨ���ĸ���
		else g_wait_flag = 2;
	}
	if(g_wait_flag==2)
	{
		s_adc_collect_flag = 1;													//��������
		while(s_adc_collect_flag);											//�ȴ���������
		g_wait_flag = 0;
	}
}

/*****************************************************************************
 * ��������:		���ݱ���
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-07-03				������д
							2018-07-16 				�޸�bug;����Ӳ����ԭ��(ģ�⿪�ش������)���µ������ݴ���65A����׼ȷ
							2018-07-19				�����Ż�;���У׼�����ʹ��,��������ӷ���ֵ,����ԭʼ����ֵ
							2018-08-02				�޸�bug;���豸��⵽�������ǻ����ݵ�ʱ��,���ݻ����(����λ�Ʋ���)
 ****************************************************************************/
double ADC_DataSave(uint8_t adc_num , int32_t *camp , double cail , PART type , uint8_t *proce)
{
	double adc_value = 0.0;
	
	s_adc_num	= adc_num;																					//��������
	*camp >>= ADC_SAMPLING_MOVE;																	//��ƽ��ֵ
	if(1){//�¶����⴦��
		if((s_adc_num == 4) || (s_adc_num == 5 )                      //�¶�����
			//||(s_adc_num == 8 )
		)
		{
			adc_value = ADC_GetTemp( *camp ) * 10;										  //�����¶�ֵ
		}else
		{
			adc_value  = (float)(*camp * COMP_COEFFIC * cail);				  //��������ֵ
		}
	}
	if(2){//�������⴦��
		if((s_adc_num == 1) || (s_adc_num == 6) || (s_adc_num == 7))
		{
			adc_value = (float)(*camp * COMP_COEFFIC * cail);
			if(adc_value > 480)//����60A���⴦��
			{
				adc_value = 0.2826*adc_value*adc_value-272.75*adc_value+66356;
			}
			adc_value *= 1.33;//10*1.2/9
		}
	}
	if(ADC_GetADC(adc_value , type))																	//���ݱ���
	{
		(*proce)-=2;																								//���ص���һ�β���(�����ڳ����� *proce ��++,���ڴ˴�Ҫ-2)
		(*camp) = 0;
		s_adc_collect_flag = 1;				                              //��ʼ����
		while( s_adc_collect_flag );	                              //�ȴ�adc�ɼ����
	}
	return adc_value;
}

void ADC_SetSta(uint8_t byte , int16_t max , int16_t min , int16_t value)
{
	if((value >= max) || (value <= min))
	{
		if(!(g_senser_flag & (0x100401 << byte)))
		{
			g_senser_flag &=~ (0x100401 << byte);//���ʣ��������������ϵ������Ϣ(����,����,�ϱ�)
			g_senser_flag |=  (0x000001 << byte);//��λbit7(ʣ���������)
		}
	}else if(g_senser_flag & (0x100401 << byte))
	{
		g_senser_flag &=~ (0x100401 << byte);//���ʣ��������������ϵ������Ϣ(����,����,�ϱ�)
		g_senser_flag |=  (0x000400 << byte);//��λbit27(ʣ������������������)
	}
}
/*****************************************************************************
 * ��������:		���������
 * ��ʽ����:		type ��λΪ���� ��λΪͨ��
 * ���ز���:		��
 * ��������;		2018-07-26				������д
 ****************************************************************************/
void ADC_SenserCheck(uint8_t type)
{
	uint32_t old_sta = g_senser_flag;//������״̬
	
	s_camp_cnt				= 0;						    //������������
	g_camp_value      = 0;								//����ֵ����
	SENSER_CHECK_OPEN;                    //�򿪿���
	switch(type)
	{
		case 0x00:{//��ѹ������,ͨ��1
			if(!(g_sys_param.shield.volat & 0x01))break;//�����вɼ��ͼ���
			ADC_SetSta( 0 , 5000 , 200 , g_coll_data[VOLAT_DATA_INDEX].data_value );
			break;}
		case 0x10:{//����������,ͨ��1
			break;}
		case 0x11:{//����������,ͨ��2
			break;}
		case 0x12:{//����������,ͨ��3
			break;}
		case 0x20:{//�¶ȴ�����,ͨ��1
			if(!(g_sys_param.shield.temp & 0x01))break;//�����вɼ��ͼ���
			ADC_SetSta( 4 , 3000 , -400 , g_coll_data[TEMP1_DATA_INDEX].data_value );
			break;}
		case 0x21:{//�¶ȴ�����,ͨ��2
			if(!(g_sys_param.shield.temp & 0x02))break;//�����вɼ��ͼ���
			ADC_SetSta( 5 , 3000 , -400 , g_coll_data[TEMP2_DATA_INDEX].data_value );
			break;}
		case 0x30:{//ʣ�����������,ͨ��1
			if(!(g_sys_param.shield.sy_curr & 0x01))break;//�����вɼ��ͼ���
			s_adc_collect_flag = 1;
			while(s_adc_collect_flag);
			g_camp_value >>= ADC_SAMPLING_MOVE;//����600��Ϊ��ʧ
			ADC_SetSta( 7 , 0x600 , 0x100 , g_camp_value);
			break;}
		default:{//����
			SENSER_CHECK_CLOS;									//�رտ���
			return;}
	}
	if(1){//���ز���
		if(((old_sta & 0x3FF) != (g_senser_flag & 0x3FF)) && ((g_senser_flag & 0x3FF) != 0))
		{
			LED_Control( L_ERROR , OPEN);
		}else if(((old_sta & 0xFFC00) != (g_senser_flag & 0xFFC00)) && (!g_nb_error_flag))
		{
			LED_Control( L_ERROR , CLOS);
		}
	}
	SENSER_CHECK_CLOS;									//�رտ���
}

/*****************************************************************************
 * ��������:		���ݲɼ�
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-20				������д
							2018-03-29				�����Ż�
							2018-05-10				������,������������������ϴ�����
							2018-06-07				������д
							2018-07-17				�޸�bug;��Ե�����adc�������зֶε���,�������ֵ��0.30141����Ϊ0.2261
							2018-07-19				�����Ż�;���У׼�����ʹ��,��������ӷ���ֵ,����ԭʼ����ֵ
							2018-07-23				�����Ż�;�ڽ���λ������������,����λ��ʱ��
							2018-07-26				�������;�ڲ���ǰ�жϴ�����״̬(�Ƿ�ʧ,�Ƿ�����)
 ****************************************************************************/
double ADC_Collection( uint8_t cmd )
{
	double re_value    = 0.0;
	static uint8_t cnt = 0;
	uint8_t tmp        = 0;
	
	if( cmd == 0xFF )
		tmp = cnt++;//�˴��ȸ�ֵ,���Լ�
	else tmp = cmd;
	s_camp_cnt				= 0;						    //������������
	switch(tmp)
	{
		case 0:{//����1���ݲɼ�(�ɼ�1024��)
			if((!(g_sys_param.shield.curr & 0x01)) && (cmd == 0xFF)){cnt++;break;}//�����вɼ��ͼ���
			ADC_DataRead( CHANNEL_3 , 1 , &g_camp_value);
			break;}
		case 1:{//�������ݶ�ȡ�ͼ���
			re_value = ADC_DataSave( 1 , &g_camp_value , 0.2261 , part_curr , &cnt);
			if(re_value <= 5){}//��ʼ�жϴ������Ƿ�ʧ
			break;}
		case 2:{//����2���ݲɼ�(�ɼ�1024��)
			if((!(g_sys_param.shield.curr & 0x02)) && (cmd == 0xFF)){cnt++;break;}//�����вɼ��ͼ���
			ADC_DataRead( CHANNEL_0 , 6 , &g_camp_value);
			break;}
		case 3:{//����2���ݶ�ȡ
			re_value = ADC_DataSave( 6 , &g_camp_value , 0.2261 , part_curr , &cnt);
			if(re_value <= 5){}//��ʼ�жϴ������Ƿ�ʧ
			break;}
		case 4:{//����3���ݲɼ�(�ɼ�1024��)
			if((!(g_sys_param.shield.curr & 0x04)) && (cmd == 0xFF)){cnt++;break;}//�����вɼ��ͼ���
			ADC_DataRead( CHANNEL_2 , 7 , &g_camp_value );
			break;}
		case 5:{//����3���ݶ�ȡ
			re_value = ADC_DataSave( 7 , &g_camp_value , 0.2261 , part_curr , &cnt);//2261
			if(re_value <= 5){}//��ʼ�жϴ������Ƿ�ʧ
			break;}
		case 6:{//ʣ��������ݲɼ�(�ɼ�1024��)
			if((!(g_sys_param.shield.sy_curr & 0x01)) && (cmd == 0xFF)){cnt++;break;}//�����вɼ��ͼ���
			ADC_DataRead( CHANNEL_NULL , 2 , &g_camp_value );
			break;}
		case 7:{//ʣ��������ݶ�ȡ
			re_value = ADC_DataSave( 2 , &g_camp_value , 5.1859 , part_sycu , &cnt);//5.1859    6.7967
			if(re_value <= 5){ADC_SenserCheck(0x30);}//��ʼ�жϴ������Ƿ�ʧ
			else g_senser_flag &=~ 0x1000;//����������
			break;}
		case 8:{//��ѹ���ݲɼ�(�ɼ�1024��)
			if((!(g_sys_param.shield.volat & 0x01)) && (cmd == 0xFF)){cnt++;break;}//�����вɼ��ͼ���
			ADC_DataRead( CHANNEL_1 , 3 , &g_camp_value );
			break;}
		case 9:{//��ѹ���ݶ�ȡ
			re_value = ADC_DataSave( 3 , &g_camp_value , 2.26056 , part_volt , &cnt);
			break;}
		case 10:{//�¶�1���ݲɼ�(�ɼ�1024��)
			if((!(g_sys_param.shield.temp & 0x01)) && (cmd == 0xFF)){cnt++;break;}//�����вɼ��ͼ���
			ADC_DataRead( CHANNEL_NULL , 4 , &g_camp_value );
			break;}
		case 11:{//�¶�1���ݶ�ȡ
			re_value = ADC_DataSave( 4 , &g_camp_value , 0.0 , part_temp , &cnt);
			ADC_SenserCheck(0x20);//��ʼ�жϴ������Ƿ�ʧ
			break;}
		case 12:{//�¶�2���ݲɼ�(�ɼ�1024��)
			if((!(g_sys_param.shield.temp & 0x02)) && (cmd == 0xFF)){cnt++;break;}//�����вɼ��ͼ���
			ADC_DataRead( CHANNEL_NULL , 5 , &g_camp_value );
			break;}
		case 13:{//�¶�2���ݶ�ȡ
			re_value = ADC_DataSave( 5 , &g_camp_value , 0.0 , part_temp , &cnt);
			ADC_SenserCheck(0x21);//��ʼ�жϴ������Ƿ�ʧ
			break;}
		/*case 14:{//�¶�3���ݲɼ�(�ɼ�1024��)
			if((!(g_sys_param.shield.temp & 0x04)) && (cmd == 0xFF)){cnt++;break;}//�����вɼ��ͼ���
			ADC_DataRead( CHANNEL_NULL , 8 , &g_camp_value );
			break;}
		case 15:{//�¶�3���ݶ�ȡ
			re_value = ADC_DataSave( 8 , &g_camp_value , 0.0 , part_temp , &cnt);
			if(re_value <= -400){ADC_SenserCheck(0x22);}//��ʼ�жϴ������Ƿ�ʧ
			break;}*/
		default:{//�������
			cnt = 0;
			g_adc_get_flag = 0;       //��־λ����
			ADC_SenserCheck(0xFF);    //������Ϣ���
			CampTime_Updeat(&g_sys_param.camp_time);//����ʱ�����
			break;}
	}
	if(g_wait_flag==1)cnt--;
	return re_value;
}

/*****************************************************************************
 * ��������:		����У׼
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-04-26				������д
							2018-05-08				�¶�У׼����bug���޸�
 ****************************************************************************/
void ADC_Cail( void )
{
	int16_t  temp = 0;
	double camp_value = 0;
		
	switch(g_cail_data.type)
	{
		case part_curr:{//����������У׼
				ADC_Collection(0);//�������ݲɼ�
				camp_value = ADC_Collection(1);//�������ݶ�ȡ
				temp = (uint16_t)(camp_value / g_cail_data.value*1000);
				if(temp <= 100)//������û�н���
				{
					g_usart_errortype = 0x01;//��������ʧ
					g_usart_errorvalu = 0x02;//����������
					USART_CRT_FunAdd(USART_SendError);
				}else if((temp >= 2000) || (temp < 800))
				{
					g_usart_errortype = 0x02;//����������
					g_usart_errorvalu = 0x02;//����������
					USART_CRT_FunAdd(USART_SendError);
				}else
				{
					g_sys_param.updat_flag = 1;
					g_sys_param.cali.curr = temp;
					g_cail_type = 4;
					USART_CRT_FunAdd(USART_SendCail);
				}
				break;}
		case part_sycu:{//ʣ�����������У׼
				ADC_Collection(6);//ʣ��������ݲɼ�
				camp_value = ADC_Collection(7);//ʣ��������ݶ�ȡ
				temp = (uint16_t)(camp_value / g_cail_data.value*1000);
				if((temp < 800) && (temp > 0) && (temp > 2000))
				{
					g_usart_errortype = 0x02;//����������
					g_usart_errorvalu = 0x04;//ʣ�����������
					USART_CRT_FunAdd(USART_SendError);
				}else if(temp == 0)
				{
					g_usart_errortype = 0x01;//��������ʧ
					g_usart_errorvalu = 0x04;//ʣ�����������
					USART_CRT_FunAdd(USART_SendError);
				}else
				{
					g_sys_param.updat_flag = 1;
					g_sys_param.cali.curr = temp;
					g_cail_type = 6;
					USART_CRT_FunAdd(USART_SendCail);
				}
			break;}
		case part_temp:{//�¶�У׼
				ADC_Collection(10);//�¶����ݲɼ�
				camp_value = ADC_Collection(11);//�¶����ݶ�ȡ
				temp = (int16_t)(camp_value / g_cail_data.value*1000);
				if(g_cail_data.value == 387)
				{
					if((temp >= 2000) || ((temp > 0) && (temp <= 800)))//�趨ֵ���ڼ��ֵ10��/�趨ֵС�ڼ��ֵ10��Ϊ����������
					{
						g_usart_errortype = 0x02;//����������
						g_usart_errorvalu = 0x01;//�¶ȴ�����
						USART_CRT_FunAdd(USART_SendError);
					}else if(temp <= 0)
					{
						g_usart_errortype = 0x01;//��������ʧ
						g_usart_errorvalu = 0x01;//�¶ȴ�����
						USART_CRT_FunAdd(USART_SendError);
					}else
					{
						g_sys_param.updat_flag = 1;
						g_sys_param.cali.temp[0] = (uint16_t)temp;
						g_cail_type = 1;
						USART_CRT_FunAdd(USART_SendCail);
					}
				}
				else if(g_cail_data.value == 542)
				{
					if(((temp >= 0) && (temp <= 800)) ||(temp >= 2000) )
					{
						g_usart_errortype = 0x02;//����������
						g_usart_errorvalu = 0x01;//�¶ȴ�����
						USART_CRT_FunAdd(USART_SendError);
					}else if(temp <= 0)
					{
						g_usart_errortype = 0x01;//��������ʧ
						g_usart_errorvalu = 0x01;//�¶ȴ�����
						USART_CRT_FunAdd(USART_SendError);
					}else
					{
						g_sys_param.updat_flag = 1;
						g_sys_param.cali.temp[1] = (uint16_t)temp;
						g_cail_type = 2;
						USART_CRT_FunAdd(USART_SendCail);
					}
				}
				else if(g_cail_data.value == 775)
				{
					if(((temp >= 0) && (temp <= 800)) ||(temp >= 2000) )
					{
						g_usart_errortype = 0x02;//����������
						g_usart_errorvalu = 0x01;//�¶ȴ�����
						USART_CRT_FunAdd(USART_SendError);
					}else if(temp <= 0)
					{
						g_usart_errortype = 0x01;//��������ʧ
						g_usart_errorvalu = 0x01;//�¶ȴ�����
						USART_CRT_FunAdd(USART_SendError);
					}else
					{
						g_sys_param.updat_flag = 1;
						g_sys_param.cali.temp[2] = (uint16_t)temp;
						g_cail_type = 3;
						USART_CRT_FunAdd(USART_SendCail);
					}
				}
			break;}
		case part_volt:{//��ѹУ׼
				ADC_Collection(8);//��ѹ���ݲɼ�
				camp_value = ADC_Collection(9);//��ѹ���ݶ�ȡ
				//У׼���� =  ���ֵ / �趨ֵ   ���� ���ֵ = ���ֵ / У׼����
				temp = (uint16_t)(camp_value / g_cail_data.value*1000);
				if(((temp < 800) && (temp > 0)) || (temp >= 2000))//����ֵ̫С,����ֵ����,����������
				{
					g_usart_errortype = 0x02;//����������
					g_usart_errorvalu = 0x03;//��ѹ������
					USART_CRT_FunAdd(USART_SendError);
				}else if(temp == 0)//�޲�������,û�нӴ�����
				{
					g_usart_errortype = 0x01;//��������ʧ
					g_usart_errorvalu = 0x03;//��ѹ������
					USART_CRT_FunAdd(USART_SendError);
				}else
				{
					g_sys_param.updat_flag = 1;
					g_sys_param.cali.volat = temp;
					g_cail_type = 5;
					USART_CRT_FunAdd(USART_SendCail);
				}
				break;}
		default:
			break;
	}
}
/*------------------------------File----------End------------------------------*/
