/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-13					文档编写
 *说    明:	
*******************************************************************************************/
#ifndef __com_H
#define __com_H
/* 头文件 ------------------------------------------------------------------*/
#include <stm32f0xx.h>
#include "rtc.h"
#include "string.h"
#include "adc_get.h"
/* 宏定义 ------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////
#define CURR_RANGE_IN                 (int16_t)1000  				 //电流互感器一次侧最大电流
#define CURR_RANGE_OUT                (int16_t)1200  				 //电流互感器二次侧最大电流
#define SY_CURR_RANGE_IN              (int16_t)10000				 //剩余电流互感器一次侧最大电流
#define SY_CURR_RANGE_OUT             (int16_t)500					 //剩余电流互感器二次侧最大电流
#define TEMP_MAX										  (int16_t)3050					 //最高可测温度
#define TEMP_MIN										  (int16_t)-500					 //最低可测温度
#define VOLAT_MAX										  (int16_t)5000					 //最高可测电压

#define SYS_VER                       "CA6523M-V101-20180725"//程序版本号

#define SYS_FLAG										  0x80									 //智慧用电系统

#define TRANS_BUFFER_MAX						  512						         //接收控制器串口数据缓存大小
#define NET_TRANS_MAX								  512									   //网络透传数据缓存大小

#define IP_LEN						            4									     //IPV4 如果是IPV6 则此处为6

#define PROTOCOL_NET_STATE_OK				  0									     //解析成功
#define PROTOCOL_NET_STATE_SHORT		  1									     //传入数据过短，小于最小长度
#define PROTOCOL_NET_STATE_LONG			  2									     //命令读取的长度过长
#define PROTOCOL_NET_STATE_HEAD			  3									     //头错误
#define PROTOCOL_NET_STATE_TAIL			  4									     //尾错误
#define PROTOCOL_NET_STATE_CHECK		  5									     //和校验错误
#define PROTOCOL_NET_STATE_LENGTH		  6									     //已接收长度小于读取的命令长度

#define PROTOCOL_NET_HEAD             '@'
#define PROTOCOL_NET_TAIL             '#'

#define PROTOCOL_NET_MIN		  			  30										 //网络数据长度的最小值
#define PROTOCOL_NET_MAX		          512										 //网络通讯协议长度

#define RECV_NET_BUFFER_MAX					  256										 //接收长度

#define NB_NET_RECVLEN_MAX					  512										 //NB最终接收长度
#define NB_NET_SENDLEN_MAX					  1024									 //NB最终发送长度

#define OBJ_LENGTH_SYS							  1									     //读取时一个系统消息对象长度
#define OBJ_LENGTH_PART							  5									     //读取时一个部件消息对象长度

//制单元命令字节定义表
#define NET_NULL_CMD									0x00				//空
#define NET_CONTROL_CMD								0x01				//控制命令    (时间同步)
#define NET_SEND_DATA                 0x02				//上传数据    (发送火灾报警和建筑消防设施运行状态等信息)
#define NET_OK       								  0x03				//确认帧      (对控制命令和发送信息的确认回答)
#define NET_GET_DATA									0x04				//请求数据    (查询火灾报警和建筑消防设施运行状态等信息)
#define NET_ACK												0x05				//应答帧				(返回查询的信息)
#define NET_DENY											0x06				//否认				(对控制命令和发送信息的否认回答)
#define NET_STARTUP                   0x08        //启动消息
#define NET_UPLOAD_CONFIG							0x80				//主动上报系统配置参数命令
#define NET_SET_PARAM     						0x81				//设定系统参数命令
#define NET_GET_CONFIG								0x82				//获取配置信息
#define NET_APP_RESET									0x0B				//设备重启
#define NET_SEVER_REGIST							0x84				//服务器注册

//数据类型标志
#define NET_NULL_DATA									0x00				//空数据
#define NET_UPLOAD_STATE              0x02				//上传建筑消防设施部件运行状态
#define NET_UPLOAD_ANLOG							0x03				//上传建筑消防设施部件模拟量值
#define NET_OPERA_INFOR               0x04        //上传建筑消防设施操作信息
#define NET_UPLOAD_CFG								0x1A				//上传用户信息传输装置配置情况
#define NET_DOWNLOAD_CFG							0x80				//下传装置配置
#define NET_UPLOAD_RUNING             0x84        //上传运行信息
#define NET_UPLOAD_STARTUP            0x85        //上传启动信息
#define NET_UPLOAD_RECOVER						0xA2				//上传恢复状态

//配置参数类型
#define IP_ADDR                       0x01        //配置类型 IP地址
#define HEART_TIME										0x05				//配置类型 心跳时间
#define SAMPLING_TIME                 0x06				//配置类型 采样时间
#define CURR_RANGE										0x0B				//配置类型 电流互感器量程
#define SY_CURR_RANGE                 0x0C				//配置类型 剩余电流互感器量程
#define TEMP_RANGE										0x0D				//配置类型 温度传感器量程
#define CURR_THRESHOLD								0x0E				//配置类型 电流阈值
#define SY_CURR_THRESHOLD             0x0F				//配置类型 剩余电流阈值
#define TEMP_THRESHOLD                0x10				//配置类型 温度阈值
#define VOLAT_THRESHOLD               0x11				//配置类型 电压阈值
#define TEMP_HUMI_THRESHOLD           0x12				//配置类型 温湿度阈值
#define VOLAT_RANGE										0x13				//配置类型 电压范围

//启动消息参数类型
#define MODULAR_TYPE									0x40				//模块类型 0x08 表示NB模块
#define OPERATOR                      0x41        //运营商   0x01 移动 0x02 联通 0x03电信 0x04 移动或联通
#define CODE_IEME                     0x42        //模块IEME码       //用于向产商提供模块编号
#define SIGNAL_CSQ                    0x43        //信号强度
#define SINGAL_POWER                  0x44				//信号功率
#define PCI_NUM                       0x45        //基站编号
#define CELL_ID                       0x46				//小区编号
#define SIGNAL_SNR                    0x47        //信噪比
#define CODE_IMSI                     0x48        //SIM卡IMSI码      //用于查话费

#define SYS_VERSION                   0xA0        //系统版本         //例如 CA6523M-V101-20180510
#define MODULAR_VERSION               0xA1        //模块固件版本     //例如 BC95B5HBR01A02W16
//消防状态
#define CURR_1_STATE									0x80				//探测器1					//电流探测器1
#define CURR_2_STATE                  0x81				//探测器2					//电流探测器2
#define CURR_3_STATE									0x82				//探测器3					//电流探测器3
#define VOLAT_1_STATE									0x83				//探测器4					//电压探测器1
#define VOLAT_2_STATE									0x84				//探测器5					//电压探测器2
#define VOLAT_3_STATE									0x85				//探测器6					//电压探测器3
#define TEMP_1_STATE									0x86				//探测器7					//温度探测器1
#define TEMP_2_STATE									0x87				//探测器8					//温度探测器2
#define TEMP_3_STATE									0x88				//探测器9					//温度探测器3
#define SY_CURR_1_STATE								0x89				//探测器10					//剩余电流探测器1


#pragma pack(1)
/* 结构体声明	--------------------------------------------------------------*/
typedef enum{																		 //数据类型
	type_null_cmd					=	NET_NULL_CMD,					 //空
	type_send_dat_cmd			=	NET_SEND_DATA,				 //上传数据
	type_ok_cmd						= NET_OK,								 //确认帧
	type_get_dat_cmd			=	NET_GET_DATA,					 //请求数据
	type_ack_cmd					=	NET_ACK,							 //应答帧	
	type_deny_cmd					=	NET_DENY,							 //否认
	type_upload_cmd				=	NET_UPLOAD_CONFIG,		 //主动上报系统配置参数命令
	type_set_param_cmd		=	NET_SET_PARAM,				 //设定系统参数命令
	type_sys_reset_cmd		=	NET_APP_RESET,				 //系统重启
	type_server_regis_cmd = NET_SEVER_REGIST,			 //服务器注册
	type_get_config_cmd		=	NET_GET_CONFIG,				 //获取配置信息
	type_startup_cmd      = NET_STARTUP,           //启动消息
}cmd_type;
typedef enum{																		 //应用数据类型
	type_null_dat					=	NET_NULL_DATA,				 //空数据
	type_send_anlog       = NET_UPLOAD_ANLOG,			 //发送模拟数据
	type_upload_config    = NET_UPLOAD_CFG,				 //上传配置数据
	type_download_config  = NET_DOWNLOAD_CFG,			 //下传配置数据
	type_upload_startup   = NET_UPLOAD_STARTUP,    //上传启动数据
	type_runing           = NET_UPLOAD_RUNING,     //上传运行数据
	type_oper_info        = NET_OPERA_INFOR,			 //上传操作信息
	type_senser_info      = NET_UPLOAD_STATE,			 //上传设备运行状态
	type_senser_recover   = NET_UPLOAD_RECOVER,		 //上传设备恢复状态
}app_type;

