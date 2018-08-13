/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-13					�ĵ���д
 *˵    ��:	
*******************************************************************************************/
#ifndef __com_H
#define __com_H
/* ͷ�ļ� ------------------------------------------------------------------*/
#include <stm32f0xx.h>
#include "rtc.h"
#include "string.h"
#include "adc_get.h"
/* �궨�� ------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////
#define CURR_RANGE_IN                 (int16_t)1000  				 //����������һ�β�������
#define CURR_RANGE_OUT                (int16_t)1200  				 //�������������β�������
#define SY_CURR_RANGE_IN              (int16_t)10000				 //ʣ�����������һ�β�������
#define SY_CURR_RANGE_OUT             (int16_t)500					 //ʣ��������������β�������
#define TEMP_MAX										  (int16_t)3050					 //��߿ɲ��¶�
#define TEMP_MIN										  (int16_t)-500					 //��Ϳɲ��¶�
#define VOLAT_MAX										  (int16_t)5000					 //��߿ɲ��ѹ

#define SYS_VER                       "CA6523M-V101-20180725"//����汾��

#define SYS_FLAG										  0x80									 //�ǻ��õ�ϵͳ

#define TRANS_BUFFER_MAX						  512						         //���տ������������ݻ����С
#define NET_TRANS_MAX								  512									   //����͸�����ݻ����С

#define IP_LEN						            4									     //IPV4 �����IPV6 ��˴�Ϊ6

#define PROTOCOL_NET_STATE_OK				  0									     //�����ɹ�
#define PROTOCOL_NET_STATE_SHORT		  1									     //�������ݹ��̣�С����С����
#define PROTOCOL_NET_STATE_LONG			  2									     //�����ȡ�ĳ��ȹ���
#define PROTOCOL_NET_STATE_HEAD			  3									     //ͷ����
#define PROTOCOL_NET_STATE_TAIL			  4									     //β����
#define PROTOCOL_NET_STATE_CHECK		  5									     //��У�����
#define PROTOCOL_NET_STATE_LENGTH		  6									     //�ѽ��ճ���С�ڶ�ȡ�������

#define PROTOCOL_NET_HEAD             '@'
#define PROTOCOL_NET_TAIL             '#'

#define PROTOCOL_NET_MIN		  			  30										 //�������ݳ��ȵ���Сֵ
#define PROTOCOL_NET_MAX		          512										 //����ͨѶЭ�鳤��

#define RECV_NET_BUFFER_MAX					  256										 //���ճ���

#define NB_NET_RECVLEN_MAX					  512										 //NB���ս��ճ���
#define NB_NET_SENDLEN_MAX					  1024									 //NB���շ��ͳ���

#define OBJ_LENGTH_SYS							  1									     //��ȡʱһ��ϵͳ��Ϣ���󳤶�
#define OBJ_LENGTH_PART							  5									     //��ȡʱһ��������Ϣ���󳤶�

//�Ƶ�Ԫ�����ֽڶ����
#define NET_NULL_CMD									0x00				//��
#define NET_CONTROL_CMD								0x01				//��������    (ʱ��ͬ��)
#define NET_SEND_DATA                 0x02				//�ϴ�����    (���ͻ��ֱ����ͽ���������ʩ����״̬����Ϣ)
#define NET_OK       								  0x03				//ȷ��֡      (�Կ�������ͷ�����Ϣ��ȷ�ϻش�)
#define NET_GET_DATA									0x04				//��������    (��ѯ���ֱ����ͽ���������ʩ����״̬����Ϣ)
#define NET_ACK												0x05				//Ӧ��֡				(���ز�ѯ����Ϣ)
#define NET_DENY											0x06				//����				(�Կ�������ͷ�����Ϣ�ķ��ϻش�)
#define NET_STARTUP                   0x08        //������Ϣ
#define NET_UPLOAD_CONFIG							0x80				//�����ϱ�ϵͳ���ò�������
#define NET_SET_PARAM     						0x81				//�趨ϵͳ��������
#define NET_GET_CONFIG								0x82				//��ȡ������Ϣ
#define NET_APP_RESET									0x0B				//�豸����
#define NET_SEVER_REGIST							0x84				//������ע��

//�������ͱ�־
#define NET_NULL_DATA									0x00				//������
#define NET_UPLOAD_STATE              0x02				//�ϴ�����������ʩ��������״̬
#define NET_UPLOAD_ANLOG							0x03				//�ϴ�����������ʩ����ģ����ֵ
#define NET_OPERA_INFOR               0x04        //�ϴ�����������ʩ������Ϣ
#define NET_UPLOAD_CFG								0x1A				//�ϴ��û���Ϣ����װ���������
#define NET_DOWNLOAD_CFG							0x80				//�´�װ������
#define NET_UPLOAD_RUNING             0x84        //�ϴ�������Ϣ
#define NET_UPLOAD_STARTUP            0x85        //�ϴ�������Ϣ
#define NET_UPLOAD_RECOVER						0xA2				//�ϴ��ָ�״̬

//���ò�������
#define IP_ADDR                       0x01        //�������� IP��ַ
#define HEART_TIME										0x05				//�������� ����ʱ��
#define SAMPLING_TIME                 0x06				//�������� ����ʱ��
#define CURR_RANGE										0x0B				//�������� ��������������
#define SY_CURR_RANGE                 0x0C				//�������� ʣ���������������
#define TEMP_RANGE										0x0D				//�������� �¶ȴ���������
#define CURR_THRESHOLD								0x0E				//�������� ������ֵ
#define SY_CURR_THRESHOLD             0x0F				//�������� ʣ�������ֵ
#define TEMP_THRESHOLD                0x10				//�������� �¶���ֵ
#define VOLAT_THRESHOLD               0x11				//�������� ��ѹ��ֵ
#define TEMP_HUMI_THRESHOLD           0x12				//�������� ��ʪ����ֵ
#define VOLAT_RANGE										0x13				//�������� ��ѹ��Χ

//������Ϣ��������
#define MODULAR_TYPE									0x40				//ģ������ 0x08 ��ʾNBģ��
#define OPERATOR                      0x41        //��Ӫ��   0x01 �ƶ� 0x02 ��ͨ 0x03���� 0x04 �ƶ�����ͨ
#define CODE_IEME                     0x42        //ģ��IEME��       //����������ṩģ����
#define SIGNAL_CSQ                    0x43        //�ź�ǿ��
#define SINGAL_POWER                  0x44				//�źŹ���
#define PCI_NUM                       0x45        //��վ���
#define CELL_ID                       0x46				//С�����
#define SIGNAL_SNR                    0x47        //�����
#define CODE_IMSI                     0x48        //SIM��IMSI��      //���ڲ黰��

#define SYS_VERSION                   0xA0        //ϵͳ�汾         //���� CA6523M-V101-20180510
#define MODULAR_VERSION               0xA1        //ģ��̼��汾     //���� BC95B5HBR01A02W16
//����״̬
#define CURR_1_STATE									0x80				//̽����1					//����̽����1
#define CURR_2_STATE                  0x81				//̽����2					//����̽����2
#define CURR_3_STATE									0x82				//̽����3					//����̽����3
#define VOLAT_1_STATE									0x83				//̽����4					//��ѹ̽����1
#define VOLAT_2_STATE									0x84				//̽����5					//��ѹ̽����2
#define VOLAT_3_STATE									0x85				//̽����6					//��ѹ̽����3
#define TEMP_1_STATE									0x86				//̽����7					//�¶�̽����1
#define TEMP_2_STATE									0x87				//̽����8					//�¶�̽����2
#define TEMP_3_STATE									0x88				//̽����9					//�¶�̽����3
#define SY_CURR_1_STATE								0x89				//̽����10					//ʣ�����̽����1


#pragma pack(1)
/* �ṹ������	--------------------------------------------------------------*/
typedef enum{																		 //��������
	type_null_cmd					=	NET_NULL_CMD,					 //��
	type_send_dat_cmd			=	NET_SEND_DATA,				 //�ϴ�����
	type_ok_cmd						= NET_OK,								 //ȷ��֡
	type_get_dat_cmd			=	NET_GET_DATA,					 //��������
	type_ack_cmd					=	NET_ACK,							 //Ӧ��֡	
	type_deny_cmd					=	NET_DENY,							 //����
	type_upload_cmd				=	NET_UPLOAD_CONFIG,		 //�����ϱ�ϵͳ���ò�������
	type_set_param_cmd		=	NET_SET_PARAM,				 //�趨ϵͳ��������
	type_sys_reset_cmd		=	NET_APP_RESET,				 //ϵͳ����
	type_server_regis_cmd = NET_SEVER_REGIST,			 //������ע��
	type_get_config_cmd		=	NET_GET_CONFIG,				 //��ȡ������Ϣ
	type_startup_cmd      = NET_STARTUP,           //������Ϣ
}cmd_type;
typedef enum{																		 //Ӧ����������
	type_null_dat					=	NET_NULL_DATA,				 //������
	type_send_anlog       = NET_UPLOAD_ANLOG,			 //����ģ������
	type_upload_config    = NET_UPLOAD_CFG,				 //�ϴ���������
	type_download_config  = NET_DOWNLOAD_CFG,			 //�´���������
	type_upload_startup   = NET_UPLOAD_STARTUP,    //�ϴ���������
	type_runing           = NET_UPLOAD_RUNING,     //�ϴ���������
	type_oper_info        = NET_OPERA_INFOR,			 //�ϴ�������Ϣ
	type_senser_info      = NET_UPLOAD_STATE,			 //�ϴ��豸����״̬
	type_senser_recover   = NET_UPLOAD_RECOVER,		 //�ϴ��豸�ָ�״̬
}app_type;

