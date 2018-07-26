/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-08					�ĵ���ֲ
						2018-03-12					485������д
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	------------------------------------------------------------------*/
#include "usart.h"
#include "delay.h"
#include "configure.h"
#include "key_led.h"
#include "timer.h"
/* �궨��	------------------------------------------------------------------*/
/** @addtogroup ������ֲ����
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
/* �ṹ�嶨��	--------------------------------------------------------------*/
/* �ڲ�����	----------------------------------------------------------------*/
/* ȫ�ֱ���	----------------------------------------------------------------*/
uint8_t  USART_RX1_BUF[RX_485_LEN];													//���ڽ�������
uint16_t USART_RX1_CNT=0;																		//�������ݵĳ���
void (*crt_fun[CRT_FUN_CNT])(void);													//CRT�������
uint8_t g_crt_fun_cnt = 0;																	//CRT������λ��
uint8_t g_crt_run_cnt = 0;																	//CRT��������λ��
uint8_t g_ack_flag    = 0;																	//CRTӦ���־
uint8_t g_re_cnt      = 0;																	//CRT�����ط�����
uint8_t g_cail_type   = 0;																	//ADCУ׼����
uint8_t g_usart_errortype = 0;															//���ڴ�������
uint8_t g_usart_errorvalu = 0;															//���ڴ���ֵ
uint8_t g_monitor_flag = 0;																	//CRT��ر�־
/* ϵͳ����	----------------------------------------------------------------*/
/*****************************************************************************
 * ��������:	���ڳ�ʼ��
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-03-08					�ĵ���ֲ
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
	/*1> 485 TX�˿�����*/
	newGPIO_Init.GPIO_Pin   = USART_485_TX_PIN;
	GPIO_Init(USART_485_TX_PORT , &newGPIO_Init);
	/*2> 485 RX�˿�����*/
	newGPIO_Init.GPIO_Pin   = USART_485_RX_PIN;
	GPIO_Init(USART_485_RX_PORT , &newGPIO_Init);
	/*3> ���ڹ���ӳ��*/
	GPIO_PinAFConfig(USART_485_TX_PORT , USART_485_TX_AFSOURCE , USART_485_AF);
	GPIO_PinAFConfig(USART_485_RX_PORT , USART_485_RX_AFSOURCE , USART_485_AF);
	
	RCC_485_USART_CMD(RCC_485_USART_CLK , ENABLE);
	newUSART_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	newUSART_Init.USART_Parity						  = USART_Parity_No;
	newUSART_Init.USART_StopBits 						= USART_StopBits_1;
	newUSART_Init.USART_WordLength 					= USART_WordLength_8b;
	/*4> 485 ���ڳ�ʼ��*/
	newUSART_Init.USART_BaudRate   					= USART_485_BAUDRATE;
	newUSART_Init.USART_Mode     						= USART_485_MODE;
	USART_Init(USART_485_COM , &newUSART_Init);
	USART_ClearFlag(USART_485_COM , USART_FLAG_TC);
	USART_Cmd(USART_485_COM , DISABLE);
	USART_ITConfig(USART_485_COM , USART_IT_RXNE , ENABLE);
	USART_ITConfig(USART_485_COM , USART_IT_ORE  , ENABLE);
	
	newNVIC_Init.NVIC_IRQChannelCmd      = ENABLE;
	/*5> 485 �����жϳ�ʼ��*/
	NVIC_SetPriority(USART_485_IRQN , 1);                             //�������
	newNVIC_Init.NVIC_IRQChannelPriority = 2;
	newNVIC_Init.NVIC_IRQChannel         = USART_485_IRQN;
	NVIC_Init(&newNVIC_Init);
	USART_Cmd(USART_485_COM , ENABLE);
	
	newGPIO_Init.GPIO_Mode	=	GPIO_Mode_OUT;
	newGPIO_Init.GPIO_PuPd	=	GPIO_PuPd_DOWN;
	/*6> 485 �շ����ƶ˿ڳ�ʼ��*/
	newGPIO_Init.GPIO_Pin	  = USART_485_CTL_PIN;
	GPIO_Init( USART_485_CTL_PORT , &newGPIO_Init );
}
/*****************************************************************************
 * ��������:		NB��ʼ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-08				������д
							2018-06-19				��������
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
	/*1> BC95 TX�˿�����*/
	newGPIO_Init.GPIO_Pin   = USART_NB_TX_PIN;
	GPIO_Init(USART_NB_TX_PORT , &newGPIO_Init);
	/*2> BC95 RX�˿�����*/
	newGPIO_Init.GPIO_Pin   = USART_NB_RX_PIN;
	GPIO_Init(USART_NB_RX_PORT , &newGPIO_Init);
	/*3> ���ڹ���ӳ��*/
	GPIO_PinAFConfig(USART_NB_TX_PORT , USART_NB_TX_AFSOURCE , USART_NB_AF);
	GPIO_PinAFConfig(USART_NB_RX_PORT , USART_NB_RX_AFSOURCE , USART_NB_AF);
	
	RCC_NB_USART_CMD(RCC_NB_USART_CLK , ENABLE);
	newUSART_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	newUSART_Init.USART_Parity						  = USART_Parity_No;
	newUSART_Init.USART_StopBits 						= USART_StopBits_1;
	newUSART_Init.USART_WordLength 					= USART_WordLength_8b;
	/*4> BC95 ���ڳ�ʼ��*/
	newUSART_Init.USART_BaudRate   					= USART_NB_BAUDRATE;
	newUSART_Init.USART_Mode     						= USART_NB_MODE;
	USART_Init(USART_NB_COM , &newUSART_Init);
	USART_ClearFlag(USART_NB_COM , USART_FLAG_TC);
	USART_Cmd(USART_NB_COM , DISABLE);
	USART_ITConfig(USART_NB_COM , USART_IT_RXNE , ENABLE);
	
	newNVIC_Init.NVIC_IRQChannelCmd 		 = ENABLE;
	/*4> BC95 �жϳ�ʼ��*/
	NVIC_SetPriority(USART_NB_IRQN , 1);                             //�������
	newNVIC_Init.NVIC_IRQChannelPriority = 1;
	newNVIC_Init.NVIC_IRQChannel 				 = USART_NB_IRQN;
	NVIC_Init(&newNVIC_Init);
	USART_Cmd(USART_NB_COM , ENABLE);
	
	/*4> BC95 ��λ�˿ڳ�ʼ��*/
	RCC_NB_RESET_CMD( RCC_NB_RESET_CLK , ENABLE );
	newGPIO_Init.GPIO_Mode = GPIO_Mode_OUT;
	newGPIO_Init.GPIO_PuPd = GPIO_PuPd_DOWN;
	newGPIO_Init.GPIO_Pin	 = NB_RESET_PIN;
	GPIO_Init( NB_RESET_PORT , &newGPIO_Init );
	/*5> NB	  ��Դ���ض˿ڳ�ʼ��*/
 #ifdef NEW_MODEL
	RCC_NB_POWER_CMD( RCC_NB_POWER_CLK , ENABLE );
	newGPIO_Init.GPIO_Pin  = NB_POWER_PIN;
	newGPIO_Init.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init( NB_POWER_PORT , &newGPIO_Init );
	/*6>NB   ״̬�˿ڳ�ʼ��*/
	RCC_NB_STATE_CMD( RCC_NB_STATE_CLK , ENABLE );
	newGPIO_Init.GPIO_Mode = GPIO_Mode_IN;
  newGPIO_Init.GPIO_Pin  = NB_STATE_PIN;
	newGPIO_Init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( NB_POWER_PORT , &newGPIO_Init );
	#endif
}

