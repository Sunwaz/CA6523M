/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-07					������ֲ
 *˵    ��:	�����ʼ��
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "configure.h"
#include "usart.h"
#include "delay.h"
#include "stmflash.h"

#include "timer.h"
#include "key_led.h"
#include "string.h"
#include <stdlib.h>
/* �궨��	--------------------------------------------------------------------*/
#define delay_nms										 					delay_ms

#define DEFAULT_CAMP_TIME										  3																//Ĭ�ϲ���ʱ��
#define DEFAULT_HART_TIME										  1800														//Ĭ������ʱ��
#define DEFAULT_CURR                          (uint16_t)(CURR_RANGE_IN * 0.1)	//Ĭ�ϵ�����ֵ
#define DEFAULT_SY_CURR											  (uint16_t)(SY_CURR_RANGE_IN)		//Ĭ��ʣ�������ֵ
#define DEFAULT_VOLAT												  3800														//Ĭ�ϵ�ѹ��ֵ
#define DEFAULT_TEMP												  600															//Ĭ���¶���ֵ
//#define DEFAULT_HJ_HUMI												100
//#define DEFAULT_HJ_TEMP												600
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
uint8_t  END[2]								= {0x0D,0x0A};        //ʹ�ý�����
uint8_t g_nb_newdata_flag               = 0;				//�����ݱ�־   
uint8_t  g_nb_error_flag      = 0;									//NB���ϱ�־
char NB_Ope				            = DIANXIN;						//����
char MODLE_VER[20]            = {0};								//ģ��汾
param_typedef   g_sys_param;												//ϵͳ����
uint16_t g_nb_rx_cnt                    = 0;				//���ռ���
uint8_t  g_nb_rx_buff[USART_REC_LEN]    = {0};      //���յ�������
uint16_t NB_Handle_TX_CNT			= 0;				          //NB���ʹ������ ���� ��ģ��
uint8_t        g_rec_time           = 0;						//�ظ���Ϣ�ĳ�ʱʱ��,���ʱ�䳬����30s,�򲻲�ѯ����,���С��30s��ÿ2s��ѯһ�ν���
uint8_t g_nb_reset_flow       = 1;	                //��ʼ������
uint8_t socket = 0;
char MODLE_NAME[20]               ={0};
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		����ת�����ַ���
 * ��ʽ����:		s0 ��ת�����ַ��� s1 ת����ɵ���������
 * ���ز���:		��
 * ��������;		2018-04-09				������ֲ
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
			if((temp	== 0) && (flag == 0) && (j == 2))//���һ����Ϊ0 �����0 
			{
				*s0 = temp +'0';
				s0++;
			}else if((temp == 0) && (flag == 0) && (j < 2))//�м�������Ϊ0 �����
			{
			}else if((temp != 0) && (flag == 0))//��Ϊ0 ���������
			{
				flag = 1;
				*s0 = temp +'0';
				s0++;
			}else//ֻ����һ�����ݲ�Ϊ0 ���������ݲ����ǲ���Ϊ0 �����
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
 * ��������:		�ȴ�������Ϣ
 * ��ʽ����:		cmd AT���� length ����� str �����ַ��� re_cnt �ط�����
 * ���ز���:		success �ɹ�
 * ��������;		2018-06-29				������ֲ
 ****************************************************************************/
ErrorStatus NB_WaitStartMessage(char *str , uint8_t* re_cnt)
{
	uint16_t l = 0;												//���ò���
	
	if(NB_WaitData(str , &l) == SUCCESS)
	{
		*re_cnt = 0;
		g_nb_reset_flow++;//��һ������
		runstate_to_usart("nmodule starting success!\r\n");
		return SUCCESS;
	}
	return ERROR;
}
/*****************************************************************************
 * ��������:		��ȡ�̼��汾
 * ��ʽ����:		cmd AT���� length ����� str �����ַ��� re_cnt �ط�����
 * ���ز���:		success �ɹ�
 * ��������;		2018-06-29				������ֲ
 ****************************************************************************/
