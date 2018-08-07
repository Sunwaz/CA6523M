/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz,王泉凯
 *修改日期:	2018-03-08					文档移植
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "app.h"
#include "transmission.h"
#include "usart.h"
#include "adc_get.h"
#include "configure.h"
#include "delay.h"
#include "stmflash.h"
#include "stdlib.h"
#include "bc95.h"
#include "nbiot.h"
#include "key_led.h"
#include "timer.h"
/* 宏定义	--------------------------------------------------------------------*/
#define delay_nms											delay_ms
#define APP_FUN_CNT                   50
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
void Netconfig_Upload( int8_t* pDr );								//设备配置信息上传
void up_startupmessage( uint8_t*  msg_buff);				//上传启动信息
void runing_state_upload(uint8_t* runing_buff);			//上传运行状态
void up_per_info(uint8_t* up_data );								//上传操作信息
void app_run_queue(void);														//队列执行
/* 全局变量	------------------------------------------------------------------*/
void (*app_fun[APP_FUN_CNT])(void);									//APP命令队列
uint8_t g_app_fun_cnt = 0;													//APP命令存放位置
uint8_t g_app_run_cnt = 0;													//APP命令运行位置
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		心跳           数据上传 和 心跳一起上传给服务器
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-07				函数移植
							2018-04-10				定时时间的计算
							2018-05-10				报警处理的移动,将处理函数移动到函数  slave_dataupdata  中
																添加功能;报警状态被消除的时候上传数据到平台
 ****************************************************************************/
void Heart_task(void)
{
	uint16_t       temp    = 0;		//中间变量
	
	if(1){//模拟数据采集
		temp = (g_sys_tim_s & 0xFFFF) % g_sys_param.hart_time ;
		if((temp == 0) || (g_alarm_flag & READ_ALARM) || (g_alarm_flag & ALARM_RECOVERY))//心跳时间到      有报警数据   报警消除(都需要上传数据)
		{
			g_alarm_flag &=~ (ALARM_RECOVERY | ALARM_RESET);//清除标志位
			sys_app(type_send_anlog);//探测器实时数据更新上传
		}
	}
	if(2){//运行状态
		temp = (g_sys_tim_s & 0xFFFF) % 7200;//2H,如果发送失败,则时间缩短
		if(!temp)
		{
			sys_app(type_runing);
		}
	}
	if(3){//本地报警
		if( g_alarm_flag & READ_ALARM )//本地报警
		{
			for(temp = 1;temp < 0x400;temp <<= 1)
			{
				if(g_alarm_flag & temp)
				{
					g_alarm_flag &=~ temp;//清除标志位
					g_alarm_flag |= (temp << ALARM_UPLOAD);//置位上传标志位
					if(!(g_alarm_flag & (temp << ALARM_SILENCE)))Alarm_Set();//如果被消音,则不产生报警
				}
			}
			LED_Control( L_CLEAR , CLOS );//关闭消音灯
		}else if(!g_alarm_flag)
		{
			Alarm_Reset();
		}
	}
	if(4){//应用队列 应用执行
		temp = (g_sys_tim_s & 0xFFFF) % 6;//2s,如果发送失败,则时间缩短
		if((!temp) && (!Build_Net_Data_Flag))
		{
			app_run_queue();
		}
	}
}

/*****************************************************************************
 * 函数功能:		串口重发
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-07-18				函数移植
 ****************************************************************************/ 
void CRT_ReSend(void)
{	
	static uint8_t s_cnt = 0;
	if(s_cnt++ < 2)return;
	s_cnt = 0;
	if(g_ack_flag)
	{
		if(++g_re_cnt == 10)
		{
			g_monitor_flag = 0;
			CRT_FunMove();
		}else
		{
			crt_fun[g_crt_run_cnt]();//函数运行
		}
	}
}
/*****************************************************************************
 * 函数功能:	应用进程函数
 * 形式参数:	函数指针
 * 返回参数:	无
 * 修改日期:	2018-08-02					文档移植
 ****************************************************************************/
