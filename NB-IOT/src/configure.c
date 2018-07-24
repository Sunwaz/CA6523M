/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-07					函数移植
 *说    明:	网络初始化
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "configure.h"
#include "usart.h"
#include "delay.h"
#include "stmflash.h"

#include "timer.h"
#include "key_led.h"
#include "string.h"
#include <stdlib.h>
/* 宏定义	--------------------------------------------------------------------*/
#define delay_nms										 					delay_ms

#define DEFAULT_CAMP_TIME										  3																//默认采样时间
#define DEFAULT_HART_TIME										  1800														//默认心跳时间
#define DEFAULT_CURR                          (uint16_t)(CURR_RANGE_IN * 0.1)	//默认电流阈值
#define DEFAULT_SY_CURR											  (uint16_t)(SY_CURR_RANGE_IN)		//默认剩余电流阈值
#define DEFAULT_VOLAT												  3800														//默认电压阈值
#define DEFAULT_TEMP												  600															//默认温度阈值
//#define DEFAULT_HJ_HUMI												100
//#define DEFAULT_HJ_TEMP												600
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
uint8_t  END[2]								= {0x0D,0x0A};        //使用结束符
uint8_t g_nb_newdata_flag               = 0;				//新数据标志   
uint8_t  g_nb_error_flag      = 0;									//NB故障标志
char NB_Ope				            = DIANXIN;						//电信
char MODLE_VER[20]            = {0};								//模块版本
param_typedef   g_sys_param;												//系统参数
uint16_t g_nb_rx_cnt                    = 0;				//接收计数
uint8_t  g_nb_rx_buff[USART_REC_LEN]    = {0};      //接收到的数据
uint16_t NB_Handle_TX_CNT			= 0;				          //NB发送待处理的 长度 到模块
uint8_t        g_rec_time           = 0;						//回复消息的超时时间,如果时间超过了30s,则不查询接收,如果小于30s则每2s查询一次接收
uint8_t g_nb_reset_flow       = 1;	                //初始化流程
uint8_t socket = 0;
char MODLE_NAME[20]               ={0};
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		数字转换成字符串
 * 形式参数:		s0 待转换的字符串 s1 转换完成的整型数组
 * 返回参数:		无
 * 更改日期;		2018-04-09				函数移植
 ****************************************************************************/
void int_to_char(char* s0 , uint8_t* s1 , uint16_t len)
{
	uint16_t i , j = 0;
	uint8_t temp = 0;
	uint16_t temp1,temp2;
	uint8_t flag = 0;
	
	for(i = 0;i < len;i++)
	{
		temp1 = 1000;
		temp2 = 100;
		flag = 0;
		for(j = 0;j < 3;j++)
		{
			temp = (s1[i] % temp1) / temp2;
			if((temp	== 0) && (flag == 0) && (j == 2))//最后一个数为0 则添加0 
			{
				*s0 = temp +'0';
				s0++;
			}else if((temp == 0) && (flag == 0) && (j < 2))//中间有数据为0 则不添加
			{
			}else if((temp != 0) && (flag == 0))//不为0 则添加数据
			{
				flag = 1;
				*s0 = temp +'0';
				s0++;
			}else//只有有一个数据不为0 则后面的数据不管是不是为0 都添加
			{
					*s0 = temp +'0';
					s0++;
			}
			temp1 /= 10;
			temp2 /= 10;
		}
		*s0 = '.';
		s0++;
	}
	s0--;
	*s0 = 0;
}

/*****************************************************************************
 * 函数功能:		等待启动消息
 * 形式参数:		cmd AT命令 length 命令长度 str 返回字符串 re_cnt 重发次数
 * 返回参数:		success 成功
 * 更改日期;		2018-06-29				函数移植
 ****************************************************************************/
