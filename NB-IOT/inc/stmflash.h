/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz,王泉凯
 *修改日期:	2018/03/08
 *说    明:	2018-03-08					文档移植
*******************************************************************************************/
#ifndef __STMFLASH_H__
#define __STMFLASH_H__
/* 头文件	--------------------------------------------------------------------*/
#include "stm32f0xx.h"
/* 宏定义	--------------------------------------------------------------------*/
#define STM32_FLASH_SIZE											64 	 								//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN											1              			//使能FLASH写入(0，不是能;1，使能)
#define STM32_FLASH_BASE											0x08000000 					//STM32 FLASH的起始地址

#define BACKUPS_CODE													(APP_CODE+STM_SECTOR_SIZE*30)
#define APP_CODE															(STM32_FLASH_BASE+STM_SECTOR_SIZE*4)

#define UPDET_FLAG														0x0800FF00
#define APP_LENGTH														0x0800FF02					//(UPDET_FLAG+2)
#define CHECK_SUM															0x0800FF04					//(APP_LENGTH+2)

#define FLASH_SAVE_ADDR_DeviceID							0x0800FF06					//(CHECK_SUM+2)
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
/* 外部接口	------------------------------------------------------------------*/
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		  							//读出半字  
void STMFLASH_WriteLenByte(uint32_t WriteAddr,uint32_t DataToWrite,uint16_t Len);	//指定地址开始写入指定长度的数据
uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr,uint16_t Len);										//指定地址开始读取指定长度数据
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//从指定地址开始读出指定长度的数据
#endif /* __RTC_H end */
/*------------------------------File----------End------------------------------*/