/*****************************************************************************
 * ��������:		NB��������
 * ��ʽ����:		p �������� len �������ݵĳ���
 * ���ز���:		��
 * ��������;		2018-03-08				������д
							2018-04-02				�������ݵ�ʱ��,��ֹadc
 ****************************************************************************/
void NB_SendData( uint8_t *p , uint16_t len )
{
	uint16_t i=0;												//ѭ������
	
	g_nb_newdata_flag = 0;              //�ȴ����ݽ���
	g_nb_rx_cnt       = 0;              //����Ϣ����
	for(i = 0;i < len;i++)
	{
		USART_SendData( USART_NB_COM , p[i] );
		while (USART_GetFlagStatus( USART_NB_COM , USART_FLAG_TXE) == RESET);//�ȴ����ͽ���
	}
}
/*****************************************************************************
 * ��������:		NB��������
 * ��ʽ����:		buf �������ݴ洢λ�� len �������ݵĳ���
 * ���ز���:		��
 * ��������;		2018-03-08				������д
 ****************************************************************************/
void NB_ReceiveData( void )
{
	uint16_t rxlen = g_nb_rx_cnt;		//�������ݵĳ���
	
	delay_ms(20);		
	if((rxlen == g_nb_rx_cnt)&&(rxlen))//20ms����û�б仯,���ж������ȶ�
	{
		if(rxlen != 0)
		{
			g_nb_newdata_flag = 1;			//��������
		}
	}
}
/*****************************************************************************
 * ��������:	���ڽ����ж�
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-04-24					�ĵ���д
 ****************************************************************************/