ErrorStatus NB_ReadMoudleVer(char *cmd , uint16_t length , char *str , uint8_t *re_cnt)
{
	uint16_t str_index = 0;												//�ַ�����λ��
	uint16_t i = 0,j = 0;//ѭ������
	uint8_t temp = *re_cnt;

	if(NB_SendCmd(cmd , length , str , &str_index , re_cnt) == SUCCESS)//�ҵ��ַ���
	{
		for(i = str_index;i < g_nb_rx_cnt;i++)
		{
			if((g_nb_rx_buff[i] == 0x0D) && (g_nb_rx_buff[i+1] == 0x0A))
			{
				MODLE_VER[j] = 0;//��β
				break;
			}
			MODLE_VER[j++] = g_nb_rx_buff[i];
		}
		MODLE_VER[j] = 0;//��β
		if( MODLE_VER[0] != 0)
		{
			runstate_to_usart("ģ��̼��汾Ϊ:");runstate_to_usart(MODLE_VER);runstate_to_usart("\r\n");
			*re_cnt = 0;						//��λ��ʱ
			return SUCCESS;
		}else
		{
			runstate_to_usart("ģ��̼��汾��ȡ����!\r\n");
			g_nb_reset_flow --;//�������ݶ�ȡ����,�������¶�ȡ,���̲�����
			*re_cnt = temp;
		}
	}
	return ERROR;
}
/*****************************************************************************
 * ��������:		��ȡIMEI��
 * ��ʽ����:		str �����ַ��� re_cnt �ط�����
 * ���ز���:		success �ɹ�
 * ��������;		2018-06-29				������ֲ
 ****************************************************************************/
ErrorStatus NB_ReadMoudleIMEI(char *str , uint8_t* re_cnt)
{
	uint16_t str_index = 0;												//�ַ�����λ��
	uint16_t i = 0;//ѭ������
	char t_IMEI[20]   = {0};										//��ȡ����IEMI����
	uint8_t temp = *re_cnt;

	if(NB_SendCmd("AT+CGSN=1" , strlen("AT+CGSN=1") , str , &str_index , re_cnt) == SUCCESS)
	{
		i = 0;
		while((g_nb_rx_buff[str_index] >= '0') && (g_nb_rx_buff[str_index] <= '9'))
		{
			t_IMEI[i] = g_nb_rx_buff[str_index];//����IEMI
			i++;
			str_index++;
		}
		t_IMEI[i]=0;               //��β
		if(t_IMEI[0] != 0)
		{
			runstate_to_usart("ģ��IMEI��Ϊ:\r\n");runstate_to_usart(t_IMEI);runstate_to_usart("\r\n");
			*re_cnt = 0;						//��λ��ʱ
			if(strcmp(t_IMEI , (char*)g_sys_param.IMEI))//ģ��Ų����
			{
				strcpy((char*)g_sys_param.IMEI , t_IMEI);
				g_sys_param.updat_flag = 1;								//ģ��ű�����
				g_sys_operation_msg |= 0x0200;
			}			
			return SUCCESS;
		}else
		{
			runstate_to_usart("ģ��IMEI�Ŷ�ȡ����!");
			g_nb_reset_flow --;//�������ݶ�ȡ����,�������¶�ȡ,���̲�����
			*re_cnt = temp;
		}
	}
	return ERROR;
}


/*****************************************************************************
 * ��������:		��ȡISMI��
 * ��ʽ����:		str �����ַ��� re_cnt �ط�����
 * ���ز���:		success �ɹ�
 * ��������;		2018-06-29				������ֲ
 ****************************************************************************/