ErrorStatus NB_WaitStartMessage(char *str , uint8_t* re_cnt)
{
	uint16_t l = 0;												//无用参数
	
	if(NB_WaitData(str , &l) == SUCCESS)
	{
		*re_cnt = 0;
		g_nb_reset_flow++;//下一个流程
		runstate_to_usart("nmodule starting success!\r\n");
		return SUCCESS;
	}
	return ERROR;
}
/*****************************************************************************
 * 函数功能:		读取固件版本
 * 形式参数:		cmd AT命令 length 命令长度 str 返回字符串 re_cnt 重发次数
 * 返回参数:		success 成功
 * 更改日期;		2018-06-29				函数移植
 ****************************************************************************/
ErrorStatus NB_ReadMoudleVer(char *cmd , uint16_t length , char *str , uint8_t *re_cnt)
{
	uint16_t str_index = 0;												//字符串的位置
	uint16_t i = 0,j = 0;//循环变量
	uint8_t temp = *re_cnt;

	if(NB_SendCmd(cmd , length , str , &str_index , re_cnt) == SUCCESS)//找到字符串
	{
		for(i = str_index;i < g_nb_rx_cnt;i++)
		{
			if((g_nb_rx_buff[i] == 0x0D) && (g_nb_rx_buff[i+1] == 0x0A))
			{
				MODLE_VER[j] = 0;//结尾
				break;
			}
			MODLE_VER[j++] = g_nb_rx_buff[i];
		}
		MODLE_VER[j] = 0;//结尾
		if( MODLE_VER[0] != 0)
		{
			runstate_to_usart("模块固件版本为:");runstate_to_usart(MODLE_VER);runstate_to_usart("\r\n");
			*re_cnt = 0;						//复位计时
			return SUCCESS;
		}else
		{
			runstate_to_usart("模块固件版本读取错误!\r\n");
			g_nb_reset_flow --;//由于数据读取错误,所以重新读取,流程不增加
			*re_cnt = temp;
		}
	}
	return ERROR;
}
/*****************************************************************************
 * 函数功能:		读取IMEI号
 * 形式参数:		str 返回字符串 re_cnt 重发次数
 * 返回参数:		success 成功
 * 更改日期;		2018-06-29				函数移植
 ****************************************************************************/
ErrorStatus NB_ReadMoudleIMEI(char *str , uint8_t* re_cnt)
{
	uint16_t str_index = 0;												//字符串的位置
	uint16_t i = 0;//循环变量
	char t_IMEI[20]   = {0};										//读取到的IEMI号码
	uint8_t temp = *re_cnt;

	if(NB_SendCmd("AT+CGSN=1" , strlen("AT+CGSN=1") , str , &str_index , re_cnt) == SUCCESS)
	{
		i = 0;
		while((g_nb_rx_buff[str_index] >= '0') && (g_nb_rx_buff[str_index] <= '9'))
		{
			t_IMEI[i] = g_nb_rx_buff[str_index];//拷贝IEMI
			i++;
			str_index++;
		}
		t_IMEI[i]=0;               //结尾
		if(t_IMEI[0] != 0)
		{
			runstate_to_usart("模块IMEI号为:\r\n");runstate_to_usart(t_IMEI);runstate_to_usart("\r\n");
			*re_cnt = 0;						//复位计时
			if(strcmp(t_IMEI , (char*)g_sys_param.IMEI))//模块号不相等
			{
				strcpy((char*)g_sys_param.IMEI , t_IMEI);
				g_sys_param.updat_flag = 1;								//模块号被更改
				g_sys_operation_msg |= 0x0200;
			}			
			return SUCCESS;
		}else
		{
			runstate_to_usart("模块IMEI号读取错误!");
			g_nb_reset_flow --;//由于数据读取错误,所以重新读取,流程不增加
			*re_cnt = temp;
		}
	}
	return ERROR;
}


/*****************************************************************************
 * 函数功能:		读取ISMI号
 * 形式参数:		str 返回字符串 re_cnt 重发次数
 * 返回参数:		success 成功
 * 更改日期;		2018-06-29				函数移植
 ****************************************************************************/
