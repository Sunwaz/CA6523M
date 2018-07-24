/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-06-20					�ĵ���д
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "me3616.h"
#include "usart.h"
#include "key_led.h"
#include "configure.h"
#include "string.h"
/* �궨��	--------------------------------------------------------------------*/
#define AT_ME3616_CSQ                   "AT+CSQ"					//��ѯ�ź�ǿ��
#define AT_ME3616_STA                   "AT*MENGINFO=0"   //��ѯ����״̬
#define AT_ME3616_GMR										"AT+GMR"					//��ѯ�̼��汾
#define AT_ME3616_CGSN                  "AT+CGSN=1"				//��ѯIEMI
#define AT_ME3616_COPS									"AT+COPS?"				//��ѯ��Ӫ��"[�ƶ�]46000,46002,46007,46008[��ͨ]46001,46006,46009[����]46003,46005,46011"
#define AT_ME3616_CFUN									"AT+CFUN?"				//��ѯSIM��
#define AT_ME3616_CIMI                  "AT+CIMI"					//��ѯIMSI��
#define AT_ME3616_CGATT									"AT+CGATT?"				//��ѯ���總�����
#define AT_ME3616_CEREG_0								"AT+CEREG=0"			//��ѯ����ע��״��    ������ʾģʽ
#define AT_ME3616_CEREG                 "AT+CEREG?"       //��ѯ����ע��״��
#define AT_ME3616_ESOC									"AT+ESOC=1,2,1"		//����UDP����
#define AT_ME3616_ESOCON								"AT+ESOCON=0,"		//����������
#define AT_ME3616_READEN								"AT+ESOREADEN=1"	//�����ϱ���ʽΪģʽ1  �����ݷ���+ESODATA ,ͨ��AT+ESOREADָ���ȡ����
#define AT_ME3616_ESOSETRPT							"AT+ESOSETRPT=0"	//���ý������ݸ�ʽΪascall�������
#define AT_ME3616_ESOSEND               "AT+ESOSEND=0,"		//��������
#define AT_ME3616_ESOREAD               "AT+ESOREAD=0,"		//���ݽ���
#define AT_ME3616_READ									"AT+ESOREAD=0,512"//��ȡ����
#define AT_ME3616_M2MCLINEW             "AT+M2MCLINEW=117.60.157.137,5683,"//������ʽƽ̨   AT+M2MCLINEW=117.60.157.137,5683,"869662030028606",28800
#define AT_ME3616_M2MCLSEND             "AT+M2MCLISEND="  //������ƽ̨��������
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
void ME3616_CSQ(uint8_t *buf)
{
	NB_ReadMoudleCSQ(buf , "+CSQ: ");
}
/*****************************************************************************
 * ��������:		��ȡ���͹���(�޷���ѯ)
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void ME3616_ReadSignal(uint8_t *buf)
{
	buf[0] = '0';
	buf[1] =  0;
}

/*****************************************************************************
 * ��������:		��ȡ��վ���
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void ME3616_ReadPCI(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//ѭ������
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
 * ��������:		�����λ
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void ME3616_Reset(void)
{
	IOT_ATsend((uint8_t*)"AT+ZRST" , strlen("AT+ZRST") , "OK");
}
/*****************************************************************************
 * ��������:		16�����ַ���ת10�����ַ���
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void HexChar_to_DecChar(uint8_t *hex_char , uint8_t *dec_char , uint8_t hex_len)
{
	uint8_t i = 0;
	uint32_t dec_int = 0;
	uint16_t n = 0;
	

	for(i = 0;i < hex_len;i++)
	{
		dec_int *= 16;//16������
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
 * ��������:		��ȡС�����
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void ME3616_ReadCellID(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//ѭ������
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
			if(l == 2)//����
			{
				t_buff[index++] = 0;
				HexChar_to_DecChar(t_buff , buf , strlen((char*)t_buff));
				return;
			}else if(l==1)//��ʼ
			{
				t_buff[index++] = g_nb_rx_buff[j];
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
 * ��������;		2018-06-19				������ֲ
 ****************************************************************************/