typedef enum{																			//配置参数类型
	config_heart          = HEART_TIME,							//心跳时间配置
	config_camp           = SAMPLING_TIME,					//采样时间
	config_ip             = IP_ADDR   ,             //IP地址配置
	config_curr_r         = CURR_RANGE,             //电流互感器量程
	config_curr_t         = CURR_THRESHOLD,					//电流阈值
	config_sy_curr_r      = SY_CURR_RANGE,					//剩余电流互感器量程
	config_sy_curr_t      = SY_CURR_THRESHOLD,			//剩余电流阈值
	config_temp_r         = TEMP_RANGE,							//温度传感器量程
	config_temp_t         = TEMP_THRESHOLD,					//温度阈值
	config_volat_r        = VOLAT_RANGE,						//电压范围
	config_volat_t        = VOLAT_THRESHOLD,				//电压阈值
//	config_temp_humi_t    = TEMP_HUMI_THRESHOLD,		//环境温湿度阈值
}CONFIG;
#define CONFIG_TYPE_NUM					11          				//CONFIG 枚举的数量
typedef enum{																			//启动信息参数类型
	startup_IEMI          = CODE_IEME,              //IEMI码
	startup_operator      = OPERATOR,               //运营商
	startup_mouldtype     = MODULAR_TYPE,           //模块类型
	startup_sys_ver       = SYS_VERSION,            //系统软件版本号
	startup_mou_ver       = MODULAR_VERSION,        //模块固件版本号
}startup_para_type;
#define STARTUP_PARA_TYPE_NUM					5           //startup_para_type 枚举的数量
typedef enum{																			//运行状态参数类型
	runing_null           = 0,										  //空数据
	runing_signal_power   = SINGAL_POWER,           //信号强度
	runing_cell_id        = CELL_ID,								//小区编号
	runing_pci            = PCI_NUM,                //基站编号
	runing_snr            = SIGNAL_SNR,							//信噪比
	runing_csq            = SIGNAL_CSQ,             //信号强度
	runing_IMSI           = CODE_IMSI,							//ISMI码
}runing_state_type;
#define RUNING_STATE_TYPE_NUM         6           //runing_state_type 枚举的数量