ErrorStatus NB_ReadMoudleISMI(char *str , uint8_t* re_cnt)
{
	uint16_t str_index = 0;												//字符串的位置
	uint16_t i = 0;//循环变量
	char t_IMSI[20]   = {0};										//读取到的IEMI号码
	uint8_t temp = *re_cnt;

	if(NB_SendCmd("AT+CIMI" , strlen("AT+CIMI") , str , &str_index , re_cnt) == SUCCESS)
	{
		i = 0;
		while((g_nb_rx_buff[str_index] >= '0') && (g_nb_rx_buff[str_index] <= '9'))
		{
			t_IMSI[i] = g_nb_rx_buff[str_index];//拷贝ISMI
			i++;
			str_index++;
		}
		t_IMSI[i]=0;               //结尾
		if(t_IMSI[0] != 0)
		{
			runstate_to_usart("模块IMSI号为:\r\n");runstate_to_usart(t_IMSI);runstate_to_usart("\r\n");
			*re_cnt = 0;						//复位计时
			if(strcmp(t_IMSI , (char*)g_sys_param.IMSI))//模块号不相等
			{
				strcpy((char*)g_sys_param.IMSI , t_IMSI);
				g_sys_param.updat_flag = 1;								//模块号被更改
				g_sys_operation_msg |= 0x80;
			}			
			return SUCCESS;
		}else
		{
			runstate_to_usart("模块IMSI号读取错误!\r\n");
			g_nb_reset_flow --;//由于数据读取错误,所以重新读取,流程不增加
			*re_cnt = temp;
		}
	}
	return ERROR;
}


/*****************************************************************************
 * 函数功能:		查询运营商
 * 形式参数:		str 返回字符串 re_cnt 重发次数
 * 返回参数:		success 成功
 * 更改日期;		2018-06-29				函数移植
 ****************************************************************************/
ErrorStatus NB_ReadMoudleOperator(char *str , uint8_t* re_cnt)
{
	uint8_t temp = *re_cnt;
	uint16_t str_index = 0;												//字符串的位置

	if(NB_SendCmd("AT+COPS?" , strlen("AT+COPS?") , str , &str_index , re_cnt) == SUCCESS)
	{
		if(strstr((char*)g_nb_rx_buff , "46000") || strstr((char*)g_nb_rx_buff , "46002") || strstr((char*)g_nb_rx_buff , "46007") || strstr((char*)g_nb_rx_buff , "46008"))
		{
			NB_Ope=YIDONG;//移动
			runstate_to_usart("入网频段:中国移动\r\n");
		}else if(strstr((char*)g_nb_rx_buff , "46001") || strstr((char*)g_nb_rx_buff , "46006") || strstr((char*)g_nb_rx_buff , "46009"))
		{
			NB_Ope=LIANTONG;//联通
			runstate_to_usart("入网频段:中国联通\r\n");
		}else if(strstr((char*)g_nb_rx_buff , "46003") || strstr((char*)g_nb_rx_buff , "46005") || strstr((char*)g_nb_rx_buff , "46011"))
		{
			NB_Ope=DIANXIN;//电信
			runstate_to_usart("入网频段:中国电信\r\n");																//电信平台固有IP
		}else
		{
			g_nb_reset_flow--;																	//由于数据读取错误,所以重新读取,流程不增加
			*re_cnt = temp;
			return ERROR;
		}
		return SUCCESS;
	}
	return ERROR;
}


/*****************************************************************************
 * 函数功能:		等待字符串
 * 形式参数:		cmd 需要等待的字符串 index字符串位置
 * 返回参数:		无
 * 更改日期;		2018-06-29				函数移植
 ****************************************************************************/
ErrorStatus NB_WaitData(char* str , uint16_t *index)
{
	uint8_t i = 25;
	
	for(;i>0;i--)
	{
		NB_ReceiveData();  //等待IOT的启动消息
		if(g_nb_newdata_flag)//如果g_nb_newdata_flag不为0 则接收到数据
		{
			g_nb_newdata_flag = 0;
			if(strbj((char *)g_nb_rx_buff , str , g_nb_rx_cnt , index)==SUCCESS)
			{
				return SUCCESS;
			}
		}
	}
	return ERROR;
}

