/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-07-02					�ĵ���д
 *˵    ��:	�������ģ�����Զ��ģ������
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "NB89.h"
#include "delay.h"
#include "configure.h"
#include "string.h"
#include "key_led.h"
#include "usart.h"
/* �궨��	--------------------------------------------------------------------*/
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
 char NB86_CreatCOAP[]={"AT+NCDP=117.60.157.137"};				//����COAP�����ض˿�6000              ��ʽƽ̨IP
//char NB86_CreatCOAP[]={"AT+NCDP=180.101.147.115"};				//����COAP�����ض˿�6000               ����ƽ̨IP
char NB86_CreatUDP[]={"AT+NSOCR=DGRAM,17,7021,1"};				//����UDP   7021�Ƕ˿ں�
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		iot��socket�˿ڴ���
 * ��ʽ����:		��
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-07-02				������ֲ
 ****************************************************************************/ 
ErrorStatus NB86_Portcreat(uint8_t *re_cnt)
{
  uint16_t i = 0;											//ѭ������
	uint16_t k = 0;
	ErrorStatus state = ERROR;
	
	/*1>�ر�UDP�˿�,ģ��Ĭ��ʹ��UDP*/
	if(IOT_ATsend((uint8_t*)"AT+NSOCL=0",strlen("AT+NSOCL=0"),"OK")==SUCCESS)
	{
		runstate_to_usart("UDP�رն˿ڳɹ�!\r\n");
	}else//�رն˿�
	{
		runstate_to_usart("UDP�رն˿�ʧ��!\r\n");
	}
	switch(NB_Ope)
	{
		case DIANXIN:{//���ſ���ʹ��COAP����
			state = IOT_ATsend((uint8_t*)NB86_CreatCOAP , strlen(NB86_CreatCOAP) , "OK");
			break;}
		case YIDONG:{//�ƶ�����ʹ��UDP����
			strbj(NB86_CreatUDP , "DGRAM,17," , strlen(NB86_CreatUDP) , &k);
			Num_exchange(g_sys_param.port,(uint8_t*)&NB86_CreatUDP[k],&i);
			k +=i;
			NB86_CreatUDP[k++]=',';
			NB86_CreatUDP[k++]=socket+'0';//��ֵ����sock
			IOT_onlyATsend((uint8_t*)NB86_CreatUDP,strlen(NB86_CreatUDP));//AT��������Ϣ
			for(i=0;i<25;i++)//500ms�ȴ�AT��Ϣ�ظ�
			{
				NB_ReceiveData();//�յ�IOT��Ϣ
				if(g_nb_newdata_flag>0)
				{
					g_nb_newdata_flag = 0;
					for(i=0;i<g_nb_rx_cnt;i++)
					{
						if(g_nb_rx_buff[i]>='0'&&g_nb_rx_buff[i]<='9')socket=g_nb_rx_buff[i]-'0';//�洢������socket  ����10����
					}
					state = SUCCESS;
					break;
				}
			}
			break;}
		default:{//����
			break;}
	}
	if(state == SUCCESS)
	{
		g_nb_reset_flow++;//��һ������
		*re_cnt = 0;			//�ظ���������
		runstate_to_usart("�����˿ڳɹ�\r\n");
	}
	return state;
}

/*****************************************************************************
 * ��������:		nbiot��ʼ��
 * ��ʽ����:		��
 * ���ز���:		SUCCESS ��ʼ���ɹ�
 * ��������;		2018-07-02				������ֲ
 ****************************************************************************/
 ErrorStatus NB86_init( uint8_t* cnt , uint8_t *flag)
 {
  uint16_t i        = 0;											//ѭ������
	ErrorStatus state	= ERROR;									//����ֵ״̬
	uint8_t max = 20;
	 
	switch(g_nb_reset_flow)
	{
		case 1:{//ģ�鿪��,��ȡ�豸��Ϣ
			max   = 70;
			state = NB_WaitStartMessage("AT+MLWEVTIND=3" , cnt);
			break;}
		case 2:{//ģ��̼��汾��ѯ
			state = NB_ReadMoudleVer("AT+CGMR" , strlen("AT+CGMR") , "SSB," ,  cnt);
			break;}
		case 3:{//IMEI�Ų�ѯ
			state = NB_ReadMoudleIMEI("+CGSN:" , cnt);
			break;}
		case 4:{//��ѯSIM��
			state = NB_SendCmd("AT+CFUN?" , strlen("AT+CFUN?") , "+CFUN:1" , &i , cnt);
			break;}
		case 5:{//ISMI�Ų�ѯ
			state = NB_ReadMoudleISMI("\r\n" , cnt);
			break;}
		case 6:{//��ѯ��Ӫ��
			max   = 70;
			state = NB_ReadMoudleOperator("+COPS:" , cnt);
			break;}
		case 7:{//���總�����(��ʱ��30s����)
			max = 60;
			if(NB_SendCmd("AT+CGATT?" , strlen("AT+CGATT?") , "+CGATT:1" , &i ,cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("���總�ųɹ�!\r\n");
			}break;}
		case 8:{//����ע�����
			max = 60;
			if(NB_SendCmd("AT+CEREG?" , strlen("AT+CEREG?") , "+CEREG:0,1" , &i, cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("������·����!\r\n");
			}break;}
		case 9:{//������Ϣ��������
			if(NB_SendCmd("AT+NNMI=2" , strlen("AT+NNMI=2") , "OK" , &i , cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("�������տ����ɹ�!\r\n");
			}break;}
			case 10:{//iot��socket�˿ڴ���
			state = NB86_Portcreat( cnt );
			break;}
		case 11:{//��ȡ�豸IP
			state = Get_ChipID( cnt );												//��ȡ�豸ID
			break;}
		default:{//����
			*flag = 1;//��ʼ�����
			break;}
	}
	if(*cnt > max)
	{
		runstate_to_usart("NB��ȡ��ʱ!\r\n");
		g_nb_reset_flow = 1;
		*cnt = 0;
	}else
	{
		state = SUCCESS;
	}
	return state;
}
/*------------------------------File----------End------------------------------*/
