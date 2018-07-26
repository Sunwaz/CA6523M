/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�ǻ��õ�
 *��    ��:	AJS.Sunwaz
 *�޸�����:	2018-03-14					�ĵ���ֲ
 *˵    ��:	
*******************************************************************************************/
#ifndef __ADC_GET_H
#define __ADC_GET_H
/* ͷ�ļ� ------------------------------------------------------------------*/
#include <stm32f0xx.h>
#include "rtc.h"
/* �궨�� ------------------------------------------------------------------*/
#define COMP_COEFFIC						1.00							//����ϵ��
#define RES_COMP								-0.020						//���貹��ϵ��

//��������
#define CURRENCY_PART                 0x00				//�������� ͨ��
#define SY_CURR_DETECTOR							0x11				//�������� ʣ�����̽����
#define TEMP_DETECTOR									0x12				//�������� ����ʽ�������ּ��̽����		
#define VOLAT_DETECTOR								0x80				//�������� ��ѹ̽����
#define CURR_DETECTOR									0x81				//�������� ����̽����
#define WSD_DETECTOR                  0x82				//�������� ��ʪ��̽����
//��������
#define TEMP_DATA											0x03				//�������� �¶�
#define VOLAT_DATA                    0x08				//�������� ��ѹ
#define CURR_DATA											0x09				//�������� ����
#define SY_CURR_DATA									0x80				//�������� ʣ�����
#define SIGNAL_DATA										0x81				//�������� �ź�ǿ��
#define HUMI_DATA											0x82				//�������� ʪ��

#define CURR1_DATA_INDEX                          0x00
#define CURR2_DATA_INDEX                          0x01
#define CURR3_DATA_INDEX                          0x02
#define SY_CURR_DATA_INDEX                        0x03
#define VOLAT_DATA_INDEX                          0x04
#define TEMP1_DATA_INDEX                          0x05
#define TEMP2_DATA_INDEX													0x06
#define TEMP3_DATA_INDEX													0x07

#define CALL_SIZE                     0x0F        //����20������
#define ADC_SIZE																	(sizeof(data_typedef)/sizeof(uint16_t))
#pragma pack(1)
/* ö������	----------------------------------------------------------------*/
typedef enum{																		  //��������
	data_temp    = (uint8_t) TEMP_DATA,							//�¶�����
	data_volat   = (uint8_t) VOLAT_DATA,						//��ѹ����
	data_curr    = (uint8_t) CURR_DATA,							//��������
	data_humi    = (uint8_t) HUMI_DATA,							//ʪ������
	data_signal  = (uint8_t) SIGNAL_DATA,						//�ź�ǿ��
	data_sy_curr = (uint8_t) SY_CURR_DATA,					//ʣ���������
}DATA;
typedef enum{																		  //��������
	part_temp = (uint8_t) TEMP_DETECTOR,						//�¶�̽����
	part_volt = (uint8_t) VOLAT_DETECTOR,						//��ѹ̽����
	part_curr = (uint8_t) CURR_DETECTOR,						//����̽����
	part_sycu = (uint8_t) SY_CURR_DETECTOR,					//ʣ�����̽����
	part_ty   = (uint8_t) CURRENCY_PART,						//ͨ�ò���
}PART;
typedef enum{																		  //��־λ��Ϣ
	bit0 = 0x01,																		//BIT0
	bit1 = 0x02,																		//BIT1
	bit2 = 0x04,																		//BIT2
	bit3 = 0x08,																		//BIT3
	bit4 = 0x10,																		//BIT4
	bit5 = 0x20,																		//BIT5
	bit6 = 0x40,																		//BIT6
	bit7 = 0x80,																		//BIT7
}FLAG_BITS;
/* �ṹ������	--------------------------------------------------------------*/
typedef struct{																		//��������
	uint16_t   curr1;																//����ֵ
	uint16_t   curr2;																//����ֵ
	uint16_t   curr3;																//����ֵ
	uint16_t   sy_curr;															//ʣ�����ֵ
	uint16_t   temp1;																//�¶�ֵ
	uint16_t   temp2;																//�¶�ֵ
//	uint16_t   temp3;																//�¶�ֵ
	uint16_t   volat;																//��ѹֵ
}data_typedef;
typedef struct{
	PART         part_type;													//��������
	uint32_t     part_addr;													//������ַ				//��λ��ǰ ��λ�ں�
	DATA         data_type;													//��������
	int16_t      data_value;							          //����ֵ					//������
	Time_TypeDef_t time;														//ʱ���
}Msg_data;
typedef struct{																		//У׼����
	PART     type;																	//У׼�Ĵ���������
	uint8_t  flag;																	//У׼��־
	uint16_t value;																	//У׼ֵ
}adc_cail;
/* ȫ�ֱ���	----------------------------------------------------------------*/
extern Msg_data g_coll_data[ADC_SIZE];	          //�ɼ�����������
extern adc_cail g_cail_data;											//У׼����
extern uint16_t g_senser_flag;										//������״̬
/* �ӿ��ṩ	----------------------------------------------------------------*/
void ADC_GetConfig( void );												//ADC��ʼ��
void ADC_DMAIRQnHandle( void );										//DMA�жϷ�����
double ADC_Collection( uint8_t cmd );								//���ݲɼ�
void ADC_Cail( void );														//����У׼
#endif       /*__ADC_GET_H end*/
/*------------------------------File----------End------------------------------*/
