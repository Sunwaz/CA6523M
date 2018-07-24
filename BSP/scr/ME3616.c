/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-06-20					文档编写
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "me3616.h"
#include "usart.h"
#include "key_led.h"
#include "configure.h"
#include "string.h"
/* 宏定义	--------------------------------------------------------------------*/
#define AT_ME3616_CSQ                   "AT+CSQ"					//查询信号强度
#define AT_ME3616_STA                   "AT*MENGINFO=0"   //查询网络状态
#define AT_ME3616_GMR										"AT+GMR"					//查询固件版本
#define AT_ME3616_CGSN                  "AT+CGSN=1"				//查询IEMI
#define AT_ME3616_COPS									"AT+COPS?"				//查询运营商"[移动]46000,46002,46007,46008[联通]46001,46006,46009[电信]46003,46005,46011"
#define AT_ME3616_CFUN									"AT+CFUN?"				//查询SIM卡
#define AT_ME3616_CIMI                  "AT+CIMI"					//查询IMSI号
#define AT_ME3616_CGATT									"AT+CGATT?"				//查询网络附着情况
#define AT_ME3616_CEREG_0								"AT+CEREG=0"			//查询网络注册状况    设置显示模式
#define AT_ME3616_CEREG                 "AT+CEREG?"       //查询网络注册状况
#define AT_ME3616_ESOC									"AT+ESOC=1,2,1"		//创建UDP链接
#define AT_ME3616_ESOCON								"AT+ESOCON=0,"		//服务器连接
#define AT_ME3616_READEN								"AT+ESOREADEN=1"	//设置上报格式为模式1  有数据返回+ESODATA ,通过AT+ESOREAD指令读取数据
#define AT_ME3616_ESOSETRPT							"AT+ESOSETRPT=0"	//设置接收数据格式为ascall码的数据
#define AT_ME3616_ESOSEND               "AT+ESOSEND=0,"		//发送数据
#define AT_ME3616_ESOREAD               "AT+ESOREAD=0,"		//数据接收
#define AT_ME3616_READ									"AT+ESOREAD=0,512"//读取数据
#define AT_ME3616_M2MCLINEW             "AT+M2MCLINEW=117.60.157.137,5683,"//链接正式平台   AT+M2MCLINEW=117.60.157.137,5683,"869662030028606",28800
#define AT_ME3616_M2MCLSEND             "AT+M2MCLISEND="  //往电信平台发送数据
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
ErrorStatus NB_messag_Read(char* Cmd , uint8_t Cmdlen);
/* 全局变量	------------------------------------------------------------------*/
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		获取信号强度
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void ME3616_CSQ(uint8_t *buf)
{
	NB_ReadMoudleCSQ(buf , "+CSQ: ");
}
/*****************************************************************************
 * 函数功能:		获取发送功率(无法查询)
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void ME3616_ReadSignal(uint8_t *buf)
{
	buf[0] = '0';
	buf[1] =  0;
}

/*****************************************************************************
 * 函数功能:		获取基站编号
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void ME3616_ReadPCI(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//循环变量
	uint16_t l = 0;

	if(NB_ReadMoudleData("AT*MENGINFO=0" , strlen("AT*MENGINFO=0") , "*MENGINFOSC: " , &k) == SUCCESS)
	{
		for(j = k;j < g_nb_rx_cnt;j++)
		{
			if(g_nb_rx_buff[j] == ',')
			{
				if(l == 1)
				{
					for(l = j+1;l < g_nb_rx_cnt;l++)
					{
						if((g_nb_rx_buff[l] !=',') && (g_nb_rx_buff[l] >= '0') && (g_nb_rx_buff[l] <= '9'))
						{
							buf[index] = g_nb_rx_buff[l];
							index ++;
						}else
						{
							buf[index] = 0;
							return;
						}
					}
				}
				l++;
			}
		}
	}
	buf[0] = '0';
	buf[1] = 0;
}
/*****************************************************************************
 * 函数功能:		软件复位
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void ME3616_Reset(void)
{
	IOT_ATsend((uint8_t*)"AT+ZRST" , strlen("AT+ZRST") , "OK");
}
/*****************************************************************************
 * 函数功能:		16进制字符串转10进制字符串
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void HexChar_to_DecChar(uint8_t *hex_char , uint8_t *dec_char , uint8_t hex_len)
{
	uint8_t i = 0;
	uint32_t dec_int = 0;
	uint16_t n = 0;
	

	for(i = 0;i < hex_len;i++)
	{
		dec_int *= 16;//16进制数
		if(hex_char[i] >= 'A' && hex_char[i] <= 'F')
		{
			dec_int += hex_char[i]-'A'+10;//A=10
		}else if(hex_char[i] >= '0' && hex_char[i] <= '9')
		{
			dec_int += hex_char[i]-'0';
		}else if(hex_char[i] >= 'a' && hex_char[i] <= 'f')
		{
			dec_int += hex_char[i]-'a'+10;//a=10
		}
	}
	Num_exchange(dec_int , dec_char , &n);
	dec_char[n]=0;
}

/*****************************************************************************
 * 函数功能:		获取小区编号
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void ME3616_ReadCellID(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//循环变量
	uint16_t l = 0;
	uint8_t t_buff[20] = {0};

	
	if(NB_ReadMoudleData("AT*MENGINFO=0" , strlen("AT*MENGINFO=0") , "*MENGINFOSC: " , &k) == SUCCESS)
	{
		for(j = k;j < g_nb_rx_cnt;j++)
		{
			if(g_nb_rx_buff[j] == '"')
			{
				l++;
				j++;
			}
			if(l == 2)//结束
			{
				t_buff[index++] = 0;
				HexChar_to_DecChar(t_buff , buf , strlen((char*)t_buff));
				return;
			}else if(l==1)//开始
			{
				t_buff[index++] = g_nb_rx_buff[j];
			}
		}
	}
	buf[0] = '0';
	buf[1] = 0;
}
/*****************************************************************************
 * 函数功能:		获取小区编号
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-19				函数移植
 ****************************************************************************/
