/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-08					�ĵ���д
						2018-03-13					������������������,��������������޸�
						2018-05-11					Ϊ�˽�Լ�ڴ�����ݱ�����и���
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "BC95.h"
#include "delay.h"
#include "configure.h"
#include "string.h"
#include "key_led.h"
#include "usart.h"
/* �궨��	--------------------------------------------------------------------*/
char BC95_CreatCOAP[]={"AT+NCDP=117.60.157.137"};				//����COAP�����ض˿�6000              ��ʽƽ̨IP
//char BC95_CreatCOAP[]={"AT+NCDP=180.101.147.115"};				//����COAP�����ض˿�6000               ����ƽ̨IP
char BC95_CreatUDP[]={"AT+NSOCR=DGRAM,17,7021,1"};				//����UDP   7021�Ƕ˿ں�
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
ErrorStatus NB_messag_Read(char* Cmd , uint8_t Cmdlen);
/* ȫ�ֱ���	------------------------------------------------------------------*/
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		��ȡ�ź�ǿ��
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void BC95_CSQ(uint8_t *buf)
{
	NB_ReadMoudleCSQ(buf , "+CSQ:");
}
/*****************************************************************************
 * ��������:		�����λ
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void BC95_Reset(void)
{
	IOT_ATsend((uint8_t*)"AT+NRB" , strlen("AT+NRB") , "OK");
}
/*****************************************************************************
 * ��������:		��ȡ���͹���
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void BC95_ReadSignal(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//ѭ������

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
 * ��������:		��ȡ��վ���
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void BC95_ReadPCI(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//ѭ������

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
 * ��������:		��ȡС�����
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void BC95_ReadCellID(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//ѭ������

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
 * ��������:		��ȡС�����
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void BC95_ReadSNR(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//ѭ������

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
 * ��������:		iot��socket�˿ڴ���
 * ��ʽ����:		��
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-03-07				������ֲ
							2018-04-02				�ڶ�ȡ������ɺ�,ʹ��ADC
							2018-07-03				�����Ż�,�ڱ������������̵����Ӻ���ʾ
 ****************************************************************************/ 
