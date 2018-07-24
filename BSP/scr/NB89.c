/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-07-02					文档编写
 *说    明:	利尔达的模块和移远的模块相差不大
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "NB89.h"
#include "delay.h"
#include "configure.h"
#include "string.h"
#include "key_led.h"
#include "usart.h"
/* 宏定义	--------------------------------------------------------------------*/
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
 char NB86_CreatCOAP[]={"AT+NCDP=117.60.157.137"};				//创建COAP，本地端口6000              正式平台IP
//char NB86_CreatCOAP[]={"AT+NCDP=180.101.147.115"};				//创建COAP，本地端口6000               测试平台IP
char NB86_CreatUDP[]={"AT+NSOCR=DGRAM,17,7021,1"};				//创建UDP   7021是端口号
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		iot的socket端口创建
 * 形式参数:		无
 * 返回参数:		SUCCESS 重启成功		ERROR 重启失败
 * 更改日期;		2018-07-02				函数移植
 ****************************************************************************/ 
ErrorStatus NB86_Portcreat(uint8_t *re_cnt)
{
  uint16_t i = 0;											//循环变量
	uint16_t k = 0;
	ErrorStatus state = ERROR;
	
	/*1>关闭UDP端口,模块默认使用UDP*/
	if(IOT_ATsend((uint8_t*)"AT+NSOCL=0",strlen("AT+NSOCL=0"),"OK")==SUCCESS)
	{
		runstate_to_usart("UDP关闭端口成功!\r\n");
	}else//关闭端口
	{
		runstate_to_usart("UDP关闭端口失败!\r\n");
	}
	switch(NB_Ope)
	{
		case DIANXIN:{//电信卡，使用COAP链接
			state = IOT_ATsend((uint8_t*)NB86_CreatCOAP , strlen(NB86_CreatCOAP) , "OK");
			break;}
		case YIDONG:{//移动卡，使用UDP链接
			strbj(NB86_CreatUDP , "DGRAM,17," , strlen(NB86_CreatUDP) , &k);
			Num_exchange(g_sys_param.port,(uint8_t*)&NB86_CreatUDP[k],&i);
			k +=i;
			NB86_CreatUDP[k++]=',';
			NB86_CreatUDP[k++]=socket+'0';//赋值最新sock
			IOT_onlyATsend((uint8_t*)NB86_CreatUDP,strlen(NB86_CreatUDP));//AT仅发送消息
			for(i=0;i<25;i++)//500ms等待AT消息回复
			{
				NB_ReceiveData();//收到IOT消息
				if(g_nb_newdata_flag>0)
				{
					g_nb_newdata_flag = 0;
					for(i=0;i<g_nb_rx_cnt;i++)
					{
						if(g_nb_rx_buff[i]>='0'&&g_nb_rx_buff[i]<='9')socket=g_nb_rx_buff[i]-'0';//存储创建的socket  保存10进制
					}
					state = SUCCESS;
					break;
				}
			}
			break;}
		default:{//其他
			break;}
	}
	if(state == SUCCESS)
	{
		g_nb_reset_flow++;//下一个流程
		*re_cnt = 0;			//重复次数清零
		runstate_to_usart("创建端口成功\r\n");
	}
	return state;
}

/*****************************************************************************
 * 函数功能:		nbiot初始化
 * 形式参数:		无
 * 返回参数:		SUCCESS 初始化成功
 * 更改日期;		2018-07-02				函数移植
 ****************************************************************************/
 ErrorStatus NB86_init( uint8_t* cnt , uint8_t *flag)
 {
  uint16_t i        = 0;											//循环变量
	ErrorStatus state	= ERROR;									//返回值状态
	uint8_t max = 20;
	 
	switch(g_nb_reset_flow)
	{
		case 1:{//模块开机,读取设备信息
			max   = 70;
			state = NB_WaitStartMessage("AT+MLWEVTIND=3" , cnt);
			break;}
		case 2:{//模块固件版本查询
			state = NB_ReadMoudleVer("AT+CGMR" , strlen("AT+CGMR") , "SSB," ,  cnt);
			break;}
		case 3:{//IMEI号查询
			state = NB_ReadMoudleIMEI("+CGSN:" , cnt);
			break;}
		case 4:{//查询SIM卡
			state = NB_SendCmd("AT+CFUN?" , strlen("AT+CFUN?") , "+CFUN:1" , &i , cnt);
			break;}
		case 5:{//ISMI号查询
			state = NB_ReadMoudleISMI("\r\n" , cnt);
			break;}
		case 6:{//查询运营商
			max   = 70;
			state = NB_ReadMoudleOperator("+COPS:" , cnt);
			break;}
		case 7:{//网络附着情况(有时候30s左右)
			max = 60;
			if(NB_SendCmd("AT+CGATT?" , strlen("AT+CGATT?") , "+CGATT:1" , &i ,cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("网络附着成功!\r\n");
			}break;}
		case 8:{//网络注册情况
			max = 60;
			if(NB_SendCmd("AT+CEREG?" , strlen("AT+CEREG?") , "+CEREG:0,1" , &i, cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("网络链路正常!\r\n");
			}break;}
		case 9:{//接收消息主动返回
			if(NB_SendCmd("AT+NNMI=2" , strlen("AT+NNMI=2") , "OK" , &i , cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("主动接收开启成功!\r\n");
			}break;}
			case 10:{//iot的socket端口创建
			state = NB86_Portcreat( cnt );
			break;}
		case 11:{//获取设备IP
			state = Get_ChipID( cnt );												//读取设备ID
			break;}
		default:{//其他
			*flag = 1;//初始化完成
			break;}
	}
	if(*cnt > max)
	{
		runstate_to_usart("NB读取超时!\r\n");
		g_nb_reset_flow = 1;
		*cnt = 0;
	}else
	{
		state = SUCCESS;
	}
	return state;
}
/*------------------------------File----------End------------------------------*/