void ME3616_ReadSNR(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//循环变量

	if(NB_ReadMoudleData("AT*MENGINFO=0" , strlen("AT*MENGINFO=0") , "*MENGINFOSC: " , &k) == SUCCESS)
	{
		for(j = k;j < g_nb_rx_cnt;j++)
		{
			if(g_nb_rx_buff[j] == '"')
			{
				if(k == 7)//结束
				{
					for(k = j;k < g_nb_rx_cnt;k++)
					{
						if((g_nb_rx_buff[k] !=',') && (g_nb_rx_buff[k] >= '0') && (g_nb_rx_buff[k] <= '9'))
						{
							buf[index] = g_nb_rx_buff[k];
							index ++;
						}else
						{
							buf[index] = 0;
							return;
						}
					}
				}
				k++;
			}
		}
	}
	buf[0] = '0';
	buf[1] = 0;
}


/*****************************************************************************
 * 函数功能:		创建端口
 * 形式参数:		无
 * 返回参数:		SUCCESS 初始化成功
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
ErrorStatus ME3616_CreatSocket(uint8_t *re_cnt)
{
	uint16_t l,j = 0;
	uint8_t buff[50] = {0};

	switch(NB_Ope)
	{
		case YIDONG:{//移动卡
			if(*re_cnt == 1)IOT_ATsend((uint8_t*)"AT+ESOC=1,2,1",strlen("AT+ESOC=1,2,1"),"+ESOC=");
			l = strlen("AT+ESOCON=0,");
			memcpy((char*)buff , "AT+ESOCON=0," , l);
			Num_exchange(g_sys_param.port , &buff[l] , &j);
			l += j;
			buff[l++] = ',';
			buff[l++] = '"';
			int_to_char((char*)&buff[l] , g_sys_param.server_ip , 4);
			l = strlen((char*)buff);
			buff[l++] = '"';
			buff[l++] = 0;
			if(NB_SendCmd((char*)buff , strlen((char*)buff) , "OK" ,&l ,re_cnt) == SUCCESS)
			{
				runstate_to_usart("端口创建成功!\r\n");
				return SUCCESS;
			}break;}
		case DIANXIN:{//电信卡
			l = strlen(AT_ME3616_M2MCLINEW);
			memcpy((char*)buff,AT_ME3616_M2MCLINEW , l);
			buff[l]='"';
			l++;
			memcpy((char*)&buff[l],g_sys_param.IMEI,strlen((char*)g_sys_param.IMEI));
			l += strlen((char*)g_sys_param.IMEI);
			buff[l++]='"';
			buff[l++]=',';
			buff[l++]='2';
			buff[l++]='8';
			buff[l++]='8';
			buff[l++]='0';
			buff[l++]='0';
			buff[l++]=0;//结尾
			if(NB_SendCmd((char*)buff , strlen((char*)buff) , "success" ,&l ,re_cnt) == SUCCESS)
			{
				runstate_to_usart("端口创建成功!\r\n");
				return SUCCESS;
			}break;}
		default:{//其他卡
			g_sys_error_flag = 1;
			LED_Control(L_WORK  , OPEN);
			LED_Control(L_ERROR , OPEN);
			while(1);}
	}
	return ERROR;
}

/*****************************************************************************
 * 函数功能:		nbiot初始化
 * 形式参数:		无
 * 返回参数:		SUCCESS 初始化成功
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
ErrorStatus ME3616_init( uint8_t* cnt , uint8_t *flag)
{
	uint16_t i        = 0;											//循环变量
	ErrorStatus state	= ERROR;									//返回值状态
	uint8_t max = 20;
	
	switch(g_nb_reset_flow)
	{
		case 1:{//关闭回显
			state = NB_SendCmd("ATE0" , strlen("ATE0") , "OK" , &i , cnt);
			break;}
		case 2:{//模块开机,读取设备信息
			max   = 70;
			state = NB_WaitStartMessage("+IP" , cnt);
			break;}
		case 3:{//查询固件版本
			state = NB_ReadMoudleVer("AT+GMR" , strlen("AT+GMR") , "\r\n" ,  cnt);
			break;}
		case 4:{//IEMI号查询
			state = NB_ReadMoudleIMEI("+CGSN: " , cnt);
			break;}
		case 5:{//判断SIM卡
			state = NB_SendCmd("AT+CFUN?" , strlen("AT+CFUN?") , "+CFUN: 1" , &i , cnt);
			break;}
		case 6:{//查询IMSI
			state = NB_ReadMoudleISMI("\r\n" , cnt);
			break;}
		case 7:{//查询运营商
			max   = 70;
			state = NB_ReadMoudleOperator("+COPS: " , cnt);
			break;}
		case 8:{//网络附着情况(有时候30s左右)
			max = 60;
			if(NB_SendCmd("AT+CGATT?" , strlen("AT+CGATT?") , "+CGATT: 1" , &i ,cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("网络附着成功!\r\n");
			}break;}
		case 9:{//设置网络注册显示模式
			state = NB_SendCmd("AT+CEREG=0" , strlen("AT+CEREG=0") , "OK" , &i ,cnt);
			break;}
		case 10:{//查询网络注册情况
			max = 60;
			if(NB_SendCmd("AT+CEREG?" , strlen("AT+CEREG?") , "+CEREG: 0,1" , &i ,cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("网络注册成功!\r\n");
			}break;}
		case 11:{//创建链接
			state = ME3616_CreatSocket(cnt);
			break;}
		case 12:{//获取设备IP
			state = Get_ChipID( cnt );												//读取设备ID
			break;}
		case 13:{//设置数据上报格式
			if(NB_Ope == YIDONG)
			{
				if(NB_SendCmd("AT+ESOREADEN=1" ,strlen("AT+ESOREADEN=1") , "OK" , &i , cnt) == SUCCESS)
				{
					state = SUCCESS;
					runstate_to_usart("打开接收回显!\r\n");
				}
			}else
			{
				state = SUCCESS;
				g_nb_reset_flow++;//下一个流程
				*cnt = 0;						//复位计时
			}break;}
		case 14:{//设置接收数据的格式
			if(NB_Ope == YIDONG)
			{
				if(NB_SendCmd("AT+ESOSETRPT=0" ,strlen("AT+ESOSETRPT=0") , "OK" , &i , cnt) == SUCCESS)
				{
					state = SUCCESS;
					runstate_to_usart("设置ASCALL输出!\r\n");
				}
			}else
			{
				state = SUCCESS;
				g_nb_reset_flow++;//下一个流程
				*cnt = 0;						//复位计时
			}break;}
		default:{//其他
			*flag = 1;//初始化完成
			break;}
	}
	if(*cnt > max)
	{
		runstate_to_usart((char*)"NB读取超时!\r\n");
		g_nb_reset_flow = 1;
		*cnt = 0;
	}else
	{
		state = SUCCESS;
	}
	return state;
}
/*****************************************************************************
 * 函数功能:		发送数据
 * 形式参数:		data 发送的数据   length 数据长度
 * 返回参数;		SUCCESS 发送成功  ERROR 发送失败
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
ErrorStatus ME3616_SendData(uint8_t *p , uint16_t len)
{
	uint8_t NB_Handle_TX_BUF[NB_TX_LEN];//NB发送待处理的 buf  到模块
	uint8_t  x[50] = {0};								//字符串数组
	uint16_t  l = 0;											//长度
	
	NB_Handle_TX_CNT=0;//清零发送长度
	switch(NB_Ope)
	{
		case YIDONG:{//移动
			memcpy(&NB_Handle_TX_BUF[NB_Handle_TX_CNT],(uint8_t *)AT_ME3616_ESOSEND,strlen(AT_ME3616_ESOSEND));
			NB_Handle_TX_CNT+=strlen(AT_ME3616_ESOSEND);
			Num_exchange(len,x,&l);//coap需传输的数据长度
			memcpy(&(NB_Handle_TX_BUF[NB_Handle_TX_CNT]),x,l);
			NB_Handle_TX_CNT+=l;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
		}break;
		case DIANXIN:{//电信
			memcpy(&NB_Handle_TX_BUF[NB_Handle_TX_CNT],(uint8_t *)AT_ME3616_M2MCLSEND,strlen(AT_ME3616_M2MCLSEND));
			NB_Handle_TX_CNT+=strlen(AT_ME3616_M2MCLSEND);
		}break;
		default:{
			break;}
	}
	return NB_SendServerData(NB_Handle_TX_BUF , p , len);
}

/*****************************************************************************
 * 函数功能:		查询接收数据
 * 形式参数:		无
 * 返回参数;		无
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void ME3616_ReceiveData( uint8_t* cnt)
{
	uint16_t p = 0;
	uint16_t j = 0;
  uint8_t buff[50] = {0};
	uint16_t index = 0;
	
	NB_ReceiveData();//查收数据消息+NSONMI:0,68
	if(g_nb_newdata_flag)//收到IOT消息
	{
		g_nb_newdata_flag = 0;
		switch(NB_Ope)
		{
			case YIDONG:{
				if(strbj((char*)g_nb_rx_buff,"+ESODATA=0,",g_nb_rx_cnt,&p)==SUCCESS)
				{
					index = strlen(AT_ME3616_ESOREAD);
					memcpy((char*)buff , AT_ME3616_ESOREAD , index);
					for(j = p;j < g_nb_rx_cnt;j++)
					{
						if((g_nb_rx_buff[j] >= '0') && (g_nb_rx_buff[j] <= '9'))
						{
							buff[index++]=g_nb_rx_buff[j];
						}else break;
					}
					NB_messag_Read((char*)buff ,strlen((char*)buff));			//发送查看消息
				}
				g_nb_rx_cnt = 0;
			}break;
			case DIANXIN:{
				if(strbj((char*)g_nb_rx_buff,"+M2MCLIRECV:",g_nb_rx_cnt,&p)==SUCCESS)
				{
					NB_messag_Read(0 ,0);			//发送查看消息
					g_nb_rx_cnt = 0;
				}
			}break;
		}
	}else
	{
		if(((g_sys_tim_s & 0xFF) < g_rec_time) && (g_rec_time != 0))
		{
			if(NB_Ope == YIDONG)
			{
				(*cnt)++;
				if((*cnt)%3 == 0)
				{
					(*cnt) = 0;
					if(NB_messag_Read(AT_ME3616_READ ,strlen(AT_ME3616_READ)) == ERROR)			//发送查看消息
					{
						(*cnt)     = 0;
						g_rec_time = 0;
					}
				}
			}
		}else
		{
			(*cnt)        = 0;
			g_rec_time = 0;
		}
	}
}
/*------------------------------File----------End------------------------------*/
