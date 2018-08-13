/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018-03-08					�ĵ���ֲ
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "transmission.h"
#include "usart.h"
#include "rtc.h"
#include "timer.h"
#include "string.h"
#include "stdlib.h"
#include "bc95.h"
#include "nbiot.h"
#include "key_led.h"
#include "configure.h"
extern uint8_t g_reset_flag;
/* �궨��	--------------------------------------------------------------------*/
#define SERIAL_NUMBER_0										2								//��ˮ�Ÿ�
#define SERIAL_NUMBER_1										3								//��ˮ�ŵ�λ
#define DATA_LEN_0												24							//���ݳ��ȸ�λ
#define DATA_LEN_1												25							//���ݳ��ȵ�λ
#define NET_CMD														26							//����
#define TIME_DATA													6								//ʱ���ڻ����е����λ��
#define CONFIG_SIZE												31							//������Ϣ�ĳ���
#define	SEND_OutTime											40							//��������ʱʱ��   ���ڵ���ƽ̨��ʱ���ӳ�,����ǰ���10s
#define	OUT_SEND_MAX 											4								//���������ɷ����ز�������
#define	OUT_POLL_MAX_TOP0  								10							//ģ���ش�����
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
uint8_t buildcmd_net(Net_CmdType_t *cmd);									//װ�����ݵ�����
/* ȫ�ֱ���	------------------------------------------------------------------*/
static uint8_t  s_net_buff[PROTOCOL_NET_MAX] = {0};       //���绺��									//ʹ��ͬһ������
static uint8_t  Net_Ack_SW           = 0;									//�Ƿ���Ҫ������Ӧ��				0������Ҫ													1����Ҫ
static uint8_t	Wait_Net_Ack_Flag    = 0;                 //�ȴ��������ظ���־				0������ȴ����������Ӧ��						1���ȴ�������Ӧ����		2���ȴ�������Ӧ��ʧ��
static uint16_t	Build_Net_Buffer_len = 0;									//���͵����������������ݻ��泤��
uint8_t 				Build_Net_Data_Flag  = 0;                 //���ݷ��ͱ�־							0���Ѿ����͵�ģ�� ���Կ�ʼ��ʱ����	1��δ����
uint8_t         g_queue_idex_s			 = 0;									//��ȡλ��
uint8_t         g_queue_idex_e			 = 0;								  //д��λ��
uint8_t         g_nb_net_buff[QUEUE_DAT_SIZE] = {0};			//�洢����                       ���1K���ڴ�
uint8_t         g_up_config_flag		 = 0;									//��ȡ������Ϣ�ı�־
uint16_t 				Serial_Number        = 0;									//��ˮ��
uint16_t        g_nb_net_buf_start   = 0;									//NB�洢���������ݵ���ʼλ��
uint16_t        g_nb_net_buf_end     = 0;									//NB�洢���������ݵĽ���λ��
app_type        g_app_type;																//��ǰ��������
net_app_typedef g_net_app_queue[QUEUE_SIZE];							//����Ӧ��APP
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		����������������
 * ��ʽ����:		num ��ˮ�� state ״̬
 * ���ز���:		��
 * ��������;		2018-05-15				������д
							2018-05-30				�������±�д
							2018-06-06				�޸�bug;���ݵ�ͷ��ʱ��������,�����ϴ�������Ϊ�����������ݵ�(�����)
							2018-08-10				�޸�bug;�������ݵ�ʱ������ȱλ(��Ҫ�ǲ�����Ϣ-������Ϣ)
 ****************************************************************************/ 