ErrorStatus NB_ReadMoudleISMI(char *str , uint8_t* re_cnt)
{
	uint16_t str_index = 0;												//�ַ�����λ��
	uint16_t i = 0;//ѭ������
	char t_IMSI[20]   = {0};										//��ȡ����IEMI����
	uint8_t temp = *re_cnt;

	if(NB_SendCmd("AT+CIMI" , strlen("AT+CIMI") , str , &str_index , re_cnt) == SUCCESS)
	{
		i = 0;
		while((g_nb_rx_buff[str_index] >= '0') && (g_nb_rx_buff[str_index] <= '9'))
		{
			t_IMSI[i] = g_nb_rx_buff[str_index];//����ISMI
			i++;
			str_index++;
		}
		t_IMSI[i]=0;               //��β
		if(t_IMSI[0] != 0)
		{
			runstate_to_usart("ģ��IMSI��Ϊ:\r\n");runstate_to_usart(t_IMSI);runstate_to_usart("\r\n");
			*re_cnt = 0;						//��λ��ʱ
			if(strcmp(t_IMSI , (char*)g_sys_param.IMSI))//ģ��Ų����
			{
				strcpy((char*)g_sys_param.IMSI , t_IMSI);
				g_sys_param.updat_flag = 1;								//ģ��ű�����
				g_sys_operation_msg |= 0x80;
			}			
			return SUCCESS;
		}else
		{
			runstate_to_usart("ģ��IMSI�Ŷ�ȡ����!\r\n");
			g_nb_reset_flow --;//�������ݶ�ȡ����,�������¶�ȡ,���̲�����
			*re_cnt = temp;
		}
	}
	return ERROR;
}


/*****************************************************************************
 * ��������:		��ѯ��Ӫ��
 * ��ʽ����:		str �����ַ��� re_cnt �ط�����
 * ���ز���:		success �ɹ�
 * ��������;		2018-06-29				������ֲ
 ****************************************************************************/
ErrorStatus NB_ReadMoudleOperator(char *str , uint8_t* re_cnt)
{
	uint8_t temp = *re_cnt;
	uint16_t str_index = 0;												//�ַ�����λ��

	if(NB_SendCmd("AT+COPS?" , strlen("AT+COPS?") , str , &str_index , re_cnt) == SUCCESS)
	{
		if(strstr((char*)g_nb_rx_buff , "46000") || strstr((char*)g_nb_rx_buff , "46002") || strstr((char*)g_nb_rx_buff , "46007") || strstr((char*)g_nb_rx_buff , "46008"))
		{
			NB_Ope=YIDONG;//�ƶ�
			runstate_to_usart("����Ƶ��:�й��ƶ�\r\n");
		}else if(strstr((char*)g_nb_rx_buff , "46001") || strstr((char*)g_nb_rx_buff , "46006") || strstr((char*)g_nb_rx_buff , "46009"))
		{
			NB_Ope=LIANTONG;//��ͨ
			runstate_to_usart("����Ƶ��:�й���ͨ\r\n");
		}else if(strstr((char*)g_nb_rx_buff , "46003") || strstr((char*)g_nb_rx_buff , "46005") || strstr((char*)g_nb_rx_buff , "46011"))
		{
			NB_Ope=DIANXIN;//����
			runstate_to_usart("����Ƶ��:�й�����\r\n");																//����ƽ̨����IP
		}else
		{
			g_nb_reset_flow--;																	//�������ݶ�ȡ����,�������¶�ȡ,���̲�����
			*re_cnt = temp;
			return ERROR;
		}
		return SUCCESS;
	}
	return ERROR;
}


/*****************************************************************************
 * ��������:		�ȴ��ַ���
 * ��ʽ����:		cmd ��Ҫ�ȴ����ַ��� index�ַ���λ��
 * ���ز���:		��
 * ��������;		2018-06-29				������ֲ
 ****************************************************************************/
