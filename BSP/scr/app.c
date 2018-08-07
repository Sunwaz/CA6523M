/************************* Copyright (c) 2018   AJS.Sunwaz  *****************************
 *��Ŀ����:	CA9523M
 *��    ��:	�����м���(��������)
 *��    ��:	AJS.Sunwaz,��Ȫ��
 *�޸�����:	2018-03-08					�ĵ���ֲ
 *˵    ��:	
*******************************************************************************************/
/* ͷ�ļ�	--------------------------------------------------------------------*/
#include "app.h"
#include "transmission.h"
#include "usart.h"
#include "adc_get.h"
#include "configure.h"
#include "delay.h"
#include "stmflash.h"
#include "stdlib.h"
#include "bc95.h"
#include "nbiot.h"
#include "key_led.h"
#include "timer.h"
/* �궨��	--------------------------------------------------------------------*/
#define delay_nms											delay_ms
#define APP_FUN_CNT                   50
/* �ṹ�嶨��	----------------------------------------------------------------*/
/* �ڲ�����	------------------------------------------------------------------*/
void Netconfig_Upload( int8_t* pDr );								//�豸������Ϣ�ϴ�
void up_startupmessage( uint8_t*  msg_buff);				//�ϴ�������Ϣ
void runing_state_upload(uint8_t* runing_buff);			//�ϴ�����״̬
void up_per_info(uint8_t* up_data );								//�ϴ�������Ϣ
void app_run_queue(void);														//����ִ��
/* ȫ�ֱ���	------------------------------------------------------------------*/
void (*app_fun[APP_FUN_CNT])(void);									//APP�������
uint8_t g_app_fun_cnt = 0;													//APP������λ��
uint8_t g_app_run_cnt = 0;													//APP��������λ��
/* ϵͳ����	------------------------------------------------------------------*/
/*****************************************************************************
 * ��������:		����           �����ϴ� �� ����һ���ϴ���������
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-07				������ֲ
							2018-04-10				��ʱʱ��ļ���
							2018-05-10				����������ƶ�,���������ƶ�������  slave_dataupdata  ��
																��ӹ���;����״̬��������ʱ���ϴ����ݵ�ƽ̨
 ****************************************************************************/
void Heart_task(void)
{
	uint16_t       temp    = 0;		//�м����
	
	if(1){//ģ�����ݲɼ�
		temp = (g_sys_tim_s & 0xFFFF) % g_sys_param.hart_time ;
		if((temp == 0) || (g_alarm_flag & READ_ALARM) || (g_alarm_flag & ALARM_RECOVERY))//����ʱ�䵽      �б�������   ��������(����Ҫ�ϴ�����)
		{
			g_alarm_flag &=~ (ALARM_RECOVERY | ALARM_RESET);//�����־λ
			sys_app(type_send_anlog);//̽����ʵʱ���ݸ����ϴ�
		}
	}
	if(2){//����״̬
		temp = (g_sys_tim_s & 0xFFFF) % 7200;//2H,�������ʧ��,��ʱ������
		if(!temp)
		{
			sys_app(type_runing);
		}
	}
	if(3){//���ر���
		if( g_alarm_flag & READ_ALARM )//���ر���
		{
			for(temp = 1;temp < 0x400;temp <<= 1)
			{
				if(g_alarm_flag & temp)
				{
					g_alarm_flag &=~ temp;//�����־λ
					g_alarm_flag |= (temp << ALARM_UPLOAD);//��λ�ϴ���־λ
					if(!(g_alarm_flag & (temp << ALARM_SILENCE)))Alarm_Set();//���������,�򲻲�������
				}
			}
			LED_Control( L_CLEAR , CLOS );//�ر�������
		}else if(!g_alarm_flag)
		{
			Alarm_Reset();
		}
	}
	if(4){//Ӧ�ö��� Ӧ��ִ��
		temp = (g_sys_tim_s & 0xFFFF) % 6;//2s,�������ʧ��,��ʱ������
		if((!temp) && (!Build_Net_Data_Flag))
		{
			app_run_queue();
		}
	}
}

/*****************************************************************************
 * ��������:		�����ط�
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-07-18				������ֲ
 ****************************************************************************/ 
