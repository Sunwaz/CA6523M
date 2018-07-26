/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-03-08					文档移植
						2018-03-12					485驱动编写
 *说    明:	
*******************************************************************************************/
/* 头文件	------------------------------------------------------------------*/
#include "usart.h"
#include "delay.h"
#include "configure.h"
#include "key_led.h"
#include "timer.h"
/* 宏定义	------------------------------------------------------------------*/
/** @addtogroup 驱动移植部分
  * @{
  */
#define RCC_USART_485_GPIO_CMD  			RCC_AHBPeriphClockCmd
#define RCC_USART_485_GPIO_CLK  			RCC_AHBPeriph_GPIOA
#define USART_485_TX_PIN        			GPIO_Pin_2
#define USART_485_TX_PORT       			GPIOA
#define USART_485_RX_PIN        			GPIO_Pin_3
#define USART_485_RX_PORT       			GPIOA
#define USART_485_TX_AFSOURCE   			GPIO_PinSource2
#define USART_485_RX_AFSOURCE   			GPIO_PinSource3
#define USART_485_AF            			GPIO_AF_1
#define RCC_485_USART_CMD       			RCC_APB1PeriphClockCmd
#define RCC_485_USART_CLK       			RCC_APB1Periph_USART2
#define USART_485_COM						      USART2
#define USART_485_BAUDRATE       			2400
#define USART_485_MODE           			(USART_Mode_Tx | USART_Mode_Rx)
#define USART_485_IRQN           			USART2_IRQn

#define USART_485_CTL_PIN        			GPIO_Pin_1
#define USART_485_CTL_PORT       			GPIOA

#define USART_485_TX_MODE							GPIO_SetBits(   USART_485_CTL_PORT , USART_485_CTL_PIN )
#define USART_485_RX_MODE        			GPIO_ResetBits( USART_485_CTL_PORT , USART_485_CTL_PIN )

#define RCC_USART_NB_GPIO_CMD    			RCC_AHBPeriphClockCmd
#define RCC_USART_NB_GPIO_CLK    			RCC_AHBPeriph_GPIOA
#define USART_NB_TX_PIN          			GPIO_Pin_9
#define USART_NB_TX_PORT         			GPIOA
#define USART_NB_RX_PIN          			GPIO_Pin_10
#define USART_NB_RX_PORT         			GPIOA
#define USART_NB_TX_AFSOURCE     			GPIO_PinSource9
#define USART_NB_RX_AFSOURCE     			GPIO_PinSource10
#define USART_NB_AF              			GPIO_AF_1

#define RCC_NB_USART_CMD         			RCC_APB2PeriphClockCmd
#define RCC_NB_USART_CLK         			RCC_APB2Periph_USART1
#define USART_NB_COM						      USART1
#define USART_NB_BAUDRATE        			9600
#define USART_NB_MODE            			(USART_Mode_Tx | USART_Mode_Rx)
#define USART_NB_IRQN           			USART1_IRQn

#ifndef NEW_MODEL
#define RCC_NB_RESET_CMD   		 		  	RCC_AHBPeriphClockCmd
#define RCC_NB_RESET_CLK   		 		  	RCC_AHBPeriph_GPIOA

#define NB_RESET_PIN       		 		  	GPIO_Pin_15
#define NB_RESET_PORT		       				GPIOA
#else 
#define RCC_NB_RESET_CMD   		 		  	RCC_AHBPeriphClockCmd
#define RCC_NB_RESET_CLK   		 		  	RCC_AHBPeriph_GPIOA
#define RCC_NB_POWER_CMD							RCC_AHBPeriphClockCmd
#define RCC_NB_POWER_CLK							RCC_AHBPeriph_GPIOB
#define RCC_NB_STATE_CMD							RCC_AHBPeriphClockCmd
#define RCC_NB_STATE_CLK							RCC_AHBPeriph_GPIOB


#define NB_RESET_PIN       		 		  	GPIO_Pin_11
#define NB_RESET_PORT		       				GPIOA
#define NB_POWER_PIN									GPIO_Pin_2
#define NB_POWER_PORT									GPIOB
#define NB_STATE_PIN                  GPIO_Pin_9
#define NB_STATE_PORT                 GPIOB

#define NB_POWER_H										GPIO_SetBits(		NB_POWER_PORT , NB_POWER_PIN )
#define NB_POWER_L										GPIO_ResetBits( NB_POWER_PORT , NB_POWER_PIN )
#define NB_READ_STATE									GPIO_ReadInputDataBit( NB_STATE_PORT , NB_STATE_PIN )
#endif
#define NB_RESET_H						      	GPIO_SetBits(		NB_RESET_PORT , NB_RESET_PIN )
#define NB_RESET_L       			 		  	GPIO_ResetBits( NB_RESET_PORT , NB_RESET_PIN )