void USART_SendOK(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x00;//����Ӧ����Ϣ
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x00;//����Ӧ����Ϣ����
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * ��������:	����У׼ֵ
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-04-24					�ĵ���д
 ****************************************************************************/
void USART_SendCail(void)
{
	uint8_t i = 0;
	uint8_t type;
	uint16_t value;
	uint8_t channel;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x08;//����У׼ֵ
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x01;//����У׼ֵ��Ϣ�����
	switch(g_cail_type)
	{
		case 1:{//�¶�1
			type = 0x06;//�¶�
			value = g_sys_param.cali.temp[0];
			channel = 1;
			break;}
		case 2:{//�¶�2
			type = 0x06;//�¶�
			value = g_sys_param.cali.temp[1];
			channel = 2;
			break;}
		case 3:{//�¶�3
			type = 0x06;//�¶�
			value = g_sys_param.cali.temp[2];
			channel = 3;
			break;}
		case 4:{//����
			type = 0x07;//����
			value = g_sys_param.cali.curr;
			channel = 1;
			break;}
		case 5:{//��ѹ
			type = 0x08;//��ѹ
			value = g_sys_param.cali.volat;
			channel = 1;
			break;}
		case 6:{//ʣ�����
			type = 0x09;//ʣ�����
			value = g_sys_param.cali.sy_curr;
			channel = 1;
			break;}
		default:{//����
			return;}
	}
	USART_RX1_BUF[USART_RX1_CNT++]		= type;//��������
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT++]		= channel;//�豸ͨ��
	USART_RX1_BUF[USART_RX1_CNT++]	  = (value >> 0x00) & 0xFF;//����
	USART_RX1_BUF[USART_RX1_CNT++]	  = (value >> 0x08) & 0xFF;
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * ��������:	������
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-19					�ĵ���д
 ****************************************************************************/
void USART_SendError(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x09;//���ʹ���
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x01;//���ʹ�����Ϣ�����
	USART_RX1_BUF[USART_RX1_CNT++]		= g_usart_errortype;//��������
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x01;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT++]		= g_usart_errorvalu;//����ֵ
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * ��������:	������Ϣ�ķ���
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-18					�ĵ���д
 ****************************************************************************/
void USART_SendSysConfig(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//�ϴ�������Ϣ
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x05;//�ϴ�������Ϣ����

	if(1){//������IP��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//������IP��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x04;//������IP��ַ����
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.server_ip[3];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.server_ip[2];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.server_ip[1];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.server_ip[0];
	}
	if(2){//�������˿�
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//�������˿�
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//�������˿ڳ���
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.port >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.port >> 0x08) & 0xFF;
	}
	if(3){//�豸IP��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//�豸IP��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//�豸IP��ַ����
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[5];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[4];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[3];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[2];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[1];
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.device_ip[0];
	}
	if(4){//����ʱ��
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x04;//����ʱ��
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//����ʱ�䳤��
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.hart_time >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.hart_time >> 0x08) & 0xFF;
	}
	if(5){//����ʱ��
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x05;//����ʱ��
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//����ʱ�䳤��
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.camp_time >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.camp_time >> 0x08) & 0xFF;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}
/*****************************************************************************
 * ��������:	�汾��Ϣ�ķ���
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-18					�ĵ���д
 ****************************************************************************/