void CRT_ReSend(void)
{	
	static uint8_t s_cnt = 0;
	if(s_cnt++ < 2)return;
	s_cnt = 0;
	if(g_ack_flag)
	{
		if(++g_re_cnt == 10)
		{
			g_monitor_flag = 0;
			CRT_FunMove();
		}else
		{
			crt_fun[g_crt_run_cnt]();//��������
		}
	}
}
/*****************************************************************************
 * ��������:	Ӧ�ý��̺���
 * ��ʽ����:	����ָ��
 * ���ز���:	��
 * �޸�����:	2018-08-02					�ĵ���ֲ
 ****************************************************************************/
void APP_FunAdd( void fun(void) )
{
	uint8_t i = g_app_run_cnt;
	
	while(i != g_app_fun_cnt)
	{
		if(app_fun[i] == fun)return;
		if(++i == APP_FUN_CNT)i = 0;
	}
	app_fun[g_app_fun_cnt] = fun;
	if(++g_app_fun_cnt == APP_FUN_CNT)g_app_fun_cnt = 0;
	if(g_app_fun_cnt   == g_app_run_cnt)
	{
		if(++g_app_run_cnt ==  APP_FUN_CNT)g_app_run_cnt = 0;
	}
}

/*****************************************************************************
 * ��������:	��������
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-08-02					�ĵ���ֲ
 ****************************************************************************/
void APP_SendData( void )
{
	static uint8_t s_cnt	= 0;				//��ѯ����
	
	CRT_ReSend();
	Heart_task();								//����
	net_to_module();            //��������
	if(radio.rece != NULL)radio.rece(&s_cnt);//��������	
}
/*****************************************************************************
 * ��������:	�豸У׼
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-08-02					�ĵ���ֲ
 ****************************************************************************/
void APP_GetADCFun( void )
{
	ADC_Collection(0xFF);         //���ݲɼ�
}
/*****************************************************************************
 * ��������:	������ϢӦ��
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-08-02					�ĵ���ֲ
 ****************************************************************************/
void APP_ConfigFun( void )
{
	sys_app(type_upload_startup);
}
/*****************************************************************************
 * ��������:	�������Ӧ��
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-08-02					�ĵ���ֲ
 ****************************************************************************/
void APP_MonitorFun( void )
{
	USART_CRT_FunAdd(USART_SendSenserSta);
	USART_CRT_FunAdd(USART_SendMonitorData);
}
/*****************************************************************************
 * ��������:	������Ϣ�ϴ�Ӧ��
 * ��ʽ����:	��
 * ���ز���:	��
 * �޸�����:	2018-08-02					�ĵ���ֲ
 ****************************************************************************/
void APP_OperationFun( void )
{
	sys_app(type_oper_info);
}

/*****************************************************************************
 * ��������:		����Ӧ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-03-07				������ֲ
							2018-06-01				�޸�bug;ͬһ��ʱ�������װ����,��������
							2018-06-11				�����Ż�;�������±�д
							2018-07-24				�����Ż�;����ģ��ĳ�ʼ�����̺���ǰ�ĸ�λ���̺ϲ�,�����˷��Ͳ�����Ϣ�����ȼ�
							2018-08-02				�����Ż�;Ӧ�ý��̵�����Ż�,ʹ�ú���ָ���������ʽ���,ȥ����ǰ��switch��ʽ
 ****************************************************************************/ 
