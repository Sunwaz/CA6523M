/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz,王泉凯
 *修改日期:	2018-03-08					文档移植
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "transmission.h"
#include "usart.h"
#include "rtc.h"
#include "timer.h"
#include "string.h"
#include "stdlib.h"
#include "bc95.h"
#include "nbiot.h"
#include "key_led.h"
#include "configure.h"
extern uint8_t g_reset_flag;
/* 宏定义	--------------------------------------------------------------------*/
#define SERIAL_NUMBER_0										2								//流水号高
#define SERIAL_NUMBER_1										3								//流水号低位
#define DATA_LEN_0												24							//数据长度高位
#define DATA_LEN_1												25							//数据长度低位
#define NET_CMD														26							//命令
#define TIME_DATA													6								//时间在缓存中的相对位置
#define CONFIG_SIZE												31							//配置信息的长度
#define	SEND_OutTime											40							//服务器超时时间   由于电信平台的时间延迟,比以前多加10s
#define	OUT_SEND_MAX 											4								//服务器不可发送重查最大次数
#define	OUT_POLL_MAX_TOP0  								10							//模块重传次数
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
uint8_t buildcmd_net(Net_CmdType_t *cmd);									//装载数据到缓存
/* 全局变量	------------------------------------------------------------------*/
static uint8_t  s_net_buff[PROTOCOL_NET_MAX] = {0};       //网络缓存									//使用同一个缓存
static uint8_t  Net_Ack_SW           = 0;									//是否需要服务器应答				0：不需要													1：需要
static uint8_t	Wait_Net_Ack_Flag    = 0;                 //等待服务器回复标志				0：无需等待或服务器已应答						1：等待服务器应答中		2：等待服务器应答失败
static uint16_t	Build_Net_Buffer_len = 0;									//发送到服务器的网络数据缓存长度
uint8_t 				Build_Net_Data_Flag  = 0;                 //数据发送标志							0：已经发送到模块 可以开始超时计数	1：未发送
uint8_t         g_queue_idex_s			 = 0;									//读取位移
uint8_t         g_queue_idex_e			 = 0;								  //写入位移
uint8_t         g_nb_net_buff[QUEUE_DAT_SIZE] = {0};			//存储缓存                       最多1K的内存
uint8_t         g_up_config_flag		 = 0;									//获取配置信息的标志
uint16_t 				Serial_Number        = 0;									//流水号
uint16_t        g_nb_net_buf_start   = 0;									//NB存储缓存中数据的起始位置
uint16_t        g_nb_net_buf_end     = 0;									//NB存储缓存中数据的结束位置
app_type        g_app_type;																//当前数据类型
net_app_typedef g_net_app_queue[QUEUE_SIZE];							//网络应用APP
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		往服务器发送数据
 * 形式参数:		num 流水号 state 状态
 * 返回参数:		无
 * 更改日期;		2018-05-15				函数编写
							2018-05-30				函数重新编写
							2018-06-06				修复bug;数据调头的时候发生错误,导致上传的数据为其他类型数据等(随机的)
							2018-08-10				修复bug;发送数据的时间数据缺位(主要是操作信息-运行信息)
 ****************************************************************************/ 
