/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz,王泉凯
 *修改日期:	2018/03/08
 *说    明:	2018-03-08					文档移植
*******************************************************************************************/
#ifndef __TRANSMISSIN_H
#define __TRANSMISSIN_H
/* 头文件	--------------------------------------------------------------------*/
#include "com.h"
/* 宏定义	--------------------------------------------------------------------*/
#define QUEUE_SIZE										20							//队列大小
#define QUEUE_DAT_MAX									0x3FF						//0-0x3FF
#define QUEUE_DAT_SIZE                1024						//队列数据的最大值
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
extern uint8_t 				Build_Net_Data_Flag;						//数据发送标志							0：已经发送到模块 可以开始超时计数	1：未发送
extern uint8_t				g_queue_idex_s;									//读取位移
extern uint8_t				g_queue_idex_e;									//读取位移
extern uint8_t				g_nb_net_buff[QUEUE_DAT_SIZE];	//存储缓存                       最多1K的内存
extern uint8_t				g_up_config_flag;								//获取配置信息的标志
extern uint16_t 			Serial_Number;									//流水号
extern uint16_t				g_nb_net_buf_end;								//NB存储缓存中数据的结束位置
extern uint16_t				g_nb_net_buf_start;							//NB存储缓存中数据的起始位置
extern net_app_typedef g_net_app_queue[QUEUE_SIZE];		//网络应用APP
extern app_type				g_app_type;											//当前数据类型
extern Time_TypeDef_t Time;														//系统时间
/* 外部接口	------------------------------------------------------------------*/
void Server_SendData(uint16_t* num , cmd_type cmd_name ,uint8_t data_type , uint8_t msg_len);//往服务器发送数据
void net_to_module(void);															//将网络发送缓存中的数据发送到输出模块
#endif /* __RTC_H end */
/*------------------------------File----------End------------------------------*/
