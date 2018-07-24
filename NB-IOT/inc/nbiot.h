/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz,王泉凯
 *修改日期:	2018/03/07
 *说    明:	2018-03-07					文档移植
*******************************************************************************************/
#ifndef __NBIOT_H
#define __NBIOT_H
/* 头文件	--------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* 宏定义	--------------------------------------------------------------------*/
/* 结构体定义	----------------------------------------------------------------*/
typedef struct{													//射频参数结构体
	ErrorStatus (*init)( uint8_t* cnt , uint8_t *flag );			//射频初始化
	ErrorStatus (*send)( uint8_t *data, uint16_t length );		//射频发送
	void        (*rece)( uint8_t* cnt );										  //射频接收
	void        (*read_csq)(uint8_t *buff );                   //信号强度 
	void        (*read_signal)( uint8_t *buff );						//读取发射功率
	void        (*read_pci)( uint8_t *buff );								//读取基站编号
	void        (*read_cellid)( uint8_t *buff );							//获取小区编号
	void        (*read_snr)( uint8_t *buff );									//获取信噪比
	void        (*reset)(void);															//复位
}Radio_s;
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
extern Radio_s radio;											//射频函数结构体
/* 外部接口	------------------------------------------------------------------*/
void Nbiot_reset(void);									//NBIOT模块复位
void Nbiot_Start(void);									//nb启动
void IOT_onlyATsend(uint8_t *p , uint16_t len);//AT仅发送消息
ErrorStatus NB_Send(uint8_t *data, uint16_t length);
#endif /* __NBIOT_H end */
/*------------------------------File----------End------------------------------*/
