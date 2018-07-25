/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-08					文档编写
						2018-03-13					驱动程序不能正常允许,对驱动程序进行修改
						2018-05-11					为了节约内存对数据保存进行更改
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "BC95.h"
#include "delay.h"
#include "configure.h"
#include "string.h"
#include "key_led.h"
#include "usart.h"
/* 宏定义	--------------------------------------------------------------------*/
char BC95_CreatCOAP[]={"AT+NCDP=117.60.157.137"};				//创建COAP，本地端口6000              正式平台IP
//char BC95_CreatCOAP[]={"AT+NCDP=180.101.147.115"};				//创建COAP，本地端口6000               测试平台IP
char BC95_CreatUDP[]={"AT+NSOCR=DGRAM,17,7021,1"};				//创建UDP   7021是端口号
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
void BC95_CSQ(uint8_t *buf)
{
	NB_ReadMoudleCSQ(buf , "+CSQ:");
}
/*****************************************************************************
 * 函数功能:		软件复位
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void BC95_Reset(void)
{
	IOT_ATsend((uint8_t*)"AT+NRB" , strlen("AT+NRB") , "OK");
}
/*****************************************************************************
 * 函数功能:		获取发送功率
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void BC95_ReadSignal(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//循环变量

	if(NB_ReadMoudleData("AT+NUESTATS" , strlen("AT+NUESTATS") , "Signal power:" , &k) == SUCCESS)
	{
		for(j = k;j < g_nb_rx_cnt;j++)
		{
			if((g_nb_rx_buff[j] != 0x0D) && (g_nb_rx_buff[j+1] != 0x0A))
			{
				buf[index] = g_nb_rx_buff[j];
				index++;
			}else
			{
				buf[index] = 0;
				return ;
			}
		}
	}
	buf[0] = '0';
	buf[1] = 0;
}

/*****************************************************************************
 * 函数功能:		获取基站编号
 * 形式参数:		无
 * 返回参数;		SUCCESS 有信号  ERROR 无信号
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void BC95_ReadPCI(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//循环变量

	if(NB_ReadMoudleData("AT+NUESTATS" , strlen("AT+NUESTATS") , "PCI:" , &k) == SUCCESS)
	{
		for(j = k;j < g_nb_rx_cnt;j++)
		{
			if((g_nb_rx_buff[j] != 0x0D) && (g_nb_rx_buff[j+1] != 0x0A))
			{
				buf[index] = g_nb_rx_buff[j];
				index++;
			}else
			{
				buf[index] = 0;
				return;
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
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void BC95_ReadCellID(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//循环变量

	if(NB_ReadMoudleData("AT+NUESTATS" , strlen("AT+NUESTATS") , "Cell ID:" , &k) == SUCCESS)
	{
		for(j = k;j < g_nb_rx_cnt;j++)
		{
			if((g_nb_rx_buff[j] != 0x0D) && (g_nb_rx_buff[j+1] != 0x0A))
			{
				buf[index] = g_nb_rx_buff[j];
				index++;
			}else
			{
				buf[index] = 0;
				return;
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
 * 更改日期;		2018-06-21				函数移植
 ****************************************************************************/
void BC95_ReadSNR(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//循环变量

	if(NB_ReadMoudleData("AT+NUESTATS" , strlen("AT+NUESTATS") , "SNR:" , &k) == SUCCESS)
	{
		for(j = k;j < g_nb_rx_cnt;j++)
		{
			if((g_nb_rx_buff[j] != 0x0D) && (g_nb_rx_buff[j+1] != 0x0A))
			{
				buf[index] = g_nb_rx_buff[j];
				index++;
			}else
			{
				buf[index] = 0;
				return;
			}
		}
	}
	buf[0] = '0';
	buf[1] = 0;
}
/*****************************************************************************
 * 函数功能:		iot的socket端口创建
 * 形式参数:		无
 * 返回参数:		SUCCESS 重启成功		ERROR 重启失败
 * 更改日期;		2018-03-07				函数移植
							2018-04-02				在读取数据完成后,使能ADC
							2018-07-03				函数优化,在本函数进行流程的增加和显示
 ****************************************************************************/ 