void APP_FunAdd( void fun(void) )
{
	uint8_t i = g_app_run_cnt;
	
	while(i != g_app_fun_cnt)
	{
		if(app_fun[i] == fun)return;
		if(++i == APP_FUN_CNT)i = 0;
	}
	app_fun[g_app_fun_cnt] = fun;
	if(++g_app_fun_cnt == APP_FUN_CNT)g_app_fun_cnt = 0;
	if(g_app_fun_cnt   == g_app_run_cnt)
	{
		if(++g_app_run_cnt ==  APP_FUN_CNT)g_app_run_cnt = 0;
	}
}

/*****************************************************************************
 * 函数功能:	发送数据
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-08-02					文档移植
 ****************************************************************************/
void APP_SendData( void )
{
	static uint8_t s_cnt	= 0;				//查询次数
	
	CRT_ReSend();
	Heart_task();								//心跳
	net_to_module();            //发送数据
	if(radio.rece != NULL)radio.rece(&s_cnt);//接收数据	
}
/*****************************************************************************
 * 函数功能:	设备校准
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-08-02					文档移植
 ****************************************************************************/
void APP_GetADCFun( void )
{
	ADC_Collection(0xFF);         //数据采集
}
/*****************************************************************************
 * 函数功能:	启动信息应用
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-08-02					文档移植
 ****************************************************************************/
void APP_ConfigFun( void )
{
	sys_app(type_upload_startup);
}
/*****************************************************************************
 * 函数功能:	监控数据应用
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-08-02					文档移植
 ****************************************************************************/
void APP_MonitorFun( void )
{
	USART_CRT_FunAdd(USART_SendSenserSta);
	USART_CRT_FunAdd(USART_SendMonitorData);
}
/*****************************************************************************
 * 函数功能:	操作信息上传应用
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-08-02					文档移植
 ****************************************************************************/
void APP_OperationFun( void )
{
	sys_app(type_oper_info);
}

/*****************************************************************************
 * 函数功能:		网络应用
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-07				函数移植
							2018-06-01				修复bug;同一个时间点多次填装数据,发送数据
							2018-06-11				函数优化;函数重新编写
							2018-07-24				函数优化;增加模块的初始化流程和以前的复位流程合并,更改了发送操作信息的优先级
							2018-08-02				函数优化;应用进程的添加优化,使用函数指针数组的形式添加,去掉以前的switch方式
 ****************************************************************************/ 
void app_net(void)
{
	static uint8_t s_old_time = 0;				//旧时间
	static uint8_t s_up_flag = 0;
	
	if((g_sys_tim_s & 0xFF) != s_old_time){ //定时器应用,1s执行1次
		APP_FunAdd(APP_SendData);
		if(g_crt_run_cnt != g_crt_fun_cnt){	  //串口应用运行
		if((!g_ack_flag) && (crt_fun[g_crt_run_cnt] != NULL)){
			g_ack_flag = 1;
			crt_fun[g_crt_run_cnt]();}}//函数运行
		s_old_time = g_sys_tim_s & 0xFF;}
	if(g_cail_data.flag){                   //设备校准应用,接收到1次校准命令执行一次
		APP_FunAdd(ADC_Cail);
		g_cail_data.flag = 0;}
	if(g_adc_get_flag && (g_wait_flag != 1)){//获取数据应用,执行一次添加一次
			APP_FunAdd(APP_GetADCFun);}
	if(g_sys_param.updat_flag){              //升级设备应用,接收到一次执行一次
		APP_FunAdd(network_parameterUpdata);
		g_sys_param.updat_flag = 0;}//更新标志清空
	if(g_up_config_flag){										 //上传配置信息应用,接收到一次执行一次
		APP_FunAdd(APP_ConfigFun);
		g_up_config_flag = 0;}
	if(g_monitor_flag){											 //上传监控数据应用,运行一次执行一次
		APP_FunAdd(APP_MonitorFun);}
	if(!g_model_config_flag){								 //模块复位应用,运行一次执行一次
		APP_FunAdd(Nbiot_reset);}
	if(g_sys_operation_msg && !s_up_flag){   //上传操作信息应用,运行一次执行一次
		APP_FunAdd(APP_OperationFun);
		s_up_flag = 1;
	}else s_up_flag = 0;
	
	if(app_fun[g_app_run_cnt] != NULL){
		app_fun[g_app_run_cnt]();//函数运行
		app_fun[g_app_run_cnt] = NULL;
		if(++g_app_run_cnt == APP_FUN_CNT)g_app_run_cnt = 0;
	}else{
	if(++g_app_run_cnt == APP_FUN_CNT)g_app_run_cnt = 0;
	}
}
/*****************************************************************************
 * 函数功能:时间数据的拷贝
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-06-05				函数移植
 ****************************************************************************/
