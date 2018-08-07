/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-14					文档编写
						2018-03-15					ADC程序的调试(电压采集,电流采集)
						2018-03-16					ADC程序的调试(温度采集)
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "adc_get.h"
#include "delay.h"
#include "usart.h"
#include "configure.h"
#include "com.h"
#include "key_led.h"
#include "timer.h"

#include "string.h"
/* 宏定义	--------------------------------------------------------------------*/
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

#ifndef NEW_MODEL																																//老板子
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

#else																																					//新板子
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



#define ADC_SAMPLING_CNT													1024												//ADC采样次数
#define ADC_SAMPLING_MOVE													10													//2<<10=1024

#define TEMP3_ADC_NUM															0														//温度2的ADC的通道
#define TEMP2_ADC_NUM															4														//温度2的ADC的通道
#define TEMP1_ADC_NUM															3														//温度1的ADC的通道
#define CD4051_ADC_NUM														2														//4051的ADC的通道
#define FLOOR_ADC_NUM															1														//漏电流的ADC的通道

//s_adc_sta 相关参数
#define READ_VOLAT_STA                            0x0000000F
#define SET_VOLAT_STA                             0x00
#define READ_CURR_1_STA                           0x000000F0
#define SET_CURR_1_STA                            0x04
#define READ_CURR_2_STA                           0x00000F00									//预留
#define SET_CURR_2_STA                            0x08
#define READ_CURR_3_STA                           0x0000F000									//预留
#define SET_CURR_3_STA														0x0C
#define READ_TEMP_1_STA														0x000F0000
#define SET_TEMP_1_STA														0x10
#define READ_TEMP_2_STA														0x00F00000									//预留
#define SET_TEMP_2_STA														0x14
#define READ_TEMP_3_STA														0x0F000000									//预留
#define SET_TEMP_3_STA														0x18
#define READ_SY_CURR_STA													0xF0000000
#define SET_SY_CURR_STA														0x1C




