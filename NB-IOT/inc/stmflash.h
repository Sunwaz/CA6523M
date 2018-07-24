/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018/03/08
 *˵    ��:	2018-03-08					�ĵ���ֲ
*******************************************************************************************/
#ifndef __STMFLASH_H__
#define __STMFLASH_H__
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* �궨��	--------------------------------------------------------------------*/
#define STM32_FLASH_SIZE											64 	 								//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN											1              			//ʹ��FLASHд��(0��������;1��ʹ��)
#define STM32_FLASH_BASE											0x08000000 					//STM32 FLASH����ʼ��ַ

#define BACKUPS_CODE													(APP_CODE+STM_SECTOR_SIZE*30)
#define APP_CODE															(STM32_FLASH_BASE+STM_SECTOR_SIZE*4)

#define UPDET_FLAG														0x0800FF00
#define APP_LENGTH														0x0800FF02					//(UPDET_FLAG+2)
#define CHECK_SUM															0x0800FF04					//(APP_LENGTH+2)

#define FLASH_SAVE_ADDR_DeviceID							0x0800FF06					//(CHECK_SUM+2)
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
/* �ⲿ�ӿ�	------------------------------------------------------------------*/
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		  							//��������  
void STMFLASH_WriteLenByte(uint32_t WriteAddr,uint32_t DataToWrite,uint16_t Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr,uint16_t Len);										//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
#endif /* __RTC_H end */
/*------------------------------File----------End------------------------------*/