void USART_SendVer(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//�ϴ�������Ϣ
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x01;//�ϴ�������Ϣ����
	if(1){//�豸����汾
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0F;//�豸����汾
		i = strlen(SYS_VER);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//�豸����汾����
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , SYS_VER , i);
		USART_RX1_CNT += i;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * ��������:	������Ϣ�ķ���
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-18					�ĵ���д
 ****************************************************************************/
void USART_SendShield(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//�ϴ�������Ϣ
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x04;//�ϴ�������Ϣ����
	if(1){//�¶ȴ���������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x15;//�¶ȴ�������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//�¶ȴ��������γ���
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.temp;//�¶ȴ�������������
	}
	if(2){//��������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x16;//������������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//�������������γ���
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.curr;//������������������
	}
	if(3){//��ѹ����������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x17;//��ѹ��������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//��ѹ���������γ���
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.volat;//��ѹ��������������
	}
	if(4){//ʣ���������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x18;//��ѹ��������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//��ѹ���������γ���
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.sy_curr;//��ѹ��������������
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}
/*****************************************************************************
 * ��������:	��ֵ����
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-18					�ĵ���д
 ****************************************************************************/
void USART_SendThread(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//�ϴ�������Ϣ
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x07;//�ϴ�������Ϣ����

	if(1){//�¶ȱ�����ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//�¶ȱ�����ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//�¶ȱ�����ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//�¶ȱ�����ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.temp1 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.temp1 >> 0x08) & 0xFF;
	}
	if(2){//�¶ȱ�����ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//�¶ȱ�����ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//�¶ȱ�����ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//�¶ȱ�����ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.temp2 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.temp2 >> 0x08) & 0xFF;
	}
	if(3){//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//����������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//����������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr1 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr1 >> 0x08) & 0xFF;
	}
	if(4){//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//����������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//����������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr2 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr2 >> 0x08) & 0xFF;
	}
	if(5){//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//����������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//����������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr3 >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.curr3 >> 0x08) & 0xFF;
	}
	if(6){//��ѹ������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x08;//��ѹ������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//��ѹ������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//��ѹ������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.volat >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.volat >> 0x08) & 0xFF;
	}
	if(7){//ʣ�������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x09;//ʣ�������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//ʣ�������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//ʣ�������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.sy_curr >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_sys_param.threa.sy_curr >> 0x08) & 0xFF;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * ��������:	������ݷ���
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-18					�ĵ���д
 ****************************************************************************/
void USART_SendMonitorData(void)
{
	uint8_t i = 0;
	uint8_t tmp = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x06;//�ϴ��������
	USART_RX1_BUF[USART_RX1_CNT++]		= tmp;//�ϴ�������Ϣ����

	if(g_sys_param.shield.temp & 0x01){//�¶ȱ�����ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//�¶ȱ�����ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//�¶ȱ�����ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//�¶ȱ�����ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[TEMP1_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[TEMP1_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.temp & 0x02){//�¶ȱ�����ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x06;//�¶ȱ�����ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//�¶ȱ�����ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//�¶ȱ�����ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[TEMP2_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[TEMP2_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.curr & 0x01){//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//����������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//����������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR1_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR1_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.curr & 0x02){//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//����������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//����������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR2_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR2_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.curr & 0x04){//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x07;//����������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//����������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//����������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR3_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[CURR3_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.volat & 0x01){//��ѹ������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x08;//��ѹ������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//��ѹ������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//��ѹ������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[VOLAT_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[VOLAT_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	if(g_sys_param.shield.sy_curr & 0x01){//ʣ�������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x09;//ʣ�������ֵ
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//ʣ�������ֵ����
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//ʣ�������ֵ��ַ
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[SY_CURR_DATA_INDEX].data_value >> 0x00) & 0xFF;
		USART_RX1_BUF[USART_RX1_CNT++]	= (g_coll_data[SY_CURR_DATA_INDEX].data_value >> 0x08) & 0xFF;
		tmp++;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[3] = tmp;          //���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}


/*****************************************************************************
 * ��������:	����ģ����Ϣ
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-18					�ĵ���д
 ****************************************************************************/
void USART_SendModelMsg(void)
{
	uint8_t i = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x03;//�ϴ�������Ϣ
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x05;//�ϴ�������Ϣ����

	if(1){//ģ���ͺ�
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0A;//ģ���ͺ�
		i = strlen(MODLE_NAME);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//ģ���ͺų���
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , MODLE_NAME , i);
		USART_RX1_CNT += i;
	}
	if(2){//��Ӫ��
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0B;//��Ӫ��
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//��Ӫ�̳���
		USART_RX1_BUF[USART_RX1_CNT++]	= NB_Ope;
	}
	if(3){//ģ��IMEI
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0C;//ģ��IMEI
		i = strlen((char*)g_sys_param.IMEI);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//ģ��IMEI����
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , (char*)g_sys_param.IMEI , i);
		USART_RX1_CNT += i;
	}
	if(4){//ģ��IMSI
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0D;//ģ��IMSI
		i = strlen((char*)g_sys_param.IMSI);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//ģ��IMSI����
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , (char*)g_sys_param.IMSI , i);
		USART_RX1_CNT += i;
	}
	if(5){//ģ��̼��汾
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x0E;//ģ��̼��汾
		i = strlen(MODLE_VER);
		USART_RX1_BUF[USART_RX1_CNT++]	= i;//ģ��̼��汾����
		memcpy(&USART_RX1_BUF[USART_RX1_CNT] , MODLE_VER , i);
		USART_RX1_CNT += i;
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * ��������:	���ʹ�����״̬
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-26					�ĵ���д
 ****************************************************************************/
void USART_SendSenserSta(void)
{
	uint8_t i = 0;
	uint8_t tmp = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x10;//�ϴ�������״̬
	USART_RX1_BUF[USART_RX1_CNT++]		= tmp; //�ϴ�������״̬��Ϣ����

	if(1){//���ͻ��ź�
		if(g_alarm_flag)
		{
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x03;//���ͻ�
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x04;//ģ���ͺų���
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_alarm_flag >> 0x00) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_alarm_flag >> 0x08) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_alarm_flag >> 0x10) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_alarm_flag >> 0x18) & 0xFF;
			USART_RX1_BUF[3]		= ++tmp;         //�ϴ�������״̬��Ϣ����
		}
	}
	if(2){//���͹��ϻָ��ź�/�����ź�
		if((g_senser_flag & 0x03) == 0x02)//�ָ�
		{
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//���͹��ϻָ�
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//���ϻָ�����
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_senser_flag >> 0x00) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_senser_flag >> 0x08) & 0xFF;
			USART_RX1_BUF[3]		= ++tmp;         //�ϴ�������״̬��Ϣ����
		}else if((g_senser_flag & 0x03) == 0x01)//����
		{
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//���͹��ϻָ�
			USART_RX1_BUF[USART_RX1_CNT++]	= 0x02;//���ϻָ�����
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_senser_flag >> 0x00) & 0xFF;
			USART_RX1_BUF[USART_RX1_CNT++]	= (g_senser_flag >> 0x08) & 0xFF;
			USART_RX1_BUF[3]		= ++tmp;         //�ϴ�������״̬��Ϣ����
		}
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}
/*****************************************************************************
 * ��������:	���ʹ���������״̬
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-18					�ĵ���д
 ****************************************************************************/