void Server_SendData(uint16_t* num , cmd_type cmd_name ,uint8_t data_type , uint8_t msg_len)
{
	uint8_t i = 0;											//ѭ������
	uint8_t j = 0;
	Net_CmdType_t cmd;									//����
	uint16_t save_index = 0;
	uint16_t read_index = 0;
	uint16_t dat_len = 0;
	
	if(1){//Э��ͷ��������
		cmd.num             = (*num)++;						//��ˮ
		cmd.vision          = 0xCBC8;							//Э��汾��
		RTC_Get();																//��ȡ��ǰʱ��
		memcpy(cmd.time, &Time, 6);               //����ʱ��
		for(i=0; i<6; i++) cmd.src_addr[i]   = Device_ID[5-i];//ԭ��ַ
		for(i=0; i<4; i++) cmd.des_addr[i]   = g_sys_param.server_ip[3-i];  //Ŀ�ĵ�ַ
		for(i=0; i<2; i++) cmd.des_addr[4+i] = 0;
		cmd.cmd							= cmd_name;          //��������
		Build_Net_Data_Flag = 1;                 //�����ݷ���
		Net_Ack_SW          = 1;                 //���������Ӧ��
	}
	if(3){//������װ
		switch(cmd.cmd)
		{
			case type_ok_cmd:{			//Ӧ���ź�
				(*num)--;
				Net_Ack_SW = 0;//����Ӧ��
				break;}
			case type_send_dat_cmd:{//�ϴ�����
				cmd.data[0] = data_type;		//��������
				cmd.data[1] = msg_len;			//��Ϣ�峤��
				save_index = 2;
				read_index = g_net_app_queue[g_queue_idex_s].idex;
				if(data_type == type_runing)
				{
					save_index = 1;
					cmd.data[save_index++]  = 0x01;														//ֻ��һ����Ϣ��
					cmd.data[save_index++]	= 0x80;														//ϵͳ��־
					cmd.data[save_index++]	= 0x01;														//ϵͳ��ַ
					cmd.data[save_index++]	= msg_len;												//��������
					for(i = 0;i < msg_len;i++)
					{
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//��������
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						dat_len = g_nb_net_buff[read_index];
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//��������
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						for(j = 0;j < dat_len;j++)
						{
							cmd.data[save_index++] = g_nb_net_buff[read_index];	//����ֵ
							read_index = (read_index+1)&QUEUE_DAT_MAX;
						}
					}
					for(i = 0;i < 6;i++)
					{
						cmd.data[save_index++] = g_nb_net_buff[read_index];    //cmd.data ���漰��ͷ,�ʲ�������
						read_index = (read_index+1)&QUEUE_DAT_MAX;
					}
				}else if(g_app_type == type_send_anlog)//������
				{
					dat_len = (0x05 * msg_len + 6) + read_index;
					for(i = 0;i < msg_len;i++)
					{
						cmd.data[save_index++]	= 0x80;													//ϵͳ��־
						cmd.data[save_index++]	= 0x01;													//ϵͳ��ַ
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	  //��������
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//������ַ
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						cmd.data[save_index++]	= 0x00;
						cmd.data[save_index++]	= 0x00;
						cmd.data[save_index++]	= 0x00;
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//��������
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						cmd.data[save_index++]	= g_nb_net_buff[read_index];	//ģ������ֵ
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						cmd.data[save_index++]	= g_nb_net_buff[read_index];
						read_index = (read_index+1)&QUEUE_DAT_MAX;
						for(j = 0;j < 6;j++)
						{
							cmd.data[save_index++] = g_nb_net_buff[dat_len];    //cmd.data ���漰��ͷ,�ʲ�������
							dat_len = (dat_len+1)&QUEUE_DAT_MAX;
						}
					}
				}else if((g_app_type == type_oper_info) || (data_type == type_senser_info) || (data_type == type_senser_recover))
				{
					cmd.data[save_index++]		= 0x80;													//ϵͳ��־
					cmd.data[save_index++]		= 0x01;													//ϵͳ��ַ
					cmd.data[save_index++]		= g_nb_net_buff[read_index];	  //��������
					read_index = (read_index+1)&QUEUE_DAT_MAX;
					cmd.data[save_index++]		= g_nb_net_buff[read_index];	  //������ַ
					read_index = (read_index+1)&QUEUE_DAT_MAX;
					cmd.data[save_index++]		= 0x00;
					cmd.data[save_index++]		= 0x00;
					cmd.data[save_index++]		= 0x00;
					for(j = 0;j < 8;j++)//2�ֽ�����+6�ֽ�ʱ��
					{
						cmd.data[save_index++] = g_nb_net_buff[read_index];    //cmd.data ���漰��ͷ,�ʲ�������
						read_index = (read_index+1)&QUEUE_DAT_MAX;
					}
				}
				break;}
			case type_startup_cmd:	//�ϴ�������Ϣ
			case type_upload_cmd:{	//�ϴ�������Ϣ
				read_index = g_net_app_queue[g_queue_idex_s].idex;//����λ��
				cmd.data[save_index++] = data_type;		//��������
				cmd.data[save_index++] = 0x01;				//��Ϣ�峤��
				cmd.data[save_index++] = 0x80;				//ϵͳ��־
				cmd.data[save_index++] = 0x01;				//ϵͳ��ַ
				cmd.data[save_index++] = msg_len;			//���ò�������
				for(i = 0;i < msg_len;i++)
				{
					cmd.data[save_index++]	= g_nb_net_buff[read_index];	//��������
					read_index = (read_index+1)&QUEUE_DAT_MAX;
					dat_len = g_nb_net_buff[read_index];
					cmd.data[save_index++]	= g_nb_net_buff[read_index];	//��������
					read_index = (read_index+1)&QUEUE_DAT_MAX;
					for(j = 0;j < dat_len;j++)
					{
						cmd.data[save_index++] = g_nb_net_buff[read_index];	//����ֵ
						read_index = (read_index+1)&QUEUE_DAT_MAX;
					}
				}
				for(j = 0;j < 6;j++)
				{
					cmd.data[save_index++] = g_nb_net_buff[read_index];    //cmd.data ���漰��ͷ,�ʲ�������
					read_index = (read_index+1)&QUEUE_DAT_MAX;
				}
				Wait_Net_Ack_Flag=0;//������ǿ�ƿ���
				break;}
			default:{								//����
				break;}
		}
	}
	cmd.length					= save_index;            //Ӧ�����ݳ���
	if(4){//�������
		buildcmd_net(&cmd);
	}
}
/*****************************************************************************
 * ��������:		�����ƶ�
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-06-05				������ֲ
 ****************************************************************************/
