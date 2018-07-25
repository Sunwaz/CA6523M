/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz,王泉凯
 *修改日期:	2018-03-07					文档移植
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "nbiot.h"
#include "bc95.h"
#include "me3616.h"
#include "NB89.h"

#include "delay.h"
#include "key_led.h"
#include "com.h"
#include "configure.h"

#include "string.h"
#include "stdlib.h"
#include "usart.h"
/* 宏定义	--------------------------------------------------------------------*/

#define  delay_nms										delay_ms
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
ErrorStatus sys_app(app_type type);
ErrorStatus IOT_Portcreat(void);													//iot的socket端口创建
/* 全局变量	------------------------------------------------------------------*/
uint8_t  Device_ID[6]         = {0,0,0,0,2,9};						//本机网关
static uint8_t s_old_time     = 0;												//旧的时间
Radio_s radio                 = {0};											//射频函数结构体
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		模块信息读取
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-06-19				函数移植
 ****************************************************************************/ 
void moudle_msg_read( void )
{
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t flag = 0;
	
	for(j = 0;j < 3;i++)
	{
		IOT_onlyATsend((uint8_t*)"AT+CGMM" , strlen("AT+CGMM"));
		for(i = 0;i < 20;i++)
		{
			NB_ReceiveData();
			if(g_nb_newdata_flag)
			{
				if(1){//模块为BC95
					if(!flag)
					{
						if(strstr((char*)g_nb_rx_buff , "BC95HB-02-STD"))
						{
							memcpy(MODLE_NAME , "BC96" , strlen("BC95"));
							radio.init        = BC95_init;
							radio.send        = BC95_SendData;
							radio.rece        = BC95_ReceiveData;
							radio.reset       = BC95_Reset;
							radio.read_csq    = BC95_CSQ;
							radio.read_signal = BC95_ReadSignal;
							radio.read_pci    = BC95_ReadPCI;
							radio.read_cellid = BC95_ReadCellID;
							radio.read_snr    = BC95_ReadSNR;
							return;
						}
					}
				}
				if(2){//模块为ME3616
					if(!flag)
					{
						if(strstr((char*)g_nb_rx_buff , "ME3616"))
						{
							memcpy(MODLE_NAME , "ME3616" , strlen("ME3616"));
							radio.init        = ME3616_init;
							radio.send        = ME3616_SendData;
							radio.rece        = ME3616_ReceiveData;
							radio.reset       = ME3616_Reset;
							radio.read_csq		= ME3616_CSQ;
							radio.read_signal = ME3616_ReadSignal;
							radio.read_pci		= ME3616_ReadPCI;
							radio.read_cellid = ME3616_ReadCellID;
							radio.read_snr		= ME3616_ReadSNR;
							return;
						}
					}
				}
				if(3){//模块NB86
					if(!flag)
					{
						if(strstr((char*)g_nb_rx_buff , "LSD4NBN-150H01"))
						{
							memcpy(MODLE_NAME , "NB86" , strlen("NB86"));
							radio.init        = NB86_init;
							radio.send        = BC95_SendData;
							radio.rece        = BC95_ReceiveData;
							radio.reset       = BC95_Reset;
							radio.read_csq    = BC95_CSQ;
							radio.read_signal = BC95_ReadSignal;
							radio.read_pci    = BC95_ReadPCI;
							radio.read_cellid = BC95_ReadCellID;
							radio.read_snr    = BC95_ReadSNR;
							return;
						}
					}
				}
			}
		}
	}
	if(j >= 3)
	{
		runstate_to_usart("模块信息读取失败\r\n");
	}
}

/*****************************************************************************
 * 函数功能:	应用初始化
 * 形式参数:	无
 * 返回参数:	无
 * 更改日期;	2018-03-08				函数移植
							2018-05-11				平台主动获取配置信息,屏蔽掉原有的函数 up_configmessage 
							2018-05-11				添加 上传启动信息
 ****************************************************************************/ 