ErrorStatus BC95_Portcreat(uint8_t *re_cnt)
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
			state = IOT_ATsend((uint8_t*)BC95_CreatCOAP , strlen(BC95_CreatCOAP) , "OK");
			break;}
		case YIDONG:{//�ƶ�����ʹ��UDP����
			strbj(BC95_CreatUDP , "DGRAM,17," , strlen(BC95_CreatUDP) , &k);
			Num_exchange(g_sys_param.port,(uint8_t*)&BC95_CreatUDP[k],&i);
			k +=i;
			BC95_CreatUDP[k++]=',';
			BC95_CreatUDP[k++]=socket+'0';//��ֵ����sock
			IOT_onlyATsend((uint8_t*)BC95_CreatUDP,strlen(BC95_CreatUDP));//AT��������Ϣ
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
 * ��������;		2018-03-07				������ֲ
							2018-04-25				���CoAPЭ������޸�
							2018-05-04				��ʱ��λ�ø���
							2018-05-28				״̬��ʾ
							2018-05-30				����ṹ����,�Ż��˳�ʼ���ṹ,ϵͳ�������������ʱ�䱻������
							2018-06-04				�޸�bug;IEMI���IMSI���ȡ����,�����˲�Ϊ0-9���ַ�
							2018-06-07				�޸�bug;�벻������ʱ�����豸����,,�ڳ���ʱǰ����ӿ��Ź�
 ****************************************************************************/
ErrorStatus BC95_init( uint8_t* cnt , uint8_t *flag)
{
  uint16_t i        = 0;											//ѭ������
	ErrorStatus state	= ERROR;									//����ֵ״̬
	uint8_t max = 20;
	
	switch(g_nb_reset_flow)
	{
		case 1:{//ģ�鿪��,��ȡ�豸��Ϣ
			max   = 70;
			state = NB_WaitStartMessage("REBOOT_CAUSE" , cnt);
			break;}
		case 2:{//ģ��̼��汾��ѯ
			state = NB_ReadMoudleVer("ATI" , strlen("ATI") , "Revision:" ,  cnt);
			break;}
		case 3:{//IMEI�Ų�ѯ
			state = NB_ReadMoudleIMEI("+CGSN:" , cnt);
			break;}
		case 4:{//��ѯSIM��
			state = NB_SendCmd("AT+CFUN?" , strlen("AT+CFUN?") , "OK" , &i , cnt);
			break;}
		case 5:{//ISMI�Ų�ѯ
			state = NB_ReadMoudleISMI("\r\n" , cnt);
			break;}
		case 6:{//���總�����(��ʱ��30s����)
			max = 60;
			if(NB_SendCmd("AT+CGATT?" , strlen("AT+CGATT?") , "+CGATT:1" , &i ,cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("���總�ųɹ�!\r\n");
			}break;}
		case 7:{//����ע�����
			max = 60;
			if(NB_SendCmd("AT+CEREG?" , strlen("AT+CEREG?") , "+CEREG:0,1" , &i, cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("������·����!\r\n");
			}break;}
		case 8:{//��ѯ��Ӫ��
			max   = 70;
			state = NB_ReadMoudleOperator("+COPS:" , cnt);
			break;}
		case 9:{//������Ϣ��������
			if(NB_SendCmd("AT+NNMI=2" , strlen("AT+NNMI=2") , "OK" , &i , cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("�������տ����ɹ�!\r\n");
			}break;}
		case 10:{//iot��socket�˿ڴ���
			state = BC95_Portcreat( cnt );
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

/*****************************************************************************
 * ��������:		NB-IOT���ͺ���
 * ��ʽ����:		ip ip��ַ port �˿ں� sockte p ���� len ���ݳ���
 * ���ز���;		SUCCESS ���ͳɹ�		ERROR ����ʧ��
 * ��������;		2018-03-08				������ֲ
							2018-03-29				��ȫ�ֱ��� NB_Handle_TX_BUF ����Ϊ�ֲ�����
							2018-04-02				�˴�����;�ڶ�ȡ��NBģ�����ݺ�,ʹ��ADC
              2018-04-25        coapЭ�����
							2018-06-08				�����Ż�;ɾ���β� socket 
							2018-07-02				�����Ż�;�����ò��ֵĳ���,����ɺ��� NB_SendServerData
 ****************************************************************************/ 
ErrorStatus BC95_SendData(uint8_t *p , uint16_t len)
{
	uint8_t NB_Handle_TX_BUF[NB_TX_LEN];//NB���ʹ������ buf  ��ģ��
	uint16_t i = 0;											//ѭ������
	uint8_t  x[50] = {0};								//�ַ�������
	uint16_t  l = 0;											//����
	
	NB_Handle_TX_CNT=0;//���㷢�ͳ���
	switch(NB_Ope)
	{
		case DIANXIN:{//���ſ�
			i = strlen("AT+NMGS=");
			memcpy(&NB_Handle_TX_BUF[NB_Handle_TX_CNT],(uint8_t *)"AT+NMGS=",i);
			NB_Handle_TX_CNT += i;
			Num_exchange(len,x,&l);//coap�贫������ݳ���
			memcpy(&(NB_Handle_TX_BUF[NB_Handle_TX_CNT]),x,l);
			NB_Handle_TX_CNT+=l;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			break;}
		case YIDONG:{//�ƶ���
			i = strlen("AT+NSOST=");
			memcpy(&NB_Handle_TX_BUF[NB_Handle_TX_CNT],(uint8_t *)"AT+NSOST=",i);
			NB_Handle_TX_CNT += i;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=socket+'0';//socket
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			int_to_char((char*)&NB_Handle_TX_BUF[NB_Handle_TX_CNT] , g_sys_param.server_ip , 4);//ip
			NB_Handle_TX_CNT = strlen((char*)NB_Handle_TX_BUF);
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			Num_exchange(g_sys_param.port , &NB_Handle_TX_BUF[NB_Handle_TX_CNT],&i);//�˿ں�
			NB_Handle_TX_CNT += i;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			Num_exchange(len , &NB_Handle_TX_BUF[NB_Handle_TX_CNT],&i);//���ݳ���
			NB_Handle_TX_CNT += i;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
			break;}
		default:{//������
			while(1);}//�ȴ�ϵͳ����
	}
	return NB_SendServerData(NB_Handle_TX_BUF , p , len);
}

/*****************************************************************************
 * ��������:		��ѯ��������
 * ��ʽ����:		��
 * ���ز���;		��
 * ��������;		2018-03-08				������ֲ
							2018-04-25				coapЭ��ĸ���
							2018-06-27				��ģ���쳣��ʱ��,��Ȼ��ȥ��������,���ĺ�Ϊ;ģ���쳣��,����ȥ��������
 ****************************************************************************/
void BC95_ReceiveData( uint8_t* cnt)
{
	uint16_t p = 0,q=0;
	uint8_t buff[50] = {0};

	if((g_nb_error_flag) || (!g_model_config_flag))return;
	NB_ReceiveData();//����������Ϣ+NSONMI:0,68
	switch(NB_Ope)
	{
		case DIANXIN:{//���ſ�
			if(g_nb_newdata_flag)//�յ�IOT��Ϣ
			{
				g_nb_newdata_flag = 0;
				if(strbj((char*)g_nb_rx_buff,"+NNMI",g_nb_rx_cnt,&p)==SUCCESS)
				{
					NB_messag_Read("AT+NMGR" ,strlen("AT+NMGR"));			//���Ͳ鿴��Ϣ
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
						NB_messag_Read("AT+NMGR" ,strlen("AT+NMGR"));			//���Ͳ鿴��Ϣ
					}
				}else
				{
					(*cnt)        = 0;
					g_rec_time = 0;
				}
			}break;}
		case YIDONG:{//�ƶ���
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
					NB_messag_Read((char*)buff ,strlen((char*)buff));			//���Ͳ鿴��Ϣ
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
						NB_messag_Read("AT+NSORF=0,255" ,strlen("AT+NSORF=0,255"));			//���Ͳ鿴��Ϣ
					}
				}else
				{
					(*cnt)        = 0;
					g_rec_time = 0;
				}
			}
			break;}
		default:{//������
			while(1);}//�豸����
	}
}
/*------------------------------File----------End------------------------------*/
