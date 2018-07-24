/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz,王泉凯
 *修改日期:	2018-03-08					文档移植
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "stmflash.h"
#include "delay.h"
#include "usart.h"
/* 宏定义	--------------------------------------------------------------------*/
#if STM32_FLASH_SIZE	<						256
	#define STM_SECTOR_SIZE					1024 										//字节
#else 
	#define STM_SECTOR_SIZE					2048
#endif
/* 结构体定义	----------------------------------------------------------------*/
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		flash读数据
 * 形式参数:		faddr flash地址
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数移植
 ****************************************************************************/ 
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
#if STM32_FLASH_WREN	//如果使能了写      
/*****************************************************************************
 * 函数功能:		不检查的写入
 * 形式参数:		WriteAddr 起始地址 pBuffer 数据指针 NumToWrite 半字(16位)数
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数移植
 ****************************************************************************/ 
void STMFLASH_Write_NoCheck(uint32_t WriteAddr , uint16_t *pBuffer , uint16_t NumToWrite)   
{
	uint16_t i = 0;											//循环变量
	
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
		WriteAddr+=2;//地址增加2.
	}  
}
/*****************************************************************************
 * 函数功能:		从指定地址开始写入指定长度的数据
 * 形式参数:		WriteAddr 起始地址(此地址必须为2的倍数!!)  pBuffer 数据指针 半字(16位)数(就是要写入的16位数据的个数.)
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数移植
 ****************************************************************************/ 		 
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)	
{
	uint32_t secpos;										//扇区地址
	uint16_t secoff;										//扇区内偏移地址(16位字计算)
	uint16_t secremain; 								//扇区内剩余地址(16位字计算)	   
	uint16_t i = 0;											//循环变量
	uint32_t offaddr;   								//去掉0X08000000后的地址
	uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2] = {0};									//最多是2K字节

	if(( WriteAddr < STM32_FLASH_BASE ) || ( WriteAddr >= (STM32_FLASH_BASE+1024*STM32_FLASH_SIZE )))return;//非法地址
	FLASH_Unlock();						//解锁
	offaddr = WriteAddr - STM32_FLASH_BASE;		//实际偏移地址.
	secpos  = offaddr   / STM_SECTOR_SIZE;				//扇区地址  0~127 for STM32F103RBT6
	secoff  = (offaddr  % STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain = STM_SECTOR_SIZE / 2 - secoff;		//扇区剩余空间大小   
	if( NumToWrite <= secremain )secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
#endif
/*****************************************************************************
 * 函数功能:		从指定地址开始读出指定长度的数据
 * 形式参数:		ReadAddr 起始地址(此地址必须为2的倍数!!)  pBuffer 数据指针 半字(16位)数(就是要写入的16位数据的个数.)
 * 返回参数:		无
 * 更改日期;		2018-03-08				函数移植
 ****************************************************************************/ 
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t tmp;
	uint16_t i = 0;											//循环变量
	
	for(i=0;i<NumToRead;i++)
	{
		tmp = STMFLASH_ReadHalfWord(ReadAddr);
		pBuffer[i]=tmp;//STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}
/*------------------------------File----------End------------------------------*/