void data_index_move( void )
{
	if(++g_queue_idex_s >= QUEUE_SIZE)g_queue_idex_s = 0;														//��������ͷ
	if(g_queue_idex_s != g_queue_idex_e)g_nb_net_buf_start = g_net_app_queue[g_queue_idex_s].idex;//��ʼ�洢λ��,�������������������Ԫ��,�������������ʼλ������Ϊ��ǰ���е�λ��
	else g_nb_net_buf_start = g_nb_net_buf_end;//�����������û�д洢����������ʼ���ڽ���
}
/*****************************************************************************
 * ��������:		�����������豸
 * ��ʽ����:		pDr ���õ����� len ����ָ��
 * ���ز���:		error ���ݴ��� success ���ݳɹ�
 * ��������;		2018-04-09				������ֲ
							2018-04-10				�Ż�
							2018-07-06				�޸�bug;������������Ĭ��Ϊint8_t���ͣ������޷��޸����ã������̣�,���ʵ��,�޸�int8_t����Ϊuint8_t����
 ****************************************************************************/
void Netconfig_Download( config_typedef* pDr , uint16_t len)
{
	uint8_t i   = 0;
	uint16_t u_temp = 0;
	
	for(i = 0;i < len;i++)
	{
		switch(pDr[i].config_type)
		{
			case config_camp:{												//���ò���ʱ��
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				g_sys_param.camp_time = u_temp;break;}
			case config_curr_t:{											//���õ�����ֵ
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				if(u_temp > CURR_RANGE_IN)
				{
					g_sys_param.threa.curr1 = CURR_RANGE_IN;
					g_sys_param.threa.curr2 = CURR_RANGE_IN;
					g_sys_param.threa.curr3 = CURR_RANGE_IN;
				}else
				{
					g_sys_param.threa.curr1 = u_temp;
					g_sys_param.threa.curr2 = u_temp;
					g_sys_param.threa.curr3 = u_temp;
				}break;}
			case config_heart:{												//��������ʱ��
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				g_sys_param.hart_time = u_temp;break;}
			case config_ip:{													//����IP��ַ
				g_sys_param.server_ip[0] = pDr[i].data[3];
				g_sys_param.server_ip[1] = pDr[i].data[2];
				g_sys_param.server_ip[2] = pDr[i].data[1];
				g_sys_param.server_ip[3] = pDr[i].data[0];
				g_sys_param.port = (pDr[i].data[5] << 8) | (pDr[i].data[4]);break;}
			case config_sy_curr_t:{										//����ʣ�������ֵ
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				if(u_temp > SY_CURR_RANGE_IN)g_sys_param.threa.sy_curr = SY_CURR_RANGE_IN;
				else g_sys_param.threa.sy_curr = u_temp;break;}
//			case config_temp_humi_t:{								 //���û�����ʪ�ȵ���ֵ
//				temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
//				if((uint16_t)temp > 1000)g_sys_param.threa.hj_temp = 1000;
//				else g_sys_param.threa.hj_temp = (uint16_t)temp;
//				temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
//				if((uint16_t)temp > 1000)g_sys_param.threa.hj_humi = 1000;
//				g_sys_param.threa.hj_humi = (uint16_t)temp;break;}
			case config_temp_t:{										//�����¶���ֵ
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				if(u_temp > TEMP_MAX)
				{
					g_sys_param.threa.temp1 = TEMP_MAX;
					g_sys_param.threa.temp2 = TEMP_MAX;
//					g_sys_param.threa.temp3 = TEMP_MAX;
				}else
				{
					g_sys_param.threa.temp1 = u_temp;
					g_sys_param.threa.temp2 = u_temp;
//					g_sys_param.threa.temp3 = temp;
				}break;}
			case config_volat_t:{										//���õ�ѹ��ֵ
				u_temp = (pDr[i].data[1] << 8) | (pDr[i].data[0]);
				if(u_temp > VOLAT_MAX)g_sys_param.threa.volat = VOLAT_MAX;
				else g_sys_param.threa.volat = u_temp;break;}
			case config_temp_r:{										//�����¶�����(�޷�����)
				break;}
			case config_sy_curr_r:{								  //����ʣ���������(�޷�����)
				break;}
			case config_curr_r:{										//���õ�������(�޷�����)
				break;}
			default:break;
		}
	}
	g_sys_param.updat_flag = 1;									//�������ݸ��±�־
}
/*****************************************************************************
 * ��������:		��������ƽ̨������
 * ��ʽ����:		data_rx ���յ����� len ���ݳ���
 * ���ز���:		��
 * ��������;		2018-03-08				������ֲ
							2018-03-13				�ڴ�����ʧ�ܵĳ����д
							2018-04-02				�޸�bug;�����ڴ��������
							2018-04-04				���ͷź��ָ�븳ֵΪnull
							2018-04-09				Э����ĺ�������Ӧ����
							2018-06-07				�޸�bug;����û���յ�Ӧ��,������Ȼ��ɾ����,�ʽ��ƶ����з��ڱ�����
							2018-06-22				�޸�bug;�����ݷ����ʱ��,�����µ����ݼ���,�������,�ʴ˴�����Ϊ���յ�Ӧ���,���ݵ�����ָ����һ�����ݶ�����ֱ�Ӹ�ֵΪ��
 ****************************************************************************/ 