void Server_SendData(uint16_t* num , cmd_type cmd_name ,uint8_t data_type , uint8_t msg_len)
{
	uint8_t i = 0;											//循环变量
	uint8_t j = 0;
	Net_CmdType_t cmd;									//命令
	uint16_t save_index = 0;
	uint16_t read_index = 0;
	uint16_t dat_len = 0;
	
	if(1){//协议头数据整理
		cmd.num             = (*num)++;						//流水
		cmd.vision          = 0xCBC8;							//协议版本号
		RTC_Get();																//获取当前时间
		memcpy(cmd.time, &Time, 6);               //拷贝时间
		for(i=0; i<6; i++) cmd.src_addr[i]   = Device_ID[5-i];//原地址
		for(i=0; i<4; i++) cmd.des_addr[i]   = g_sys_param.server_ip[3-i];  //目的地址
		for(i=0; i<2; i++) cmd.des_addr[4+i] = 0;
		cmd.cmd							= cmd_name;          //命令类型
		Build_Net_Data_Flag = 1;                 //有数据发送
		Net_Ack_SW          = 1;                 //无需服务器应答
	}
	if(3){//数据填装
		switch(cmd.cmd)
		{
			case type_ok_cmd:{			//应答信号
				(*num)--;
				Net_Ack_SW = 0;//无需应答
				break;}
			case type_send_dat_cmd:{//上传数据
				cmd.data[0] = data_type;		//数据类型
				cmd.data[1] = msg_len;			//信息体长度
				save_index = 2;
				read_index = g_net_app_queue[g_queue_idex_s].idex;
				if(data_type == type_runing)
				{
					save_index = 1;
					cmd.data[save_index++]  = 0x01;														//只有一个信息体
					cmd.data[save_index++]	= 0x80;														//系统标志
					cmd.data[save_index++]	= 0x01;														//系统地址
					cmd.data[save_index++]	= msg_len;												//参数个数
					for(i = 0;i < msg_len;i++)
					{
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//参数类型
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						dat_len = g_nb_net_buff[read_index];
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//参数长度
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						for(j = 0;j < dat_len;j++)
						{
							cmd.data[save_index++] = g_nb_net_buff[read_index];	//参数值
							read_index = (read_index+1)&QUEUE_DAT_MAX;
						}
					}
					for(i = 0;i < 6;i++)
					{
						cmd.data[save_index++] = g_nb_net_buff[read_index];    //cmd.data 不涉及调头,故不做处理
						read_index = (read_index+1)&QUEUE_DAT_MAX;
					}
				}else if(g_app_type == type_send_anlog)//有问题
				{
					dat_len = (0x05 * msg_len + 6) + read_index;
					for(i = 0;i < msg_len;i++)
					{
						cmd.data[save_index++]	= 0x80;													//系统标志
						cmd.data[save_index++]	= 0x01;													//系统地址
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	  //部件类型
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//部件地址
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						cmd.data[save_index++]	= 0x00;
						cmd.data[save_index++]	= 0x00;
						cmd.data[save_index++]	= 0x00;
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//数据类型
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//模拟量的值
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						cmd.data[save_index++]	= g_nb_net_buff[read_index];
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						for(j = 0;j < 6;j++)
						{
							cmd.data[save_index++] = g_nb_net_buff[dat_len];    //cmd.data 不涉及调头,故不做处理
							dat_len = (dat_len+1)&QUEUE_DAT_MAX;
						}
					}
				}else if((g_app_type == type_oper_info) || (data_type == type_senser_info) || (data_type == type_senser_recover))
				{
					cmd.data[save_index++]		= 0x80;													//系统标志
					cmd.data[save_index++]		= 0x01;													//系统地址
					cmd.data[save_index++]		= g_nb_net_buff[read_index];	  //部件类型
					read_index = (read_index+1)&QUEUE_DAT_MAX;
					cmd.data[save_index++]		= g_nb_net_buff[read_index];	  //部件地址
					read_index = (read_index+1)&QUEUE_DAT_MAX;
					cmd.data[save_index++]		= 0x00;
					cmd.data[save_index++]		= 0x00;
					cmd.data[save_index++]		= 0x00;
					for(j = 0;j < 8;j++)//2字节数据+6字节时间
					{
						cmd.data[save_index++] = g_nb_net_buff[read_index];    //cmd.data 不涉及调头,故不做处理
						read_index = (read_index+1)&QUEUE_DAT_MAX;
					}
				}
				break;}
			case type_startup_cmd:	//上传启动信息
			case type_upload_cmd:{	//上传配置信息
				read_index = g_net_app_queue[g_queue_idex_s].idex;//缓存位移
				cmd.data[save_index++] = data_type;		//数据类型
				cmd.data[save_index++] = 0x01;				//信息体长度
				cmd.data[save_index++] = 0x80;				//系统标志
				cmd.data[save_index++] = 0x01;				//系统地址
				cmd.data[save_index++] = msg_len;			//配置参数个数
				for(i = 0;i < msg_len;i++)
				{
					cmd.data[save_index++]	= g_nb_net_buff[read_index];	//参数类型
					read_index = (read_index+1)&QUEUE_DAT_MAX;
					dat_len = g_nb_net_buff[read_index];
					cmd.data[save_index++]	= g_nb_net_buff[read_index];	//参数长度
					read_index = (read_index+1)&QUEUE_DAT_MAX;
					for(j = 0;j < dat_len;j++)
					{
						cmd.data[save_index++] = g_nb_net_buff[read_index];	//参数值
						read_index = (read_index+1)&QUEUE_DAT_MAX;
					}
				}
				for(j = 0;j < 6;j++)
				{
					cmd.data[save_index++] = g_nb_net_buff[read_index];    //cmd.data 不涉及调头,故不做处理
					read_index = (read_index+1)&QUEUE_DAT_MAX;
				}
				Wait_Net_Ack_Flag=0;//服务器强制开启
				break;}
			default:{								//其他
				break;}
		}
	}
	cmd.length					= save_index;            //应用数据长度
	if(4){//数据填充
		buildcmd_net(&cmd);
	}
}
/*****************************************************************************
 * 函数功能:		队列移动
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-06-05				函数移植
 ****************************************************************************/
