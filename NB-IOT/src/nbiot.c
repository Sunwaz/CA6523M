/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018-03-07					�ĵ���ֲ
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
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
/* �궨��	--------------------------------------------------------------------*/

#define  delay_nms										delay_ms
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
ErrorStatus IOT_Portcreat(void);													//iot��socket�˿ڴ���
/* ȫ�ֱ���	------------------------------------------------------------------*/
uint8_t  Device_ID[6]         = {0,0,0,0,2,9};						//��������
static uint8_t s_old_time     = 0;												//�ɵ�ʱ��
Radio_s radio                 = {0};											//��Ƶ�����ṹ��
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		ģ����Ϣ��ȡ
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-06-19				������ֲ
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
				if(1){//ģ��ΪBC95
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
				if(2){//ģ��ΪME3616
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
				if(3){//ģ��NB86
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
		runstate_to_usart("ģ����Ϣ��ȡʧ��\r\n");
	}
}
/*****************************************************************************
 * ��������:		nb����
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-07				������ֲ
							2018-03-08				��Ӳ����λ�������޸�(����� BC95_HardwareReset() )
							2018-06-11				�����Ż�;����ʱ�ŵ���Nbiot_init����������
 ****************************************************************************/ 
void Nbiot_Start(void)
{
	uint8_t reset_num=0;                //��������
	uint8_t flag = 0;
	uint8_t cnt = 1;
	
	NB_OpenPower();//NB����
	NB_HardwareReset();	//����ָ��
	while(1);//�չ̼���ʱ���
	
	moudle_msg_read();//��ȡģ����Ϣ
	NB_HardwareReset();	//����ָ��
	do
	{
		if((g_sys_tim_s & 0xFF) != s_old_time)
		{
			if(radio.init( &cnt , &flag) == SUCCESS)
			{
				if(flag)
				{
					g_nb_reset_flow = 1;
					LED_Control( L_ERROR , CLOS );
					return;
				}
				cnt++;
			}else
			{
				NB_HardwareReset();	
				cnt = 1;
				reset_num++;
				LED_Control( L_ERROR , OPEN );		
			}
			s_old_time = g_sys_tim_s & 0xFF;
		}
		IWDG_ReloadCounter();
	}while(reset_num < 2);//���2��
}

/*****************************************************************************
 * ��������:		nbӲ������
 * ��ʽ����:		��
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-03-07				������ֲ
							2018-03-08				��Ӳ����λ�������޸�(����� BC95_HardwareReset() )
							2018-06-04				�޸�bug;��ʼ������˳��������
							2018-06-19				�޸�bug;�������ʧ��,���¿������²��ϵĸ�λģ��,��Ϊû�м�ʱ��
							2018-06-29				�����Ż�;������֮��,������ģ��������λ,�������λ��10s����Ӳ����λ,��ʼ��λ����
																�����Ż�;������֮ǰ,������ģ���ʶ��,ʵ��ģ����Ȳ��,���ģ���IMEI���뱻�ı�,���ϱ��仯���(�Ա仯������غ��ϱ�)
 ****************************************************************************/ 
void Nbiot_reset(void)
{
	static uint8_t cnt        = 1;
	static uint8_t reset_num  = 0;//��������
	static uint8_t reset_time = 0;//��λ��ʱ
	static uint8_t power_time = 0;
	uint8_t flag = 0;

	if(g_nb_error_flag)
	{
		if((reset_num < 3) )
		{
			if((g_sys_tim_s & 0xFF) != s_old_time)
			{
				if((g_nb_reset_flow == 1) && (cnt == 1))
				{
					if(!power_time)
					{
						power_time = g_sys_tim_s + 10;									//�����λ10s��ſ�ʼӲ����λ
						if(power_time == 0)power_time = 1;
						radio.reset();																	//�����λ
						return;																					//��λ�����˳�,������ִ�к���ĳ���
					}else if((g_sys_tim_s & 0xFF) >= power_time)
					{
						NB_OpenPower();																	//��ʱʱ�䵽,NBģ�鿪��
						moudle_msg_read();															//��ȡģ����Ϣ
						NB_HardwareReset();															//ģ�鸴λ
						power_time = 0;																	//��ʱ����,�ȴ��´������λ,��ʼִ����ʽ�ĸ�λ����
					}else
					{
						return;																					//��ʱʱ��û�е���,������ִ�к���ĳ���
					}
				}
				if(radio.init(&cnt , &flag) == SUCCESS)
				{
					if(flag)
					{
						cnt = 1;
						g_nb_error_flag = 0;                           //���³�ʼ���ɹ�,��λ��־
					}else
					{
						cnt++;																				//��ʼ����������
					}
				}else
				{
					cnt = 1;																				//������λ,�������¿�ʼ
					reset_num++;
					LED_Control( L_ERROR , OPEN );									//�򿪹��ϵ�
					if(reset_num == 3)
					{
						reset_time = g_sys_tim_s + 40;						     //�����λ����3�ζ������ɹ�,��40s���ٸ�λ
						if(reset_time == 0)reset_time = 1;
					}
				}
				s_old_time = g_sys_tim_s & 0xFF;										//1s��ִ��һ������
			}
		}else
		{
			if(reset_time && ((g_sys_tim_s & 0xFF) >= reset_time))//40s��ʱʱ�䵽,���¿�ʼ����ִ��
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
 * ��������:		NB��������
 * ��ʽ����:		��
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/ 
ErrorStatus NB_Send(uint8_t *data, uint16_t length)
{
	ErrorStatus state;									//����״̬
 
	LED_Control(L_SIGNAL , OPEN);
	state=radio.send(data,length);	             //���͵�NBIOT
	if(state==SUCCESS)runstate_to_usart("module send success!\r\n");
	else runstate_to_usart("module send failed!\r\n");
	LED_Control(L_SIGNAL , CLOS);
	
	return state;
}
/*------------------------------File----------End------------------------------*/