#define BUFF_SIZE                     50

/**
	* @}
 */
/* 结构体定义	--------------------------------------------------------------*/
/* 内部引用	----------------------------------------------------------------*/
/* 全局变量	----------------------------------------------------------------*/
uint8_t  USART_RX1_BUF[RX_485_LEN];													//串口接收数组
uint16_t USART_RX1_CNT=0;																		//串口数据的长度
void (*crt_fun[CRT_FUN_CNT])(void);													//CRT命令队列
uint8_t g_crt_fun_cnt = 0;																	//CRT命令存放位置
uint8_t g_crt_run_cnt = 0;																	//CRT命令运行位置
uint8_t g_ack_flag    = 0;																	//CRT应答标志
uint8_t g_re_cnt      = 0;																	//CRT命令重发计数
uint8_t g_cail_type   = 0;																	//ADC校准类型
uint8_t g_usart_errortype = 0;															//串口错误类型
uint8_t g_usart_errorvalu = 0;															//串口错误值
uint8_t g_monitor_flag = 0;																	//CRT监控标志
/* 系统函数	----------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:	串口初始化
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-03-08					文档移植
 ****************************************************************************/
void USART_485_Config(void)
{
	GPIO_InitTypeDef  newGPIO_Init;
	USART_InitTypeDef newUSART_Init;
	NVIC_InitTypeDef  newNVIC_Init;
	
	RCC_USART_485_GPIO_CMD(RCC_USART_485_GPIO_CLK , ENABLE);
	newGPIO_Init.GPIO_Mode  = GPIO_Mode_AF;
	newGPIO_Init.GPIO_OType = GPIO_OType_PP;
	newGPIO_Init.GPIO_PuPd  = GPIO_PuPd_UP;
	newGPIO_Init.GPIO_Speed = GPIO_Speed_50MHz;
	/*1> 485 TX端口配置*/
	newGPIO_Init.GPIO_Pin   = USART_485_TX_PIN;
	GPIO_Init(USART_485_TX_PORT , &newGPIO_Init);
	/*2> 485 RX端口配置*/
	newGPIO_Init.GPIO_Pin   = USART_485_RX_PIN;
	GPIO_Init(USART_485_RX_PORT , &newGPIO_Init);
	/*3> 串口功能映射*/
	GPIO_PinAFConfig(USART_485_TX_PORT , USART_485_TX_AFSOURCE , USART_485_AF);
	GPIO_PinAFConfig(USART_485_RX_PORT , USART_485_RX_AFSOURCE , USART_485_AF);
	
	RCC_485_USART_CMD(RCC_485_USART_CLK , ENABLE);
	newUSART_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	newUSART_Init.USART_Parity						  = USART_Parity_No;
	newUSART_Init.USART_StopBits 						= USART_StopBits_1;
	newUSART_Init.USART_WordLength 					= USART_WordLength_8b;
	/*4> 485 串口初始化*/
	newUSART_Init.USART_BaudRate   					= USART_485_BAUDRATE;
	newUSART_Init.USART_Mode     						= USART_485_MODE;
	USART_Init(USART_485_COM , &newUSART_Init);
	USART_ClearFlag(USART_485_COM , USART_FLAG_TC);
	USART_Cmd(USART_485_COM , DISABLE);
	USART_ITConfig(USART_485_COM , USART_IT_RXNE , ENABLE);
	USART_ITConfig(USART_485_COM , USART_IT_ORE  , ENABLE);
	
	newNVIC_Init.NVIC_IRQChannelCmd      = ENABLE;
	/*5> 485 串口中断初始化*/
	NVIC_SetPriority(USART_485_IRQN , 1);                             //优先组别
	newNVIC_Init.NVIC_IRQChannelPriority = 2;
	newNVIC_Init.NVIC_IRQChannel         = USART_485_IRQN;
	NVIC_Init(&newNVIC_Init);
	USART_Cmd(USART_485_COM , ENABLE);
	
	newGPIO_Init.GPIO_Mode	=	GPIO_Mode_OUT;
	newGPIO_Init.GPIO_PuPd	=	GPIO_PuPd_DOWN;
	/*6> 485 收发控制端口初始化*/
	newGPIO_Init.GPIO_Pin	  = USART_485_CTL_PIN;
	GPIO_Init( USART_485_CTL_PORT , &newGPIO_Init );
}
/*****************************************************************************
 * 函数功能:		NB初始化
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数编写
							2018-06-19				函数整理
 ****************************************************************************/ 