void Init_app(void)
{
	sys_app( type_upload_startup );    //上传一次启动信息
	sys_app( type_upload_config );     //上传一次配置信息
	sys_app( type_runing );            //上传一次运行信息
}
/*****************************************************************************
 * 函数功能:	nb硬件重启
 * 形式参数:	无
 * 返回参数:	SUCCESS 重启成功	ERROR 重启失败
 * 更改日期;	2018-03-07				函数移植
							2018-03-08				将硬件复位进行了修改(替代成 BC95_HardwareReset() )
							2018-06-04				修复bug;初始化流程顺序有问题
							2018-06-19				修复bug;如果发送失败,重新开机导致不断的复位模块,因为没有加时间
							2018-06-29				函数优化;在重启之间,增加了模块的软件复位,在软件复位后10s进行硬件复位,开始复位流程
																函数优化;在重启之前,增加了模块的识别,实现模块的热插拔,如果模块的IMEI号码被改变,会上报变化情况(以变化后的网关号上报)
							2018-07-24				函数优化;在使用无线的函数指针的时候,如果函数指针为空的时候,不执行函数,如果执行初始化函数为空,则重新获取模块信息
 ****************************************************************************/ 
void Nbiot_reset(void)
{
	static uint8_t cnt        = 1;
	static uint8_t reset_num  = 0;//重启次数
	static uint8_t reset_time = 0;//复位计时
	static uint8_t power_time = 0;
	uint8_t flag = 0;

	if(g_nb_error_flag || (!g_model_config_flag))//如果nb设备错误,或者模块没有初始化过,则复位模块
	{
		LED_Control(L_SIGNAL , OPEN );
		if((reset_num < 3) )
		{
			if((g_sys_tim_s & 0xFF) != s_old_time)
			{
				if(1){//模块超时/初始状态的设置
					if((g_nb_reset_flow == 1) && (cnt == 1))
					{
						if(!power_time)
						{
							power_time = g_sys_tim_s + 10;									//软件复位10s后才开始硬件复位
							if(power_time == 0)power_time = 1;
							if(radio.reset !=  NULL)radio.reset();																	//软件复位
							return;																					//复位后则退出,不继续执行后面的程序
						}else if((g_sys_tim_s & 0xFF) >= power_time)
						{
							NB_OpenPower();																	//定时时间到,NB模块开机
							moudle_msg_read();															//读取模块信息
							NB_HardwareReset();															//模块复位
							power_time = 0;																	//计时清零,等待下次软件复位,开始执行正式的复位流程
						}else
						{
							return;																					//定时时间没有到达,不继续执行后面的程序
						}
					}
				}
				if(2){//函数指针判断
					if(radio.init == NULL)															//如果本函数指针为空,则重新获取模块信息
					{
							NB_OpenPower();																	//定时时间到,NB模块开机
							moudle_msg_read();															//读取模块信息
					}
				}
				if(3){//模块初始化流程
					if(radio.init(&cnt , &flag) == SUCCESS)
					{
						if(flag)
						{
							cnt = 1;
							g_nb_error_flag = 0;                           //重新初始化成功,则复位标志
							if(!g_model_config_flag)
							{
								g_model_config_flag = 1;
								Init_app();
							}
							LED_Control(L_SIGNAL , CLOS);
						}else
						{
							cnt++;																				//初始化次数增加
						}
					}else
					{
						cnt = 1;																				//计数复位,流程重新开始
						reset_num++;
						LED_Control( L_ERROR , OPEN );									//打开故障灯
						if(reset_num == 3)
						{
							reset_time = g_sys_tim_s + 40;						     //如果复位超过3次都还不成功,则40s后再复位
							if(reset_time == 0)reset_time = 1;
						}
					}
					s_old_time = g_sys_tim_s & 0xFF;										//1s钟执行一个流程
				}
			}
			
		}else
		{
			if(reset_time && ((g_sys_tim_s & 0xFF) >= reset_time))//40s定时时间到,重新开始流程执行
			{
				cnt             = 1;
				g_nb_reset_flow = 1;
				reset_num       = 0;
				reset_time      = 0;
			}
		}
	}
}
/*****************************************************************************
 * 函数功能:		NB发送数据
 * 形式参数:		无
 * 返回参数:		SUCCESS 重启成功		ERROR 重启失败
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/ 
ErrorStatus NB_Send(uint8_t *data, uint16_t length)
{
	ErrorStatus state;									//发送状态
 
	LED_Control(L_SIGNAL , OPEN);
	state=radio.send(data,length);	             //发送到NBIOT
	if(state==SUCCESS)runstate_to_usart("module send success!\r\n");
	else runstate_to_usart("module send failed!\r\n");
	LED_Control(L_SIGNAL , CLOS);
	
	return state;
}
/*------------------------------File----------End------------------------------*/