ErrorStatus BC95_Portcreat(uint8_t *re_cnt)
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
			state = IOT_ATsend((uint8_t*)BC95_CreatCOAP , strlen(BC95_CreatCOAP) , "OK");
			break;}
		case YIDONG:{//移动卡，使用UDP链接
			strbj(BC95_CreatUDP , "DGRAM,17," , strlen(BC95_CreatUDP) , &k);
			Num_exchange(g_sys_param.port,(uint8_t*)&BC95_CreatUDP[k],&i);
			k +=i;
			BC95_CreatUDP[k++]=',';
			BC95_CreatUDP[k++]=socket+'0';//赋值最新sock
			IOT_onlyATsend((uint8_t*)BC95_CreatUDP,strlen(BC95_CreatUDP));//AT仅发送消息
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
 * 更改日期;		2018-03-07				函数移植
							2018-04-25				针对CoAP协议进行修改
							2018-05-04				延时的位置更改
							2018-05-28				状态显示
							2018-05-30				程序结构更改,优化了初始化结构,系统待在这个函数的时间被缩短了
							2018-06-04				修复bug;IEMI码和IMSI码读取错误,包含了不为0-9的字符
							2018-06-07				修复bug;入不了网的时候导致设备重启,,在长延时前面添加看门狗
 ****************************************************************************/
ErrorStatus BC95_init( uint8_t* cnt , uint8_t *flag)
{
  uint16_t i        = 0;											//循环变量
	ErrorStatus state	= ERROR;									//返回值状态
	uint8_t max = 20;
	
	switch(g_nb_reset_flow)
	{
		case 1:{//模块开机,读取设备信息
			max   = 70;
			state = NB_WaitStartMessage("REBOOT_CAUSE" , cnt);
			break;}
		case 2:{//模块固件版本查询
			state = NB_ReadMoudleVer("ATI" , strlen("ATI") , "Revision:" ,  cnt);
			break;}
		case 3:{//IMEI号查询
			state = NB_ReadMoudleIMEI("+CGSN:" , cnt);
			break;}
		case 4:{//查询SIM卡
			state = NB_SendCmd("AT+CFUN?" , strlen("AT+CFUN?") , "OK" , &i , cnt);
			break;}
		case 5:{//ISMI号查询
			state = NB_ReadMoudleISMI("\r\n" , cnt);
			break;}
		case 6:{//网络附着情况(有时候30s左右)
			max = 60;
			if(NB_SendCmd("AT+CGATT?" , strlen("AT+CGATT?") , "+CGATT:1" , &i ,cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("网络附着成功!\r\n");
			}break;}
		case 7:{//网络注册情况
			max = 60;
			if(NB_SendCmd("AT+CEREG?" , strlen("AT+CEREG?") , "+CEREG:0,1" , &i, cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("网络链路正常!\r\n");
			}break;}
		case 8:{//查询运营商
			max   = 70;
			state = NB_ReadMoudleOperator("+COPS:" , cnt);
			break;}
		case 9:{//接收消息主动返回
			if(NB_SendCmd("AT+NNMI=2" , strlen("AT+NNMI=2") , "OK" , &i , cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("主动接收开启成功!\r\n");
			}break;}
		case 10:{//iot的socket端口创建
			state = BC95_Portcreat( cnt );
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

/*****************************************************************************
 * 函数功能:		NB-IOT发送函数
 * 形式参数:		ip ip地址 port 端口号 sockte p 数据 len 数据长度
 * 返回参数;		SUCCESS 发送成功		ERROR 发送失败
 * 更改日期;		2018-03-08				函数移植
							2018-03-29				将全局变量 NB_Handle_TX_BUF 设置为局部变量
							2018-04-02				此处增加;在读取完NB模块数据后,使能ADC
              2018-04-25        coap协议更改
							2018-06-08				程序优化;删除形参 socket 
							2018-07-02				程序优化;将公用部分的程序,打包成函数 NB_SendServerData
 ****************************************************************************/ 
ErrorStatus BC95_SendData(uint8_t *p , uint16_t len)
{
	uint8_t NB_Handle_TX_BUF[NB_TX_LEN];//NB发送待处理的 buf  到模块
	uint16_t i = 0;											//循环变量
	uint8_t  x[50] = {0};								//字符串数组
	uint16_t  l = 0;											//长度
	
	NB_Handle_TX_CNT=0;//清零发送长度
	switch(NB_Ope)
	{
		case DIANXIN:{//电信卡
			i = strlen("AT+NMGS=");
			memcpy(&NB_Handle_TX_BUF[NB_Handle_TX_CNT],(uint8_t *)"AT+NMGS=",i);
			NB_Handle_TX_CNT += i;
			Num_exchange(len,x,&l);//coap需传输的数据长度
			memcpy(&(NB_Handle_TX_BUF[NB_Handle_TX_CNT]),x,l);
			NB_Handle_TX_CNT+=l;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			break;}
		case YIDONG:{//移动卡
			i = strlen("AT+NSOST=");
			memcpy(&NB_Handle_TX_BUF[NB_Handle_TX_CNT],(uint8_t *)"AT+NSOST=",i);
			NB_Handle_TX_CNT += i;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=socket+'0';//socket
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			int_to_char((char*)&NB_Handle_TX_BUF[NB_Handle_TX_CNT] , g_sys_param.server_ip , 4);//ip
			NB_Handle_TX_CNT = strlen((char*)NB_Handle_TX_BUF);
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			Num_exchange(g_sys_param.port , &NB_Handle_TX_BUF[NB_Handle_TX_CNT],&i);//端口号
			NB_Handle_TX_CNT += i;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			Num_exchange(len , &NB_Handle_TX_BUF[NB_Handle_TX_CNT],&i);//数据长度
			NB_Handle_TX_CNT += i;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			break;}
		default:{//其他卡
			while(1);}//等待系统重启
	}
	return NB_SendServerData(NB_Handle_TX_BUF , p , len);
}

/*****************************************************************************
 * 函数功能:		查询接收数据
 * 形式参数:		无
 * 返回参数;		无
 * 更改日期;		2018-03-08				函数移植
							2018-04-25				coap协议的更改
							2018-06-27				当模块异常的时候,依然在去接收数据,更改后为;模块异常后,不再去接收数据
 ****************************************************************************/
void BC95_ReceiveData( uint8_t* cnt)
{
	uint16_t p = 0,q=0;
	uint8_t buff[50] = {0};

	if((g_nb_error_flag) || (!g_model_config_flag))return;
	NB_ReceiveData();//查收数据消息+NSONMI:0,68
	switch(NB_Ope)
	{
		case DIANXIN:{//电信卡
			if(g_nb_newdata_flag)//收到IOT消息
			{
				g_nb_newdata_flag = 0;
				if(strbj((char*)g_nb_rx_buff,"+NNMI",g_nb_rx_cnt,&p)==SUCCESS)
				{
					NB_messag_Read("AT+NMGR" ,strlen("AT+NMGR"));			//发送查看消息
				}
				g_nb_rx_cnt = 0;
			}else
			{
				if(((g_sys_tim_s & 0xFF) < g_rec_time) && (g_rec_time != 0))
				{
					(*cnt)++;
					if((*cnt)%2 == 0)
					{
						(*cnt) = 0;
						NB_messag_Read("AT+NMGR" ,strlen("AT+NMGR"));			//发送查看消息
					}
				}else
				{
					(*cnt)        = 0;
					g_rec_time = 0;
				}
			}break;}
		case YIDONG:{//移动卡
			if(g_nb_newdata_flag)
			{
				g_nb_newdata_flag = 0;
				p = strlen("+NSONMI:");
				memcpy(buff,"+NSONMI:",p);
				buff[p++]=socket+'0';
				buff[p++] = ',';
				if(strbj((char*)g_nb_rx_buff,(char*)buff,g_nb_rx_cnt,&p)==SUCCESS)
				{
					q = strlen("AT+NSORF=");
					memcpy(buff , "AT+NSORF=" , q);
					buff[q++]=socket+'0';
					buff[q++]=',';
					for(;p<g_nb_rx_cnt;p++)
					{
						if(g_nb_rx_buff[p] != '\r')
						{
							buff[q++] = g_nb_rx_buff[p];
						}else break;
					}					
					NB_messag_Read((char*)buff ,strlen((char*)buff));			//发送查看消息
				}
				g_nb_rx_cnt = 0;
			}else
			{
				if(((g_sys_tim_s & 0xFF) < g_rec_time) && (g_rec_time != 0))
				{
					(*cnt)++;
					if((*cnt)%2 == 0)
					{
						(*cnt) = 0;
						NB_messag_Read("AT+NSORF=0,255" ,strlen("AT+NSORF=0,255"));			//发送查看消息
					}
				}else
				{
					(*cnt)        = 0;
					g_rec_time = 0;
				}
			}
			break;}
		default:{//其他卡
			while(1);}//设备重启
	}
}
/*------------------------------File----------End------------------------------*/
