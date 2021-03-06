/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	智慧用电
 *设    计:	AJS.Sunwaz
 *修改日期:	2018-06-20					文档移植
 *说    明:	
*******************************************************************************************/
#ifndef __me3616_H
#define __me3616_H
/* 头文件 ------------------------------------------------------------------*/
#include <stm32f0xx.h>
#include "stdio.h"
/* 宏定义 ------------------------------------------------------------------*/
/* 结构体声明	--------------------------------------------------------------*/
/* 枚举声明	----------------------------------------------------------------*/
/* 全局变量	----------------------------------------------------------------*/
/* 接口提供	----------------------------------------------------------------*/
void ME3616_CSQ(uint8_t *buf);
void ME3616_ReadSignal(uint8_t *buf);
void ME3616_ReadPCI(uint8_t *buf);
void ME3616_ReadCellID(uint8_t *buf);
void ME3616_ReadSNR(uint8_t *buf);
ErrorStatus ME3616_init( uint8_t* cnt , uint8_t *flag);
ErrorStatus ME3616_SendData(uint8_t *p , uint16_t len);
void ME3616_ReceiveData( uint8_t* cnt);
void ME3616_Reset(void);
#endif       /*__me3616_H end*/
/*------------------------------File----------End------------------------------*/