/*****************************************************************************
 * 函数功能:		nbiot 发送AT指令
 * 形式参数:		cmd AT命令 length 命令长度 str 返回字符串 index字符串位置 re_cnt 重发次数
 * 返回参数:		SUCCESS 初始化成功
 * 更改日期;		2018-06-29				函数移植
 ****************************************************************************/
ErrorStatus NB_SendCmd(char* cmd , uint16_t length, char* str, uint16_t *index , uint8_t* re_cnt)
{
	if((*re_cnt == 1 ) || (!(*re_cnt%5)))//第1,5,15,20,25...次重发的时候,向模块发送AT指令
	{
		IOT_onlyATsend((uint8_t*)cmd,length);
	}
	if(NB_WaitData(str , index) == SUCCESS)				//查找并返回字符串所在位置
	{
		g_nb_reset_flow++;//下一个流程
		*re_cnt = 0;
		return SUCCESS;
	}
	return ERROR;
}
/*****************************************************************************
 * 函数功能:		读取信号强度
 * 形式参数:		p 发送的数据 len 数据长度 c AT指令回复数据
 * 返回参数:		SUCCESS 重启成功		ERROR 重启失败
 * 更改日期;		2018-06-29				函数移植
 ****************************************************************************/
void NB_ReadMoudleCSQ(uint8_t *buff , char *str)
{
	uint16_t j = 0 , str_index;
	uint16_t index;
	
	if(NB_ReadMoudleData("AT+CSQ" , strlen("AT+CSQ") , str , &str_index) == SUCCESS)				//查找并返回字符串所在位置
	{
		for(j = str_index;j < g_nb_rx_cnt;j++)
		{
			if((g_nb_rx_buff[j] >= '0') && (g_nb_rx_buff[j] <= '9'))
			{
				buff[index] = g_nb_rx_buff[j];
				index++;
			}else
			{
				buff[index] = 0;
				return;
			}
		}
	}
	buff[0] = '0';
	buff[1] = 0;
}

/*****************************************************************************
 * 函数功能:		读取数据
 * 形式参数:		p 发送的数据 len 数据长度 c AT指令回复数据
 * 返回参数:		SUCCESS 重启成功		ERROR 重启失败
 * 更改日期;		2018-06-29				函数移植
 ****************************************************************************/
ErrorStatus NB_ReadMoudleData(char *cmd , uint16_t length , char *str , uint16_t *str_index)
{	
	IOT_onlyATsend((uint8_t*)cmd,strlen(cmd));//查询信号强度
	return NB_WaitData(str , str_index);				//查找并返回字符串所在位置
}
/*****************************************************************************
 * 函数功能:		将运行状态发送到上位机
 * 形式参数:		cmd 命令字符串
 * 返回参数:		无
 * 更改日期;		2018-03-07				函数移植
							2018-04-04				为了节约内存,将config_data数组设置为局部变量
 ****************************************************************************/
void runstate_to_usart(char* cmd)
{
	uint8_t length      = 0;						//长度
	uint8_t Config_Data[512] = {0};

	memcpy(&Config_Data[length], cmd, strlen(cmd));//值
	length+=strlen(cmd);
	USART_CONFIG_SEND(Config_Data,length);//发送到串口
	delay_nms(10);
}
/*****************************************************************************
 * 函数功能:		更新网络参数(字符串转成数字)
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-07				函数移植
							2018-04-10				系统数据更新的添加
							2018-05-04				系统数据最新的是int型的数组,在保存的时候,应先由int型转换成char型再保存,删除数据更新
							2018-05-17				端口部分的设置程序有问题,已修复
							2018-06-08				删除全局变量 Device_IDchar Serv_Ipchar UDPPort UDPPortchar等
 ****************************************************************************/
void network_parameterUpdata(void)
{
	uint16_t temp   = 0;							  //中间变量					最高位数据
	
	if(1){//数据保存
	temp = sizeof(g_sys_param)/sizeof(uint16_t);
	STMFLASH_Write( FLASH_SAVE_ADDR_DeviceID , (uint16_t*)&g_sys_param , temp);}		//数据保存
	if(1){//采样时间设置
		Sampling_TimerConfig( g_sys_param.camp_time );										//设置采样时间
	}
}
/*****************************************************************************
 * 函数功能:		初始化网络参数
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-07				函数移植
 ****************************************************************************/