uint8_t parse_trans_anjisi_RX(uint8_t *data_rx , uint16_t *len)
{
	uint16_t i 							= 0;				//ѭ������
	uint16_t data_len				= 0;				//���ݳ���
	uint16_t length					=	0;				//����������ݳ���
	uint16_t num						= 0;				//��ˮ��
	uint8_t found 					= 0;				//��ʼ����־λ
	uint8_t	check_sum 			= 0;				//У���
	Time_TypeDef_t otime    = {0};			//ʱ��
	config_typedef* app_config = NULL;	//����ָ��
	
  data_len  = *len;
	if(data_len < PROTOCOL_NET_MIN)	//���Ȳ�����С����,�ȴ�
	{
		return PROTOCOL_NET_STATE_SHORT;
	}

	for(i=1; i<data_len; i++)	//������ʼ��
	{
		if((data_rx[i-1] == PROTOCOL_NET_HEAD) && (data_rx[i] == PROTOCOL_NET_HEAD))
		{
			found = 1;
			break;
		}
	}
	
	if(found != 1)	//δ�ҵ���ʼ�����������
	{
		runstate_to_usart("net: err, not find head\r\n");
		return PROTOCOL_NET_STATE_HEAD;
	}
	
	if(data_len < PROTOCOL_NET_MIN)	//���Ȳ�����С���ȣ��ȴ�
	{
		return PROTOCOL_NET_STATE_SHORT;
	}
	
	//���㳤��
	length = data_rx[DATA_LEN_0] + (data_rx[DATA_LEN_1] << 8);
	if(PROTOCOL_NET_MIN + length > PROTOCOL_NET_MAX)	//ָ��ȴ���������󳤶ȣ�ȥ��ͷ
	{
		runstate_to_usart("net: err, data length too long!\r\n");
		return PROTOCOL_NET_STATE_LONG;
	}
	if(data_len < PROTOCOL_NET_MIN + length)	//�ѽ��ճ���С��ʵ��ָ��ȣ��ȴ�
	{
		runstate_to_usart("net: err, data length too short!\r\n");
		return PROTOCOL_NET_STATE_LENGTH;
	}
	
	//δ�ҵ���������ȥ��ͷ
	if(data_rx[PROTOCOL_NET_MIN + length - 2] != PROTOCOL_NET_TAIL || data_rx[PROTOCOL_NET_MIN + length - 1] != PROTOCOL_NET_TAIL)
	{
		runstate_to_usart("net: err, not find tail\r\n");
		return PROTOCOL_NET_STATE_TAIL;
	}
	
	//����У���
	for(i=2; i<27+length; i++)
	{
		check_sum += data_rx[i];
	}
	
	if(check_sum != data_rx[length+27])	//У�����ȥ��ͷ
	{
		runstate_to_usart("net: err, check sum fail\r\n");
		return PROTOCOL_NET_STATE_CHECK;
	}
	
	//�����������ṹ��
	num = data_rx[SERIAL_NUMBER_0] + (data_rx[SERIAL_NUMBER_1] << 8);//��ˮ
  //copyӦ������
	//����RTC
	otime.sec			=	data_rx[TIME_DATA+0];
	otime.min			=	data_rx[TIME_DATA+1];
	otime.hour		=	data_rx[TIME_DATA+2];
	otime.day			=	data_rx[TIME_DATA+3];
	otime.mon			=	data_rx[TIME_DATA+4];
	otime.year		=	data_rx[TIME_DATA+5];
	RTC_Set(otime.year+2000,otime.mon,otime.day,otime.hour,otime.min,otime.sec);//���±���ʱ��
	runstate_to_usart("net: success,receive from server TIME! \r\n");						//�ɹ�����ʱ��
	if(g_app_type != type_null_dat)
	{
		g_app_type = type_null_dat;									//��һ�����ݷ���
		g_net_app_queue[g_queue_idex_s].type = type_null_dat;		//��ǰ��������Ϊ��
		data_index_move();
	}
	if(!g_senser_flag)LED_Control( L_ERROR , CLOS );
	//��������
	switch((cmd_type)(data_rx[NET_CMD]))
	{
		case type_ack_cmd:{															    //���ݷ������
				runstate_to_usart("net: success,receive from server ok! \r\n");
				break;}											
		case type_set_param_cmd:{									          //�´�������Ϣ
				do{
					app_config = calloc(data_rx[CONFIG_SIZE] , sizeof(config_typedef));
				}while(app_config == NULL);								//���붯̬�ڴ�
				length = 0;									//���ݳ���
				for(i = 0;i < data_rx[CONFIG_SIZE];i++)		//���ݷ���
				{
					app_config[i].config_type =  (CONFIG)data_rx[CONFIG_SIZE+i+1+length];
					app_config[i].leng        =  data_rx[CONFIG_SIZE+i+2+length];
					memcpy(app_config[i].data , &data_rx[CONFIG_SIZE+i+3+length] , app_config[i].leng);
					length += (app_config[i].leng+1);
				}
				Netconfig_Download(app_config , data_rx[CONFIG_SIZE]);
				runstate_to_usart("net: success,receive from server config cmd! \r\n");
				if(Wait_Net_Ack_Flag)Wait_Net_Ack_Flag = 0;
				Server_SendData(&num , type_ok_cmd , 0 , 0);
				free(app_config);
				app_config = NULL;
				runstate_to_usart("net: success,�ͷ��ڴ����\r\n");
				break;}									
			case type_get_config_cmd:{									      //ƽ̨��ȡ������Ϣ
				g_up_config_flag = 1;
				break;}
		case type_sys_reset_cmd:{														//�����������豸
				runstate_to_usart("net: success,receive from server restart cmd! \r\n");
				NVIC_SystemReset();												//����											
				break;}
		case type_ok_cmd:{
				if(Wait_Net_Ack_Flag)Wait_Net_Ack_Flag = 0;
				if(g_reset_flag & 0xF0)NVIC_SystemReset();
			break;}
		case type_get_dat_cmd:{break;}										//ƽ̨��ȡ����
		case type_server_regis_cmd:{}break;
		case type_send_dat_cmd:{}break;										//���ݷ�������(����������)
		case type_upload_cmd:{}break;									    //����������Ϣ(����������)
		case type_deny_cmd:{}break;
		case type_null_cmd:{}break;
		default:break;
	}
	return PROTOCOL_NET_STATE_OK;
}