void data_time_copy( void )
{
	uint8_t i = 0;
	uint8_t len = sizeof(Time_TypeDef_t)/sizeof(uint8_t);
	uint8_t* p = (uint8_t*)&Time;
	
	for(i = 0;i < len;i++)
	{
		g_nb_net_buff[g_nb_net_buf_end] = *p;
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		p++;
	}
}
/*****************************************************************************
 * 函数功能:		超界时的操作
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-05-29				函数编写
							2018-06-05				修复bug;数据调头的时候,出现错误,电信平台无法解析
																修复bug;数据区被占满的时候,出现错误,操作非法内存
 ****************************************************************************/
void data_indexcheck( uint16_t index  , uint16_t *old_index )
{
	uint8_t flag = 0;					//超界标志             1;数据区被占满
	
	if(g_nb_net_buf_end >= g_nb_net_buf_start)
	{
		if((QUEUE_DAT_SIZE-(g_nb_net_buf_end - g_nb_net_buf_start)) < index)flag = 1;//超界
	}else
	{
		if((g_nb_net_buf_start - g_nb_net_buf_end) < index)flag = 1;//超界
	}
	if(flag)
	{
		g_net_app_queue[g_queue_idex_s].type = type_null_dat;
		data_index_move();//删除掉被占用的那个应用队列的空间
	}
	*old_index	= g_nb_net_buf_end;//起始位置
}
/*****************************************************************************
 * 函数功能:		模拟数据添加
 * 形式参数:		无
 * 返回参数:		数据的起始位置
 * 更改日期;		2018-05-28				函数编写
							2018-05-29				保存的数据为;部件类型-部件地址(一位)-数据类型-数据(两位)-时间
							2018-06-05				修复bug;数据在buff里面调头
 ****************************************************************************/
uint16_t data_add_anlog( void )
{
	uint8_t i = 0;										//循环变量
	uint8_t max = sizeof(data_typedef)/sizeof(uint16_t);
	uint16_t old_index  = 0;//老的位置
	
	old_index = 0x05 * max + 6;																								//每个信息体5个数据 最后加一个时间
	data_indexcheck( old_index , &old_index);
	for( i= 0;i < max;i++)                                                   //此处,buff的最大值是1K,即0-0x3FFF,故此处要并上一个0x3FFF
	{
		g_nb_net_buff[g_nb_net_buf_end] =  g_coll_data[i].part_type;								//部件类型
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		g_nb_net_buff[g_nb_net_buf_end] = (g_coll_data[i].part_addr >> 0)  & 0xFF;  //部件地址
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		g_nb_net_buff[g_nb_net_buf_end] =  g_coll_data[i].data_type;								//类型
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		g_nb_net_buff[g_nb_net_buf_end] = (g_coll_data[i].data_value >> 0) & 0xFF;  //数据
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		g_nb_net_buff[g_nb_net_buf_end] = (g_coll_data[i].data_value >> 8) & 0xFF;
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
	}
	data_time_copy();//时间的拷贝
	return old_index;//返回起始位置
}

/*****************************************************************************
 * 函数功能:		配置数据的添加
 * 形式参数:		无
 * 返回参数:		数据的起始位置
 * 更改日期;		2018-05-29				函数编写
 ****************************************************************************/
uint16_t data_add_config( void )
{
	uint16_t old_index  = 0;//老的位置
	uint16_t dat_len    = 0;//数据长度
	
	dat_len = 60;																												//6字节的IP+6字节的时间+2字节心跳时间+2字节采样时间
	data_indexcheck( dat_len , &old_index );								//输入数据长度 ,获取起始位置和保存位置
	Netconfig_Upload((int8_t*)g_nb_net_buff);      				//数据添加
	data_time_copy();//时间的拷贝
	return old_index;                                                   //返回起始位置
}
/*****************************************************************************
 * 函数功能:		启动信息数据的添加
 * 形式参数:		无
 * 返回参数:		数据的起始位置
 * 更改日期;		2018-05-29				函数编写
 ****************************************************************************/