void data_index_move( void )
{
	if(++g_queue_idex_s >= QUEUE_SIZE)g_queue_idex_s = 0;														//数据区调头
	if(g_queue_idex_s != g_queue_idex_e)g_nb_net_buf_start = g_net_app_queue[g_queue_idex_s].idex;//起始存储位置,如果队列里面有其他的元素,则把数据区的起始位置设置为当前队列的位置
	else g_nb_net_buf_start = g_nb_net_buf_end;//如果队列里面没有存储的数据则起始等于结束
}
/*****************************************************************************
 * 函数功能:		服务器配置设备
 * 形式参数:		pDr 配置的数据 len 长度指针
 * 返回参数:		error 数据错误 success 数据成功
 * 更改日期;		2018-04-09				函数移植
							2018-04-10				优化
							2018-07-06				修复bug;由于配置数据默认为int8_t类型，导致无法修改配置（超量程）,结合实际,修改int8_t类型为uint8_t类型
 ****************************************************************************/
void Netconfig_Download( config_typedef* pDr , uint16_t len)
{
	uint8_t i   = 0;
	uint16_t u_temp = 0;
	
	for(i = 0;i < len;i++)
	{
		switch(pDr[i].config_type)
		{
			case config_camp:{												//配置采样时间
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				g_sys_param.camp_time = u_temp;break;}
			case config_curr_t:{											//配置电流阈值
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				if(u_temp > CURR_RANGE_IN)
				{
					g_sys_param.threa.curr1 = CURR_RANGE_IN;
					g_sys_param.threa.curr2 = CURR_RANGE_IN;
					g_sys_param.threa.curr3 = CURR_RANGE_IN;
				}else
				{
					g_sys_param.threa.curr1 = u_temp;
					g_sys_param.threa.curr2 = u_temp;
					g_sys_param.threa.curr3 = u_temp;
				}break;}
			case config_heart:{												//配置心跳时间
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				g_sys_param.hart_time = u_temp;break;}
			case config_ip:{													//配置IP地址
				g_sys_param.server_ip[0] = pDr[i].data[3];
				g_sys_param.server_ip[1] = pDr[i].data[2];
				g_sys_param.server_ip[2] = pDr[i].data[1];
				g_sys_param.server_ip[3] = pDr[i].data[0];
				g_sys_param.port = (pDr[i].data[5] << 8) | (pDr[i].data[4]);break;}
			case config_sy_curr_t:{										//配置剩余电流阈值
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				if(u_temp > SY_CURR_RANGE_IN)g_sys_param.threa.sy_curr = SY_CURR_RANGE_IN;
				else g_sys_param.threa.sy_curr = u_temp;break;}
//			case config_temp_humi_t:{								 //配置环境温湿度的阈值
//				temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
//				if((uint16_t)temp > 1000)g_sys_param.threa.hj_temp = 1000;
//				else g_sys_param.threa.hj_temp = (uint16_t)temp;
//				temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
//				if((uint16_t)temp > 1000)g_sys_param.threa.hj_humi = 1000;
//				g_sys_param.threa.hj_humi = (uint16_t)temp;break;}
			case config_temp_t:{										//配置温度阈值
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				if(u_temp > TEMP_MAX)
				{
					g_sys_param.threa.temp1 = TEMP_MAX;
					g_sys_param.threa.temp2 = TEMP_MAX;
//					g_sys_param.threa.temp3 = TEMP_MAX;
				}else
				{
					g_sys_param.threa.temp1 = u_temp;
					g_sys_param.threa.temp2 = u_temp;
//					g_sys_param.threa.temp3 = temp;
				}break;}
			case config_volat_t:{										//配置电压阈值
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				if(u_temp > VOLAT_MAX)g_sys_param.threa.volat = VOLAT_MAX;
				else g_sys_param.threa.volat = u_temp;break;}
			case config_temp_r:{										//配置温度量程(无法配置)
				break;}
			case config_sy_curr_r:{								  //配置剩余电流量程(无法配置)
				break;}
			case config_curr_r:{										//配置电流量程(无法配置)
				break;}
			default:break;
		}
	}
	g_sys_param.updat_flag = 1;									//网络数据更新标志
}
/*****************************************************************************
 * 函数功能:		解析来自平台的数据
 * 形式参数:		data_rx 接收的数据 len 数据长度
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数移植
							2018-03-13				内存申请失败的程序编写
							2018-04-02				修复bug;导致内存申请错误
							2018-04-04				将释放后的指针赋值为null
							2018-04-09				协议更改后程序的相应更改
							2018-06-07				修复bug;数据没有收到应答,但是依然被删除了,故将移动队列放在本函数
							2018-06-22				修复bug;在数据发完的时候,又有新的数据加入,会出问题,故此处更改为接收到应答后,数据的类型指向下一个数据而不是直接赋值为空
 ****************************************************************************/ 