void app_net(void)
{
	static uint8_t s_old_time = 0;				//��ʱ��
	static uint8_t s_up_flag = 0;
	
	if((g_sys_tim_s & 0xFF) != s_old_time){ //��ʱ��Ӧ��,1sִ��1��
		APP_FunAdd(APP_SendData);
		if(g_crt_run_cnt != g_crt_fun_cnt){	  //����Ӧ������
		if((!g_ack_flag) && (crt_fun[g_crt_run_cnt] != NULL)){
			g_ack_flag = 1;
			crt_fun[g_crt_run_cnt]();}}//��������
		s_old_time = g_sys_tim_s & 0xFF;}
	if(g_cail_data.flag){                   //�豸У׼Ӧ��,���յ�1��У׼����ִ��һ��
		APP_FunAdd(ADC_Cail);
		g_cail_data.flag = 0;}
	if(g_adc_get_flag && (g_wait_flag != 1)){//��ȡ����Ӧ��,ִ��һ�����һ��
			APP_FunAdd(APP_GetADCFun);}
	if(g_sys_param.updat_flag){              //�����豸Ӧ��,���յ�һ��ִ��һ��
		APP_FunAdd(network_parameterUpdata);
		g_sys_param.updat_flag = 0;}//���±�־���
	if(g_up_config_flag){										 //�ϴ�������ϢӦ��,���յ�һ��ִ��һ��
		APP_FunAdd(APP_ConfigFun);
		g_up_config_flag = 0;}
	if(g_monitor_flag){											 //�ϴ��������Ӧ��,����һ��ִ��һ��
		APP_FunAdd(APP_MonitorFun);}
	if(!g_model_config_flag){								 //ģ�鸴λӦ��,����һ��ִ��һ��
		APP_FunAdd(Nbiot_reset);}
	if(g_sys_operation_msg && !s_up_flag){   //�ϴ�������ϢӦ��,����һ��ִ��һ��
		APP_FunAdd(APP_OperationFun);
		s_up_flag = 1;
	}else s_up_flag = 0;
	
	if(app_fun[g_app_run_cnt] != NULL){
		app_fun[g_app_run_cnt]();//��������
		app_fun[g_app_run_cnt] = NULL;
		if(++g_app_run_cnt == APP_FUN_CNT)g_app_run_cnt = 0;
	}else{
	if(++g_app_run_cnt == APP_FUN_CNT)g_app_run_cnt = 0;
	}
}
/*****************************************************************************
 * ��������:ʱ�����ݵĿ���
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-06-05				������ֲ
 ****************************************************************************/
void data_time_copy( void )
{
	uint8_t i = 0;
	uint8_t len = sizeof(Time_TypeDef_t)/sizeof(uint8_t);
	uint8_t* p = (uint8_t*)&Time;
	
	for(i = 0;i < len;i++)
	{
		g_nb_net_buff[g_nb_net_buf_end] = *p;
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		p++;
	}
}
/*****************************************************************************
 * ��������:		����ʱ�Ĳ���
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-05-29				������д
							2018-06-05				�޸�bug;���ݵ�ͷ��ʱ��,���ִ���,����ƽ̨�޷�����
																�޸�bug;��������ռ����ʱ��,���ִ���,�����Ƿ��ڴ�
 ****************************************************************************/
void data_indexcheck( uint16_t index  , uint16_t *old_index )
{
	uint8_t flag = 0;					//�����־             1;��������ռ��
	
	if(g_nb_net_buf_end >= g_nb_net_buf_start)
	{
		if((QUEUE_DAT_SIZE-(g_nb_net_buf_end - g_nb_net_buf_start)) < index)flag = 1;//����
	}else
	{
		if((g_nb_net_buf_start - g_nb_net_buf_end) < index)flag = 1;//����
	}
	if(flag)
	{
		g_net_app_queue[g_queue_idex_s].type = type_null_dat;
		data_index_move();//ɾ������ռ�õ��Ǹ�Ӧ�ö��еĿռ�
	}
	*old_index	= g_nb_net_buf_end;//��ʼλ��
}
/*****************************************************************************
 * ��������:		ģ���������
 * ��ʽ����:		��
 * ���ز���:		���ݵ���ʼλ��
 * ��������;		2018-05-28				������д
							2018-05-29				���������Ϊ;��������-������ַ(һλ)-��������-����(��λ)-ʱ��
							2018-06-05				�޸�bug;������buff�����ͷ
 ****************************************************************************/