typedef enum{																			//���ò�������
	config_heart          = HEART_TIME,							//����ʱ������
	config_camp           = SAMPLING_TIME,					//����ʱ��
	config_ip             = IP_ADDR   ,             //IP��ַ����
	config_curr_r         = CURR_RANGE,             //��������������
	config_curr_t         = CURR_THRESHOLD,					//������ֵ
	config_sy_curr_r      = SY_CURR_RANGE,					//ʣ���������������
	config_sy_curr_t      = SY_CURR_THRESHOLD,			//ʣ�������ֵ
	config_temp_r         = TEMP_RANGE,							//�¶ȴ���������
	config_temp_t         = TEMP_THRESHOLD,					//�¶���ֵ
	config_volat_r        = VOLAT_RANGE,						//��ѹ��Χ
	config_volat_t        = VOLAT_THRESHOLD,				//��ѹ��ֵ
//	config_temp_humi_t    = TEMP_HUMI_THRESHOLD,		//������ʪ����ֵ
}CONFIG;
#define CONFIG_TYPE_NUM					11          				//CONFIG ö�ٵ�����
typedef enum{																			//������Ϣ��������
	startup_IEMI          = CODE_IEME,              //IEMI��
	startup_operator      = OPERATOR,               //��Ӫ��
	startup_mouldtype     = MODULAR_TYPE,           //ģ������
	startup_sys_ver       = SYS_VERSION,            //ϵͳ����汾��
	startup_mou_ver       = MODULAR_VERSION,        //ģ��̼��汾��
}startup_para_type;
#define STARTUP_PARA_TYPE_NUM					5           //startup_para_type ö�ٵ�����
typedef enum{																			//����״̬��������
	runing_null           = 0,										  //������
	runing_signal_power   = SINGAL_POWER,           //�ź�ǿ��
	runing_cell_id        = CELL_ID,								//С�����
	runing_pci            = PCI_NUM,                //��վ���
	runing_snr            = SIGNAL_SNR,							//�����
	runing_csq            = SIGNAL_CSQ,             //�ź�ǿ��
	runing_IMSI           = CODE_IMSI,							//ISMI��
}runing_state_type;
#define RUNING_STATE_TYPE_NUM         6           //runing_state_type ö�ٵ�����

typedef struct{																		//�������ݽṹ��
	CONFIG			config_type;												//��������
	uint8_t			leng;																//���ݳ���
	uint8_t      data[6];													  //����					//������
}config_typedef;
typedef struct{																	  //��������ṹ��
	uint16_t  num;																  //��ˮ��
	uint16_t	vision;															  //Э��汾
	uint8_t		time[6];													  	//ʱ��
	uint8_t		src_addr[6];											  	//Դ��ַ
	uint8_t		des_addr[6];										  		//Ŀ�ĵ�ַ
	uint16_t	length;												  			//���ݳ���
	cmd_type	cmd;												  			  //������
	uint8_t		data[512];										  			//����
}Net_CmdType_t;
typedef struct{																		//У׼���ݽṹ��
	uint16_t volat;																	//��ѹУ׼����
	uint16_t curr;																	//����У׼����
	uint16_t sy_curr;																//ʣ�����У׼����
	uint16_t temp[3];																//�¶�У׼����
}cail_typedef;
typedef struct{																		//�������ݽṹ��
	uint8_t temp;																		//�¶�ͨ������       ����λԤ��,��6λΪͨ����
	uint8_t curr;																		//����ͨ������       ����λԤ��,��6λΪͨ����
	uint8_t volat;																	//��ѹͨ������       ����λԤ��,��6λΪͨ����
	uint8_t sy_curr;																//ʣ�����ͨ������    ����λԤ��,��6λΪͨ����
}shield_typedef;
typedef struct{																		//ϵͳ�����ṹ��
	uint16_t    updat_flag;													//���±�־λ
	uint16_t    camp_time;													//����ʱ��
	uint16_t    hart_time;													//����ʱ��
	uint8_t     server_ip[6];												//������IP
	uint8_t     device_ip[6];												//�豸�ն�IP
	uint8_t     IMEI[20];														//�豸IEMI��
	uint8_t     IMSI[20];														//�豸IMSI��
	uint16_t    port;																//�˿ں�
	data_typedef threa;															//��ֵ����
	cail_typedef cali;                              //У׼����
	shield_typedef shield;                          //�������ݽṹ��
}param_typedef;
typedef struct{																		//����Ӧ�ö���
	app_type type;																	//����
	uint16_t idex;																	//��λ��
}net_app_typedef;
/* ö������	----------------------------------------------------------------*/
/* ȫ�ֱ���	----------------------------------------------------------------*/
extern uint8_t Device_ID[6];											//��������
extern uint8_t NET_outtime;												//��������ʱʱ��
extern uint32_t g_sys_tim_s;											//����ʱ��
/* �ӿ��ṩ	----------------------------------------------------------------*/
#endif       /*__com_H end*/
/*------------------------------File----------End------------------------------*/