uint8_t parse_trans_anjisi_RX(uint8_t *data_rx , uint16_t *len)
{
	uint16_t i 							= 0;				//循环变量
	uint16_t data_len				= 0;				//数据长度
	uint16_t length					=	0;				//计算出的数据长度
	uint16_t num						= 0;				//流水号
	uint8_t found 					= 0;				//起始符标志位
	uint8_t	check_sum 			= 0;				//校验和
	Time_TypeDef_t otime    = {0};			//时间
	config_typedef* app_config = NULL;	//数据指针
	
  data_len  = *len;
	if(data_len < PROTOCOL_NET_MIN)	//长度不够最小长度,等待
	{
		return PROTOCOL_NET_STATE_SHORT;
	}

	for(i=1; i<data_len; i++)	//查找起始符
	{
		if((data_rx[i-1] == PROTOCOL_NET_HEAD) && (data_rx[i] == PROTOCOL_NET_HEAD))
		{
			found = 1;
			break;
		}
	}
	
	if(found != 1)	//未找到起始符，清除数据
	{
		runstate_to_usart("net: err, not find head\r\n");
		return PROTOCOL_NET_STATE_HEAD;
	}
	
	if(data_len < PROTOCOL_NET_MIN)	//长度不够最小长度，等待
	{
		return PROTOCOL_NET_STATE_SHORT;
	}
	
	//计算长度
	length = data_rx[DATA_LEN_0] + (data_rx[DATA_LEN_1] << 8);
	if(PROTOCOL_NET_MIN + length > PROTOCOL_NET_MAX)	//指令长度大于限制最大长度，去掉头
	{
		runstate_to_usart("net: err, data length too long!\r\n");
		return PROTOCOL_NET_STATE_LONG;
	}
	if(data_len < PROTOCOL_NET_MIN + length)	//已接收长度小于实际指令长度，等待
	{
		runstate_to_usart("net: err, data length too short!\r\n");
		return PROTOCOL_NET_STATE_LENGTH;
	}
	
	//未找到结束符，去掉头
	if(data_rx[PROTOCOL_NET_MIN + length - 2] != PROTOCOL_NET_TAIL || data_rx[PROTOCOL_NET_MIN + length - 1] != PROTOCOL_NET_TAIL)
	{
		runstate_to_usart("net: err, not find tail\r\n");
		return PROTOCOL_NET_STATE_TAIL;
	}
	
	//计算校验和
	for(i=2; i<27+length; i++)
	{
		check_sum += data_rx[i];
	}
	
	if(check_sum != data_rx[length+27])	//校验错误，去掉头
	{
		runstate_to_usart("net: err, check sum fail\r\n");
		return PROTOCOL_NET_STATE_CHECK;
	}
	
	//填充网络命令结构体
	num = data_rx[SERIAL_NUMBER_0] + (data_rx[SERIAL_NUMBER_1] << 8);//流水
  //copy应用数据
	//设置RTC
	otime.sec			=	data_rx[TIME_DATA+0];
	otime.min			=	data_rx[TIME_DATA+1];
	otime.hour		=	data_rx[TIME_DATA+2];
	otime.day			=	data_rx[TIME_DATA+3];
	otime.mon			=	data_rx[TIME_DATA+4];
	otime.year		=	data_rx[TIME_DATA+5];
	RTC_Set(otime.year+2000,otime.mon,otime.day,otime.hour,otime.min,otime.sec);//更新本地时间
	runstate_to_usart("net: success,receive from server TIME! \r\n");						//成功更新时间
	if(g_app_type != type_null_dat)
	{
		g_app_type = type_null_dat;									//下一个数据发送
		g_net_app_queue[g_queue_idex_s].type = type_null_dat;		//当前数据设置为空
		data_index_move();
	}
	if(!g_senser_flag)LED_Control( L_ERROR , CLOS );
	//解析数据
	switch((cmd_type)(data_rx[NET_CMD]))
	{
		case type_ack_cmd:{															    //数据发送完成
				runstate_to_usart("net: success,receive from server ok! \r\n");
				break;}											
		case type_set_param_cmd:{									          //下传配置信息
				do{
					app_config = calloc(data_rx[CONFIG_SIZE] , sizeof(config_typedef));
				}while(app_config == NULL);								//申请动态内存
				length = 0;									//数据长度
				for(i = 0;i < data_rx[CONFIG_SIZE];i++)		//数据分配
				{
					app_config[i].config_type =  (CONFIG)data_rx[CONFIG_SIZE+i+1+length];
					app_config[i].leng        =  data_rx[CONFIG_SIZE+i+2+length];
					memcpy(app_config[i].data , &data_rx[CONFIG_SIZE+i+3+length] , app_config[i].leng);
					length += (app_config[i].leng+1);
				}
				Netconfig_Download(app_config , data_rx[CONFIG_SIZE]);
				runstate_to_usart("net: success,receive from server config cmd! \r\n");
				if(Wait_Net_Ack_Flag)Wait_Net_Ack_Flag = 0;
				Server_SendData(&num , type_ok_cmd , 0 , 0);
				free(app_config);
				app_config = NULL;
				runstate_to_usart("net: success,释放内存完毕\r\n");
				break;}									
			case type_get_config_cmd:{									      //平台获取配置信息
				g_up_config_flag = 1;
				break;}
		case type_sys_reset_cmd:{														//服务器重启设备
				runstate_to_usart("net: success,receive from server restart cmd! \r\n");
				NVIC_SystemReset();												//重启											
				break;}
		case type_ok_cmd:{
				if(Wait_Net_Ack_Flag)Wait_Net_Ack_Flag = 0;
				if(g_reset_flag & 0xF0)NVIC_SystemReset();
			break;}
		case type_get_dat_cmd:{break;}										//平台获取数据
		case type_server_regis_cmd:{}break;
		case type_send_dat_cmd:{}break;										//数据发送命令(服务器处理)
		case type_upload_cmd:{}break;									    //发送配置信息(服务器处理)
		case type_deny_cmd:{}break;
		case type_null_cmd:{}break;
		default:break;
	}
	return PROTOCOL_NET_STATE_OK;
}