void USART_SendSenserShield(void)
{
	uint8_t i = 0;
	uint8_t tmp = 0;
	
	USART_RX1_CNT = 2;
	USART_RX1_BUF[0] = 0xAA;
	
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x10;//�ϴ�������״̬
	USART_RX1_BUF[USART_RX1_CNT++]		= 0x04;//�ϴ�������״̬��Ϣ����
	if(1){//�¶ȴ���������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x15;//�¶ȴ�������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//�¶ȴ��������γ���
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.temp;//�¶ȴ�������������
	}
	if(2){//��������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x16;//������������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//�������������γ���
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.curr;//������������������
	}
	if(3){//��ѹ����������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x17;//��ѹ��������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//��ѹ���������γ���
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.volat;//��ѹ��������������
	}
	if(4){//ʣ���������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x18;//��ѹ��������������
		USART_RX1_BUF[USART_RX1_CNT++]	= 0x01;//��ѹ���������γ���
		USART_RX1_BUF[USART_RX1_CNT++]	= g_sys_param.shield.sy_curr;//��ѹ��������������
	}
	USART_RX1_BUF[1] = USART_RX1_CNT;//���ݳ���
	USART_RX1_BUF[USART_RX1_CNT] = 0;
	for(i = 0;i < USART_RX1_BUF[1];i++)
	{
		USART_RX1_BUF[USART_RX1_CNT] += USART_RX1_BUF[i];
	}
	USART_RX1_BUF[++USART_RX1_CNT] = 0x55;

	USART_CONFIG_SEND(USART_RX1_BUF , USART_RX1_CNT+1);
}