uint16_t data_add_anlog( void )
{
	uint8_t i = 0;										//ѭ������
	uint8_t max = sizeof(data_typedef)/sizeof(uint16_t);
	uint16_t old_index  = 0;//�ϵ�λ��
	
	old_index = 0x05 * max + 6;																								//ÿ����Ϣ��5������ ����һ��ʱ��
	data_indexcheck( old_index , &old_index);
	for( i= 0;i < max;i++)                                                   //�˴�,buff�����ֵ��1K,��0-0x3FFF,�ʴ˴�Ҫ����һ��0x3FFF
	{
		g_nb_net_buff[g_nb_net_buf_end] =  g_coll_data[i].part_type;								//��������
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		g_nb_net_buff[g_nb_net_buf_end] = (g_coll_data[i].part_addr >> 0)  & 0xFF;  //������ַ
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		g_nb_net_buff[g_nb_net_buf_end] =  g_coll_data[i].data_type;								//����
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		g_nb_net_buff[g_nb_net_buf_end] = (g_coll_data[i].data_value >> 0) & 0xFF;  //����
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
		g_nb_net_buff[g_nb_net_buf_end] = (g_coll_data[i].data_value >> 8) & 0xFF;
		g_nb_net_buf_end = (g_nb_net_buf_end+1) & QUEUE_DAT_MAX;
	}
	data_time_copy();//ʱ��Ŀ���
	return old_index;//������ʼλ��
}

/*****************************************************************************
 * ��������:		�������ݵ����
 * ��ʽ����:		��
 * ���ز���:		���ݵ���ʼλ��
 * ��������;		2018-05-29				������д
 ****************************************************************************/
uint16_t data_add_config( void )
{
	uint16_t old_index  = 0;//�ϵ�λ��
	uint16_t dat_len    = 0;//���ݳ���
	
	dat_len = 60;																												//6�ֽڵ�IP+6�ֽڵ�ʱ��+2�ֽ�����ʱ��+2�ֽڲ���ʱ��
	data_indexcheck( dat_len , &old_index );								//�������ݳ��� ,��ȡ��ʼλ�úͱ���λ��
	Netconfig_Upload((int8_t*)g_nb_net_buff);      				//�������
	data_time_copy();//ʱ��Ŀ���
	return old_index;                                                   //������ʼλ��
}
/*****************************************************************************
 * ��������:		������Ϣ���ݵ����
 * ��ʽ����:		��
 * ���ز���:		���ݵ���ʼλ��
 * ��������;		2018-05-29				������д
 ****************************************************************************/
uint16_t data_add_startup( void )
{
	uint16_t old_index  = 0;//�ϵ�λ��
	uint16_t dat_len    = 0;//���ݳ���
	
	dat_len = strlen((char*)g_sys_param.IMEI) + strlen(SYS_VER) + strlen(MODLE_VER) + 18;	//(һ���ֽڵ�����+һ���ֽڵĳ���)*3+(�����ֽڵ���Ӫ��+�����ֽڵ�ģ������)+�����ֽڵ�ʱ��
	data_indexcheck( dat_len , &old_index);             	//�������ݳ��� ,��ȡ��ʼλ�úͱ���λ��
	up_startupmessage(g_nb_net_buff);                    	//������� 
	data_time_copy();//ʱ��Ŀ���
	return old_index;                                                   //������ʼλ��
}

/*****************************************************************************
 * ��������:		������Ϣ���ݵ����
 * ��ʽ����:		��
 * ���ز���:		���ݵ���ʼλ��
 * ��������;		2018-05-29				������д
 ****************************************************************************/
uint16_t data_add_running( void )
{
	uint16_t old_index  = 0;//�ϵ�λ��
	uint16_t dat_len    = 0;//���ݳ���
	
	dat_len = 55;	                                                      //��󳤶�//�ĸ��ֽڵ��ź�ǿ��+�����ֽڵĻ�վ���+�Ÿ��ֽڵ�С�����+�ĸ��ֽ������+�����ֽ��ź�ǿ��+ʮ����ֽڵ�IMSI+�����ֽ�ʱ��
	data_indexcheck( dat_len , &old_index );								//�������ݳ��� ,��ȡ��ʼλ�úͱ���λ��
	runing_state_upload(g_nb_net_buff );					//�������,��ȡʵ�ʵ����ݳ��� 
	data_time_copy();//ʱ��Ŀ���
	return old_index;                                                   //������ʼλ��
}

/*****************************************************************************
 * ��������:		������Ϣ���ݵ����
 * ��ʽ����:		��
 * ���ز���:		���ݵ���ʼλ��
 * ��������;		2018-05-29				������д
 ****************************************************************************/