/*****************************************************************************
 * 函数功能:		装载数据到缓存
 * 形式参数:		cmd 命令  dat 数据指针
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数移植
 ****************************************************************************/
uint8_t buildcmd_net(Net_CmdType_t *cmd)
{
	uint16_t i = 0;											//循环变量
	uint8_t check_sum = 0;							//校验和

	Build_Net_Buffer_len = 0;	
	//起始符
	s_net_buff[Build_Net_Buffer_len++] = PROTOCOL_NET_HEAD;
	s_net_buff[Build_Net_Buffer_len++] = PROTOCOL_NET_HEAD;
	//流水号
	s_net_buff[Build_Net_Buffer_len++] = cmd->num & 0xff;
	s_net_buff[Build_Net_Buffer_len++] = (cmd->num >> 8) & 0xff;
	//版本号
	s_net_buff[Build_Net_Buffer_len++] = cmd->vision & 0xff;
	s_net_buff[Build_Net_Buffer_len++] = (cmd->vision >> 8) & 0xff;
	//时间
	memcpy(&s_net_buff[Build_Net_Buffer_len], cmd->time, 6);
	Build_Net_Buffer_len += 6;
	//源地址
	memcpy(&s_net_buff[Build_Net_Buffer_len], &cmd->src_addr, 6);
	Build_Net_Buffer_len += 6;
	//目的地址
	memcpy(&s_net_buff[Build_Net_Buffer_len], &cmd->des_addr, 6);
	Build_Net_Buffer_len += 6;
	//长度
	s_net_buff[Build_Net_Buffer_len++] = cmd->length & 0xff;
	s_net_buff[Build_Net_Buffer_len++] = (cmd->length >> 8) & 0xff;
	//命令
	s_net_buff[Build_Net_Buffer_len++] = cmd->cmd;
	//数据
	memcpy(&s_net_buff[Build_Net_Buffer_len], cmd->data, cmd->length);
	Build_Net_Buffer_len += cmd->length;
	//校验
	for(i=2; i<Build_Net_Buffer_len; i++)
	{
		check_sum += s_net_buff[i];
	}
	s_net_buff[Build_Net_Buffer_len++] = check_sum;
	//结束符
	s_net_buff[Build_Net_Buffer_len++] = PROTOCOL_NET_TAIL;
	s_net_buff[Build_Net_Buffer_len++] = PROTOCOL_NET_TAIL;
	return 0;
}
/*****************************************************************************
 * 函数功能:		查询接收数据
 * 形式参数:		Cmd 命令 socket Cmdlen 命令长度
 * 返回参数;		SUCCESS 接收成功 ERROR 接收失败
 * 更改日期;		2018-03-08				函数移植
							2018-03-29				将全局变量 NB_Handle_TX_BUF 设置为局部变量
              2018-04-25				coap协议更改
							2018-06-29				修复bug;无法识别小写字母
 ****************************************************************************/
