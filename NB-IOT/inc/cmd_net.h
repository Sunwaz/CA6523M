/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018/03/07
 *˵    ��:	2018-03-07					�ĵ���д
*******************************************************************************************/
#ifndef __CMD_NET_H
#define __CMD_NET_H
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* �궨��	--------------------------------------------------------------------*/
#define SYSTEM_STA_zongxinfault (1<<10) //̽�������ӹ��� ����
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
/* �ⲿ�ӿ�	------------------------------------------------------------------*/
void app(void);
ErrorStatus Init_all(void);//�����緢�ͻ����е����ݷ��͵����ģ��
void net_to_module(void);
#endif /* __CMD_NET_H end */
/*------------------------------File----------End------------------------------*/