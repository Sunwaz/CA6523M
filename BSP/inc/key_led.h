/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-09					文档编写
 *说    明:	
*******************************************************************************************/
#ifndef __KEY_LED_H
#define __KEY_LED_H
/* 头文件	--------------------------------------------------------------------*/
#include <stm32f0xx.h>
/* 宏定义	--------------------------------------------------------------------*/
#define NEW_MODEL

//报警标志位的操作
#define VOLAT_ALARM                   0x01				//电压
#define CURR_1_ALARM									0x02				//电流1                VOLAT_ALARM    << 1       
#define CURR_2_ALARM									0x04				//电流2                CURR_1_ALARM   << 1
#define CURR_3_ALARM                  0x08				//电流3                CURR_2_ALARM   << 1
#define TEMP_1_ALARM									0x10				//温度1                CURR_3_ALARM   << 1
#define TEMP_2_ALARM                  0x20				//温度2                TEMP_1_ALARM   << 1
#define TEMP_3_ALARM									0x40				//温度3                TEMP_2_ALARM   << 1
#define SY_CURR_ALARM									0x80				//剩余电流             TEMP_3_ALARM   << 1
#define RESERVE_1_ALARM								0x100				//预留                 SY_CURR_ALARM  << 1
#define RESERVE_2_ALARM								0x200				//预留                 RESERVE_1_ALARM<< 1
#define ALARM_RECOVERY                0x40000000  //火警恢复
#define ALARM_RESET                   0x80000000  //火警复位
#define LOCAL_ALARM                   0x00				//本地报警
#define ALARM_SILENCE									0x0A				//报警消音
#define ALARM_UPLOAD									0x14				//报警上传
#define READ_VOLAT                    0x100401    //读取电压报警数据        
#define READ_CURR_1										0x200802    //读取电流1报警数据       READ_VOLAT  << 2
#define READ_CURR_2										0x401004    //读取电流2报警数据       READ_CURR_1 << 2
#define READ_CURR_3                   0x802008    //读取电流3报警数据       READ_CURR_2 << 2
#define READ_TEMP_1                   0x1004010   //读取温度1报警数据       READ_CURR_3 << 2
#define READ_TEMP_2                   0x2008020   //读取温度2报警数据       READ_TEMP_1 << 2
#define READ_TEMP_3										0x4010040   //读取温度3报警数据       READ_TEMP_2 << 2
#define READ_SY_CURR                  0x8020080   //读取剩余电流报警数据    READ_TEMP_3 << 2
#define READ_RESERVE_1								0x10040100  //读取预留报警数据        READ_SY_CURR<< 2
#define READ_RESERVE_2                0x20080200  //读取预留报警数据        READ_RESERVE_1<<2
#define READ_ALARM                    0x3FF       //读取报警数据            
#define READ_SILENCE                  0xFFC00     //读取消音数据            READ_ALARM  << 10
#define READ_UPLOAD                   0x3FF00000  //读取上报数据						 READ_SILENCE<< 10
/* 结构体定义	----------------------------------------------------------------*/
typedef enum{														//LED类型
	L_WORK					=	0,									//工作指示灯
	L_ALARM					= 1,									//报警指示灯
	L_ERROR					= 2,									//故障指示灯
	L_CLEAR					=	3,									//消音指示灯
	L_ALL						=	4,									//所有指示灯
	L_SIGNAL        = 5,									//信号指示灯
}LED_Typedef;
typedef enum{														//LED控制类型
	OPEN						=	0,									//打开
	CLOS						=	1,									//关闭
}LED_Ctl_Typedef;
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
extern uint8_t	g_sys_error_flag;				//系统故障标志位    0,无故障,LED正常闪烁 1,有故障,LED保持当前状态
extern uint8_t	g_reset_flag;						//复位标志
extern uint32_t	g_alarm_flag;						//报警标志位
extern uint16_t g_sys_operation_msg;		//bit0 设备复位 bit1 设备消音 bit2 手动报警 bit3 警情消除 bit4 自检 bit5 确认 bit6 测试 bit7 SIM卡被更改 bit8 设备掉电
/* 外部接口	------------------------------------------------------------------*/
void KeyLED_Config( void );							//按键和LED 外设初始化
void LED_TimerIrqHandle( void );				//定时器中断服务函数
void LED_Control( LED_Typedef Led , LED_Ctl_Typedef State );//LED灯控制函数
void Key_Scan( void );									//按键扫描
void Alarm_Set( void );									//报警
void Alarm_Reset( void );								//报警恢复
#endif /* __KEY_LED_H end */
/*------------------------------File----------End------------------------------*/