ErrorStatus NB_messag_Read(char* Cmd , uint8_t Cmdlen)
{
	uint8_t  state    = 0;							//数据解析状态
  uint16_t sp_start = 0;							//字符串起始位置
	uint16_t sp       = 0;							//字符串位置
	uint16_t i        = 0;							//循环变量
	uint16_t length   = 0;							//数据长度
  uint16_t Recv_Net_Buffer_len=0;														//接收到服务器的网络数据缓存长度

	
	NB_Handle_TX_CNT=0;//清零发送长度		
	if(Cmd != 0)
	{
		IOT_onlyATsend((uint8_t*)Cmd , (uint16_t)Cmdlen);
		
		for(i = 0;i < 25;i++)//等待500ms消息回复
		{ 
			NB_ReceiveData();//收到IOT消息
			if(g_nb_newdata_flag)
			{
				g_nb_newdata_flag = 0;
				break;
			}
		}
	}
	
	g_nb_newdata_flag = 0;
	if(strbj((char*)(g_nb_rx_buff),"ERROR",g_nb_rx_cnt,&sp)==SUCCESS) return ERROR;
	if(strbj((char*)(g_nb_rx_buff),"4040",g_nb_rx_cnt,&sp)==SUCCESS)//寻找",@@"存储数据
	{   
		sp_start = sp-4;
		Recv_Net_Buffer_len=0;//接收到服务器数据长度清零
		for(;sp_start<g_nb_rx_cnt;)//转化为16进值并放到接收缓存中
		{
			if((g_nb_rx_buff[sp_start]==',') || (g_nb_rx_buff[sp_start] =='\r'))break;	//删除','接收完毕,对于COAP而言,不影响,对于UDP而言,数据后面还有一个数据,以逗号结束
			
			if((g_nb_rx_buff[sp_start] >= '0') && (g_nb_rx_buff[sp_start] <= '9'))//高位
			{
				s_net_buff[Recv_Net_Buffer_len]=(g_nb_rx_buff[sp_start++]-'0')<<4;
			}else if((g_nb_rx_buff[sp_start] >= 'A') && (g_nb_rx_buff[sp_start] <= 'Z'))
			{
				s_net_buff[Recv_Net_Buffer_len]=(g_nb_rx_buff[sp_start++]-'A'+10)<<4;
			}else if((g_nb_rx_buff[sp_start] >= 'a') && (g_nb_rx_buff[sp_start] <= 'z'))
			{
				s_net_buff[Recv_Net_Buffer_len]=(g_nb_rx_buff[sp_start++]-'a'+10)<<4;
			}
			
			if((g_nb_rx_buff[sp_start] >= '0') && (g_nb_rx_buff[sp_start] <= '9'))//低位
			{
				s_net_buff[Recv_Net_Buffer_len++]+=g_nb_rx_buff[sp_start++]-'0';
			}else if((g_nb_rx_buff[sp_start] >= 'A') && (g_nb_rx_buff[sp_start] <= 'Z'))
			{
				s_net_buff[Recv_Net_Buffer_len++]+=g_nb_rx_buff[sp_start++]-'A'+10;
			}else if((g_nb_rx_buff[sp_start] >= 'a') && (g_nb_rx_buff[sp_start] <= 'z'))
			{
				s_net_buff[Recv_Net_Buffer_len++]+=g_nb_rx_buff[sp_start++]-'a'+10;
			}
		}
		sp_start=0;//数据收尾清空
		do			//解析数据
		{
			if((s_net_buff[sp_start] == '@') && (s_net_buff[sp_start+1] == '@'))
			{					
				length=Recv_Net_Buffer_len-sp_start;
				state=parse_trans_anjisi_RX(&s_net_buff[sp_start],&length);//解析函数
				if(state == PROTOCOL_NET_STATE_OK) sp_start+=30+length;//新的起始地址至少30
				else sp_start++;//继续寻找		
			}else sp_start++;//起始位置增加
		}while((sp_start+1)<Recv_Net_Buffer_len);
	}
	return SUCCESS;
}