void NB_Config(void)
{
	GPIO_InitTypeDef  newGPIO_Init;
	USART_InitTypeDef newUSART_Init;
	NVIC_InitTypeDef  newNVIC_Init;
	
	RCC_USART_NB_GPIO_CMD( RCC_USART_NB_GPIO_CLK , ENABLE );
	newGPIO_Init.GPIO_Mode  = GPIO_Mode_AF;
	newGPIO_Init.GPIO_OType = GPIO_OType_PP;
	newGPIO_Init.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	newGPIO_Init.GPIO_Speed = GPIO_Speed_50MHz;
	/*1> BC95 TX端口配置*/
	newGPIO_Init.GPIO_Pin   = USART_NB_TX_PIN;
	GPIO_Init(USART_NB_TX_PORT , &newGPIO_Init);
	/*2> BC95 RX端口配置*/
	newGPIO_Init.GPIO_Pin   = USART_NB_RX_PIN;
	GPIO_Init(USART_NB_RX_PORT , &newGPIO_Init);
	/*3> 串口功能映射*/
	GPIO_PinAFConfig(USART_NB_TX_PORT , USART_NB_TX_AFSOURCE , USART_NB_AF);
	GPIO_PinAFConfig(USART_NB_RX_PORT , USART_NB_RX_AFSOURCE , USART_NB_AF);
	
	RCC_NB_USART_CMD(RCC_NB_USART_CLK , ENABLE);
	newUSART_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	newUSART_Init.USART_Parity						  = USART_Parity_No;
	newUSART_Init.USART_StopBits 						= USART_StopBits_1;
	newUSART_Init.USART_WordLength 					= USART_WordLength_8b;
	/*4> BC95 串口初始化*/
	newUSART_Init.USART_BaudRate   					= USART_NB_BAUDRATE;
	newUSART_Init.USART_Mode     						= USART_NB_MODE;
	USART_Init(USART_NB_COM , &newUSART_Init);
	USART_ClearFlag(USART_NB_COM , USART_FLAG_TC);
	USART_Cmd(USART_NB_COM , DISABLE);
	USART_ITConfig(USART_NB_COM , USART_IT_RXNE , ENABLE);
	
	newNVIC_Init.NVIC_IRQChannelCmd 		 = ENABLE;
	/*4> BC95 中断初始化*/
	NVIC_SetPriority(USART_NB_IRQN , 1);                             //优先组别
	newNVIC_Init.NVIC_IRQChannelPriority = 1;
	newNVIC_Init.NVIC_IRQChannel 				 = USART_NB_IRQN;
	NVIC_Init(&newNVIC_Init);
	USART_Cmd(USART_NB_COM , ENABLE);
	
	/*4> BC95 复位端口初始化*/
	RCC_NB_RESET_CMD( RCC_NB_RESET_CLK , ENABLE );
	newGPIO_Init.GPIO_Mode = GPIO_Mode_OUT;
	newGPIO_Init.GPIO_PuPd = GPIO_PuPd_DOWN;
	newGPIO_Init.GPIO_Pin	 = NB_RESET_PIN;
	GPIO_Init( NB_RESET_PORT , &newGPIO_Init );
	/*5> NB	  电源开关端口初始化*/
 #ifdef NEW_MODEL
	RCC_NB_POWER_CMD( RCC_NB_POWER_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin  = NB_POWER_PIN;
	newGPIO_Init.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init( NB_POWER_PORT , &newGPIO_Init );
	/*6>NB   状态端口初始化*/
	RCC_NB_STATE_CMD( RCC_NB_STATE_CLK , ENABLE );
	newGPIO_Init.GPIO_Mode = GPIO_Mode_IN;
  newGPIO_Init.GPIO_Pin  = NB_STATE_PIN;
	newGPIO_Init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( NB_POWER_PORT , &newGPIO_Init );
	#endif
}

/*****************************************************************************
 * 函数功能:		NB发送数据
 * 形式参数:		p 发送数据 len 接收数据的长度
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数编写
							2018-04-02				发送数据的时候,禁止adc
 ****************************************************************************/
void NB_SendData( uint8_t *p , uint16_t len )
{
	uint16_t i=0;												//循环变量
	
	g_nb_newdata_flag = 0;              //等待数据接收
	g_nb_rx_cnt       = 0;              //收消息清零
	for(i = 0;i < len;i++)
	{
		USART_SendData( USART_NB_COM , p[i] );
		while (USART_GetFlagStatus( USART_NB_COM , USART_FLAG_TXE) == RESET);//等待发送结束
	}
}
/*****************************************************************************
 * 函数功能:		NB接收数据
 * 形式参数:		buf 接收数据存储位置 len 接收数据的长度
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数编写
 ****************************************************************************/
void NB_ReceiveData( void )
{
	uint16_t rxlen = g_nb_rx_cnt;		//接收数据的长度
	
	delay_ms(20);		
	if((rxlen == g_nb_rx_cnt)&&(rxlen))//20ms数据没有变化,则判断数据稳定
	{
		if(rxlen != 0)
		{
			g_nb_newdata_flag = 1;			//有新数据
		}
	}
}
/*****************************************************************************
 * 函数功能:	串口接收中断
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-04-24					文档编写
 ****************************************************************************/
void USART_SendOK(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x00;//发送应答信息
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x00;//发送应答信息个数
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * 函数功能:	发送校准值
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-04-24					文档编写
 ****************************************************************************/
void USART_SendCail(void)
{
	uint8_t i = 0;
	uint8_t type;
	uint16_t value;
	uint8_t channel;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x08;//发送校准值
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x01;//发送校准值信息体个数
	switch(g_cail_type)
	{
		case 1:{//温度1
			type = 0x06;//温度
			value = g_sys_param.cali.temp[0];
			channel = 1;
			break;}
		case 2:{//温度2
			type = 0x06;//温度
			value = g_sys_param.cali.temp[1];
			channel = 2;
			break;}
		case 3:{//温度3
			type = 0x06;//温度
			value = g_sys_param.cali.temp[2];
			channel = 3;
			break;}
		case 4:{//电流
			type = 0x07;//电流
			value = g_sys_param.cali.curr;
			channel = 1;
			break;}
		case 5:{//电压
			type = 0x08;//电压
			value = g_sys_param.cali.volat;
			channel = 1;
			break;}
		case 6:{//剩余电流
			type = 0x09;//剩余电流
			value = g_sys_param.cali.sy_curr;
			channel = 1;
			break;}
		default:{//其他
			return;}
	}
	USART_RX1_BUF[USART_RX1_CNT++]		= type;//数据类型
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//数据长度
	USART_RX1_BUF[USART_RX1_CNT++]		= channel;//设备通道
	USART_RX1_BUF[USART_RX1_CNT++]	  = (value >> 0x00) & 0xFF;//数据
	USART_RX1_BUF[USART_RX1_CNT++]	  = (value >> 0x08) & 0xFF;
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * 函数功能:	错误发送
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-19					文档编写
 ****************************************************************************/
void USART_SendError(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x09;//发送错误
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x01;//发送错误信息体个数
	USART_RX1_BUF[USART_RX1_CNT++]		= g_usart_errortype;//错误类型
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x01;//数据长度
	USART_RX1_BUF[USART_RX1_CNT++]		= g_usart_errorvalu;//数据值
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * 函数功能:	配置信息的发送
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-18					文档编写
 ****************************************************************************/
void USART_SendSysConfig(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//上传配置信息
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x05;//上传配置信息个数

	if(1){//服务器IP地址
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//服务器IP地址
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x04;//服务器IP地址长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.server_ip[3];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.server_ip[2];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.server_ip[1];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.server_ip[0];
	}
	if(2){//服务器端口
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//服务器端口
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//服务器端口长度
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.port >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.port >> 0x08) & 0xFF;
	}
	if(3){//设备IP地址
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//设备IP地址
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//设备IP地址长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[5];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[4];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[3];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[2];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[1];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[0];
	}
	if(4){//心跳时间
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x04;//心跳时间
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//心跳时间长度
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.hart_time >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.hart_time >> 0x08) & 0xFF;
	}
	if(5){//采样时间
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x05;//采样时间
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//采样时间长度
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.camp_time >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.camp_time >> 0x08) & 0xFF;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}
/*****************************************************************************
 * 函数功能:	版本信息的发送
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-18					文档编写
 ****************************************************************************/
void USART_SendVer(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//上传配置信息
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x01;//上传配置信息个数
	if(1){//设备程序版本
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0F;//设备程序版本
		i = strlen(SYS_VER);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//设备程序版本长度
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , SYS_VER , i);
		USART_RX1_CNT += i;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * 函数功能:	屏蔽信息的发送
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-18					文档编写
 ****************************************************************************/
void USART_SendShield(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//上传配置信息
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x04;//上传配置信息个数
	if(1){//温度传感器屏蔽
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x15;//温度传感器屏蔽类型
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//温度传感器屏蔽长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.temp;//温度传感器屏蔽数据
	}
	if(2){//电流传感器屏蔽
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x16;//电流传感器屏蔽类型
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电流传感器屏蔽长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.curr;//电流传感器屏蔽数据
	}
	if(3){//电压传感器屏蔽
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x17;//电压传感器屏蔽类型
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电压传感器屏蔽长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.volat;//电压传感器屏蔽数据
	}
	if(4){//剩余电流传感器屏蔽
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x18;//电压传感器屏蔽类型
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电压传感器屏蔽长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.sy_curr;//电压传感器屏蔽数据
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}
/*****************************************************************************
 * 函数功能:	阈值发送
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-18					文档编写
 ****************************************************************************/
void USART_SendThread(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//上传配置信息
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x07;//上传配置信息个数

	if(1){//温度报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//温度报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//温度报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//温度报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.temp1 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.temp1 >> 0x08) & 0xFF;
	}
	if(2){//温度报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//温度报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//温度报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//温度报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.temp2 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.temp2 >> 0x08) & 0xFF;
	}
	if(3){//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电流报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电流报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr1 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr1 >> 0x08) & 0xFF;
	}
	if(4){//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电流报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//电流报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr2 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr2 >> 0x08) & 0xFF;
	}
	if(5){//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电流报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电流报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr3 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr3 >> 0x08) & 0xFF;
	}
	if(6){//电压报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x08;//电压报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电压报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电压报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.volat >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.volat >> 0x08) & 0xFF;
	}
	if(7){//剩余电流阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x09;//剩余电流阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//剩余电流阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//剩余电流阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.sy_curr >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.sy_curr >> 0x08) & 0xFF;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * 函数功能:	监控数据发送
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-18					文档编写
 ****************************************************************************/
void USART_SendMonitorData(void)
{
	uint8_t i = 0;
	uint8_t tmp = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x06;//上传监控数据
	USART_RX1_BUF[USART_RX1_CNT++]		= tmp;//上传配置信息个数

	if(g_sys_param.shield.temp & 0x01){//温度报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//温度报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//温度报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//温度报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[TEMP1_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[TEMP1_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.temp & 0x02){//温度报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//温度报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//温度报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//温度报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[TEMP2_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[TEMP2_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.curr & 0x01){//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电流报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电流报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR1_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR1_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.curr & 0x02){//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电流报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//电流报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR2_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR2_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.curr & 0x04){//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//电流报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电流报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电流报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR3_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR3_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.volat & 0x01){//电压报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x08;//电压报警阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//电压报警阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电压报警阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[VOLAT_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[VOLAT_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.sy_curr & 0x01){//剩余电流阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x09;//剩余电流阈值
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//剩余电流阈值长度
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//剩余电流阈值地址
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[SY_CURR_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[SY_CURR_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[3] = tmp;          //数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}


/*****************************************************************************
 * 函数功能:	发送模块信息
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-18					文档编写
 ****************************************************************************/
void USART_SendModelMsg(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//上传配置信息
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x05;//上传配置信息个数

	if(1){//模块型号
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0A;//模块型号
		i = strlen(MODLE_NAME);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//模块型号长度
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , MODLE_NAME , i);
		USART_RX1_CNT += i;
	}
	if(2){//运营商
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0B;//运营商
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//运营商长度
		USART_RX1_BUF[USART_RX1_CNT++]	= NB_Ope;
	}
	if(3){//模块IMEI
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0C;//模块IMEI
		i = strlen((char*)g_sys_param.IMEI);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//模块IMEI长度
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , (char*)g_sys_param.IMEI , i);
		USART_RX1_CNT += i;
	}
	if(4){//模块IMSI
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0D;//模块IMSI
		i = strlen((char*)g_sys_param.IMSI);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//模块IMSI长度
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , (char*)g_sys_param.IMSI , i);
		USART_RX1_CNT += i;
	}
	if(5){//模块固件版本
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0E;//模块固件版本
		i = strlen(MODLE_VER);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//模块固件版本长度
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , MODLE_VER , i);
		USART_RX1_CNT += i;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * 函数功能:	发送传感器状态
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-26					文档编写
 ****************************************************************************/
void USART_SendSenserSta(void)
{
	uint8_t i = 0;
	uint8_t tmp = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x10;//上传传感器状态
	USART_RX1_BUF[USART_RX1_CNT++]		= tmp; //上传传感器状态信息个数

	if(1){//发送火警信号
		if(g_alarm_flag)
		{
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//发送火警
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x04;//模块型号长度
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_alarm_flag >> 0x00) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_alarm_flag >> 0x08) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_alarm_flag >> 0x10) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_alarm_flag >> 0x18) & 0xFF;
			USART_RX1_BUF[3]		= ++tmp;         //上传传感器状态信息个数
		}
	}
	if(2){//发送故障恢复信号/故障信号
		if((g_senser_flag & 0x03) == 0x02)//恢复
		{
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//发送故障恢复
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//故障恢复长度
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_senser_flag >> 0x00) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_senser_flag >> 0x08) & 0xFF;
			USART_RX1_BUF[3]		= ++tmp;         //上传传感器状态信息个数
		}else if((g_senser_flag & 0x03) == 0x01)//故障
		{
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//发送故障恢复
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//故障恢复长度
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_senser_flag >> 0x00) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_senser_flag >> 0x08) & 0xFF;
			USART_RX1_BUF[3]		= ++tmp;         //上传传感器状态信息个数
		}
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}
/*****************************************************************************
 * 函数功能:	发送传感器屏蔽状态
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-18					文档编写
 ****************************************************************************/
void USART_SendSenserShield(void)
{
	uint8_t i = 0;
	uint8_t tmp = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x10;//上传传感器状态
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x04;//上传传感器状态信息个数
	if(1){//温度传感器屏蔽
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x15;//温度传感器屏蔽类型
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//温度传感器屏蔽长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.temp;//温度传感器屏蔽数据
	}
	if(2){//电流传感器屏蔽
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x16;//电流传感器屏蔽类型
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电流传感器屏蔽长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.curr;//电流传感器屏蔽数据
	}
	if(3){//电压传感器屏蔽
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x17;//电压传感器屏蔽类型
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电压传感器屏蔽长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.volat;//电压传感器屏蔽数据
	}
	if(4){//剩余电流传感器屏蔽
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x18;//电压传感器屏蔽类型
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//电压传感器屏蔽长度
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.sy_curr;//电压传感器屏蔽数据
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//数据长度
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * 函数功能:	串口发送函数
 * 形式参数:	函数指针
 * 返回参数:	无
 * 修改日期:	2018-07-18					文档移植
 ****************************************************************************/
void USART_CRT_FunAdd( void fun(void) )
{
	crt_fun[g_crt_fun_cnt] = fun;
	if(++g_crt_fun_cnt == CRT_FUN_CNT)g_crt_fun_cnt = 0;
	if(g_crt_fun_cnt   == g_crt_run_cnt)
	{
		if(++g_crt_run_cnt ==  CRT_FUN_CNT)g_crt_run_cnt = 0;
	}
}
/*****************************************************************************
 * 函数功能:	升级配置信息
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-17					文档移植
 ****************************************************************************/
void USART_UpdeatConfig(uint8_t* buff)
{
	uint8_t cnt = buff[0];
	uint8_t i = 0;
	uint8_t index = 1;
	uint8_t length = 0;
	uint8_t start = 0;
	
	for(i = 0;i < cnt;i++)
	{
		switch(buff[index])
		{
			case 0x01:{//服务器IP地址
				length = buff[++index];
				start = ++index;
				memcpy(g_sys_param.server_ip , &buff[start] , length);
				break;}
			case 0x02:{//端口号
				length = index+2;
				g_sys_param.port = buff[length++];
				g_sys_param.port |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x03:{//设备网关号
				length = buff[++index];
				start = ++index;
				memcpy(g_sys_param.device_ip , &buff[start] , length);
				break;}
			case 0x04:{//心跳时间
				length = index+2;
				g_sys_param.hart_time = buff[length++];
				g_sys_param.hart_time |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x05:{//采样时间
				length = index+2;
				g_sys_param.camp_time = buff[length++];
				g_sys_param.camp_time |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x06:{//温度阈值
				length = index+2;
				switch(buff[length++])
				{
					case 1:{//通道1
						g_sys_param.threa.temp1  = buff[length++];
						g_sys_param.threa.temp1 |= (buff[length++] << 8);
						break;}
					case 2:{//通道2
						g_sys_param.threa.temp2  = buff[length++];
						g_sys_param.threa.temp2 |= (buff[length++] << 8);
						break;}
					default:{
						break;}
				}
				length = 5;
				break;}
			case 0x07:{//电流阈值
				length = index+2;
				switch(buff[length++])
				{
					case 1:{//通道1
						g_sys_param.threa.curr1  = buff[length++];
						g_sys_param.threa.curr1 |= (buff[length++] << 8);
						break;}
					case 2:{//通道2
						g_sys_param.threa.curr2  = buff[length++];
						g_sys_param.threa.curr2 |= (buff[length++] << 8);
						break;}
					case 3:{//通道3
						g_sys_param.threa.curr3  = buff[length++];
						g_sys_param.threa.curr3 |= (buff[length++] << 8);
						break;}
					default:{
						break;}
				}
				length = 5;
				break;}
			case 0x08:{//电压阈值
				length = index+2;
				switch(buff[length++])
				{
					case 1:{//通道1
						g_sys_param.threa.volat  = buff[length++];
						g_sys_param.threa.volat |= (buff[length++] << 8);
						break;}
					default:{
						break;}
				}
				length = 5;
				break;}
			case 0x09:{//剩余电流阈值
				length = index+2;
				switch(buff[length++])
				{
					case 1:{//通道1
						g_sys_param.threa.sy_curr  = buff[length++];
						g_sys_param.threa.sy_curr |= (buff[length++] << 8);
						break;}
					default:{
						break;}
				}
				length = 5;
				break;}
			case 0x15:{//温度屏蔽
				length = index+2;
				g_sys_param.shield.temp = buff[length++];
				length = 3;
				break;}
			case 0x16:{//电流屏蔽
				length = index+2;
				g_sys_param.shield.curr = buff[length++];
				length = 3;
				break;}
			case 0x17:{//电压屏蔽
				length = index+2;
				g_sys_param.shield.volat = buff[length++];
				length = 3;
				break;}
			case 0x18:{//剩余电流屏蔽
				length = index+2;
				g_sys_param.shield.sy_curr = buff[length++];
				length = 3;
				break;}
			default:{
					break;}
		}
		index += length;
	}
	g_sys_param.updat_flag = 1;//保存到flash
}

/*****************************************************************************
 * 函数功能:	校准数据解析
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-19					文档移植
 ****************************************************************************/
void USART_CailHander(uint8_t* buff)
{
	uint8_t cnt = buff[0];
	uint8_t i = 0;
	uint8_t index = 1;
	uint8_t length = 0;
	
	for(i = 0;i < cnt;i++)
	{
		switch(buff[index])
		{
			case 0x06:{//温度阈值
				length = index+2;
				g_cail_data.type = part_temp;
				g_cail_data.value  = buff[length++];
				g_cail_data.value |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x07:{//电流阈值
				length = index+2;
				g_cail_data.type = part_curr;
				g_cail_data.value  = buff[length++];
				g_cail_data.value |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x08:{//电压阈值
				length = index+2;
				g_cail_data.type = part_volt;
				g_cail_data.value  = buff[length++];
				g_cail_data.value |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x09:{//剩余电流阈值
				length = index+2;
				g_cail_data.type = part_sycu;
				g_cail_data.value  = buff[length++];
				g_cail_data.value |= (buff[length++] << 8);
				length = 4;
				break;}
		}
		index += length;
	}
	g_cail_data.flag = 1;
}
/*****************************************************************************
 * 函数功能:	串口数据解析
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-17					文档移植
 ****************************************************************************/
void USART_485_DataHandle(uint8_t* buff , uint8_t length)
{
	uint8_t sum = 0xAA+length;
	uint8_t i   = 0;
	uint8_t max = length-2;
	
	if(buff[length-1] != 0x55)return;
	for(i = 0;i < max;i++)
	{
		sum += buff[i];
	}
	if(sum != buff[i])return;

	switch(buff[0])
	{
		case 0x00://应答信息
			if(g_crt_run_cnt != g_crt_fun_cnt)
			{
				if(crt_fun[g_crt_run_cnt] == USART_SendSenserSta)
				{
					if((g_senser_flag&0x03) == 0x01)
					{
						g_senser_flag &=~ 0x03;
					}else if((g_senser_flag & 0x03) == 0x02)
					{
						g_senser_flag = 0;
					}
				}
				crt_fun[g_crt_run_cnt] = NULL;
				if(++g_crt_run_cnt == CRT_FUN_CNT)g_crt_run_cnt = 0;
				g_ack_flag = 0;
				g_re_cnt = 0;
			}
			break;
		case 0x01://读取配置信息
			USART_CRT_FunAdd(USART_SendSysConfig);//发送配置信息
			USART_CRT_FunAdd(USART_SendThread);//发送阈值信息
			USART_CRT_FunAdd(USART_SendModelMsg);//发送模块信息
			USART_CRT_FunAdd(USART_SendVer);//发送程序版本号
		  USART_CRT_FunAdd(USART_SendShield);//发送端口配置
			USART_CRT_FunAdd(USART_SendOK);//发送应答信号
			break;
		case 0x02://下传配置信息
			USART_UpdeatConfig(&buff[1]);
			USART_CRT_FunAdd(USART_SendOK);//发送应答信号
			break;
		case 0x04://打开数据监控
			g_monitor_flag = 1;
		  g_senser_flag  = 0;
			USART_CRT_FunAdd(USART_SendSenserShield);
			USART_CRT_FunAdd(USART_SendOK);//发送应答信号
			break;
		case 0x05://关闭数据监控
			g_monitor_flag = 0;
			USART_CRT_FunAdd(USART_SendOK);//发送应答信号
			break;
		case 0x07://下传校准信息
			USART_CailHander(&buff[1]);
			break;
	}
}
/*****************************************************************************
 * 函数功能:	串口异常处理
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-07-17					文档移植
 ****************************************************************************/
void USART_ERROR_Header(USART_TypeDef* USARTx)
{
	if(USART_GetFlagStatus(USARTx , USART_FLAG_ORE ) != RESET)//清除ORE中断
	{
		USART_ReceiveData(USARTx); //取出来扔掉
		USART_ClearFlag(USARTx , USART_FLAG_ORE);
	}
	if(USART_GetFlagStatus(USARTx , USART_FLAG_FE) != RESET)
	{
		USART_ClearFlag(USARTx , USART_FLAG_FE);
	}
	if(USART_GetFlagStatus(USARTx , USART_FLAG_NE) != RESET)
	{
		USART_ClearFlag(USARTx , USART_FLAG_NE);
	}
}
/*****************************************************************************
 * 函数功能:	串口接收中断
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-03-08					文档移植
						2018-07-25					修复bug;增加了接收超时时间,修复干扰无法接收完数据的时候,不能接收数据
 ****************************************************************************/
void USART_485_IRQnHandle(void)
{
	uint8_t re_data = 0;
	static uint8_t s_len = 0;

	USART_ERROR_Header(USART_485_COM);//串口异常处理
	if(USART_GetITStatus(USART_485_COM , USART_IT_RXNE) != RESET)
	{
		re_data = USART_ReceiveData( USART_485_COM );
		if((!g_crt_flag) && (re_data == 0xAA))//收到帧头
		{
			g_crt_flag = 1;
			g_crt_time = 20;//20s超时时间
			USART_RX1_CNT = 0;
			s_len = 0;
		}else if((g_crt_flag) && (!s_len))//收到数据长度
		{
			s_len = re_data;
		}else if((g_crt_flag) && (s_len))//数据保存到缓冲区
		{
			USART_RX1_BUF[USART_RX1_CNT++] = re_data;
			if(USART_RX1_CNT == s_len)//解析
			{
				USART_485_DataHandle(USART_RX1_BUF , s_len);
				s_len = 0;
				USART_RX1_CNT = 0;
				g_crt_flag = 0;
			}
		}else
		{
			s_len = 0;
			USART_RX1_CNT = 0;
			g_crt_flag = 0;
		}
		USART_ClearITPendingBit(USART_485_COM , USART_IT_RXNE);
	}
}

/*****************************************************************************
 * 函数功能:	串口数据处理函数
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-03-14					文档移植
 ****************************************************************************/
void USART_CONFIG_SEND(uint8_t *p , uint16_t len)
{
	uint16_t i;

	USART_485_TX_MODE;//发送
	for( i = 0;i < len+1;i++ )
	{
		USART_SendData(USART_485_COM , p[i]);
		while (USART_GetFlagStatus(USART_485_COM , USART_FLAG_TXE) == RESET){};	
	}
	USART_485_RX_MODE;//接收			
}
/*****************************************************************************
 * 函数功能:	重定向c库函数printf到串口
 * 形式参数:	无
 * 返回参数:	无
 * 修改日期:	2018-03-08					文档移植
 ****************************************************************************/
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_485_TX_MODE;
		USART_SendData(USART_485_COM, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART_485_COM, USART_FLAG_TXE) == RESET){};		
	  
	  USART_485_RX_MODE;
		return (ch);
}

/*****************************************************************************
 * 函数功能:		BC95中断服务函数
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-13				函数编写
 ****************************************************************************/
void NB_IRQHandle( void )
{
	uint8_t tmp = 0;										//中间变量		读取串口数据

	USART_ERROR_Header(USART_NB_COM);//串口故障处理
	if(2){//数据接收
		if( USART_GetITStatus( USART_NB_COM , USART_IT_RXNE ) != RESET )
		{
			if( g_nb_rx_cnt < USART_REC_LEN )
			{ 
				tmp =USART_ReceiveData( USART_NB_COM );		
				g_nb_rx_buff[ g_nb_rx_cnt ] = tmp;		
				g_nb_rx_cnt++;
			}
			USART_ClearITPendingBit( USART_NB_COM , USART_IT_RXNE );
		}
	}
}

/*****************************************************************************
 * 函数功能:		BC95硬件复位
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数编写
 ****************************************************************************/
void NB_HardwareReset( void )
{
	NB_RESET_H;
	IWDG_ReloadCounter();
	delay_ms( 500 );
	IWDG_ReloadCounter();
	NB_RESET_L;
}
/*****************************************************************************
 * 函数功能:		nb开机
 * 形式参数:		无
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数编写
 ****************************************************************************/
void NB_OpenPower( void )
{
	uint16_t cnt = 0;
	#ifdef NEW_MODEL
	NB_POWER_H;
	IWDG_ReloadCounter();
	while(cnt < 200)
	{
		cnt++;
		delay_ms(10);
		IWDG_ReloadCounter();
	}
	NB_POWER_L;
	#endif
}

/*------------------------------File----------End------------------------------*/