uint16_t data_add_startup( void )
{
	uint16_t old_index  = 0;//老的位置
	uint16_t dat_len    = 0;//数据长度
	
	dat_len = strlen((char*)g_sys_param.IMEI) + strlen(SYS_VER) + strlen(MODLE_VER) + 18;	//(一个字节的命令+一个字节的长度)*3+(三个字节的运营商+三个字节的模块类型)+六个字节的时间
	data_indexcheck( dat_len , &old_index);             	//输入数据长度 ,获取起始位置和保存位置
	up_startupmessage(g_nb_net_buff);                    	//数据添加 
	data_time_copy();//时间的拷贝
	return old_index;                                                   //返回起始位置
}

/*****************************************************************************
 * 函数功能:		运行信息数据的添加
 * 形式参数:		无
 * 返回参数:		数据的起始位置
 * 更改日期;		2018-05-29				函数编写
 ****************************************************************************/
uint16_t data_add_running( void )
{
	uint16_t old_index  = 0;//老的位置
	uint16_t dat_len    = 0;//数据长度
	
	dat_len = 55;	                                                      //最大长度//四个字节的信号强度+三个字节的基站编号+九个字节的小区编号+四个字节信噪比+两个字节信号强度+十五个字节的IMSI+六个字节时间
	data_indexcheck( dat_len , &old_index );								//输入数据长度 ,获取起始位置和保存位置
	runing_state_upload(g_nb_net_buff );					//数据添加,获取实际的数据长度 
	data_time_copy();//时间的拷贝
	return old_index;                                                   //返回起始位置
}

/*****************************************************************************
 * 函数功能:		操作信息数据的添加
 * 形式参数:		无
 * 返回参数:		数据的起始位置
 * 更改日期;		2018-05-29				函数编写
 ****************************************************************************/
uint16_t data_add_opera( void )
{
	uint16_t old_index  = 0;//老的位置
	uint16_t dat_len    = 0;//数据长度
	
	dat_len = 10;	                                                      //一个字节类型+一个字节地址+两个字节状态+六个字节时间
	data_indexcheck( dat_len , &old_index );								//输入数据长度 ,获取起始位置和保存位置
	up_per_info(g_nb_net_buff);
  data_time_copy();//时间的拷贝
	return old_index;                                                   //返回起始位置
}
/*****************************************************************************
 * 函数功能:		队列添加
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-05-16				函数移植
							2018-05-22				修复bug;如果在添加队列的时候,本位置存在数据
							2018-05-28				修复bug;同一个数据,多次发送
																新增功能;在队列里面加入此时的数据
 ****************************************************************************/
void net_app_add( app_type type)
{
	uint8_t cnt          = 0;				//循环计数			当达到最大值的时候，则说明整个应用区被占满，强制退出
	uint16_t start_index = 0;				//数据存放位置
	
	while((g_net_app_queue[g_queue_idex_e].type != type_null_dat) && (cnt <= QUEUE_SIZE))
	{
		cnt++;
		if(++g_queue_idex_e >= QUEUE_SIZE)//结束为止调头
			g_queue_idex_e = 0;
		if(g_queue_idex_s == g_queue_idex_e )//数据区被存满,则起始位置后移
		{
			data_index_move();
		}
	}
	switch(type)
	{
		case type_send_anlog:{    //发送模拟量
			start_index = data_add_anlog();
			break;}
		case type_upload_config:{ //上传配置数据
			start_index = data_add_config();
			break;}
		case type_upload_startup:{//上传启动信息
			start_index = data_add_startup();
			break;}
		case type_runing:{        //上传运行信息
			start_index = data_add_running();
			break;}
		case type_oper_info:{     //上传操作信息
			start_index = data_add_opera();
			break;}
		default:return;
	}
	g_net_app_queue[g_queue_idex_e].type = type;
	g_net_app_queue[g_queue_idex_e].idex = start_index;
	if(++g_queue_idex_e >= QUEUE_SIZE)
	{
		g_queue_idex_e = 0;
	}
	if(g_queue_idex_e == g_queue_idex_s)//数据被填满
	{
		data_index_move();
	}
}
/*****************************************************************************
 * 函数功能:		队列执行
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-05-16				函数移植
							2018-05-22				发现bug;当队列被排满,则无法进行队列里面相关函数的执行
																队列没有删除机制,导致发送失败会不断的占满整个队列
							2018-05-29				新增功能;当应用队列更新的时候,数据缓存的起始位置也要改变
							2018-07-24				函数优化;当模块没有被初始化的时候,不允许运行app应用
 ****************************************************************************/
