#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "ILI93xx.h"
#include "key.h"
#include "malloc.h" 
#include "usmart.h" 
#include "GUI.h"
#include "touch.h"
#include "includes.h"

//ALIENTEK Mini STM32�����巶������27
//�ڴ����ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
extern void _MY_GetTouchPos(void);

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
#define START_TASK_PRIO      		20        //��ʼ��������ȼ�����Ϊ���
#define START_STK_SIZE  				64        //���������ջ��С
OS_STK START_TASK_STK[START_STK_SIZE];    //�����ջ	
void start_task(void *pdata);	            //������

#define LED_TASK_PRIO      			9        //��ʼ��������ȼ�����Ϊ���
#define LED_STK_SIZE  					64        //���������ջ��С
OS_STK LED_TASK_STK[LED_STK_SIZE];    //�����ջ	
void led_task(void *pdata);	            //������

#define EMWIN_DEMO_TASK_PRIO    8        //��ʼ��������ȼ�����Ϊ���
#define EMWIN_STK_SIZE  				3096        //���������ջ��С
OS_STK EMWIN_TASK_STK[EMWIN_STK_SIZE];    //�����ջ	
void emwin_demo_task(void *pdata);	            //������

#define TOUCH_TASK_PRIO      		10        //��ʼ��������ȼ�����Ϊ���
#define TOUCH_STK_SIZE  				64        //���������ջ��С
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];    //�����ջ	
void touch_task(void *pdata);	            //������

void BSP_Init(void)
{
	NVIC_Configuration();	 
	delay_init();	    			 //��ʱ������ʼ��	  
	uart_init(115200);	 		//���ڳ�ʼ��Ϊ9600
	LED_Init();		  				//��ʼ����LED���ӵ�Ӳ���ӿ�
	TFTLCD_Init();			   	//��ʼ��LCD		 
	tp_dev.init();
//	tp_dev.adjust();
	mem_init();				//��ʼ���ڴ��

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);
  GUI_Init();
	
}
void main_ui(void)
{
#if 0
	 _MY_GetTouchPos();
#endif
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("Hello World!", 30, 200);
	GUI_DispStringAt("Hello emWin!", 30, 216);
	GUI_DrawRoundedRect(0,0,200,200,5);
	GUI_DrawRoundedFrame(2,2,180,20,5,2);

}

int main(void)
{
	BSP_Init();
//	main_ui();

	OSInit();
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);//������ʼ����
	OSStart();
}
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;

//	GUI_Delay(1000);
	OS_ENTER_CRITICAL();
	OSTaskCreate(emwin_demo_task,(void *)0,&EMWIN_TASK_STK[EMWIN_STK_SIZE-1],EMWIN_DEMO_TASK_PRIO);
	OSTaskCreate(touch_task,(void *)0,&TOUCH_TASK_STK[TOUCH_STK_SIZE-1],TOUCH_TASK_PRIO);
	OSTaskCreate(led_task,(void *)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();
}
void led_task(void *pdata)
{
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500);
	}
}
void touch_task(void *pdata)
{
	while(1)
	{
		GUI_TOUCH_Exec();
		OSTimeDlyHMSM(0,0,0,10);
	}
}
void emwin_demo_task(void *pdata)
{
	while(1)
	{
//		GUIDEMO_Main();
		OSTimeDlyHMSM(0,0,0,10);
	}
}
