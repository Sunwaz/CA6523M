/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-14					文档移植
 *说    明:	
*******************************************************************************************/
#ifndef __ADC_GET_H
#define __ADC_GET_H
/* 头文件 ------------------------------------------------------------------*/
#include <stm32f0xx.h>
#include "rtc.h"
/* 宏定义 ------------------------------------------------------------------*/
#define COMP_COEFFIC						1.00							//补偿系数
#define RES_COMP								-0.020						//电阻补偿系数

//部件类型
#define CURRENCY_PART                 0x00				//部件类型 通用
#define SY_CURR_DETECTOR							0x11				//部件类型 剩余电流探测器
#define TEMP_DETECTOR									0x12				//部件类型 测温式电气火灾监控探测器		
#define VOLAT_DETECTOR								0x80				//部件类型 电压探测器
#define CURR_DETECTOR									0x81				//部件类型 电流探测器
#define WSD_DETECTOR                  0x82				//部件类型 温湿度探测器
//数据类型
#define TEMP_DATA											0x03				//数据类型 温度
#define VOLAT_DATA                    0x08				//数据类型 电压
#define CURR_DATA											0x09				//数据类型 电流
#define SY_CURR_DATA									0x80				//数据类型 剩余电流
#define SIGNAL_DATA										0x81				//数据类型 信号强度
#define HUMI_DATA											0x82				//数据类型 湿度

#define CURR1_DATA_INDEX                          0x00
#define CURR2_DATA_INDEX                          0x01
#define CURR3_DATA_INDEX                          0x02
#define SY_CURR_DATA_INDEX                        0x03
#define VOLAT_DATA_INDEX                          0x04
#define TEMP1_DATA_INDEX                          0x05
#define TEMP2_DATA_INDEX													0x06
#define TEMP3_DATA_INDEX													0x07

#define CALL_SIZE                     0x0F        //保存20个数据
#define ADC_SIZE																	(sizeof(data_typedef)/sizeof(uint16_t))
#pragma pack(1)
/* 枚举声明	----------------------------------------------------------------*/
typedef enum{																		  //数据类型
	data_temp    = (uint8_t) TEMP_DATA,							//温度数据
	data_volat   = (uint8_t) VOLAT_DATA,						//电压数据
	data_curr    = (uint8_t) CURR_DATA,							//电流数据
	data_humi    = (uint8_t) HUMI_DATA,							//湿度数据
	data_signal  = (uint8_t) SIGNAL_DATA,						//信号强度
	data_sy_curr = (uint8_t) SY_CURR_DATA,					//剩余电流数据
}DATA;
typedef enum{																		  //部件类型
	part_temp = (uint8_t) TEMP_DETECTOR,						//温度探测器
	part_volt = (uint8_t) VOLAT_DETECTOR,						//电压探测器
	part_curr = (uint8_t) CURR_DETECTOR,						//电流探测器
	part_sycu = (uint8_t) SY_CURR_DETECTOR,					//剩余电流探测器
	part_ty   = (uint8_t) CURRENCY_PART,						//通用部件
}PART;
typedef enum{																		  //标志位信息
	bit0 = 0x01,																		//BIT0
	bit1 = 0x02,																		//BIT1
	bit2 = 0x04,																		//BIT2
	bit3 = 0x08,																		//BIT3
	bit4 = 0x10,																		//BIT4
	bit5 = 0x20,																		//BIT5
	bit6 = 0x40,																		//BIT6
	bit7 = 0x80,																		//BIT7
}FLAG_BITS;
/* 结构体声明	--------------------------------------------------------------*/
typedef struct{																		//数据类型
	uint16_t   curr1;																//电流值
	uint16_t   curr2;																//电流值
	uint16_t   curr3;																//电流值
	uint16_t   sy_curr;															//剩余电流值
	uint16_t   temp1;																//温度值
	uint16_t   temp2;																//温度值
//	uint16_t   temp3;																//温度值
	uint16_t   volat;																//电压值
}data_typedef;
typedef struct{
	PART         part_type;													//部件类型
	uint32_t     part_addr;													//部件地址				//低位在前 高位在后
	DATA         data_type;													//数据类型
	int16_t      data_value;							          //数据值					//带符号
	Time_TypeDef_t time;														//时间戳
}Msg_data;
typedef struct{																		//校准数据
	PART     type;																	//校准的传感器类型
	uint8_t  flag;																	//校准标志
	uint16_t value;																	//校准值
}adc_cail;
/* 全局变量	----------------------------------------------------------------*/
extern Msg_data g_coll_data[ADC_SIZE];	          //采集上来的数据
extern adc_cail g_cail_data;											//校准数据
extern uint16_t g_senser_flag;										//传感器状态
/* 接口提供	----------------------------------------------------------------*/
void ADC_GetConfig( void );												//ADC初始化
void ADC_DMAIRQnHandle( void );										//DMA中断服务函数
double ADC_Collection( uint8_t cmd );								//数据采集
void ADC_Cail( void );														//数据校准
#endif       /*__ADC_GET_H end*/
/*------------------------------File----------End------------------------------*/