typedef struct{																		//配置数据结构体
	CONFIG			config_type;												//参数类型
	uint8_t			leng;																//数据长度
	uint8_t      data[6];													  //数据					//带符号
}config_typedef;
typedef struct{																	  //网络命令结构体
	uint16_t  num;																  //流水号
	uint16_t	vision;															  //协议版本
	uint8_t		time[6];													  	//时间
	uint8_t		src_addr[6];											  	//源地址
	uint8_t		des_addr[6];										  		//目的地址
	uint16_t	length;												  			//数据长度
	cmd_type	cmd;												  			  //命令字
	uint8_t		data[512];										  			//数据
}Net_CmdType_t;
typedef struct{																		//校准数据结构体
	uint16_t volat;																	//电压校准数据
	uint16_t curr;																	//电流校准数据
	uint16_t sy_curr;																//剩余电流校准数据
	uint16_t temp[3];																//温度校准数据
}cail_typedef;
typedef struct{																		//屏蔽数据结构体
	uint8_t temp;																		//温度通道屏蔽       高两位预留,低6位为通道号
	uint8_t curr;																		//电流通道屏蔽       高两位预留,低6位为通道号
	uint8_t volat;																	//电压通道屏蔽       高两位预留,低6位为通道号
	uint8_t sy_curr;																//剩余电流通道屏蔽    高两位预留,低6位为通道号
}shield_typedef;
typedef struct{																		//系统参数结构体
	uint16_t    updat_flag;													//更新标志位
	uint16_t    camp_time;													//采样时间
	uint16_t    hart_time;													//心跳时间
	uint8_t     server_ip[6];												//服务器IP
	uint8_t     device_ip[6];												//设备终端IP
	uint8_t     IMEI[20];														//设备IEMI号
	uint8_t     IMSI[20];														//设备IMSI号
	uint16_t    port;																//端口号
	data_typedef threa;															//阈值数据
	cail_typedef cali;                              //校准数据
	shield_typedef shield;                          //屏蔽数据结构体
}param_typedef;
typedef struct{																		//网络应用队列
	app_type type;																	//类型
	uint16_t idex;																	//首位置
}net_app_typedef;
/* 枚举声明	----------------------------------------------------------------*/
/* 全局变量	----------------------------------------------------------------*/
extern uint8_t Device_ID[6];											//本机网关
extern uint8_t NET_outtime;												//服务器超时时间
extern uint32_t g_sys_tim_s;											//运行时间
/* 接口提供	----------------------------------------------------------------*/
#endif       /*__com_H end*/
/*------------------------------File----------End------------------------------*/