/*****************************************************************************
 * ��������:		װ�����ݵ�����
 * ��ʽ����:		cmd ����  dat ����ָ��
 * ���ز���:		��
 * ��������;		2018-03-08				������ֲ
 ****************************************************************************/
uint8_t buildcmd_net(Net_CmdType_t *cmd)
{
	uint16_t i = 0;											//ѭ������
	uint8_t check_sum = 0;							//У���

	Build_Net_Buffer_len = 0;	
	//��ʼ��
	s_net_buff[Build_Net_Buffer_len++] = PROTOCOL_NET_HEAD;
	s_net_buff[Build_Net_Buffer_len++] = PROTOCOL_NET_HEAD;
	//��ˮ��
	s_net_buff[Build_Net_Buffer_len++] = cmd->num & 0xff;
	s_net_buff[Build_Net_Buffer_len++] = (cmd->num >> 8) & 0xff;
	//�汾��
	s_net_buff[Build_Net_Buffer_len++] = cmd->vision & 0xff;
	s_net_buff[Build_Net_Buffer_len++] = (cmd->vision >> 8) & 0xff;
	//ʱ��
	memcpy(&s_net_buff[Build_Net_Buffer_len], cmd->time, 6);
	Build_Net_Buffer_len += 6;
	//Դ��ַ
	memcpy(&s_net_buff[Build_Net_Buffer_len], &cmd->src_addr, 6);
	Build_Net_Buffer_len += 6;
	//Ŀ�ĵ�ַ
	memcpy(&s_net_buff[Build_Net_Buffer_len], &cmd->des_addr, 6);
	Build_Net_Buffer_len += 6;
	//����
	s_net_buff[Build_Net_Buffer_len++] = cmd->length & 0xff;
	s_net_buff[Build_Net_Buffer_len++] = (cmd->length >> 8) & 0xff;
	//����
	s_net_buff[Build_Net_Buffer_len++] = cmd->cmd;
	//����
	memcpy(&s_net_buff[Build_Net_Buffer_len], cmd->data, cmd->length);
	Build_Net_Buffer_len += cmd->length;
	//У��
	for(i=2; i<Build_Net_Buffer_len; i++)
	{
		check_sum += s_net_buff[i];
	}
	s_net_buff[Build_Net_Buffer_len++] = check_sum;
	//������
	s_net_buff[Build_Net_Buffer_len++] = PROTOCOL_NET_TAIL;
	s_net_buff[Build_Net_Buffer_len++] = PROTOCOL_NET_TAIL;
	return 0;
}
/*****************************************************************************
 * ��������:		��ѯ��������
 * ��ʽ����:		Cmd ���� socket Cmdlen �����
 * ���ز���;		SUCCESS ���ճɹ� ERROR ����ʧ��
 * ��������;		2018-03-08				������ֲ
							2018-03-29				��ȫ�ֱ��� NB_Handle_TX_BUF ����Ϊ�ֲ�����
              2018-04-25				coapЭ�����
							2018-06-29				�޸�bug;�޷�ʶ��Сд��ĸ
 ****************************************************************************/