void network_parameter_flashRead(void)
{
	param_typedef * p = (param_typedef*)FLASH_SAVE_ADDR_DeviceID;
	uint8_t data[2] = {0};							//从flash里面读出来的数据
	
	STMFLASH_Read(FLASH_SAVE_ADDR_DeviceID , (uint16_t*)data ,2);
	if((data[0] == 0x00) && (data[1] == 0x00))
	{
////////////////系统数据读取////////////////
		if(1){
		memcpy(&g_sys_param , p , sizeof(g_sys_param));
		}
////////////////系统数据设置////////////////
		if(1){
			Sampling_TimerConfig( g_sys_param.camp_time );										//设置采样时间
		}
////////////////异常数据变更////////////////
		if(1){
			if((g_sys_param.cali.curr <= 100) || (g_sys_param.cali.curr >= 10000))
			{
				g_sys_param.cali.curr = 1000;
				g_sys_param.updat_flag = 1;
			}
			if((g_sys_param.cali.sy_curr <= 100) || (g_sys_param.cali.sy_curr >= 10000))
			{
				g_sys_param.cali.sy_curr = 1000;
				g_sys_param.updat_flag = 1;
			}
			if((g_sys_param.cali.temp[0] <= 100) || (g_sys_param.cali.temp[0] >= 10000))
			{
				g_sys_param.cali.temp[0] = 1000;
				g_sys_param.updat_flag = 1;
			}
			if((g_sys_param.cali.temp[1] <= 100) || (g_sys_param.cali.temp[1] >= 10000))
			{
				g_sys_param.cali.temp[1] = 1000;
				g_sys_param.updat_flag = 1;
			}
			if((g_sys_param.cali.temp[2] <= 100) || (g_sys_param.cali.temp[2] >= 10000))
			{
				g_sys_param.cali.temp[2] = 1000;
				g_sys_param.updat_flag = 1;
			}
			if((g_sys_param.cali.volat <= 100) || (g_sys_param.cali.volat >= 10000))
			{
				g_sys_param.cali.volat = 1000;
				g_sys_param.updat_flag = 1;
			}
		}
	}else//如果flash里面没有数据,则将设备设置为默认服务器
	{
		g_sys_param.server_ip[0] = 182;
		g_sys_param.server_ip[1] = 131;
		g_sys_param.server_ip[2] = 21;
		g_sys_param.server_ip[3] = 106;
		g_sys_param.server_ip[4] = 0;
		g_sys_param.server_ip[5] = 0;
		g_sys_param.port         = 7021;

		g_sys_param.camp_time  = DEFAULT_CAMP_TIME;												  //默认采样时间为		2s
		g_sys_param.hart_time  = DEFAULT_HART_TIME;													//默认心跳时间为		30s
		g_sys_param.threa.curr1 = DEFAULT_CURR;						                  //默认报警电流			最大量程的10%
		g_sys_param.threa.curr2 = DEFAULT_CURR;						                  //默认报警电流			最大量程的10%
		g_sys_param.threa.curr3 = DEFAULT_CURR;						                  //默认报警电流			最大量程的10%
		g_sys_param.threa.sy_curr = DEFAULT_SY_CURR;												//默认报警剩余电流	最大量程的10%
		g_sys_param.threa.volat = DEFAULT_VOLAT;														//默认报警电压			380V
		g_sys_param.threa.temp1 = DEFAULT_TEMP;															//默认报警温度			60℃
		g_sys_param.threa.temp2 = DEFAULT_TEMP;															//默认报警温度			60℃
		g_sys_param.cali.curr = 1000;
		g_sys_param.cali.sy_curr = 1000;
		g_sys_param.cali.temp[0] = 1000;
		g_sys_param.cali.temp[2] = 1000;
		g_sys_param.cali.volat = 1000;
//		g_sys_param.threa.temp3 = DEFAULT_TEMP;															//默认报警温度			60℃
//		g_sys_param.threa.hj_humi = DEFAULT_HJ_HUMI;												//默认报警环境湿度	10%RH
//		g_sys_param.threa.hj_temp = DEFAULT_HJ_TEMP;												//默认报警环境温度 60℃
		Sampling_TimerConfig( g_sys_param.camp_time );										  //设置采样时间
	}
}



