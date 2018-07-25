/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz,王泉凯
 *修改日期:	2018/03/07
 *说    明:	2018-03-07					文档编写
*******************************************************************************************/
#ifndef __CONFIGURE_H
#define __CONFIGURE_H
/* 头文件	--------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "com.h"
/* 宏定义	--------------------------------------------------------------------*/
#define NB_TX_LEN           512*2+50    //定义NB发送到模块最大长度[实际数据长度=256字节](AT+NSOST=0,192.53.100.53,5683,2,4002)

#define YIDONG                        0x01
#define LIANTONG                      0x02
#define DIANXIN												0x03

/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
extern uint8_t	g_nb_newdata_flag;						//新数据标志 
extern uint8_t	g_nb_error_flag;				      //nbiot模块故障标志
extern char MODLE_VER[20];										//固件版本
extern char NB_Ope;                           //电信
extern param_typedef g_sys_param;             //系统参数
extern uint16_t g_nb_rx_cnt;									//接收计数
extern uint8_t  END[2];                       //使用结束符
extern uint8_t	g_rec_time;									  //回复消息的超时时间,如果时间超过了30s,则不查询接收,如果小于30s则每2s查询一次接收
extern uint8_t g_nb_reset_flow;	              //初始化流程
extern uint16_t NB_Handle_TX_CNT;				      //NB发送待处理的 长度 到模块
extern uint8_t socket;												//socket号
extern char MODLE_NAME[20];										//模块型号
extern uint8_t g_model_config_flag;						//模块初始化标志
/* 外部接口	------------------------------------------------------------------*/
void runstate_to_usart(char* cmd);											//发送到上位机
void network_parameterUpdata(void);											//更新网络参数(字符串转成数字)
void int_to_char(char* s0 , uint8_t* s1 , uint16_t len);//数字转换成字符串
void network_parameter_flashRead(void);									//初始化网络参数
ErrorStatus IOT_ATsend(uint8_t *p , uint16_t len , char *c);//发送AT指令
void IOT_onlyATsend(uint8_t *p , uint16_t len);         //发送AT指令
ErrorStatus Get_ChipID( uint8_t *cnt);																	//获取设备ID
ErrorStatus strbj(char *a,char *b,uint16_t len, uint16_t *k);//字符串搜索函数。添加字符长度
ErrorStatus NB_send(uint8_t *ip , u16 port , uint8_t *p , uint16_t len);
void Num_exchange(uint32_t num , uint8_t *p , uint16_t* len);

ErrorStatus NB_WaitStartMessage(char *str , uint8_t* re_cnt);
ErrorStatus NB_ReadMoudleVer(char* cmd , uint16_t length , char *str , uint8_t *re_cnt);
ErrorStatus NB_ReadMoudleIMEI(char *str , uint8_t* re_cnt);
ErrorStatus NB_ReadMoudleISMI(char *str , uint8_t* re_cnt);
ErrorStatus NB_ReadMoudleOperator(char *str , uint8_t* re_cnt);
ErrorStatus NB_SendCmd(char * cmd , uint16_t length, char* str, uint16_t *index , uint8_t* re_cnt);
ErrorStatus NB_ReadMoudleData(char *cmd , uint16_t length , char *str , uint16_t *str_index);
void NB_ReadMoudleCSQ(uint8_t *buff , char *str);
ErrorStatus NB_WaitData(char* str , uint16_t *index);
ErrorStatus NB_SendServerData(uint8_t *NB_Handle_TX_BUF , uint8_t *data , uint16_t len);
#endif /* __CONFIGURE_H end */
/*------------------------------File----------End------------------------------*/