ErrorStatus NB_WaitData(char* str , uint16_t *index)
{
	uint8_t i = 25;
	
	for(;i>0;i--)
	{
		NB_ReceiveData();  //�ȴ�IOT��������Ϣ
		if(g_nb_newdata_flag)//���g_nb_newdata_flag��Ϊ0 ����յ�����
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
 * ��������:		nbiot ����ATָ��
 * ��ʽ����:		cmd AT���� length ����� str �����ַ��� index�ַ���λ�� re_cnt �ط�����
 * ���ز���:		SUCCESS ��ʼ���ɹ�
 * ��������;		2018-06-29				������ֲ
 ****************************************************************************/
ErrorStatus NB_SendCmd(char* cmd , uint16_t length, char* str, uint16_t *index , uint8_t* re_cnt)
{
	if((*re_cnt == 1 ) || (!(*re_cnt%5)))//��1,5,15,20,25...���ط���ʱ��,��ģ�鷢��ATָ��
	{
		IOT_onlyATsend((uint8_t*)cmd,length);
	}
	if(NB_WaitData(str , index) == SUCCESS)				//���Ҳ������ַ�������λ��
	{
		g_nb_reset_flow++;//��һ������
		*re_cnt = 0;
		return SUCCESS;
	}
	return ERROR;
}
/*****************************************************************************
 * ��������:		��ȡ�ź�ǿ��
 * ��ʽ����:		p ���͵����� len ���ݳ��� c ATָ��ظ�����
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-06-29				������ֲ
 ****************************************************************************/
void NB_ReadMoudleCSQ(uint8_t *buff , char *str)
{
	uint16_t j = 0 , str_index;
	uint16_t index;
	
	if(NB_ReadMoudleData("AT+CSQ" , strlen("AT+CSQ") , str , &str_index) == SUCCESS)				//���Ҳ������ַ�������λ��
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
 * ��������:		��ȡ����
 * ��ʽ����:		p ���͵����� len ���ݳ��� c ATָ��ظ�����
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-06-29				������ֲ
 ****************************************************************************/
ErrorStatus NB_ReadMoudleData(char *cmd , uint16_t length , char *str , uint16_t *str_index)
{	
	IOT_onlyATsend((uint8_t*)cmd,strlen(cmd));//��ѯ�ź�ǿ��
	return NB_WaitData(str , str_index);				//���Ҳ������ַ�������λ��
}
/*****************************************************************************
 * ��������:		������״̬���͵���λ��
 * ��ʽ����:		cmd �����ַ���
 * ���ز���:		��
 * ��������;		2018-03-07				������ֲ
							2018-04-04				Ϊ�˽�Լ�ڴ�,��config_data��������Ϊ�ֲ�����
 ****************************************************************************/
void runstate_to_usart(char* cmd)
{
	uint8_t length      = 0;						//����
	uint8_t Config_Data[512] = {0};

	memcpy(&Config_Data[length], cmd, strlen(cmd));//ֵ
	length+=strlen(cmd);
	USART_CONFIG_SEND(Config_Data,length);//���͵�����
	delay_nms(10);
}
/*****************************************************************************
 * ��������:		�����������(�ַ���ת������)
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-07				������ֲ
							2018-04-10				ϵͳ���ݸ��µ����
							2018-05-04				ϵͳ�������µ���int�͵�����,�ڱ����ʱ��,Ӧ����int��ת����char���ٱ���,ɾ�����ݸ���
							2018-05-17				�˿ڲ��ֵ����ó���������,���޸�
							2018-06-08				ɾ��ȫ�ֱ��� Device_IDchar Serv_Ipchar UDPPort UDPPortchar��
 ****************************************************************************/
void network_parameterUpdata(void)
{
	uint16_t temp   = 0;							  //�м����					���λ����
	
	if(1){//���ݱ���
	temp = sizeof(g_sys_param)/sizeof(uint16_t);
	STMFLASH_Write( FLASH_SAVE_ADDR_DeviceID , (uint16_t*)&g_sys_param , temp);}		//���ݱ���
	if(1){//����ʱ������
		Sampling_TimerConfig( g_sys_param.camp_time );										//���ò���ʱ��
	}
}
/*****************************************************************************
 * ��������:		��ʼ���������
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-07				������ֲ
 ****************************************************************************/
void network_parameter_flashRead(void)
{
	param_typedef * p = (param_typedef*)FLASH_SAVE_ADDR_DeviceID;
	uint8_t data[2] = {0};							//��flash���������������
	
	STMFLASH_Read(FLASH_SAVE_ADDR_DeviceID , (uint16_t*)data ,2);
	if((data[0] == 0x00) && (data[1] == 0x00))
	{
////////////////ϵͳ���ݶ�ȡ////////////////
		if(1){
		memcpy(&g_sys_param , p , sizeof(g_sys_param));
		}
////////////////ϵͳ��������////////////////
		if(1){
			Sampling_TimerConfig( g_sys_param.camp_time );										//���ò���ʱ��
		}
////////////////�쳣���ݱ��////////////////
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
	}else//���flash����û������,���豸����ΪĬ�Ϸ�����
	{
		g_sys_param.server_ip[0] = 182;
		g_sys_param.server_ip[1] = 131;
		g_sys_param.server_ip[2] = 21;
		g_sys_param.server_ip[3] = 106;
		g_sys_param.server_ip[4] = 0;
		g_sys_param.server_ip[5] = 0;
		g_sys_param.port         = 7021;

		g_sys_param.camp_time  = DEFAULT_CAMP_TIME;												  //Ĭ�ϲ���ʱ��Ϊ		2s
		g_sys_param.hart_time  = DEFAULT_HART_TIME;													//Ĭ������ʱ��Ϊ		30s
		g_sys_param.threa.curr1 = DEFAULT_CURR;						                  //Ĭ�ϱ�������			������̵�10%
		g_sys_param.threa.curr2 = DEFAULT_CURR;						                  //Ĭ�ϱ�������			������̵�10%
		g_sys_param.threa.curr3 = DEFAULT_CURR;						                  //Ĭ�ϱ�������			������̵�10%
		g_sys_param.threa.sy_curr = DEFAULT_SY_CURR;												//Ĭ�ϱ���ʣ�����	������̵�10%
		g_sys_param.threa.volat = DEFAULT_VOLAT;														//Ĭ�ϱ�����ѹ			380V
		g_sys_param.threa.temp1 = DEFAULT_TEMP;															//Ĭ�ϱ����¶�			60��
		g_sys_param.threa.temp2 = DEFAULT_TEMP;															//Ĭ�ϱ����¶�			60��
		g_sys_param.cali.curr = 1000;
		g_sys_param.cali.sy_curr = 1000;
		g_sys_param.cali.temp[0] = 1000;
		g_sys_param.cali.temp[2] = 1000;
		g_sys_param.cali.volat = 1000;
//		g_sys_param.threa.temp3 = DEFAULT_TEMP;															//Ĭ�ϱ����¶�			60��
//		g_sys_param.threa.hj_humi = DEFAULT_HJ_HUMI;												//Ĭ�ϱ�������ʪ��	10%RH
//		g_sys_param.threa.hj_temp = DEFAULT_HJ_TEMP;												//Ĭ�ϱ��������¶� 60��
		Sampling_TimerConfig( g_sys_param.camp_time );										  //���ò���ʱ��
	}
}



/*****************************************************************************
 * ��������:		�ַ����Ƚ�
 * ��ʽ����:		a ���Ƚϵ��ַ��� b �Ƚ��ַ��� len �ַ������� k ����λ��
 * ���ز���:		SUCCESS �ҵ��ַ��� ERROR δ�ҵ��ַ���
 * ��������;		2018-03-08				������ֲ
 ****************************************************************************/ 
ErrorStatus strbj(char *a , char *b ,uint16_t len, uint16_t *k)
{ 
	uint16_t i = 0;											//ѭ������  
	uint16_t j = 0;											//ѭ������
	uint16_t len1 = 0;									//�Ƚ��ַ����ĳ���
	char *m;														//�ַ���������ַ�
	
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
 * ��������:		AT����
 * ��ʽ����:		p ���͵����� len ���ݳ��� c ATָ��ظ�����
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-03-07				������ֲ
							2018-04-02				�ڶ�ȡ������ݺ�,ʹ��ADC,�������ݵ�ʱ���ֹADC
 ****************************************************************************/ 
ErrorStatus IOT_ATsend(uint8_t *p , uint16_t len , char *c)
{
	uint8_t i=0;												//ѭ������
	uint8_t length = 0;									//���ݳ���
	uint8_t AT_buf[100] = {0};					//ATָ�������
	uint16_t k = 0;											//�ַ�����ַ
	
	
	memcpy(AT_buf       , p   , len);
	memcpy(&AT_buf[len] , END , 2);
	length = len + 2;
	
	NB_SendData(AT_buf , length);
	for(i = 0;i < 40;i++)//500ms�ȴ�AT��Ϣ�ظ� 25*20ms
	{
		NB_ReceiveData();//�յ�IOT��Ϣ
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
 * ��������:		AT��������Ϣ
 * ��ʽ����:		p ���͵����� len ���ݳ��� c
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-03-07				������ֲ
							2018-03-08				�Դ˴������Ż�;����ڴ�����ʧ��,�����������ڴ�,ֱ������ɹ�
							2018-04-02				��������֮ǰ��ֹadc,�������ݺ�ʹ��adc
 ****************************************************************************/ 
 void IOT_onlyATsend(uint8_t *p , uint16_t len)
{
	uint16_t length = 0;								//���ݳ���
	uint8_t  *buf;											//�ַ�������
	uint8_t temp = 0;
	
	do
	{
		buf = (uint8_t*)calloc(len+2 , sizeof(uint8_t));//�����ڴ�
		temp++;
	}while((buf == NULL) && (temp < 0xF0));										//����ʧ��,����������
	if(temp >= 0xF0){runstate_to_usart("Iot_onlyATsend �����ڴ�ʧ��\r\n");return;}
	
	memcpy(buf , p , len);
	memcpy(&buf[len],END,2);
	length=len+2;
	
	NB_SendData(buf,length);
	free(buf);//�ͷ��ڴ�	
}
/*****************************************************************************
 * ��������:		��ȡSTM32F030��оƬID
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-07				������ֲ
							2018-06-08			  �����Ż�;ͨ��IEMI����ȷ�����غ�
							2018-06-15				�޸�bug;ƽ̨���ص�ַ�������һ��0,tmp_ip�����һ��
							2018-07-03				�����Ż�;�ڱ������������̵�����
 ****************************************************************************/
ErrorStatus Get_ChipID( uint8_t *cnt)
{
	uint64_t tmp_id = 0;
	uint8_t  i      = 0;
	uint8_t  len    = 0;
	
	len = strlen((char*)g_sys_param.IMEI);
	for(i = 2;i < len;i++)//ȥ�� 86 
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
 * ��������:		������ת�����ַ���
 * ��ʽ����:		num Ҫת�������� p ת��������� len ���鳤��
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-03-08				������ֲ
 ****************************************************************************/ 
void Num_exchange(uint32_t num , uint8_t *p , uint16_t* len)
{
	uint32_t	i		=	0;									//ѭ������
	uint32_t	a		=	0;									//
	uint32_t	nu	=	num;								//��Ҫת��������
	uint8_t		*k;												//ָ��ת���������ָ��
	
	k=p;
	do
	{
		*(k+i) = nu%10+'0';
		nu    /= 10;
		i++;
	}while(nu>0);												//��һ������,ת���� �ַ�
	*(k+i)=0;														//�ַ�����β
	*len=i;															//�ַ�������
	for(i = 0; i< (*len/2);i++)
	{
		a             = *(k+i);
		*(k+i)        = *(k+*len-1-i);
		*(k+*len-1-i) = a;
	}
}

/*****************************************************************************
 * ��������:		��������
 * ��ʽ����:		num Ҫת�������� p ת��������� len ���鳤��
 * ���ز���:		SUCCESS �����ɹ�		ERROR ����ʧ��
 * ��������;		2018-03-08				������ֲ
 ****************************************************************************/ 
ErrorStatus NB_SendServerData(uint8_t *NB_Handle_TX_BUF , uint8_t *data , uint16_t len)
{
	uint8_t temp = 0;
	uint16_t i = 0;											//ѭ������
	uint16_t m = 0;											//�ַ�λ��

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
	for(i=0;i<60;i++)//500ms�ȴ�AT��Ϣ�ظ�
	{
		NB_ReceiveData();//�յ�IOT��Ϣ
		if(len>0)
		{
			if(strbj((char*)g_nb_rx_buff,"ERROR",g_nb_rx_cnt,&m)==SUCCESS){return ERROR;}
			if(strbj((char*)g_nb_rx_buff,"OK"   ,g_nb_rx_cnt,&m)==SUCCESS){return SUCCESS;}
		}
	}
	return ERROR;
}
/*------------------------------File----------End------------------------------*/