uint16_t data_add_opera( void )
{
	uint16_t old_index  = 0;//�ϵ�λ��
	uint16_t dat_len    = 0;//���ݳ���
	
	dat_len = 10;	                                                      //һ���ֽ�����+һ���ֽڵ�ַ+�����ֽ�״̬+�����ֽ�ʱ��
	data_indexcheck( dat_len , &old_index );								//�������ݳ��� ,��ȡ��ʼλ�úͱ���λ��
	up_per_info(g_nb_net_buff);
  data_time_copy();//ʱ��Ŀ���
	return old_index;                                                   //������ʼλ��
}
/*****************************************************************************
 * ��������:		�������
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-05-16				������ֲ
							2018-05-22				�޸�bug;�������Ӷ��е�ʱ��,��λ�ô�������
							2018-05-28				�޸�bug;ͬһ������,��η���
																��������;�ڶ�����������ʱ������
 ****************************************************************************/
void net_app_add( app_type type)
{
	uint8_t cnt          = 0;				//ѭ������			���ﵽ���ֵ��ʱ����˵������Ӧ������ռ����ǿ���˳�
	uint16_t start_index = 0;				//���ݴ��λ��
	
	while((g_net_app_queue[g_queue_idex_e].type != type_null_dat) && (cnt <= QUEUE_SIZE))
	{
		cnt++;
		if(++g_queue_idex_e >= QUEUE_SIZE)//����Ϊֹ��ͷ
			g_queue_idex_e = 0;
		if(g_queue_idex_s == g_queue_idex_e )//������������,����ʼλ�ú���
		{
			data_index_move();
		}
	}
	switch(type)
	{
		case type_send_anlog:{    //����ģ����
			start_index = data_add_anlog();
			break;}
		case type_upload_config:{ //�ϴ���������
			start_index = data_add_config();
			break;}
		case type_upload_startup:{//�ϴ�������Ϣ
			start_index = data_add_startup();
			break;}
		case type_runing:{        //�ϴ�������Ϣ
			start_index = data_add_running();
			break;}
		case type_oper_info:{     //�ϴ�������Ϣ
			start_index = data_add_opera();
			break;}
		default:return;
	}
	g_net_app_queue[g_queue_idex_e].type = type;
	g_net_app_queue[g_queue_idex_e].idex = start_index;
	if(++g_queue_idex_e >= QUEUE_SIZE)
	{
		g_queue_idex_e = 0;
	}
	if(g_queue_idex_e == g_queue_idex_s)//���ݱ�����
	{
		data_index_move();
	}
}
/*****************************************************************************
 * ��������:		����ִ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-05-16				������ֲ
							2018-05-22				����bug;�����б�����,���޷����ж���������غ�����ִ��
																����û��ɾ������,���·���ʧ�ܻ᲻�ϵ�ռ����������
							2018-05-29				��������;��Ӧ�ö��и��µ�ʱ��,���ݻ������ʼλ��ҲҪ�ı�
							2018-07-24				�����Ż�;��ģ��û�б���ʼ����ʱ��,����������appӦ��
 ****************************************************************************/
void app_run_queue(void)
{
	if(!g_model_config_flag)return;
	if(g_queue_idex_s != g_queue_idex_e)
	{
		sys_app(type_null_dat);//�������гɹ�,��ɾ�������ε�����
	}
}

/*****************************************************************************
 * ��������:		�豸������Ϣ�ϴ�
 * ��ʽ����:		pDr ���� len ����ָ��
 * ���ز���:		error �ڴ治�� success ������װ�ɹ�
 * ��������;		2018-04-09				������ֲ
							2018-05-29				ɾ������ len
							2018-06-05				�޸�bug;������buff�����ͷ
 ****************************************************************************/