/*****************************************************************************
 * 函数功能:		将网络发送缓存中的数据发送到输出模块
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-07				函数编写
							2018-06-11				函数优化;对本函数进行了优化 Wait_Net_Ack_Flag 的选择判断
							2018-06-22				函数优化;屏蔽 g_app_type          = type_null_dat;             可能有问题
 ****************************************************************************/ 
void net_to_module(void)
{
	static uint8_t out_poll_max=0;			//模块最大重传次数
	static uint8_t out_send_max=0;			//服务器最大重传次数

	if((g_nb_error_flag) || (!g_model_config_flag))return;
	//有数据需要发送
	if(Build_Net_Data_Flag == 1)//数据未发送到模块且服务器空闲
	{//无需等待服务器（第一次发送数据）
		out_poll_max++;//模块重传次数
		if(NB_Send( s_net_buff, Build_Net_Buffer_len ) == SUCCESS)//模块回复成功
		{
			g_rec_time = g_sys_tim_s + 30;
			if(g_rec_time == 0)g_rec_time = 1;								//接收超时的时间
			
			Build_Net_Data_Flag = 0;//数据已经发送到模块
			NET_outtime = SEND_OutTime;//等到时间(s)
			out_poll_max = 0;//模块重传次数清除
			if(Net_Ack_SW)
			{
				Net_Ack_SW        = 0;
				Wait_Net_Ack_Flag = 1;
			}
		}
				
		if(out_poll_max >= OUT_POLL_MAX_TOP0)//模块重传超限
		{
			out_poll_max        = 0;
//			g_app_type          = type_null_dat;
			Build_Net_Data_Flag = 0;
			g_nb_error_flag     = 1;
		}
	}else
	{
		switch(Wait_Net_Ack_Flag)
		{
			case 0:{//服务器应答成功
				out_send_max = 0;//服务器重传次数清除
				break;}
			case 1:{//服务器应答中...
				if(NET_outtime == 0)Wait_Net_Ack_Flag=2;//应答超时
				break;}
			case 2:{//服务器应答失败
				runstate_to_usart("To server Overtime!\r\n");//超时
				out_send_max++;//服务器重传次增加
				if(out_send_max >= OUT_SEND_MAX)//服务器超限重传
				{ 
					out_send_max=0;//服务器重传次数清除
					Wait_Net_Ack_Flag=1;//服务器等待
					Build_Net_Data_Flag=1;//再次装载
					runstate_to_usart("To server failed!\r\n");//服务器上传失败
					g_nb_error_flag = 1;
				}else if(out_send_max >= 2)//超时发生两次
				{
					g_nb_error_flag = 1;
					runstate_to_usart("To server outnumber 2!\r\n");//服务器上传失败
					Wait_Net_Ack_Flag=1;//服务器等待
					Build_Net_Data_Flag=1;//再次装载
				}else
				{
						Wait_Net_Ack_Flag=1;//服务器等待
						Build_Net_Data_Flag=1;//再次装载
				}break;}
			default:{//其他
				break;}
		}
	}
}

/*------------------------------File----------End------------------------------*/
