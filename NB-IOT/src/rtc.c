/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *项目名称:	CA9523M
 *描    述:	无线中继器(自组网型)
 *设    计:	AJS.Sunwaz,王泉凯
 *修改日期:	2018-03-08					文档移植
 *说    明:	
*******************************************************************************************/
/* 头文件	--------------------------------------------------------------------*/
#include "delay.h"
#include "rtc.h" 
/* 宏定义	--------------------------------------------------------------------*/
/* 结构体定义	----------------------------------------------------------------*/
typedef struct{                   //时间结构体
	RTC_TimeTypeDef   time;         //时间
  RTC_DateTypeDef   data;         //日期
}_calendar_obj;
/* 内部引用	------------------------------------------------------------------*/
/* 全局变量	------------------------------------------------------------------*/
Time_TypeDef_t Time={0};					//系统时间
/* 系统函数	------------------------------------------------------------------*/
/*****************************************************************************
 * 函数功能:		实时时钟配置 初始化RTC时钟
 * 形式参数:		无
 * 返回参数:		0:正常 其他:错误代码
 * 更改日期;		2018-03-08				函数移植
              2018-03-13        函数重新编写
 ****************************************************************************/ 
uint8_t RTC_Config(void)
{
  //检查是不是第一次配置时钟
  uint8_t temp = 0;											   //循环变量
	RTC_InitTypeDef  newRTC_Init;
  
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR , ENABLE );	//使能PWR 外设时钟
	
  PWR_BackupAccessCmd( ENABLE );	      //使能后备寄存器访问
  
  RCC_BackupResetCmd( ENABLE );
  RCC_BackupResetCmd( DISABLE );
  
  RCC_LSICmd( ENABLE );                 //使能LSI时钟
  while(( RCC_GetFlagStatus( RCC_FLAG_LSIRDY ) == RESET ) && ( temp < 0xFF ))	//检查指定的RCC标志位设置与否,等待低速晶振就绪
  {
    temp++;
    delay_ms( 10 );
  }
  if( temp >= 250 )return 1;//初始化时钟失败,晶振有问题
  
  RCC_RTCCLKConfig( RCC_RTCCLKSource_LSI );//链接RTC的时钟
  RCC_RTCCLKCmd(ENABLE);	            //使能RTC时钟  
  RTC_WaitForSynchro();               //等待RTC寄存器同步 
 
  /*1> RTC 配置结构体初始化*/
  newRTC_Init.RTC_HourFormat    = RTC_HourFormat_24;//24H制
  newRTC_Init.RTC_SynchPrediv   = 0x190 - 1;				//计数值
  newRTC_Init.RTC_AsynchPrediv  = 0x64   - 1;       //分频值
  RTC_Init( &newRTC_Init );
 
  /*2> RTC 时钟设置*/
  return RTC_Set( 2017 , 1 , 1 , 1 , 1 , 1 );//ok
}

/*****************************************************************************
 * 函数功能:		获得现在是星期几
 * 形式参数:		year 年 month 月 day 日
 * 返回参数:		星期号	
 * 注意事项;		输入公历日期得到星期(只允许2000-2099年)
 * 更改日期;		2018-03-08				函数移植
 ****************************************************************************/ 
static uint8_t RTC_Get_Week(uint16_t year , uint8_t month , uint8_t day)
{	
	uint16_t temp2  = 0;								//中间变量
	uint8_t yearH   = 0;								//年的最高位
	uint8_t yearL   = 0;								//年的最低位
	uint8_t table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 	//月修正数据表
	
	yearH = year  / 100;
	yearL = year  % 100; 
	// 如果为21世纪,年份数加100  
	if (yearH > 19)yearL  +=  100;
	// 所过闰年数只算1900年之后的  
	temp2 = yearL + yearL /  4;
	temp2 = temp2 % 7; 
	temp2 = temp2 + day +  table_week[  month - 1 ];
	if(( yearL % 4 ==  0 ) && ( month < 3 ))temp2--;
	return( temp2 % 7 );
}	
/*****************************************************************************
 * 函数功能:		设置时钟
 * 形式参数:		syear 年份 smon 月 sday 日 hour 时 min 分 sec 秒
 * 返回参数:		0,成功;其他:错误代码.
 * 注意事项;		把输入的时钟转换为秒钟
								以1970年1月1日为基准
								1970~2099年为合法年份
 * 更改日期;		2018-03-08					函数移植
              2018-03-13					函数重新编写
 ****************************************************************************/ 
uint8_t RTC_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec)
{
	RTC_TimeTypeDef   newTime_Init;
  RTC_DateTypeDef   newData_Init;
    
	if(( syear < 2000 ) || ( syear > 2099 ))
		return 1;
  /*1> 时间的设置*/
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
  
  /*2> 日期的设置*/
  newData_Init.RTC_Date     = sday;
  newData_Init.RTC_Month    = smon;
  newData_Init.RTC_Year     = syear -  2000;
  newData_Init.RTC_WeekDay  = RTC_Get_Week( syear , smon , sday );
  RTC_SetDate( RTC_Format_BIN , &newData_Init );

  /*3> 获取当前时间*/
	RTC_Get();
	return 0;	    
}
/*****************************************************************************
 * 函数功能:		得到当前的时间
 * 形式参数:		无
 * 返回参数:		返回值:0,成功;其他:错误代码.
 * 更改日期;		2018-03-08				函数移植
              2018-03-13        函数重新编写
 ****************************************************************************/ 
uint8_t RTC_Get(void)
{
	_calendar_obj calendar;																		//时钟结构体 

  /*1> 从RTC中获取时间*/
  RTC_GetTime( RTC_Format_BIN , &calendar.time );
  RTC_GetDate( RTC_Format_BIN , &calendar.data );
  
  /*2> 赋值*/
	Time.year 	= calendar.data.RTC_Year;
  Time.mon    = calendar.data.RTC_Month;
	Time.day    = calendar.data.RTC_Date;
  Time.hour   = calendar.time.RTC_Hours;
  Time.min    = calendar.time.RTC_Minutes;
  Time.sec    = calendar.time.RTC_Seconds;
	return 0;
}		  
/*------------------------------File----------End------------------------------*/