/*****************************************************************************
 * 函数功能:		字符串比较
 * 形式参数:		a 被比较的字符串 b 比较字符串 len 字符串长度 k 结束位置
 * 返回参数:		SUCCESS 找到字符串 ERROR 未找到字符串
 * 更改日期;		2018-03-08				函数移植
 ****************************************************************************/ 
ErrorStatus strbj(char *a , char *b ,uint16_t len, uint16_t *k)
{ 
	uint16_t i = 0;											//循环变量  
	uint16_t j = 0;											//循环变量
	uint16_t len1 = 0;									//比较字符串的长度
	char *m;														//字符串里面的字符
	
	len1=strlen(b);
	m=a;
	for(i=0;i<len;i++)
	{
		if(m[i]==*b)
		{
			  for(j=0;j<len1;j++)
	       {
					 if(m[i+j]!=b[j])break;
				 }
				 if(j==len1)
				 {
					 *k=i+j;
					 return SUCCESS;
				 }
		}
	}
	return ERROR;
}
/*****************************************************************************
 * 函数功能:		AT发送
 * 形式参数:		p 发送的数据 len 数据长度 c AT指令回复数据
 * 返回参数:		SUCCESS 重启成功		ERROR 重启失败
 * 更改日期;		2018-03-07				函数移植
							2018-04-02				在读取完成数据后,使能ADC,发送数据的时候禁止ADC
 ****************************************************************************/ 