ErrorStatus NB_messag_Read(char* Cmd , uint8_t Cmdlen)
{
	uint8_t  state    = 0;							//���ݽ���״̬
  uint16_t sp_start = 0;							//�ַ�����ʼλ��
	uint16_t sp       = 0;							//�ַ���λ��
	uint16_t i        = 0;							//ѭ������
	uint16_t length   = 0;							//���ݳ���
  uint16_t Recv_Net_Buffer_len=0;														//���յ����������������ݻ��泤��

	
	NB_Handle_TX_CNT=0;//���㷢�ͳ���		
	if(Cmd != 0)
	{
		IOT_onlyATsend((uint8_t*)Cmd , (uint16_t)Cmdlen);
		
		for(i = 0;i < 25;i++)//�ȴ�500ms��Ϣ�ظ�
		{ 
			NB_ReceiveData();//�յ�IOT��Ϣ
			if(g_nb_newdata_flag)
			{
				g_nb_newdata_flag = 0;
				break;
			}
		}
	}
	
	g_nb_newdata_flag = 0;
	if(strbj((char*)(g_nb_rx_buff),"ERROR",g_nb_rx_cnt,&sp)==SUCCESS) return ERROR;
	if(strbj((char*)(g_nb_rx_buff),"4040",g_nb_rx_cnt,&sp)==SUCCESS)//Ѱ��",@@"�洢����
	{   
		sp_start = sp-4;
		Recv_Net_Buffer_len=0;//���յ����������ݳ�������
		for(;sp_start<g_nb_rx_cnt;)//ת��Ϊ16��ֵ���ŵ����ջ�����
		{
			if((g_nb_rx_buff[sp_start]==',') || (g_nb_rx_buff[sp_start] =='\r'))break;	//ɾ��','�������,����COAP����,��Ӱ��,����UDP����,���ݺ��滹��һ������,�Զ��Ž���
			
			if((g_nb_rx_buff[sp_start] >= '0') && (g_nb_rx_buff[sp_start] <= '9'))//��λ
			{
				s_net_buff[Recv_Net_Buffer_len]=(g_nb_rx_buff[sp_start++]-'0')<<4;
			}else if((g_nb_rx_buff[sp_start] >= 'A') && (g_nb_rx_buff[sp_start] <= 'Z'))
			{
				s_net_buff[Recv_Net_Buffer_len]=(g_nb_rx_buff[sp_start++]-'A'+10)<<4;
			}else if((g_nb_rx_buff[sp_start] >= 'a') && (g_nb_rx_buff[sp_start] <= 'z'))
			{
				s_net_buff[Recv_Net_Buffer_len]=(g_nb_rx_buff[sp_start++]-'a'+10)<<4;
			}
			
			if((g_nb_rx_buff[sp_start] >= '0') && (g_nb_rx_buff[sp_start] <= '9'))//��λ
			{
				s_net_buff[Recv_Net_Buffer_len++]+=g_nb_rx_buff[sp_start++]-'0';
			}else if((g_nb_rx_buff[sp_start] >= 'A') && (g_nb_rx_buff[sp_start] <= 'Z'))
			{
				s_net_buff[Recv_Net_Buffer_len++]+=g_nb_rx_buff[sp_start++]-'A'+10;
			}else if((g_nb_rx_buff[sp_start] >= 'a') && (g_nb_rx_buff[sp_start] <= 'z'))
			{
				s_net_buff[Recv_Net_Buffer_len++]+=g_nb_rx_buff[sp_start++]-'a'+10;
			}
		}
		sp_start=0;//������β���
		do			//��������
		{
			if((s_net_buff[sp_start] == '@') && (s_net_buff[sp_start+1] == '@'))
			{					
				length=Recv_Net_Buffer_len-sp_start;
				state=parse_trans_anjisi_RX(&s_net_buff[sp_start],&length);//��������
				if(state == PROTOCOL_NET_STATE_OK) sp_start+=30+length;//�µ���ʼ��ַ����30
				else sp_start++;//����Ѱ��		
			}else sp_start++;//��ʼλ������
		}while((sp_start+1)<Recv_Net_Buffer_len);
	}
	return SUCCESS;
}