/*****************************************************************************
 * ��������:	���ڷ��ͺ���
 * ��ʽ����:	����ָ��
 * ���ز���:	��
 * �޸�����:	2018-07-18					�ĵ���ֲ
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
 * ��������:	����������Ϣ
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-17					�ĵ���ֲ
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
			case 0x01:{//������IP��ַ
				length = buff[++index];
				start = ++index;
				memcpy(g_sys_param.server_ip , &buff[start] , length);
				break;}
			case 0x02:{//�˿ں�
				length = index+2;
				g_sys_param.port = buff[length++];
				g_sys_param.port |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x03:{//�豸���غ�
				length = buff[++index];
				start = ++index;
				memcpy(g_sys_param.device_ip , &buff[start] , length);
				break;}
			case 0x04:{//����ʱ��
				length = index+2;
				g_sys_param.hart_time = buff[length++];
				g_sys_param.hart_time |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x05:{//����ʱ��
				length = index+2;
				g_sys_param.camp_time = buff[length++];
				g_sys_param.camp_time |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x06:{//�¶���ֵ
				length = index+2;
				switch(buff[length++])
				{
					case 1:{//ͨ��1
						g_sys_param.threa.temp1  = buff[length++];
						g_sys_param.threa.temp1 |= (buff[length++] << 8);
						break;}
					case 2:{//ͨ��2
						g_sys_param.threa.temp2  = buff[length++];
						g_sys_param.threa.temp2 |= (buff[length++] << 8);
						break;}
					default:{
						break;}
				}
				length = 5;
				break;}
			case 0x07:{//������ֵ
				length = index+2;
				switch(buff[length++])
				{
					case 1:{//ͨ��1
						g_sys_param.threa.curr1  = buff[length++];
						g_sys_param.threa.curr1 |= (buff[length++] << 8);
						break;}
					case 2:{//ͨ��2
						g_sys_param.threa.curr2  = buff[length++];
						g_sys_param.threa.curr2 |= (buff[length++] << 8);
						break;}
					case 3:{//ͨ��3
						g_sys_param.threa.curr3  = buff[length++];
						g_sys_param.threa.curr3 |= (buff[length++] << 8);
						break;}
					default:{
						break;}
				}
				length = 5;
				break;}
			case 0x08:{//��ѹ��ֵ
				length = index+2;
				switch(buff[length++])
				{
					case 1:{//ͨ��1
						g_sys_param.threa.volat  = buff[length++];
						g_sys_param.threa.volat |= (buff[length++] << 8);
						break;}
					default:{
						break;}
				}
				length = 5;
				break;}
			case 0x09:{//ʣ�������ֵ
				length = index+2;
				switch(buff[length++])
				{
					case 1:{//ͨ��1
						g_sys_param.threa.sy_curr  = buff[length++];
						g_sys_param.threa.sy_curr |= (buff[length++] << 8);
						break;}
					default:{
						break;}
				}
				length = 5;
				break;}
			case 0x15:{//�¶�����
				length = index+2;
				g_sys_param.shield.temp = buff[length++];
				length = 3;
				break;}
			case 0x16:{//��������
				length = index+2;
				g_sys_param.shield.curr = buff[length++];
				length = 3;
				break;}
			case 0x17:{//��ѹ����
				length = index+2;
				g_sys_param.shield.volat = buff[length++];
				length = 3;
				break;}
			case 0x18:{//ʣ���������
				length = index+2;
				g_sys_param.shield.sy_curr = buff[length++];
				length = 3;
				break;}
			default:{
					break;}
		}
		index += length;
	}
	g_sys_param.updat_flag = 1;//���浽flash
}

/*****************************************************************************
 * ��������:	У׼���ݽ���
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-19					�ĵ���ֲ
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
			case 0x06:{//�¶���ֵ
				length = index+2;
				g_cail_data.type = part_temp;
				g_cail_data.value  = buff[length++];
				g_cail_data.value |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x07:{//������ֵ
				length = index+2;
				g_cail_data.type = part_curr;
				g_cail_data.value  = buff[length++];
				g_cail_data.value |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x08:{//��ѹ��ֵ
				length = index+2;
				g_cail_data.type = part_volt;
				g_cail_data.value  = buff[length++];
				g_cail_data.value |= (buff[length++] << 8);
				length = 4;
				break;}
			case 0x09:{//ʣ�������ֵ
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
 * ��������:	�������ݽ���
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-17					�ĵ���ֲ
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
		case 0x00://Ӧ����Ϣ
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
		case 0x01://��ȡ������Ϣ
			USART_CRT_FunAdd(USART_SendSysConfig);//����������Ϣ
			USART_CRT_FunAdd(USART_SendThread);//������ֵ��Ϣ
			USART_CRT_FunAdd(USART_SendModelMsg);//����ģ����Ϣ
			USART_CRT_FunAdd(USART_SendVer);//���ͳ���汾��
		  USART_CRT_FunAdd(USART_SendShield);//���Ͷ˿�����
			USART_CRT_FunAdd(USART_SendOK);//����Ӧ���ź�
			break;
		case 0x02://�´�������Ϣ
			USART_UpdeatConfig(&buff[1]);
			USART_CRT_FunAdd(USART_SendOK);//����Ӧ���ź�
			break;
		case 0x04://�����ݼ��
			g_monitor_flag = 1;
		  g_senser_flag  = 0;
			USART_CRT_FunAdd(USART_SendSenserShield);
			USART_CRT_FunAdd(USART_SendOK);//����Ӧ���ź�
			break;
		case 0x05://�ر����ݼ��
			g_monitor_flag = 0;
			USART_CRT_FunAdd(USART_SendOK);//����Ӧ���ź�
			break;
		case 0x07://�´�У׼��Ϣ
			USART_CailHander(&buff[1]);
			break;
	}
}
/*****************************************************************************
 * ��������:	�����쳣����
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-07-17					�ĵ���ֲ
 ****************************************************************************/
