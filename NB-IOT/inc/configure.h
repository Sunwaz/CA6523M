/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018/03/07
 *˵    ��:	2018-03-07					�ĵ���д
*******************************************************************************************/
#ifndef __CONFIGURE_H
#define __CONFIGURE_H
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "com.h"
/* �궨��	--------------------------------------------------------------------*/
#define NB_TX_LEN           512*2+50    //����NB���͵�ģ����󳤶�[ʵ�����ݳ���=256�ֽ�](AT+NSOST=0,192.53.100.53,5683,2,4002)

#define YIDONG                        0x01
#define LIANTONG                      0x02
#define DIANXIN												0x03

/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
extern uint8_t	g_nb_newdata_flag;						//�����ݱ�־ 
extern uint8_t	g_nb_error_flag;				      //nbiotģ����ϱ�־
extern char MODLE_VER[20];										//�̼��汾
extern char NB_Ope;                           //����
extern param_typedef g_sys_param;             //ϵͳ����
extern uint16_t g_nb_rx_cnt;									//���ռ���
extern uint8_t  END[2];                       //ʹ�ý�����
extern uint8_t	g_rec_time;									  //�ظ���Ϣ�ĳ�ʱʱ��,���ʱ�䳬����30s,�򲻲�ѯ����,���С��30s��ÿ2s��ѯһ�ν���
extern uint8_t g_nb_reset_flow;	              //��ʼ������
extern uint16_t NB_Handle_TX_CNT;				      //NB���ʹ������ ���� ��ģ��
extern uint8_t socket;												//socket��
extern char MODLE_NAME[20];										//ģ���ͺ�
extern uint8_t g_model_config_flag;						//ģ���ʼ����־
/* �ⲿ�ӿ�	------------------------------------------------------------------*/
void runstate_to_usart(char* cmd);											//���͵���λ��
void network_parameterUpdata(void);											//�����������(�ַ���ת������)
void int_to_char(char* s0 , uint8_t* s1 , uint16_t len);//����ת�����ַ���
void network_parameter_flashRead(void);									//��ʼ���������
ErrorStatus IOT_ATsend(uint8_t *p , uint16_t len , char *c);//����ATָ��
void IOT_onlyATsend(uint8_t *p , uint16_t len);         //����ATָ��
ErrorStatus Get_ChipID( uint8_t *cnt);																	//��ȡ�豸ID
ErrorStatus strbj(char *a,char *b,uint16_t len, uint16_t *k);//�ַ�����������������ַ�����
ErrorStatus NB_send(uint8_t *ip , u16 port , uint8_t *p , uint16_t len);
void Num_exchange(uint32_t num , uint8_t *p , uint16_t* len);

ErrorStatus NB_WaitStartMessage(char *str , uint8_t* re_cnt);
ErrorStatus NB_ReadMoudleVer(char* cmd , uint16_t length , char *str , uint8_t *re_cnt);
ErrorStatus NB_ReadMoudleIMEI(char *str , uint8_t* re_cnt);
ErrorStatus NB_ReadMoudleISMI(char *str , uint8_t* re_cnt);
ErrorStatus NB_ReadMoudleOperator(char *str , uint8_t* re_cnt);
ErrorStatus NB_SendCmd(char * cmd , uint16_t length, char* str, uint16_t *index , uint8_t* re_cnt);
ErrorStatus NB_ReadMoudleData(char *cmd , uint16_t length , char *str , uint16_t *str_index);
void NB_ReadMoudleCSQ(uint8_t *buff , char *str);
ErrorStatus NB_WaitData(char* str , uint16_t *index);
ErrorStatus NB_SendServerData(uint8_t *NB_Handle_TX_BUF , uint8_t *data , uint16_t len);
#endif /* __CONFIGURE_H end */
/*------------------------------File----------End------------------------------*/
