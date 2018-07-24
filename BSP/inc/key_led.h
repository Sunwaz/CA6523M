/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-09					�ĵ���д
 *˵    ��:	
*******************************************************************************************/
#ifndef __KEY_LED_H
#define __KEY_LED_H
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include <stm32f0xx.h>
/* �궨��	--------------------------------------------------------------------*/
#define NEW_MODEL

//������־λ�Ĳ���
#define VOLAT_ALARM                   0x01				//��ѹ
#define CURR_1_ALARM									0x02				//����1                VOLAT_ALARM    << 1       
#define CURR_2_ALARM									0x04				//����2                CURR_1_ALARM   << 1
#define CURR_3_ALARM                  0x08				//����3                CURR_2_ALARM   << 1
#define TEMP_1_ALARM									0x10				//�¶�1                CURR_3_ALARM   << 1
#define TEMP_2_ALARM                  0x20				//�¶�2                TEMP_1_ALARM   << 1
#define TEMP_3_ALARM									0x40				//�¶�3                TEMP_2_ALARM   << 1
#define SY_CURR_ALARM									0x80				//ʣ�����             TEMP_3_ALARM   << 1
#define RESERVE_1_ALARM								0x100				//Ԥ��                 SY_CURR_ALARM  << 1
#define RESERVE_2_ALARM								0x200				//Ԥ��                 RESERVE_1_ALARM<< 1
#define ALARM_RECOVERY                0x40000000  //�𾯻ָ�
#define ALARM_RESET                   0x80000000  //�𾯸�λ
#define LOCAL_ALARM                   0x00				//���ر���
#define ALARM_SILENCE									0x0A				//��������
#define ALARM_UPLOAD									0x14				//�����ϴ�
#define READ_VOLAT                    0x100401    //��ȡ��ѹ��������        
#define READ_CURR_1										0x200802    //��ȡ����1��������       READ_VOLAT  << 2
#define READ_CURR_2										0x401004    //��ȡ����2��������       READ_CURR_1 << 2
#define READ_CURR_3                   0x802008    //��ȡ����3��������       READ_CURR_2 << 2
#define READ_TEMP_1                   0x1004010   //��ȡ�¶�1��������       READ_CURR_3 << 2
#define READ_TEMP_2                   0x2008020   //��ȡ�¶�2��������       READ_TEMP_1 << 2
#define READ_TEMP_3										0x4010040   //��ȡ�¶�3��������       READ_TEMP_2 << 2
#define READ_SY_CURR                  0x8020080   //��ȡʣ�������������    READ_TEMP_3 << 2
#define READ_RESERVE_1								0x10040100  //��ȡԤ����������        READ_SY_CURR<< 2
#define READ_RESERVE_2                0x20080200  //��ȡԤ����������        READ_RESERVE_1<<2
#define READ_ALARM                    0x3FF       //��ȡ��������            
#define READ_SILENCE                  0xFFC00     //��ȡ��������            READ_ALARM  << 10
#define READ_UPLOAD                   0x3FF00000  //��ȡ�ϱ�����						 READ_SILENCE<< 10
/* �ṹ�嶨��	----------------------------------------------------------------*/
typedef enum{														//LED����
	L_WORK					=	0,									//����ָʾ��
	L_ALARM					= 1,									//����ָʾ��
	L_ERROR					= 2,									//����ָʾ��
	L_CLEAR					=	3,									//����ָʾ��
	L_ALL						=	4,									//����ָʾ��
	L_SIGNAL        = 5,									//�ź�ָʾ��
}LED_Typedef;
typedef enum{														//LED��������
	OPEN						=	0,									//��
	CLOS						=	1,									//�ر�
}LED_Ctl_Typedef;
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
extern uint8_t	g_sys_error_flag;				//ϵͳ���ϱ�־λ    0,�޹���,LED������˸ 1,�й���,LED���ֵ�ǰ״̬
extern uint8_t	g_reset_flag;						//��λ��־
extern uint32_t	g_alarm_flag;						//������־λ
extern uint16_t g_sys_operation_msg;		//bit0 �豸��λ bit1 �豸���� bit2 �ֶ����� bit3 �������� bit4 �Լ� bit5 ȷ�� bit6 ���� bit7 SIM�������� bit8 �豸����
/* �ⲿ�ӿ�	------------------------------------------------------------------*/
void KeyLED_Config( void );							//������LED �����ʼ��
void LED_TimerIrqHandle( void );				//��ʱ���жϷ�����
void LED_Control( LED_Typedef Led , LED_Ctl_Typedef State );//LED�ƿ��ƺ���
void Key_Scan( void );									//����ɨ��
void Alarm_Set( void );									//����
void Alarm_Reset( void );								//�����ָ�
#endif /* __KEY_LED_H end */
/*------------------------------File----------End------------------------------*/
