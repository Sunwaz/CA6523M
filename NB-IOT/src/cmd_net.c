/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018-03-07					�ĵ���ֲ
 *˵    ��:	���������
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "cmd_net.h"
#include "rtc.h"
#include "com.h"
#include "nbiot.h"
//#include "transmission.h"

#include "string.h"
/* �궨��	--------------------------------------------------------------------*/
#define	OUT_POLL_MAX 			 						5										//��ѯģ��״̬���ɷ����ز�������
#define	OUT_POLL_MAX_DIS							2
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
uint8_t g_rec_time = 0;
/* ϵͳ����	------------------------------------------------------------------*/

/*------------------------------File----------End------------------------------*/