/* 结构体定义	----------------------------------------------------------------*/
typedef enum{																																	//CD4051通道
	CHANNEL_0 = 0,																															//电流b 的通道
	CHANNEL_1 = 1,																															//电流c 的通道
	CHANNEL_2 = 2,																															//电压a 的通道
	CHANNEL_3 = 3,																															//电流a 的通道
	CHANNEL_4 = 4,																															//电压b 的通道
	CHANNEL_5 = 5,																															//电压c 的通道
	CHANNEL_6 = 6,																															//保留
	CHANNEL_7 = 7,																															//保留
	CHANNEL_NULL = 8,																														//空
}CD4051_CHANNEL;
typedef struct{																																//adc状态结构体
	int16_t		value;																														//ADC的值
	int16_t		threa;																														//ADC的阈值
	uint32_t	alarm_flag;																												//ADC报警标志
	uint32_t	read_flag;																												//ADC读取报警位
	uint32_t	read_sta;																													//ADC读取状态	
	uint8_t		move_sta;																													//ADC状态位
}adc_sta_typedef;
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
static uint16_t s_temp_table[72] ={																						//温度范围是:-50 ~ 305
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
static uint8_t	s_adc_num       = 0;																					//adc序号
static uint8_t	s_adc_collect_flag = 0;																				//adc采集标志
static uint16_t s_ADC_Value[5] = {0};																					//ADC的值
static uint32_t s_adc_sta     = 0;																						//0-3bit(0-2bit计数,3bit标志位)电压 4-7bit电流1 8-11bit电路2 12-15bit电流3 16-19bit温度1 20-23bit温度2 24-27bit温度3 28-31bit漏电流
static uint32_t s_curr1_max		= 0;																						//电流最大值
static uint32_t s_curr1_min    = 0;																						//电流最小值
static uint32_t s_curr2_max		= 0;																						//电流最大值
static uint32_t s_curr2_min    = 0;																						//电流最小值
static uint32_t s_curr3_max		= 0;																						//电流最大值
static uint32_t s_curr3_min    = 0;																						//电流最小值
static uint32_t s_volat_max   = 0;																						//电压最大值
static uint32_t s_volat_min   = 0;																						//电压最小值
static uint16_t s_camp_cnt    = 0;																						//adc采样次数
static uint16_t s_old_adc     = 0;
adc_cail				g_cail_data;																							    //校准数据
Msg_data				g_coll_data[ADC_SIZE];	                                      //采集上来的数据
uint8_t					g_early_alarm = 0;												                    //预警标志位
int32_t					g_camp_value = 0;																					    //采样值
uint32_t        g_senser_flag = 0;										                        //故障标志位
/*
  bit0					bit1					bit2					bit3					bit4					bit5					bit6					bit7					bit8					bit9
电压故障			  电流1故障			电流2故障			电流3故障			温度1故障			温度2故障			温度3故障			漏电流故障	  		预留	    		预留
  bit10					bit11					bit12					bit13					bit14					bit15					bit16					bit17					bit18					bit19
电压故障消除 	电流1故障消除	电流2故障消除	电流3故障消除	温度1故障消除	温度2故障消除	温度3故障消除	漏电流故障消除	  预留	    		预留
  bit20					bit21					bit22					bit23					bit24					bit25					bit26					bit27					bit28					bit29
电压故障上报	  电流1故障上报	电流2故障上报	电流3故障上报	温度1故障上报	温度2故障上报	温度3故障上报	漏电流故障上报	  预留	    		预留
  bit30	        bit31							
   预留	        预留							
*/
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		CD4051初始化
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-14				函数编写
 ****************************************************************************/
static void CD4051_Config( void )
{
	GPIO_InitTypeDef		newGPIO_Init;
	
	RCC_HSEConfig( RCC_HSE_OFF );
	RCC_LSEConfig( RCC_LSE_OFF );							//复用 HSE 和 LSE 功能的GPIO,要把时钟相关功能关闭
	
	newGPIO_Init.GPIO_Mode	=	GPIO_Mode_OUT;
	newGPIO_Init.GPIO_OType	=	GPIO_OType_OD;
	newGPIO_Init.GPIO_PuPd	=	GPIO_PuPd_DOWN;
	newGPIO_Init.GPIO_Speed	=	GPIO_Speed_50MHz;
	
	/*1> CS 端口初始化*/
	RCC_CD4051_CS_GPIO_CMD( RCC_CD4051_CS_GPIO_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin		= CD4051_CS_PIN;
	GPIO_Init( CD4051_CS_PORT , &newGPIO_Init );
	
	/*2> C 端口初始化*/
	RCC_CD4051_C_GPIO_CMD( RCC_CD4051_C_GPIO_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin		=	CD4051_C_PIN;
	GPIO_Init( CD4051_C_PORT , &newGPIO_Init );
	
	/*3> B 端口初始化*/
	RCC_CD4051_B_GPIO_CMD( RCC_CD4051_B_GPIO_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin		=	CD4051_B_PIN;
	GPIO_Init( CD4051_B_PORT , &newGPIO_Init );
	
	/*4> A 端口初始化*/
	RCC_CD4051_A_GPIO_CMD( RCC_CD4051_A_GPIO_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin		=	CD4051_A_PIN;
	GPIO_Init( CD4051_A_PORT , &newGPIO_Init );
	
}
/*****************************************************************************
 * 函数功能:		ADC初始化
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-15				函数编写
 ****************************************************************************/
static void ADC_Config( void )
{
	ADC_InitTypeDef     ADC_InitStruct;
	DMA_InitTypeDef     DMA_InitStruct;
	GPIO_InitTypeDef    GPIO_InitStruct;
	NVIC_InitTypeDef		newNVIC_Init;

	/*1> 时钟的初始化*/
	ADC_DeInit( ADC_COM );        
	RCC_ADC_CMD( RCC_ADC_CLK , ENABLE);  
	RCC_ADC_DMA_CMD( RCC_ADC_DMA_CLK , ENABLE ); 
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);  //48M/4=6M
 
	GPIO_InitStruct.GPIO_Mode		= GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd		= GPIO_PuPd_NOPULL ;
	
	/*2.1> CD4051 端口的配置*/
	GPIO_InitStruct.GPIO_Pin		= CD4051_ADC_PIN;
	RCC_CD4051_ADC_GPIO_CMD( RCC_CD4051_ADC_GPIO_CLK , ENABLE );
	GPIO_Init( CD4051_ADC_PORT, &GPIO_InitStruct );            
	/*2.2> Temp1	端口配置*/
	GPIO_InitStruct.GPIO_Pin		= TEMP1_ADC_PIN;
	RCC_TEMP1_ADC_GPIO_CMD( RCC_TEMP1_ADC_GPIO_CLK , ENABLE );
	GPIO_Init( TEMP1_ADC_PORT , &GPIO_InitStruct );
	/*2.3> Floor ele2 端口配置*/
	GPIO_InitStruct.GPIO_Pin		=	FLOOR_ELE1_ADC_PIN;
	RCC_FLOOR_ELE_GPIO_CMD( RCC_FLOOR_ELE_GPIO_CLK , ENABLE );
	GPIO_Init( FLOOR_ELE1_ADC_PORT , &GPIO_InitStruct );
	/*2.4> Floor ele1 端口配置*/
	/*2.5> Temp2   端口配置*/
	GPIO_InitStruct.GPIO_Pin    = TEMP2_ADC_PIN;
	RCC_TEMP2_ADC_GPIO_CMD(RCC_TEMP2_ADC_GPIO_CLK , ENABLE);
	GPIO_Init( TEMP2_ADC_PORT , &GPIO_InitStruct );
	/*2.5> Temp3   端口配置*/
	#ifdef NEW_MODEL
	GPIO_InitStruct.GPIO_Pin    = TEMP3_ADC_PIN;
	RCC_TEMP3_ADC_GPIO_CMD(RCC_TEMP3_ADC_GPIO_CLK , ENABLE);
	GPIO_Init( TEMP3_ADC_PORT , &GPIO_InitStruct );
	
	GPIO_InitStruct.GPIO_Mode		=	GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd		= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	/*2.6> 传感器检测端口配置*/
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
  
	/*3> DMA 配置*/
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
 	
	/*4> ADC 配置*/	
	ADC_InitStruct.ADC_ContinuousConvMode  		= ENABLE;
	ADC_Init( ADC_COM, &ADC_InitStruct);  
	ADC_ChannelConfig( ADC_COM , ADC_CD4051_ADC_CH     , ADC_SampleTime_239_5Cycles );               
	ADC_ChannelConfig( ADC_COM , ADC_TEMP1_ADC_CH      , ADC_SampleTime_239_5Cycles );
  ADC_ChannelConfig( ADC_COM , ADC_TEMP2_ADC_CH      , ADC_SampleTime_239_5Cycles );	
	#ifdef NEW_MODEL
  ADC_ChannelConfig( ADC_COM , ADC_TEMP3_ADC_CH      , ADC_SampleTime_239_5Cycles );	
	#endif
	ADC_ChannelConfig( ADC_COM , ADC_FLOOR_ELE1_ADC_CH , ADC_SampleTime_239_5Cycles );

	ADC_VrefintCmd( ENABLE );																																//使能基准电压检测
	ADC_GetCalibrationFactor( ADC_COM ); 																										//adc校验
	
	ADC_Cmd( ADC_COM , ENABLE);     
 
	while(!ADC_GetFlagStatus( ADC_COM , ADC_FLAG_ADRDY)){}; 	
		
	newNVIC_Init.NVIC_IRQChannelCmd      = ENABLE;
	/*5> 中断 配置*/	
	newNVIC_Init.NVIC_IRQChannel				 = ADC_DMA_IRQn;
	newNVIC_Init.NVIC_IRQChannelPriority = 1;
	NVIC_SetPriority( ADC_DMA_IRQn	, 1);
	NVIC_Init( &newNVIC_Init );
	 
  ADC_StartOfConversion( ADC_COM );		
}

/*****************************************************************************
 * 函数功能:		ADC初始化
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-15				函数编写
 ****************************************************************************/
void ADC_GetConfig( void )
{
	CD4051_Config();
	ADC_Config();
}

/*****************************************************************************
 * 函数功能:		温度值获取
 * 形式参数:		temp_adc 温度的adc值
 * 返回参数:		温度值
 * 更改日期;		2018-03-16				函数编写
							2018-03-19				算法的更新(以前算法有问题)
							2018-03-28				程序上有bug
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
	vin   = (double)vin/4+RES_COMP;//运放扩大了3倍,3.3v扩大10倍,就是30倍
	resis = (double)(102000+1243030.30*vin)/(2000-621.51*vin)/1.007;//*0.994;
	//printf("%.3f \r\n  ",resis);delay_ms(10);
	temp = (uint16_t)(resis * 100);				//电阻扩大100倍
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
 * 函数功能:		报警处理
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-04-11				函数编写
							2018-06-28				优化程序;以前增加的多次判断导致系统报警响应很慢
							2018-07-20				优化程序;屏蔽掉报警自动清除的语句(如果以后需要直接打开屏蔽即可)
 ****************************************************************************/
uint32_t ADC_Alarmjudge(adc_sta_typedef sta)
{
	if((sta.value >= (0.85*sta.threa)) && (sta.threa != 0) && (sta.value < sta.threa))//达到报警值的90%产生预警
	{
		g_early_alarm = 1;                                                              //预警
		return 0x80000000;                                                                    //无效值
	}else if((sta.value <= (0.7*sta.threa)) && (g_alarm_flag & sta.read_flag))        //已经报警,而且下降到设定值的70%则取消报警
	{
//		s_adc_sta &=~ sta.read_sta;                                                     //清零标志
//		return 0;                                                                       //恢复
		return 0x80000000;
	}else if((sta.value >= sta.threa) && (sta.threa != 0))                            //达到报警值,本地报警
	{
		s_adc_sta += (1 << sta.move_sta);																								//次数自增
		if(((s_adc_sta & sta.read_sta) >> sta.move_sta) == 0x04)												//0-4=8次,如果采样到8次都大于则确定为火警
		{
			s_adc_sta &=~ sta.read_sta;                                                   //清零标志,重新计数
			return sta.alarm_flag;                                                        //报警
		}else
		return 0x40000000;
	}
	return 0x80000000;                                                                      //无效值
}

/*****************************************************************************
 * 函数功能:		采样值保存
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-04-11				函数编写
 ****************************************************************************/
uint8_t ADC_GetADC( float dat , PART prt_type )
{
	uint8_t  dat_index   = 0;						//数据位移           指数据在 buffer 缓冲里面的位置
	uint32_t data_type   = 0xFF;				//报警产生类型        0xFF->无报警 0->报警恢复 其他->报警值
	uint16_t cail_value  = 0;           //校准值
	uint8_t  data_ch     = 0;						//数据通道
	DATA     adc_type;                  //采样数据类型
	adc_sta_typedef adc_sta;	
  uint32_t *max,*min;
	int16_t value = 0;
	
	if(1){//数据分类
		switch( prt_type )
		{
			case part_curr:{//电流数据
					switch(s_adc_num)
					{
						case 1:{//电流1通道
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
						case 6:{//电流2通道
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
						case 7:{//电流3通道
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
						default:{//其他
							break;}
					}

					break;}
			case part_sycu:{//剩余电流数据
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
			case part_temp:{//温度数据
				  switch(s_adc_num)
					{
						case 4:{//温度通道1
							adc_sta.read_flag = READ_TEMP_1;
							adc_sta.read_sta  = READ_TEMP_1_STA;
							adc_sta.alarm_flag= TEMP_1_ALARM;
							adc_sta.move_sta  = SET_TEMP_1_STA;
							adc_sta.threa     = g_sys_param.threa.temp1;
							data_ch           = 1;
							dat_index         = TEMP1_DATA_INDEX;
							break;}
						case 5:{//温度通道2
							adc_sta.read_flag = READ_TEMP_2;
							adc_sta.read_sta  = READ_TEMP_2_STA;
							adc_sta.alarm_flag= TEMP_2_ALARM;
							adc_sta.move_sta  = SET_TEMP_2_STA;
							adc_sta.threa     = g_sys_param.threa.temp2;
							data_ch           = 2;
							dat_index         = TEMP2_DATA_INDEX;
							break;}
						case 8:{//温度通道3
							/*adc_sta.read_flag = READ_TEMP_3;
							adc_sta.read_sta  = READ_TEMP_3_STA;
							adc_sta.alarm_flag= TEMP_3_ALARM;
							adc_sta.move_sta  = SET_TEMP_3_STA;
							adc_sta.threa     = g_sys_param.threa.temp3;
							data_ch           = 3;
							dat_index         = TEMP3_DATA_INDEX;*/
							break;}
						default:{//其他
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
			case part_volt:{//电压数据
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
			default:return 0;//其他数据则直接退出,不进行偏移,下次数据过来直接覆盖
		}
	}
	if(2){//数据赋值
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
		g_coll_data[dat_index].data_value = value;//检测数据           温度数据有负数
		g_coll_data[dat_index].data_type  = adc_type;//数据类型
		g_coll_data[dat_index].part_type	=	prt_type;
		g_coll_data[dat_index].part_addr  = data_ch;
		RTC_Get();																									//获取时间
		memcpy(&g_coll_data[dat_index].time , &Time , sizeof(Time));//时间拷贝
	}
	if(3){//报警处理
		data_type = ADC_Alarmjudge( adc_sta );//火警判断
		if((!(g_alarm_flag & adc_sta.read_flag)) && !(data_type&0xC0000000))//如果产生报警数据
		{
			g_alarm_flag |= data_type;
		}else if((g_alarm_flag & adc_sta.read_flag) && (!data_type))//如果报警恢复
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
 * 函数功能:		采样值判断
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-07-13				函数编写
 ****************************************************************************/
uint32_t ADC_CollValueCheck(uint16_t CollValue)
{
	static uint16_t error_cnt = 0;                      //错误数据计数
  uint16_t error_max = ADC_SAMPLING_CNT*2/3;          //干扰最大值	
	uint32_t value = 0;                                 //存放计算值的中间变量
	
	if(s_old_adc>100)//最小50个数字量的变化为干扰
	{
		if((CollValue > s_old_adc))
		{
			if((CollValue-s_old_adc)>(s_old_adc/2))
			{
				s_camp_cnt--;
				error_cnt++;
				if(error_cnt > error_max)//1个周期的2/3都存在干扰,则退出采集,采集平均值为前一个采集值的平均值
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
				if(error_cnt > error_max)//1个周期的2/3
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
 s_old_adc = CollValue;//旧的ADC值为本次采样值
 error_cnt = 0;
 return s_old_adc;
}
/*****************************************************************************
 * 函数功能:		DMA中断服务函数
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-15				函数编写
							2018-03-29				函数优化,减少内存使用
							2018-05-03				数据报警删除,移动到函数 ADC_GetADC 中
							2018-05-04				修复bug;某些板子的漏电流采集到的adc大于0x10
							2018-06-07				函数重写
							2018-06-11				修复bug;计数值算错了,导致计算出来的ADC值偏大
              2018-07-13        函数优化;针对干扰进行了处理,模拟量突然产生100以上的偏差时重新采集
 ****************************************************************************/
void ADC_DMAIRQnHandle( void )
{
	static uint16_t	sy_curr_cnt	  = 0;//ADC采集次数
	uint16_t        sy_curr_test  = 0;//剩余电流的测试值
	
	if( DMA_GetITStatus( DMA_IT_TC ) != RESET )
	{
		if(s_adc_collect_flag)//允许采集
		{
			switch(s_adc_num)
			{
				case 1:{//电流采集
					g_camp_value += ADC_CollValueCheck(s_ADC_Value[CD4051_ADC_NUM]);
					break;}
				case 2:{//剩余电流采集(波形比较特殊,需特殊处理)
					sy_curr_test = s_ADC_Value[FLOOR_ADC_NUM];
					if(sy_curr_test <= 0x10)
					{
						sy_curr_cnt++;
						if(sy_curr_cnt > 1000)
						{
							sy_curr_cnt        = 0;															//清零计数
							g_camp_value       = 0;
							s_adc_collect_flag = 0;															//采集完成
						}
						DMA_ClearITPendingBit( DMA_IT_TC );
						return;
					}else
					{
						sy_curr_cnt         = 0;															 //清零计数
						g_camp_value       += ADC_CollValueCheck(s_ADC_Value[FLOOR_ADC_NUM]);
					}
					break;}
				case 3:{//电压采集
					g_camp_value     		 += ADC_CollValueCheck(s_ADC_Value[CD4051_ADC_NUM]);
					break;}
				case 4:{//温度1采集
					g_camp_value    		 += ADC_CollValueCheck(s_ADC_Value[TEMP1_ADC_NUM]);
					break;}
				case 5:{//温度2采集
					g_camp_value         += ADC_CollValueCheck(s_ADC_Value[TEMP2_ADC_NUM]);
					break;}
				case 6:{//电流2采集
					g_camp_value         += ADC_CollValueCheck(s_ADC_Value[CD4051_ADC_NUM]);
					break;}
				case 7:{//电流3采集
					g_camp_value         += ADC_CollValueCheck(s_ADC_Value[CD4051_ADC_NUM]);
					break;}
				case 8:{//温度3采集
//					g_camp_value       += ADC_CollValueCheck(s_ADC_Value[TEMP3_ADC_NUM]);
					break;}
				default:{//其他参数
					break;
				}
			}
			if(++s_camp_cnt == ADC_SAMPLING_CNT)
				s_adc_collect_flag = 0;															//采集完成
		}
		DMA_ClearITPendingBit( DMA_IT_TC );
	}
}

/*****************************************************************************
 * 函数功能:		通道切换
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-20				函数编写
							2018-07-03				函数修改;删掉语句 s_adc_collect_flag = 1;
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
	g_wait_flag = 1;//等待500ms,一个定时周期
	SysTimeReset();
}

/*****************************************************************************
 * 函数功能:		数据采集
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-07-03				函数编写
 ****************************************************************************/
void ADC_DataRead(CD4051_CHANNEL ch , uint8_t adc_num , int32_t* camp)
{
	if(!g_wait_flag)
	{
		s_old_adc = 0;
		s_adc_num = adc_num;														//采样类型
		(*camp)	  = 0;																	//采样数据清空
		if(ch != CHANNEL_NULL)ADC_ChannelChange( ch );	//CD4051通道的更改
		else g_wait_flag = 2;
	}
	if(g_wait_flag==2)
	{
		s_adc_collect_flag = 1;													//启动采样
		while(s_adc_collect_flag);											//等待采样结束
		g_wait_flag = 0;
	}
}

/*****************************************************************************
 * 函数功能:		数据保存
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-07-03				函数编写
							2018-07-16 				修复bug;由于硬件的原因(模拟开关存在误差)导致电流数据大于65A过后不准确
							2018-07-19				函数优化;针对校准程序的使用,本函数添加返回值,反回原始采样值
							2018-08-02				修复bug;在设备检测到的数据是火警数据的时候,数据会错误(进程位移不对)
 ****************************************************************************/
double ADC_DataSave(uint8_t adc_num , int32_t *camp , double cail , PART type , uint8_t *proce)
{
	double adc_value = 0.0;
	
	s_adc_num	= adc_num;																					//采样类型
	*camp >>= ADC_SAMPLING_MOVE;																	//求平均值
	if(1){//温度特殊处理
		if((s_adc_num == 4) || (s_adc_num == 5 )                      //温度数据
			//||(s_adc_num == 8 )
		)
		{
			adc_value = ADC_GetTemp( *camp ) * 10;										  //计算温度值
		}else
		{
			adc_value  = (float)(*camp * COMP_COEFFIC * cail);				  //计算其他值
		}
	}
	if(2){//电流特殊处理
		if((s_adc_num == 1) || (s_adc_num == 6) || (s_adc_num == 7))
		{
			adc_value = (float)(*camp * COMP_COEFFIC * cail);
			if(adc_value > 480)//大于60A特殊处理
			{
				adc_value = 0.2826*adc_value*adc_value-272.75*adc_value+66356;
			}
			adc_value *= 1.33;//10*1.2/9
		}
	}
	if(ADC_GetADC(adc_value , type))																	//数据保存
	{
		(*proce)-=2;																								//返回到上一次操作(由于在程序中 *proce 先++,故在此处要-2)
		(*camp) = 0;
		s_adc_collect_flag = 1;				                              //开始采样
		while( s_adc_collect_flag );	                              //等待adc采集完成
	}
	return adc_value;
}

void ADC_SetSta(uint8_t byte , int16_t max , int16_t min , int16_t value)
{
	if((value >= max) || (value <= min))
	{
		if(!(g_senser_flag & (0x100401 << byte)))
		{
			g_senser_flag &=~ (0x100401 << byte);//清除剩余电流传感器故障的相关信息(故障,消除,上报)
			g_senser_flag |=  (0x000001 << byte);//置位bit7(剩余电流故障)
		}
	}else if(g_senser_flag & (0x100401 << byte))
	{
		g_senser_flag &=~ (0x100401 << byte);//清除剩余电流传感器故障的相关信息(故障,消除,上报)
		g_senser_flag |=  (0x000400 << byte);//置位bit27(剩余电流传感器故障清除)
	}
}
/*****************************************************************************
 * 函数功能:		传感器检测
 * 形式参数:		type 高位为类型 低位为通道
 * 返回参数:		无
 * 更改日期;		2018-07-26				函数编写
 ****************************************************************************/
void ADC_SenserCheck(uint8_t type)
{
	uint32_t old_sta = g_senser_flag;//保留老状态
	
	s_camp_cnt				= 0;						    //采样次数清零
	g_camp_value      = 0;								//采样值清零
	SENSER_CHECK_OPEN;                    //打开开关
	switch(type)
	{
		case 0x00:{//电压传感器,通道1
			if(!(g_sys_param.shield.volat & 0x01))break;//不进行采集和计算
			ADC_SetSta( 0 , 5000 , 200 , g_coll_data[VOLAT_DATA_INDEX].data_value );
			break;}
		case 0x10:{//电流传感器,通道1
			break;}
		case 0x11:{//电流传感器,通道2
			break;}
		case 0x12:{//电流传感器,通道3
			break;}
		case 0x20:{//温度传感器,通道1
			if(!(g_sys_param.shield.temp & 0x01))break;//不进行采集和计算
			ADC_SetSta( 4 , 3000 , -400 , g_coll_data[TEMP1_DATA_INDEX].data_value );
			break;}
		case 0x21:{//温度传感器,通道2
			if(!(g_sys_param.shield.temp & 0x02))break;//不进行采集和计算
			ADC_SetSta( 5 , 3000 , -400 , g_coll_data[TEMP2_DATA_INDEX].data_value );
			break;}
		case 0x30:{//剩余电流传感器,通道1
			if(!(g_sys_param.shield.sy_curr & 0x01))break;//不进行采集和计算
			s_adc_collect_flag = 1;
			while(s_adc_collect_flag);
			g_camp_value >>= ADC_SAMPLING_MOVE;//大于600则为丢失
			ADC_SetSta( 7 , 0x600 , 0x100 , g_camp_value);
			break;}
		default:{//其他
			SENSER_CHECK_CLOS;									//关闭开关
			return;}
	}
	if(1){//本地操作
		if(((old_sta & 0x3FF) != (g_senser_flag & 0x3FF)) && ((g_senser_flag & 0x3FF) != 0))
		{
			LED_Control( L_ERROR , OPEN);
		}else if(((old_sta & 0xFFC00) != (g_senser_flag & 0xFFC00)) && (!g_nb_error_flag))
		{
			LED_Control( L_ERROR , CLOS);
		}
	}
	SENSER_CHECK_CLOS;									//关闭开关
}

/*****************************************************************************
 * 函数功能:		数据采集
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-20				函数编写
							2018-03-29				函数优化
							2018-05-10				报警后,如果报警消除则立即上传数据
							2018-06-07				函数重写
							2018-07-17				修复bug;针对电流的adc采样进行分段调整,输入矫正值由0.30141更改为0.2261
							2018-07-19				函数优化;针对校准程序的使用,本函数添加返回值,反回原始采样值
							2018-07-23				函数优化;在结束位置清零采样标记,并复位定时器
							2018-07-26				功能添加;在采样前判断传感器状态(是否丢失,是否被屏蔽)
 ****************************************************************************/
double ADC_Collection( uint8_t cmd )
{
	double re_value    = 0.0;
	static uint8_t cnt = 0;
	uint8_t tmp        = 0;
	
	if( cmd == 0xFF )
		tmp = cnt++;//此处先赋值,再自加
	else tmp = cmd;
	s_camp_cnt				= 0;						    //采样次数清零
	switch(tmp)
	{
		case 0:{//电流1数据采集(采集1024次)
			if((!(g_sys_param.shield.curr & 0x01)) && (cmd == 0xFF)){cnt++;break;}//不进行采集和计算
			ADC_DataRead( CHANNEL_3 , 1 , &g_camp_value);
			break;}
		case 1:{//电流数据读取和计算
			re_value = ADC_DataSave( 1 , &g_camp_value , 0.2261 , part_curr , &cnt);
			if(re_value <= 5){}//开始判断传感器是否丢失
			break;}
		case 2:{//电流2数据采集(采集1024次)
			if((!(g_sys_param.shield.curr & 0x02)) && (cmd == 0xFF)){cnt++;break;}//不进行采集和计算
			ADC_DataRead( CHANNEL_0 , 6 , &g_camp_value);
			break;}
		case 3:{//电流2数据读取
			re_value = ADC_DataSave( 6 , &g_camp_value , 0.2261 , part_curr , &cnt);
			if(re_value <= 5){}//开始判断传感器是否丢失
			break;}
		case 4:{//电流3数据采集(采集1024次)
			if((!(g_sys_param.shield.curr & 0x04)) && (cmd == 0xFF)){cnt++;break;}//不进行采集和计算
			ADC_DataRead( CHANNEL_2 , 7 , &g_camp_value );
			break;}
		case 5:{//电流3数据读取
			re_value = ADC_DataSave( 7 , &g_camp_value , 0.2261 , part_curr , &cnt);//2261
			if(re_value <= 5){}//开始判断传感器是否丢失
			break;}
		case 6:{//剩余电流数据采集(采集1024次)
			if((!(g_sys_param.shield.sy_curr & 0x01)) && (cmd == 0xFF)){cnt++;break;}//不进行采集和计算
			ADC_DataRead( CHANNEL_NULL , 2 , &g_camp_value );
			break;}
		case 7:{//剩余电流数据读取
			re_value = ADC_DataSave( 2 , &g_camp_value , 5.1859 , part_sycu , &cnt);//5.1859    6.7967
			if(re_value <= 5){ADC_SenserCheck(0x30);}//开始判断传感器是否丢失
			else g_senser_flag &=~ 0x1000;//传感器正常
			break;}
		case 8:{//电压数据采集(采集1024次)
			if((!(g_sys_param.shield.volat & 0x01)) && (cmd == 0xFF)){cnt++;break;}//不进行采集和计算
			ADC_DataRead( CHANNEL_1 , 3 , &g_camp_value );
			break;}
		case 9:{//电压数据读取
			re_value = ADC_DataSave( 3 , &g_camp_value , 2.26056 , part_volt , &cnt);
			break;}
		case 10:{//温度1数据采集(采集1024次)
			if((!(g_sys_param.shield.temp & 0x01)) && (cmd == 0xFF)){cnt++;break;}//不进行采集和计算
			ADC_DataRead( CHANNEL_NULL , 4 , &g_camp_value );
			break;}
		case 11:{//温度1数据读取
			re_value = ADC_DataSave( 4 , &g_camp_value , 0.0 , part_temp , &cnt);
			ADC_SenserCheck(0x20);//开始判断传感器是否丢失
			break;}
		case 12:{//温度2数据采集(采集1024次)
			if((!(g_sys_param.shield.temp & 0x02)) && (cmd == 0xFF)){cnt++;break;}//不进行采集和计算
			ADC_DataRead( CHANNEL_NULL , 5 , &g_camp_value );
			break;}
		case 13:{//温度2数据读取
			re_value = ADC_DataSave( 5 , &g_camp_value , 0.0 , part_temp , &cnt);
			ADC_SenserCheck(0x21);//开始判断传感器是否丢失
			break;}
		/*case 14:{//温度3数据采集(采集1024次)
			if((!(g_sys_param.shield.temp & 0x04)) && (cmd == 0xFF)){cnt++;break;}//不进行采集和计算
			ADC_DataRead( CHANNEL_NULL , 8 , &g_camp_value );
			break;}
		case 15:{//温度3数据读取
			re_value = ADC_DataSave( 8 , &g_camp_value , 0.0 , part_temp , &cnt);
			if(re_value <= -400){ADC_SenserCheck(0x22);}//开始判断传感器是否丢失
			break;}*/
		default:{//采样完成
			cnt = 0;
			g_adc_get_flag = 0;       //标志位清零
			ADC_SenserCheck(0xFF);    //故障信息清除
			CampTime_Updeat(&g_sys_param.camp_time);//采样时间更新
			break;}
	}
	if(g_wait_flag==1)cnt--;
	return re_value;
}

/*****************************************************************************
 * 函数功能:		数据校准
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-04-26				函数编写
							2018-05-08				温度校准部分bug的修复
 ****************************************************************************/
void ADC_Cail( void )
{
	int16_t  temp = 0;
	double camp_value = 0;
		
	switch(g_cail_data.type)
	{
		case part_curr:{//电流传感器校准
				ADC_Collection(0);//电流数据采集
				camp_value = ADC_Collection(1);//电流数据读取
				temp = (uint16_t)(camp_value / g_cail_data.value*1000);
				if(temp <= 100)//传感器没有接入
				{
					g_usart_errortype = 0x01;//传感器丢失
					g_usart_errorvalu = 0x02;//电流传感器
					USART_CRT_FunAdd(USART_SendError);
				}else if((temp >= 2000) || (temp < 800))
				{
					g_usart_errortype = 0x02;//传感器故障
					g_usart_errorvalu = 0x02;//电流传感器
					USART_CRT_FunAdd(USART_SendError);
				}else
				{
					g_sys_param.updat_flag = 1;
					g_sys_param.cali.curr = temp;
					g_cail_type = 4;
					USART_CRT_FunAdd(USART_SendCail);
				}
				break;}
		case part_sycu:{//剩余电流传感器校准
				ADC_Collection(6);//剩余电流数据采集
				camp_value = ADC_Collection(7);//剩余电流数据读取
				temp = (uint16_t)(camp_value / g_cail_data.value*1000);
				if((temp < 800) && (temp > 0) && (temp > 2000))
				{
					g_usart_errortype = 0x02;//传感器故障
					g_usart_errorvalu = 0x04;//剩余电流传感器
					USART_CRT_FunAdd(USART_SendError);
				}else if(temp == 0)
				{
					g_usart_errortype = 0x01;//传感器丢失
					g_usart_errorvalu = 0x04;//剩余电流传感器
					USART_CRT_FunAdd(USART_SendError);
				}else
				{
					g_sys_param.updat_flag = 1;
					g_sys_param.cali.curr = temp;
					g_cail_type = 6;
					USART_CRT_FunAdd(USART_SendCail);
				}
			break;}
		case part_temp:{//温度校准
				ADC_Collection(10);//温度数据采集
				camp_value = ADC_Collection(11);//温度数据读取
				temp = (int16_t)(camp_value / g_cail_data.value*1000);
				if(g_cail_data.value == 387)
				{
					if((temp >= 2000) || ((temp > 0) && (temp <= 800)))//设定值大于检测值10倍/设定值小于检测值10倍为传感器错误
					{
						g_usart_errortype = 0x02;//传感器故障
						g_usart_errorvalu = 0x01;//温度传感器
						USART_CRT_FunAdd(USART_SendError);
					}else if(temp <= 0)
					{
						g_usart_errortype = 0x01;//传感器丢失
						g_usart_errorvalu = 0x01;//温度传感器
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
						g_usart_errortype = 0x02;//传感器故障
						g_usart_errorvalu = 0x01;//温度传感器
						USART_CRT_FunAdd(USART_SendError);
					}else if(temp <= 0)
					{
						g_usart_errortype = 0x01;//传感器丢失
						g_usart_errorvalu = 0x01;//温度传感器
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
						g_usart_errortype = 0x02;//传感器故障
						g_usart_errorvalu = 0x01;//温度传感器
						USART_CRT_FunAdd(USART_SendError);
					}else if(temp <= 0)
					{
						g_usart_errortype = 0x01;//传感器丢失
						g_usart_errorvalu = 0x01;//温度传感器
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
		case part_volt:{//电压校准
				ADC_Collection(8);//电压数据采集
				camp_value = ADC_Collection(9);//电压数据读取
				//校准比例 =  检测值 / 设定值   这样 输出值 = 检测值 / 校准比例
				temp = (uint16_t)(camp_value / g_cail_data.value*1000);
				if(((temp < 800) && (temp > 0)) || (temp >= 2000))//采样值太小,采样值过大,传感器问题
				{
					g_usart_errortype = 0x02;//传感器故障
					g_usart_errorvalu = 0x03;//电压传感器
					USART_CRT_FunAdd(USART_SendError);
				}else if(temp == 0)//无采样数据,没有接传感器
				{
					g_usart_errortype = 0x01;//传感器丢失
					g_usart_errorvalu = 0x03;//电压传感器
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
