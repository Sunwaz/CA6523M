/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018-03-08					�ĵ���ֲ
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "delay.h"
#include "rtc.h" 
/* �궨��	--------------------------------------------------------------------*/
/* �ṹ�嶨��	----------------------------------------------------------------*/
typedef struct{                   //ʱ��ṹ��
	RTC_TimeTypeDef   time;         //ʱ��
  RTC_DateTypeDef   data;         //����
}_calendar_obj;
/* �ڲ�����	------------------------------------------------------------------*/
/* ȫ�ֱ���	------------------------------------------------------------------*/
Time_TypeDef_t Time={0};					//ϵͳʱ��
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		ʵʱʱ������ ��ʼ��RTCʱ��
 * ��ʽ����:		��
 * ���ز���:		0:���� ����:�������
 * ��������;		2018-03-08				������ֲ
              2018-03-13        �������±�д
 ****************************************************************************/ 
uint8_t RTC_Config(void)
{
  //����ǲ��ǵ�һ������ʱ��
  uint8_t temp = 0;											   //ѭ������
	RTC_InitTypeDef  newRTC_Init;
  
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR , ENABLE );	//ʹ��PWR ����ʱ��
	
  PWR_BackupAccessCmd( ENABLE );	      //ʹ�ܺ󱸼Ĵ�������
  
  RCC_BackupResetCmd( ENABLE );
  RCC_BackupResetCmd( DISABLE );
  
  RCC_LSICmd( ENABLE );                 //ʹ��LSIʱ��
  while(( RCC_GetFlagStatus( RCC_FLAG_LSIRDY ) == RESET ) && ( temp < 0xFF ))	//���ָ����RCC��־λ�������,�ȴ����پ������
  {
    temp++;
    delay_ms( 10 );
  }
  if( temp >= 250 )return 1;//��ʼ��ʱ��ʧ��,����������
  
  RCC_RTCCLKConfig( RCC_RTCCLKSource_LSI );//����RTC��ʱ��
  RCC_RTCCLKCmd(ENABLE);	            //ʹ��RTCʱ��  
  RTC_WaitForSynchro();               //�ȴ�RTC�Ĵ���ͬ�� 
 
  /*1> RTC ���ýṹ���ʼ��*/
  newRTC_Init.RTC_HourFormat    = RTC_HourFormat_24;//24H��
  newRTC_Init.RTC_SynchPrediv   = 0x190 - 1;				//����ֵ
  newRTC_Init.RTC_AsynchPrediv  = 0x64   - 1;       //��Ƶֵ
  RTC_Init( &newRTC_Init );
 
  /*2> RTC ʱ������*/
  return RTC_Set( 2017 , 1 , 1 , 1 , 1 , 1 );//ok
}

/*****************************************************************************
 * ��������:		������������ڼ�
 * ��ʽ����:		year �� month �� day ��
 * ���ز���:		���ں�	
 * ע������;		���빫�����ڵõ�����(ֻ����2000-2099��)
 * ��������;		2018-03-08				������ֲ
 ****************************************************************************/ 
static uint8_t RTC_Get_Week(uint16_t year , uint8_t month , uint8_t day)
{	
	uint16_t temp2  = 0;								//�м����
	uint8_t yearH   = 0;								//������λ
	uint8_t yearL   = 0;								//������λ
	uint8_t table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 	//���������ݱ�
	
	yearH = year  / 100;
	yearL = year  % 100; 
	// ���Ϊ21����,�������100  
	if (yearH > 19)yearL  +=  100;
	// ����������ֻ��1900��֮���  
	temp2 = yearL + yearL /  4;
	temp2 = temp2 % 7; 
	temp2 = temp2 + day +  table_week[  month - 1 ];
	if(( yearL % 4 ==  0 ) && ( month < 3 ))temp2--;
	return( temp2 % 7 );
}	
/*****************************************************************************
 * ��������:		����ʱ��
 * ��ʽ����:		syear ��� smon �� sday �� hour ʱ min �� sec ��
 * ���ز���:		0,�ɹ�;����:�������.
 * ע������;		�������ʱ��ת��Ϊ����
								��1970��1��1��Ϊ��׼
								1970~2099��Ϊ�Ϸ����
 * ��������;		2018-03-08					������ֲ
              2018-03-13					�������±�д
 ****************************************************************************/ 
uint8_t RTC_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec)
{
	RTC_TimeTypeDef   newTime_Init;
  RTC_DateTypeDef   newData_Init;
    
	if(( syear < 2000 ) || ( syear > 2099 ))
		return 1;
  /*1> ʱ�������*/
  if(hour <= 12)
  {
    newTime_Init.RTC_H12    = RTC_H12_AM;
  }else
  {
    newTime_Init.RTC_H12    = RTC_H12_PM;
  }
  newTime_Init.RTC_Hours    = hour;
  newTime_Init.RTC_Minutes  = min;
  newTime_Init.RTC_Seconds  = sec;
  RTC_SetTime( RTC_Format_BIN , &newTime_Init );
  
  /*2> ���ڵ�����*/
  newData_Init.RTC_Date     = sday;
  newData_Init.RTC_Month    = smon;
  newData_Init.RTC_Year     = syear -  2000;
  newData_Init.RTC_WeekDay  = RTC_Get_Week( syear , smon , sday );
  RTC_SetDate( RTC_Format_BIN , &newData_Init );

  /*3> ��ȡ��ǰʱ��*/
	RTC_Get();
	return 0;	    
}
/*****************************************************************************
 * ��������:		�õ���ǰ��ʱ��
 * ��ʽ����:		��
 * ���ز���:		����ֵ:0,�ɹ�;����:�������.
 * ��������;		2018-03-08				������ֲ
              2018-03-13        �������±�д
 ****************************************************************************/ 
uint8_t RTC_Get(void)
{
	_calendar_obj calendar;																		//ʱ�ӽṹ�� 

  /*1> ��RTC�л�ȡʱ��*/
  RTC_GetTime( RTC_Format_BIN , &calendar.time );
  RTC_GetDate( RTC_Format_BIN , &calendar.data );
  
  /*2> ��ֵ*/
	Time.year 	= calendar.data.RTC_Year;
  Time.mon    = calendar.data.RTC_Month;
	Time.day    = calendar.data.RTC_Date;
  Time.hour   = calendar.time.RTC_Hours;
  Time.min    = calendar.time.RTC_Minutes;
  Time.sec    = calendar.time.RTC_Seconds;
	return 0;
}		  
/*------------------------------File----------End------------------------------*/
