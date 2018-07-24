/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018-03-08					�ĵ���ֲ
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "stmflash.h"
#include "delay.h"
#include "usart.h"
/* �궨��	--------------------------------------------------------------------*/
#if STM32_FLASH_SIZE	<						256
	#define STM_SECTOR_SIZE					1024 										//�ֽ�
#else 
	#define STM_SECTOR_SIZE					2048
#endif
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		flash������
 * ��ʽ����:		faddr flash��ַ
 * ���ز���:		��
 * ��������;		2018-03-08				������ֲ
 ****************************************************************************/ 
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
#if STM32_FLASH_WREN	//���ʹ����д      
/*****************************************************************************
 * ��������:		������д��
 * ��ʽ����:		WriteAddr ��ʼ��ַ pBuffer ����ָ�� NumToWrite ����(16λ)��
 * ���ز���:		��
 * ��������;		2018-03-08				������ֲ
 ****************************************************************************/ 
void STMFLASH_Write_NoCheck(uint32_t WriteAddr , uint16_t *pBuffer , uint16_t NumToWrite)   
{
	uint16_t i = 0;											//ѭ������
	
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
		WriteAddr+=2;//��ַ����2.
	}  
}
/*****************************************************************************
 * ��������:		��ָ����ַ��ʼд��ָ�����ȵ�����
 * ��ʽ����:		WriteAddr ��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)  pBuffer ����ָ�� ����(16λ)��(����Ҫд���16λ���ݵĸ���.)
 * ���ز���:		��
 * ��������;		2018-03-08				������ֲ
 ****************************************************************************/ 		 
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)	
{
	uint32_t secpos;										//������ַ
	uint16_t secoff;										//������ƫ�Ƶ�ַ(16λ�ּ���)
	uint16_t secremain; 								//������ʣ���ַ(16λ�ּ���)	   
	uint16_t i = 0;											//ѭ������
	uint32_t offaddr;   								//ȥ��0X08000000��ĵ�ַ
	uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2] = {0};									//�����2K�ֽ�

	if(( WriteAddr < STM32_FLASH_BASE ) || ( WriteAddr >= (STM32_FLASH_BASE+1024*STM32_FLASH_SIZE )))return;//�Ƿ���ַ
	FLASH_Unlock();						//����
	offaddr = WriteAddr - STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos  = offaddr   / STM_SECTOR_SIZE;				//������ַ  0~127 for STM32F103RBT6
	secoff  = (offaddr  % STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain = STM_SECTOR_SIZE / 2 - secoff;		//����ʣ��ռ��С   
	if( NumToWrite <= secremain )secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	FLASH_Lock();//����
}
#endif
/*****************************************************************************
 * ��������:		��ָ����ַ��ʼ����ָ�����ȵ�����
 * ��ʽ����:		ReadAddr ��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)  pBuffer ����ָ�� ����(16λ)��(����Ҫд���16λ���ݵĸ���.)
 * ���ز���:		��
 * ��������;		2018-03-08				������ֲ
 ****************************************************************************/ 
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t tmp;
	uint16_t i = 0;											//ѭ������
	
	for(i=0;i<NumToRead;i++)
	{
		tmp = STMFLASH_ReadHalfWord(ReadAddr);
		pBuffer[i]=tmp;//STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}
/*------------------------------File----------End------------------------------*/