void app_run_queue(void)
{
	if(!g_model_config_flag)return;
	if(g_queue_idex_s != g_queue_idex_e)
	{
		sys_app(type_null_dat);//队列运行成功,则删除掉本次的内容
	}
}

/*****************************************************************************
 * 函数功能:		设备配置信息上传
 * 形式参数:		pDr 数据 len 长度指针
 * 返回参数:		error 内存不够 success 数据填装成功
 * 更改日期;		2018-04-09				函数移植
							2018-05-29				删除参数 len
							2018-06-05				修复bug;数据在buff里面调头
 ****************************************************************************/
void Netconfig_Upload( int8_t* pDr )
{	
	if(1){//1>服务器IP地址//8
  pDr[g_nb_net_buf_end]	= config_ip;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x06;//数据长度
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]  = g_sys_param.server_ip[3];
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
  pDr[g_nb_net_buf_end]  = g_sys_param.server_ip[2];
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
  pDr[g_nb_net_buf_end]  = g_sys_param.server_ip[1];
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
  pDr[g_nb_net_buf_end]  = g_sys_param.server_ip[0];		
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.port &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.port >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(2){//2>数据心跳时间//8+4=12
	pDr[g_nb_net_buf_end]	= config_heart;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.hart_time &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.hart_time >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(3){//3>电流互感器量程//12+6=18
	pDr[g_nb_net_buf_end]	= config_curr_r;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x04;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= CURR_RANGE_IN  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= CURR_RANGE_IN  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= CURR_RANGE_OUT &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= CURR_RANGE_OUT >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(4){//4>剩余电流互感器量程//18+6=24
	pDr[g_nb_net_buf_end]	= config_sy_curr_r;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x04;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= SY_CURR_RANGE_IN  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= SY_CURR_RANGE_IN  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= SY_CURR_RANGE_OUT &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= SY_CURR_RANGE_OUT >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(5){//5>温度传感器量程//24+6=30
	pDr[g_nb_net_buf_end]	= config_temp_r;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x04;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= TEMP_MIN  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= TEMP_MIN  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= TEMP_MAX  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= TEMP_MAX  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(6){//6>数据采集周期//30+4=34
	pDr[g_nb_net_buf_end]	= config_camp;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.camp_time  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.camp_time  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(7){//7>电流波动阈值//34+4=38
	pDr[g_nb_net_buf_end]	= config_curr_t;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.curr1  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end] = g_sys_param.threa.curr1  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(8){//8>漏电流波动阈值//38+4=42
	pDr[g_nb_net_buf_end]	= config_sy_curr_t;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.sy_curr  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end] = g_sys_param.threa.sy_curr  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(9){//9>温度波动阈值//42+4=46
	pDr[g_nb_net_buf_end]	= config_temp_t;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.temp1 &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.temp1  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(10){//10>电压波动阈值//46+4=50
	pDr[g_nb_net_buf_end]	= config_volat_t;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.volat  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.volat  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}	
  /*11>环境温湿度波动阈值*/
//	if(1){
//	if(++g_nb_net_buf_end > len)return ERROR;
//	index = 0;
//	pDr[g_nb_net_buf_end].config_type   = config_temp_humi_t;
//	pDr[g_nb_net_buf_end].leng				  = 0x02;
//	pDr[g_nb_net_buf_end].data[index]		= g_sys_param.threa.hj_temp  &  0xFF;
//	pDr[g_nb_net_buf_end].data[++index]	= g_sys_param.threa.hj_temp  >> 0x08;
//	pDr[g_nb_net_buf_end].data[++index]	= g_sys_param.threa.hj_humi  &  0xFF;
//	pDr[g_nb_net_buf_end].data[++index]	= g_sys_param.threa.hj_humi  >> 0x08;}
	if(12){//12>电压互感器量程//50+4=54
	pDr[g_nb_net_buf_end]	= config_volat_r;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= VOLAT_MAX  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= VOLAT_MAX  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
}
/*****************************************************************************
 * 函数功能:		上传配置信息
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-05-07				函数编写
							2018-05-29				由于参数改变,对上传配置信息进行了处理
 ****************************************************************************/
void up_configmessage( void )
{
	sys_app( type_upload_config );     //上传一次配置信息
}
/*****************************************************************************
 * 函数功能:		把数据保存到buff里面
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-06-27				函数编写
 ****************************************************************************/
void save_data_to_buff(uint8_t *save_buff ,uint8_t* buff,uint8_t len , uint16_t len_index)
{
	uint16_t i        = 0;							//循环变量

	for(i = 0;i < len;i++)
	{
		save_buff[g_nb_net_buf_end] = buff[i];
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	}
	save_buff[len_index] = len;
}
/*****************************************************************************
 * 函数功能:		上传运行状态
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-05-15				函数移植
							2018-05-28				修复bug;上传IMSI数据错误,在数据结尾未加'\0'
							2018-05-30				添加了变量 runing_buff
							2018-06-05				修复bug;数据在buff里面调头
							2018-06-20				函数优化;通过函数指针,增强了函数的可扩展性
 ****************************************************************************/
void runing_state_upload(uint8_t* runing_buff)
{
	uint16_t i        = 0;							//循环变量
	uint16_t index    = 0;							//数据的位置
	uint16_t len      = 0;							//数据长度
	uint8_t buff[100] = {0};
		
	if(1){//信号功率
		runing_buff[g_nb_net_buf_end] = runing_signal_power;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_signal(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(2){//基站编号
		runing_buff[g_nb_net_buf_end] = runing_pci;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_pci(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(3){//小区编号
		runing_buff[g_nb_net_buf_end] = runing_cell_id;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_cellid(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(4){//信噪比
		runing_buff[g_nb_net_buf_end] = runing_snr;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_snr(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(5){//信号强度
		runing_buff[g_nb_net_buf_end] = runing_csq;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_csq(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(6){//查询IMSI
		runing_buff[g_nb_net_buf_end] = runing_IMSI;//长度15个字节32+15+2=49
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		runing_buff[g_nb_net_buf_end] = strlen((char*)g_sys_param.IMSI);
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		len = strlen((char*)g_sys_param.IMSI);
		for(i = 0;i < len;i++)
		{
			runing_buff[g_nb_net_buf_end] = g_sys_param.IMSI[i];
			g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		}
	}
}
/*****************************************************************************
 * 函数功能:		上传启动信息
 * 形式参数:		msg_buff 启动信息数组
 * 返回参数:		无
 * 更改日期;		2018-05-11				函数编写
							2018-05-22				启动信息里面删除 <IMSI码 的上传>
							2018-05-29				删除了 数据填装 部分
																增加了 msg_buff 的形参
							2018-06-05				修复bug;数据在buff里面调头
 ****************************************************************************/
void up_startupmessage( uint8_t*  msg_buff)
{
	uint8_t len		= 0;				//数据长度
	uint8_t j     = 0;				//循环变量
	
	if(1){//获取 IEMI 码
		msg_buff[g_nb_net_buf_end]	= startup_IEMI;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		len           = strlen((char*)g_sys_param.IMEI);
		msg_buff[g_nb_net_buf_end] = len;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		for(j = 0;j < len;j++)
		{
			msg_buff[g_nb_net_buf_end] = g_sys_param.IMEI[j];
			g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		}
	}
	if(2){//获取 运营商
		msg_buff[g_nb_net_buf_end]	= startup_operator;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		msg_buff[g_nb_net_buf_end]	= 1;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		msg_buff[g_nb_net_buf_end]	= NB_Ope;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	}
	if(3){//设置 模块类型
		msg_buff[g_nb_net_buf_end]	= startup_mouldtype;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		msg_buff[g_nb_net_buf_end]	= 1;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		msg_buff[g_nb_net_buf_end]	= 0x08;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	}
	if(4){//设置 程序版本号
		msg_buff[g_nb_net_buf_end]	= startup_sys_ver;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		len						= strlen(SYS_VER);
		msg_buff[g_nb_net_buf_end]	= len;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		for(j = 0;j < len;j++)
		{
			msg_buff[g_nb_net_buf_end] = SYS_VER[j];
			g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		}
	}
	if(5){//设置 模块固件版本号
		msg_buff[g_nb_net_buf_end]	= startup_mou_ver;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		len						= strlen(MODLE_VER);
		msg_buff[g_nb_net_buf_end] = len;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		for(j = 0;j < len;j++)
		{
			msg_buff[g_nb_net_buf_end] = MODLE_VER[j];
			g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		}
	}
}
/*****************************************************************************
 * 函数功能:		上传操作信息
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-05-16				函数移植
							2018-05-30				添加参数 up_data
							2018-06-05				修复bug;数据在buff里面调头
							2018-06-07				修复bug;数据填装错误,导致平台显示操作信息有误
 ****************************************************************************/
void up_per_info(uint8_t* up_data )
{	
	if(g_sys_operation_msg & 0x01)//如果是重启设备则,置位g_reset_flag标志位
	{
		g_reset_flag &=~ 0x0F;
		g_reset_flag |=  0xF0;
	}
	up_data[g_nb_net_buf_end] = part_ty;
	g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	up_data[g_nb_net_buf_end] = 0x01;
	g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	up_data[g_nb_net_buf_end] = g_sys_operation_msg & 0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	up_data[g_nb_net_buf_end] = g_sys_operation_msg >> 8;
	g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	g_sys_operation_msg = 0;											//使用完了过后就清空
}
/*****************************************************************************
 * 函数功能:		系统app执行
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-05-16				函数移植
							2018-05-30				函数重写
							2018-06-07				修复bug;数据没有发送成功,但是数据被删除了,故删除掉队列移动,在收到数据后才移动队列
							2018-06-22				修复bug;数据填装错误
							2018-07-24				函数优化;模块没有初始化,不能进行app应用的添加和执行
 ****************************************************************************/
ErrorStatus sys_app(app_type type)
{
	uint16_t msg_len = 0;				//信息体长度
	cmd_type cmd;								//命令
	
	if(!g_model_config_flag)return ERROR;						//模块没有初始化,则不能执行下列程序
	if(1){//数据判断
		if(type != type_null_dat)
		{
			net_app_add(type);
		}
		if  (g_app_type != type_null_dat)return ERROR;//如果进程正在运行,则返回
		else g_app_type  = g_net_app_queue[g_queue_idex_s].type;
	}
	
	if(2){//APP执行部分 
		switch(g_app_type)
		{
			case type_send_anlog:{			//上传模拟量
				msg_len = sizeof(data_typedef)/sizeof(uint16_t);
				cmd			= type_send_dat_cmd;
				runstate_to_usart("上传模拟量\r\n");
				break;}
			case type_upload_config:{		//上传配置信息
				msg_len = CONFIG_TYPE_NUM;
				cmd			= type_upload_cmd;
				runstate_to_usart("上传配置信息\r\n");
				break;}
			case type_upload_startup:{	//上传启动信息
				msg_len	= STARTUP_PARA_TYPE_NUM;
				cmd     = type_startup_cmd;
				runstate_to_usart("上传启动信息\r\n");
				break;}
			case type_runing:{					//上传运行信息
				msg_len = RUNING_STATE_TYPE_NUM;
				cmd     = type_send_dat_cmd;
				runstate_to_usart("上传运行信息\r\n");
				break;}
			case type_oper_info:{				//上传操作信息
				msg_len = 1;
				cmd     = type_send_dat_cmd;
				runstate_to_usart("上传操作信息\r\n");
				break;}
			default:{										//其他
				msg_len = g_queue_idex_s + 1;
				if(msg_len >= QUEUE_SIZE)msg_len = 0;
				g_app_type = type_null_dat;
				return ERROR;}
		}
	}
	if(3){//数据填装
		Server_SendData(&Serial_Number ,  cmd , g_app_type , msg_len);//将数据写入到数据帧里面
	}
	return SUCCESS;
}


/*------------------------------File----------End------------------------------*/