/*****************************************************************************
 * ��������:		�����緢�ͻ����е����ݷ��͵����ģ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-07				������д
							2018-06-11				�����Ż�;�Ա������������Ż� Wait_Net_Ack_Flag ��ѡ���ж�
							2018-06-22				�����Ż�;���� g_app_type          = type_null_dat;             ����������
 ****************************************************************************/ 
void net_to_module(void)
{
	static uint8_t out_poll_max=0;			//ģ������ش�����
	static uint8_t out_send_max=0;			//����������ش�����

	if((g_nb_error_flag) || (!g_model_config_flag))return;
	//��������Ҫ����
	if(Build_Net_Data_Flag == 1)//����δ���͵�ģ���ҷ���������
	{//����ȴ�����������һ�η������ݣ�
		out_poll_max++;//ģ���ش�����
		if(NB_Send( s_net_buff, Build_Net_Buffer_len ) == SUCCESS)//ģ��ظ��ɹ�
		{
			g_rec_time = g_sys_tim_s + 30;
			if(g_rec_time == 0)g_rec_time = 1;								//���ճ�ʱ��ʱ��
			
			Build_Net_Data_Flag = 0;//�����Ѿ����͵�ģ��
			NET_outtime = SEND_OutTime;//�ȵ�ʱ��(s)
			out_poll_max = 0;//ģ���ش��������
			if(Net_Ack_SW)
			{
				Net_Ack_SW        = 0;
				Wait_Net_Ack_Flag = 1;
			}
		}
				
		if(out_poll_max >= OUT_POLL_MAX_TOP0)//ģ���ش�����
		{
			out_poll_max        = 0;
//			g_app_type          = type_null_dat;
			Build_Net_Data_Flag = 0;
			g_nb_error_flag     = 1;
		}
	}else
	{
		switch(Wait_Net_Ack_Flag)
		{
			case 0:{//������Ӧ��ɹ�
				out_send_max = 0;//�������ش��������
				break;}
			case 1:{//������Ӧ����...
				if(NET_outtime == 0)Wait_Net_Ack_Flag=2;//Ӧ��ʱ
				break;}
			case 2:{//������Ӧ��ʧ��
				runstate_to_usart("To server Overtime!\r\n");//��ʱ
				out_send_max++;//�������ش�������
				if(out_send_max >= OUT_SEND_MAX)//�����������ش�
				{ 
					out_send_max=0;//�������ش��������
					Wait_Net_Ack_Flag=1;//�������ȴ�
					Build_Net_Data_Flag=1;//�ٴ�װ��
					runstate_to_usart("To server failed!\r\n");//�������ϴ�ʧ��
					g_nb_error_flag = 1;
				}else if(out_send_max >= 2)//��ʱ��������
				{
					g_nb_error_flag = 1;
					runstate_to_usart("To server outnumber 2!\r\n");//�������ϴ�ʧ��
					Wait_Net_Ack_Flag=1;//�������ȴ�
					Build_Net_Data_Flag=1;//�ٴ�װ��
				}else
				{
						Wait_Net_Ack_Flag=1;//�������ȴ�
						Build_Net_Data_Flag=1;//�ٴ�װ��
				}break;}
			default:{//����
				break;}
		}
	}
}

/*------------------------------File----------End------------------------------*/