void Netconfig_Upload( int8_t* pDr )
{	
	if(1){//1>������IP��ַ//8
  pDr[g_nb_net_buf_end]	= config_ip;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x06;//���ݳ���
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]  = g_sys_param.server_ip[3];
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
  pDr[g_nb_net_buf_end]  = g_sys_param.server_ip[2];
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
  pDr[g_nb_net_buf_end]  = g_sys_param.server_ip[1];
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
  pDr[g_nb_net_buf_end]  = g_sys_param.server_ip[0];		
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.port &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.port >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(2){//2>��������ʱ��//8+4=12
	pDr[g_nb_net_buf_end]	= config_heart;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.hart_time &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.hart_time >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(3){//3>��������������//12+6=18
	pDr[g_nb_net_buf_end]	= config_curr_r;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x04;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= CURR_RANGE_IN  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= CURR_RANGE_IN  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= CURR_RANGE_OUT &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= CURR_RANGE_OUT >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(4){//4>ʣ���������������//18+6=24
	pDr[g_nb_net_buf_end]	= config_sy_curr_r;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x04;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= SY_CURR_RANGE_IN  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= SY_CURR_RANGE_IN  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= SY_CURR_RANGE_OUT &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= SY_CURR_RANGE_OUT >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(5){//5>�¶ȴ���������//24+6=30
	pDr[g_nb_net_buf_end]	= config_temp_r;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x04;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= TEMP_MIN  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= TEMP_MIN  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= TEMP_MAX  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= TEMP_MAX  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(6){//6>���ݲɼ�����//30+4=34
	pDr[g_nb_net_buf_end]	= config_camp;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.camp_time  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.camp_time  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(7){//7>����������ֵ//34+4=38
	pDr[g_nb_net_buf_end]	= config_curr_t;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.curr1  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end] = g_sys_param.threa.curr1  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(8){//8>©����������ֵ//38+4=42
	pDr[g_nb_net_buf_end]	= config_sy_curr_t;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.sy_curr  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end] = g_sys_param.threa.sy_curr  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(9){//9>�¶Ȳ�����ֵ//42+4=46
	pDr[g_nb_net_buf_end]	= config_temp_t;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.temp1 &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.temp1  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
	if(10){//10>��ѹ������ֵ//46+4=50
	pDr[g_nb_net_buf_end]	= config_volat_t;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.volat  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= g_sys_param.threa.volat  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}	
  /*11>������ʪ�Ȳ�����ֵ*/
//	if(1){
//	if(++g_nb_net_buf_end > len)return ERROR;
//	index = 0;
//	pDr[g_nb_net_buf_end].config_type   = config_temp_humi_t;
//	pDr[g_nb_net_buf_end].leng				  = 0x02;
//	pDr[g_nb_net_buf_end].data[index]		= g_sys_param.threa.hj_temp  &  0xFF;
//	pDr[g_nb_net_buf_end].data[++index]	= g_sys_param.threa.hj_temp  >> 0x08;
//	pDr[g_nb_net_buf_end].data[++index]	= g_sys_param.threa.hj_humi  &  0xFF;
//	pDr[g_nb_net_buf_end].data[++index]	= g_sys_param.threa.hj_humi  >> 0x08;}
	if(12){//12>��ѹ����������//50+4=54
	pDr[g_nb_net_buf_end]	= config_volat_r;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= 0x02;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= VOLAT_MAX  &  0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;
	pDr[g_nb_net_buf_end]	= VOLAT_MAX  >> 0x08;
	g_nb_net_buf_end = (g_nb_net_buf_end+1)&QUEUE_DAT_MAX;}
}
/*****************************************************************************
 * ��������:		�ϴ�������Ϣ
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-05-07				������д
							2018-05-29				���ڲ����ı�,���ϴ�������Ϣ�����˴���
 ****************************************************************************/
void up_configmessage( void )
{
	sys_app( type_upload_config );     //�ϴ�һ��������Ϣ
}
/*****************************************************************************
 * ��������:		�����ݱ��浽buff����
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-06-27				������д
 ****************************************************************************/
void save_data_to_buff(uint8_t *save_buff ,uint8_t* buff,uint8_t len , uint16_t len_index)
{
	uint16_t i        = 0;							//ѭ������

	for(i = 0;i < len;i++)
	{
		save_buff[g_nb_net_buf_end] = buff[i];
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	}
	save_buff[len_index] = len;
}
/*****************************************************************************
 * ��������:		�ϴ�����״̬
 * ��ʽ����:		��
 * ���ز���;		SUCCESS ���ź�  ERROR ���ź�
 * ��������;		2018-05-15				������ֲ
							2018-05-28				�޸�bug;�ϴ�IMSI���ݴ���,�����ݽ�βδ��'\0'
							2018-05-30				����˱��� runing_buff
							2018-06-05				�޸�bug;������buff�����ͷ
							2018-06-20				�����Ż�;ͨ������ָ��,��ǿ�˺����Ŀ���չ��
 ****************************************************************************/
void runing_state_upload(uint8_t* runing_buff)
{
	uint16_t i        = 0;							//ѭ������
	uint16_t index    = 0;							//���ݵ�λ��
	uint16_t len      = 0;							//���ݳ���
	uint8_t buff[100] = {0};
		
	if(1){//�źŹ���
		runing_buff[g_nb_net_buf_end] = runing_signal_power;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_signal(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(2){//��վ���
		runing_buff[g_nb_net_buf_end] = runing_pci;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_pci(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(3){//С�����
		runing_buff[g_nb_net_buf_end] = runing_cell_id;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_cellid(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(4){//�����
		runing_buff[g_nb_net_buf_end] = runing_snr;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_snr(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(5){//�ź�ǿ��
		runing_buff[g_nb_net_buf_end] = runing_csq;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		index = g_nb_net_buf_end;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		radio.read_csq(buff);
		save_data_to_buff(runing_buff , buff , strlen((char*)buff) , index);
	}
	if(6){//��ѯIMSI
		runing_buff[g_nb_net_buf_end] = runing_IMSI;//����15���ֽ�32+15+2=49
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		runing_buff[g_nb_net_buf_end] = strlen((char*)g_sys_param.IMSI);
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		len = strlen((char*)g_sys_param.IMSI);
		for(i = 0;i < len;i++)
		{
			runing_buff[g_nb_net_buf_end] = g_sys_param.IMSI[i];
			g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		}
	}
}
/*****************************************************************************
 * ��������:		�ϴ�������Ϣ
 * ��ʽ����:		msg_buff ������Ϣ����
 * ���ز���:		��
 * ��������;		2018-05-11				������д
							2018-05-22				������Ϣ����ɾ�� <IMSI�� ���ϴ�>
							2018-05-29				ɾ���� ������װ ����
																������ msg_buff ���β�
							2018-06-05				�޸�bug;������buff�����ͷ
 ****************************************************************************/
void up_startupmessage( uint8_t*  msg_buff)
{
	uint8_t len		= 0;				//���ݳ���
	uint8_t j     = 0;				//ѭ������
	
	if(1){//��ȡ IEMI ��
		msg_buff[g_nb_net_buf_end]	= startup_IEMI;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		len           = strlen((char*)g_sys_param.IMEI);
		msg_buff[g_nb_net_buf_end] = len;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		for(j = 0;j < len;j++)
		{
			msg_buff[g_nb_net_buf_end] = g_sys_param.IMEI[j];
			g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		}
	}
	if(2){//��ȡ ��Ӫ��
		msg_buff[g_nb_net_buf_end]	= startup_operator;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		msg_buff[g_nb_net_buf_end]	= 1;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		msg_buff[g_nb_net_buf_end]	= NB_Ope;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	}
	if(3){//���� ģ������
		msg_buff[g_nb_net_buf_end]	= startup_mouldtype;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		msg_buff[g_nb_net_buf_end]	= 1;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		msg_buff[g_nb_net_buf_end]	= 0x08;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	}
	if(4){//���� ����汾��
		msg_buff[g_nb_net_buf_end]	= startup_sys_ver;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		len						= strlen(SYS_VER);
		msg_buff[g_nb_net_buf_end]	= len;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		for(j = 0;j < len;j++)
		{
			msg_buff[g_nb_net_buf_end] = SYS_VER[j];
			g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		}
	}
	if(5){//���� ģ��̼��汾��
		msg_buff[g_nb_net_buf_end]	= startup_mou_ver;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		len						= strlen(MODLE_VER);
		msg_buff[g_nb_net_buf_end] = len;
		g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		for(j = 0;j < len;j++)
		{
			msg_buff[g_nb_net_buf_end] = MODLE_VER[j];
			g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
		}
	}
}
/*****************************************************************************
 * ��������:		�ϴ�������Ϣ
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-05-16				������ֲ
							2018-05-30				��Ӳ��� up_data
							2018-06-05				�޸�bug;������buff�����ͷ
							2018-06-07				�޸�bug;������װ����,����ƽ̨��ʾ������Ϣ����
 ****************************************************************************/
void up_per_info(uint8_t* up_data )
{	
	if(g_sys_operation_msg & 0x01)//����������豸��,��λg_reset_flag��־λ
	{
		g_reset_flag &=~ 0x0F;
		g_reset_flag |=  0xF0;
	}
	up_data[g_nb_net_buf_end] = part_ty;
	g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	up_data[g_nb_net_buf_end] = 0x01;
	g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	up_data[g_nb_net_buf_end] = g_sys_operation_msg & 0xFF;
	g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	up_data[g_nb_net_buf_end] = g_sys_operation_msg >> 8;
	g_nb_net_buf_end = (g_nb_net_buf_end + 1) & QUEUE_DAT_MAX;
	g_sys_operation_msg = 0;											//ʹ�����˹�������
}
/*****************************************************************************
 * ��������:		ϵͳappִ��
 * ��ʽ����:		��
 * ���ز���:		��
 * ��������;		2018-05-16				������ֲ
							2018-05-30				������д
							2018-06-07				�޸�bug;����û�з��ͳɹ�,�������ݱ�ɾ����,��ɾ���������ƶ�,���յ����ݺ���ƶ�����
							2018-06-22				�޸�bug;������װ����
							2018-07-24				�����Ż�;ģ��û�г�ʼ��,���ܽ���appӦ�õ���Ӻ�ִ��
 ****************************************************************************/
ErrorStatus sys_app(app_type type)
{
	uint16_t msg_len = 0;				//��Ϣ�峤��
	cmd_type cmd;								//����
	
	if(!g_model_config_flag)return ERROR;						//ģ��û�г�ʼ��,����ִ�����г���
	if(1){//�����ж�
		if(type != type_null_dat)
		{
			net_app_add(type);
		}
		if  (g_app_type != type_null_dat)return ERROR;//���������������,�򷵻�
		else g_app_type  = g_net_app_queue[g_queue_idex_s].type;
	}
	
	if(2){//APPִ�в��� 
		switch(g_app_type)
		{
			case type_send_anlog:{			//�ϴ�ģ����
				msg_len = sizeof(data_typedef)/sizeof(uint16_t);
				cmd			= type_send_dat_cmd;
				runstate_to_usart("�ϴ�ģ����\r\n");
				break;}
			case type_upload_config:{		//�ϴ�������Ϣ
				msg_len = CONFIG_TYPE_NUM;
				cmd			= type_upload_cmd;
				runstate_to_usart("�ϴ�������Ϣ\r\n");
				break;}
			case type_upload_startup:{	//�ϴ�������Ϣ
				msg_len	= STARTUP_PARA_TYPE_NUM;
				cmd     = type_startup_cmd;
				runstate_to_usart("�ϴ�������Ϣ\r\n");
				break;}
			case type_runing:{					//�ϴ�������Ϣ
				msg_len = RUNING_STATE_TYPE_NUM;
				cmd     = type_send_dat_cmd;
				runstate_to_usart("�ϴ�������Ϣ\r\n");
				break;}
			case type_oper_info:{				//�ϴ�������Ϣ
				msg_len = 1;
				cmd     = type_send_dat_cmd;
				runstate_to_usart("�ϴ�������Ϣ\r\n");
				break;}
			default:{										//����
				msg_len = g_queue_idex_s + 1;
				if(msg_len >= QUEUE_SIZE)msg_len = 0;
				g_app_type = type_null_dat;
				return ERROR;}
		}
	}
	if(3){//������װ
		Server_SendData(&Serial_Number ,  cmd , g_app_type , msg_len);//������д�뵽����֡����
	}
	return SUCCESS;
}


/*------------------------------File----------End------------------------------*/