ErrorStatus IOT_ATsend(uint8_t *p , uint16_t len , char *c)
{
	uint8_t i=0;												//循环变量
	uint8_t length = 0;									//数据长度
	uint8_t AT_buf[100] = {0};					//AT指令的数组
	uint16_t k = 0;											//字符串地址
	
	
	memcpy(AT_buf       , p   , len);
	memcpy(&AT_buf[len] , END , 2);
	length = len + 2;
	
	NB_SendData(AT_buf , length);
	for(i = 0;i < 40;i++)//500ms等待AT消息回复 25*20ms
	{
		NB_ReceiveData();//收到IOT消息
		if(g_nb_newdata_flag)
		{
			g_nb_newdata_flag = 0;
			if(strbj((char*)g_nb_rx_buff,c,g_nb_rx_cnt,&k) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}
	return ERROR;
}
/*****************************************************************************
 * 函数功能:		AT仅发送消息
 * 形式参数:		p 发送的数据 len 数据长度 c
 * 返回参数:		SUCCESS 重启成功		ERROR 重启失败
 * 更改日期;		2018-03-07				函数移植
							2018-03-08				对此处进行优化;如果内存申请失败,则重新申请内存,直到申请成功
							2018-04-02				发送数据之前禁止adc,发送数据后使能adc
 ****************************************************************************/ 
 void IOT_onlyATsend(uint8_t *p , uint16_t len)
{
	uint16_t length = 0;								//数据长度
	uint8_t  *buf;											//字符串数组
	uint8_t temp = 0;
	
	do
	{
		buf = (uint8_t*)calloc(len+2 , sizeof(uint8_t));//申请内存
		temp++;
	}while((buf == NULL) && (temp < 0xF0));										//申请失败,则重新申请
	if(temp >= 0xF0){runstate_to_usart("Iot_onlyATsend 申请内存失败\r\n");return;}
	
	memcpy(buf , p , len);
	memcpy(&buf[len],END,2);
	length=len+2;
	
	NB_SendData(buf,length);
	free(buf);//释放内存	
}
/*****************************************************************************
 * 函数功能:		获取STM32F030的芯片ID
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-07				函数移植
							2018-06-08			  函数优化;通过IEMI码来确定网关号
							2018-06-15				修复bug;平台网关地址上面多了一个0,tmp_ip多乘了一次
							2018-07-03				函数优化;在本函数进行流程的增加
 ****************************************************************************/
ErrorStatus Get_ChipID( uint8_t *cnt)
{
	uint64_t tmp_id = 0;
	uint8_t  i      = 0;
	uint8_t  len    = 0;
	
	len = strlen((char*)g_sys_param.IMEI);
	for(i = 2;i < len;i++)//去掉 86 
	{
		tmp_id *= 10;
		tmp_id = tmp_id + g_sys_param.IMEI[i] - 0x30;
	}
	Device_ID[0] = (uint8_t)(tmp_id >> 0 );
	Device_ID[1] = (uint8_t)(tmp_id >> 8 );
	Device_ID[2] = (uint8_t)(tmp_id >> 16);
	Device_ID[3] = (uint8_t)(tmp_id >> 24);
	Device_ID[4] = (uint8_t)(tmp_id >> 32);
	Device_ID[5] = (uint8_t)(tmp_id >> 40);
	memcpy(g_sys_param.device_ip , Device_ID , 6);
	
	g_nb_reset_flow++;
	*cnt = 0;
	return SUCCESS;
}
/*****************************************************************************
 * 函数功能:		将数字转换成字符串
 * 形式参数:		num 要转换的数字 p 转换后的数组 len 数组长度
 * 返回参数:		SUCCESS 重启成功		ERROR 重启失败
 * 更改日期;		2018-03-08				函数移植
 ****************************************************************************/ 
void Num_exchange(uint32_t num , uint8_t *p , uint16_t* len)
{
	uint32_t	i		=	0;									//循环变量
	uint32_t	a		=	0;									//
	uint32_t	nu	=	num;								//需要转换的数字
	uint8_t		*k;												//指向转换后数组的指针
	
	k=p;
	do
	{
		*(k+i) = nu%10+'0';
		nu    /= 10;
		i++;
	}while(nu>0);												//将一个数字,转换成 字符
	*(k+i)=0;														//字符串结尾
	*len=i;															//字符串长度
	for(i = 0; i< (*len/2);i++)
	{
		a             = *(k+i);
		*(k+i)        = *(k+*len-1-i);
		*(k+*len-1-i) = a;
	}
}

/*****************************************************************************
 * 函数功能:		发送数据
 * 形式参数:		num 要转换的数字 p 转换后的数组 len 数组长度
 * 返回参数:		SUCCESS 重启成功		ERROR 重启失败
 * 更改日期;		2018-03-08				函数移植
 ****************************************************************************/ 
ErrorStatus NB_SendServerData(uint8_t *NB_Handle_TX_BUF , uint8_t *data , uint16_t len)
{
	uint8_t temp = 0;
	uint16_t i = 0;											//循环变量
	uint16_t m = 0;											//字符位置

	for(i = 0;i < len;i++)
	{
		temp = data[i] >> 4;
		if(temp > 9)
		{
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++] = temp-10+'A';
		}else
		{
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++] = temp+'0';
		}
		temp = data[i] & 0x0F;
		if(temp > 9)
		{
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++] = temp-10+'A';
		}else
		{
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++] = temp+'0';
		}
	}
	memcpy(&(NB_Handle_TX_BUF[NB_Handle_TX_CNT]),END,2);//?
	NB_Handle_TX_CNT+=2;
	
	NB_SendData(NB_Handle_TX_BUF,NB_Handle_TX_CNT);
	for(i=0;i<60;i++)//500ms等待AT消息回复
	{
		NB_ReceiveData();//收到IOT消息
		if(len>0)
		{
			if(strbj((char*)g_nb_rx_buff,"ERROR",g_nb_rx_cnt,&m)==SUCCESS){return ERROR;}
			if(strbj((char*)g_nb_rx_buff,"OK"   ,g_nb_rx_cnt,&m)==SUCCESS){return SUCCESS;}
		}
	}
	return ERROR;
}
/*------------------------------File----------End------------------------------*/