void USART_ERROR_Header(USART_TypeDef* USARTx)
{
	if(USART_GetFlagStatus(USARTx , USART_FLAG_ORE ) != RESET)//���ORE�ж�
	{
		USART_ReceiveData(USARTx); //ȡ�����ӵ�
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
 * ��������:	���ڽ����ж�
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-03-08					�ĵ���ֲ
						2018-07-25					�޸�bug;�����˽��ճ�ʱʱ��,�޸������޷����������ݵ�ʱ��,���ܽ�������
 ****************************************************************************/
void USART_485_IRQnHandle(void)
{
	uint8_t re_data = 0;
	static uint8_t s_len = 0;

	USART_ERROR_Header(USART_485_COM);//�����쳣����
	if(USART_GetITStatus(USART_485_COM , USART_IT_RXNE) != RESET)
	{
		re_data = USART_ReceiveData( USART_485_COM );
		if((!g_crt_flag) && (re_data == 0xAA))//�յ�֡ͷ
		{
			g_crt_flag = 1;
			g_crt_time = 20;//20s��ʱʱ��
			USART_RX1_CNT = 0;
			s_len = 0;
		}else if((g_crt_flag) && (!s_len))//�յ����ݳ���
		{
			s_len = re_data;
		}else if((g_crt_flag) && (s_len))//���ݱ��浽������
		{
			USART_RX1_BUF[USART_RX1_CNT++] = re_data;
			if(USART_RX1_CNT == s_len)//����
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
 * ��������:	�������ݴ�����
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-03-14					�ĵ���ֲ
 ****************************************************************************/
void USART_CONFIG_SEND(uint8_t *p , uint16_t len)
{
	uint16_t i;

	USART_485_TX_MODE;//����
	for( i = 0;i < len+1;i++ )
	{
		USART_SendData(USART_485_COM , p[i]);
		while (USART_GetFlagStatus(USART_485_COM , USART_FLAG_TXE) == RESET){};	
	}
	USART_485_RX_MODE;//����			
}
/*****************************************************************************
 * ��������:	�ض���c�⺯��printf������
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-03-08					�ĵ���ֲ
 ****************************************************************************/
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ�USART1 */
		USART_485_TX_MODE;
		USART_SendData(USART_485_COM, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART_485_COM, USART_FLAG_TXE) == RESET){};		
	  
	  USART_485_RX_MODE;
		return (ch);
}

/*****************************************************************************
 * ��������:		BC95�жϷ�����
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-13				������д
 ****************************************************************************/
void NB_IRQHandle( void )
{
	uint8_t tmp = 0;										//�м����		��ȡ��������

	USART_ERROR_Header(USART_NB_COM);//���ڹ��ϴ���
	if(2){//���ݽ���
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
 * ��������:		BC95Ӳ����λ
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-08				������д
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
 * ��������:		nb����
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-08				������д
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
