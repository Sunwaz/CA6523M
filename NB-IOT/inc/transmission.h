/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018/03/08
 *˵    ��:	2018-03-08					�ĵ���ֲ
*******************************************************************************************/
#ifndef __TRANSMISSIN_H
#define __TRANSMISSIN_H
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "com.h"
/* �궨��	--------------------------------------------------------------------*/
#define QUEUE_SIZE										20							//���д�С
#define QUEUE_DAT_MAX									0x3FF						//0-0x3FF
#define QUEUE_DAT_SIZE                1024						//�������ݵ����ֵ
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
extern uint8_t 				Build_Net_Data_Flag;						//���ݷ��ͱ�־							0���Ѿ����͵�ģ�� ���Կ�ʼ��ʱ����	1��δ����
extern uint8_t				g_queue_idex_s;									//��ȡλ��
extern uint8_t				g_queue_idex_e;									//��ȡλ��
extern uint8_t				g_nb_net_buff[QUEUE_DAT_SIZE];	//�洢����                       ���1K���ڴ�
extern uint8_t				g_up_config_flag;								//��ȡ������Ϣ�ı�־
extern uint16_t 			Serial_Number;									//��ˮ��
extern uint16_t				g_nb_net_buf_end;								//NB�洢���������ݵĽ���λ��
extern uint16_t				g_nb_net_buf_start;							//NB�洢���������ݵ���ʼλ��
extern net_app_typedef g_net_app_queue[QUEUE_SIZE];		//����Ӧ��APP
extern app_type				g_app_type;											//��ǰ��������
extern Time_TypeDef_t Time;														//ϵͳʱ��
/* �ⲿ�ӿ�	------------------------------------------------------------------*/
void Server_SendData(uint16_t* num , cmd_type cmd_name ,uint8_t data_type , uint8_t msg_len);//����������������
void net_to_module(void);															//�����緢�ͻ����е����ݷ��͵����ģ��
#endif /* __RTC_H end */
/*------------------------------File----------End------------------------------*/