void ME3616_ReadSNR(uint8_t *buf)
{
	uint16_t k = 0, j = 0 , index = 0;//ѭ������

	if(NB_ReadMoudleData("AT*MENGINFO=0" , strlen("AT*MENGINFO=0") , "*MENGINFOSC: " , &k) == SUCCESS)
	{
		for(j = k;j < g_nb_rx_cnt;j++)
		{
			if(g_nb_rx_buff[j] == '"')
			{
				if(k == 7)//����
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
 * ��������:		�����˿�
 * ��ʽ����:		��
 * ���ز���:		SUCCESS ��ʼ���ɹ�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
ErrorStatus ME3616_CreatSocket(uint8_t *re_cnt)
{
	uint16_t l,j = 0;
	uint8_t buff[50] = {0};

	switch(NB_Ope)
	{
		case YIDONG:{//�ƶ���
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
				runstate_to_usart("�˿ڴ����ɹ�!\r\n");
				return SUCCESS;
			}break;}
		case DIANXIN:{//���ſ�
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
			buff[l++]=0;//��β
			if(NB_SendCmd((char*)buff , strlen((char*)buff) , "success" ,&l ,re_cnt) == SUCCESS)
			{
				runstate_to_usart("�˿ڴ����ɹ�!\r\n");
				return SUCCESS;
			}break;}
		default:{//������
			g_sys_error_flag = 1;
			LED_Control(L_WORK  , OPEN);
			LED_Control(L_ERROR , OPEN);
			while(1);}
	}
	return ERROR;
}

/*****************************************************************************
 * ��������:		nbiot��ʼ��
 * ��ʽ����:		��
 * ���ز���:		SUCCESS ��ʼ���ɹ�
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
ErrorStatus ME3616_init( uint8_t* cnt , uint8_t *flag)
{
	uint16_t i        = 0;											//ѭ������
	ErrorStatus state	= ERROR;									//����ֵ״̬
	uint8_t max = 20;
	
	switch(g_nb_reset_flow)
	{
		case 1:{//�رջ���
			state = NB_SendCmd("ATE0" , strlen("ATE0") , "OK" , &i , cnt);
			break;}
		case 2:{//ģ�鿪��,��ȡ�豸��Ϣ
			max   = 70;
			state = NB_WaitStartMessage("+IP" , cnt);
			break;}
		case 3:{//��ѯ�̼��汾
			state = NB_ReadMoudleVer("AT+GMR" , strlen("AT+GMR") , "\r\n" ,  cnt);
			break;}
		case 4:{//IEMI�Ų�ѯ
			state = NB_ReadMoudleIMEI("+CGSN: " , cnt);
			break;}
		case 5:{//�ж�SIM��
			state = NB_SendCmd("AT+CFUN?" , strlen("AT+CFUN?") , "+CFUN: 1" , &i , cnt);
			break;}
		case 6:{//��ѯIMSI
			state = NB_ReadMoudleISMI("\r\n" , cnt);
			break;}
		case 7:{//��ѯ��Ӫ��
			max   = 70;
			state = NB_ReadMoudleOperator("+COPS: " , cnt);
			break;}
		case 8:{//���總�����(��ʱ��30s����)
			max = 60;
			if(NB_SendCmd("AT+CGATT?" , strlen("AT+CGATT?") , "+CGATT: 1" , &i ,cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("���總�ųɹ�!\r\n");
			}break;}
		case 9:{//��������ע����ʾģʽ
			state = NB_SendCmd("AT+CEREG=0" , strlen("AT+CEREG=0") , "OK" , &i ,cnt);
			break;}
		case 10:{//��ѯ����ע�����
			max = 60;
			if(NB_SendCmd("AT+CEREG?" , strlen("AT+CEREG?") , "+CEREG: 0,1" , &i ,cnt) == SUCCESS)
			{
				state	= SUCCESS;
				runstate_to_usart("����ע��ɹ�!\r\n");
			}break;}
		case 11:{//��������
			state = ME3616_CreatSocket(cnt);
			break;}
		case 12:{//��ȡ�豸IP
			state = Get_ChipID( cnt );												//��ȡ�豸ID
			break;}
		case 13:{//���������ϱ���ʽ
			if(NB_Ope == YIDONG)
			{
				if(NB_SendCmd("AT+ESOREADEN=1" ,strlen("AT+ESOREADEN=1") , "OK" , &i , cnt) == SUCCESS)
				{
					state = SUCCESS;
					runstate_to_usart("�򿪽��ջ���!\r\n");
				}
			}else
			{
				state = SUCCESS;
				g_nb_reset_flow++;//��һ������
				*cnt = 0;						//��λ��ʱ
			}break;}
		case 14:{//���ý������ݵĸ�ʽ
			if(NB_Ope == YIDONG)
			{
				if(NB_SendCmd("AT+ESOSETRPT=0" ,strlen("AT+ESOSETRPT=0") , "OK" , &i , cnt) == SUCCESS)
				{
					state = SUCCESS;
					runstate_to_usart("����ASCALL���!\r\n");
				}
			}else
			{
				state = SUCCESS;
				g_nb_reset_flow++;//��һ������
				*cnt = 0;						//��λ��ʱ
			}break;}
		default:{//����
			*flag = 1;//��ʼ�����
			break;}
	}
	if(*cnt > max)
	{
		runstate_to_usart((char*)"NB��ȡ��ʱ!\r\n");
		g_nb_reset_flow = 1;
		*cnt = 0;
	}else
	{
		state = SUCCESS;
	}
	return state;
}
/*****************************************************************************
 * ��������:		��������
 * ��ʽ����:		data ���͵�����   length ���ݳ���
 * ���ز���;		SUCCESS ���ͳɹ�  ERROR ����ʧ��
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
ErrorStatus ME3616_SendData(uint8_t *p , uint16_t len)
{
	uint8_t NB_Handle_TX_BUF[NB_TX_LEN];//NB���ʹ������ buf  ��ģ��
	uint8_t  x[50] = {0};								//�ַ�������
	uint16_t  l = 0;											//����
	
	NB_Handle_TX_CNT=0;//���㷢�ͳ���
	switch(NB_Ope)
	{
		case YIDONG:{//�ƶ�
			memcpy(&NB_Handle_TX_BUF[NB_Handle_TX_CNT],(uint8_t *)AT_ME3616_ESOSEND,strlen(AT_ME3616_ESOSEND));
			NB_Handle_TX_CNT+=strlen(AT_ME3616_ESOSEND);
			Num_exchange(len,x,&l);//coap�贫������ݳ���
			memcpy(&(NB_Handle_TX_BUF[NB_Handle_TX_CNT]),x,l);
			NB_Handle_TX_CNT+=l;
			NB_Handle_TX_BUF[NB_Handle_TX_CNT++]=',';
		}break;
		case DIANXIN:{//����
			memcpy(&NB_Handle_TX_BUF[NB_Handle_TX_CNT],(uint8_t *)AT_ME3616_M2MCLSEND,strlen(AT_ME3616_M2MCLSEND));
			NB_Handle_TX_CNT+=strlen(AT_ME3616_M2MCLSEND);
		}break;
		default:{
			break;}
	}
	return NB_SendServerData(NB_Handle_TX_BUF , p , len);
}

/*****************************************************************************
 * ��������:		��ѯ��������
 * ��ʽ����:		��
 * ���ز���;		��
 * ��������;		2018-06-21				������ֲ
 ****************************************************************************/
void ME3616_ReceiveData( uint8_t* cnt)
{
	uint16_t p = 0;
	uint16_t j = 0;
  uint8_t buff[50] = {0};
	uint16_t index = 0;
	
	NB_ReceiveData();//����������Ϣ+NSONMI:0,68
	if(g_nb_newdata_flag)//�յ�IOT��Ϣ
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
					NB_messag_Read((char*)buff ,strlen((char*)buff));			//���Ͳ鿴��Ϣ
				}
				g_nb_rx_cnt = 0;
			}break;
			case DIANXIN:{
				if(strbj((char*)g_nb_rx_buff,"+M2MCLIRECV:",g_nb_rx_cnt,&p)==SUCCESS)
				{
					NB_messag_Read(0 ,0);			//���Ͳ鿴��Ϣ
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
					if(NB_messag_Read(AT_ME3616_READ ,strlen(AT_ME3616_READ)) == ERROR)			//���Ͳ鿴��Ϣ
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
