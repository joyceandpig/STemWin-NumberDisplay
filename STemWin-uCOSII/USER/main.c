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

//ALIENTEK Mini STM32开发板范例代码27
//内存管理实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
extern void _MY_GetTouchPos(void);

/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
#define START_TASK_PRIO      		20        //开始任务的优先级设置为最低
#define START_STK_SIZE  				64        //设置任务堆栈大小
OS_STK START_TASK_STK[START_STK_SIZE];    //任务堆栈	
void start_task(void *pdata);	            //任务函数

#define LED_TASK_PRIO      			9        //开始任务的优先级设置为最低
#define LED_STK_SIZE  					64        //设置任务堆栈大小
OS_STK LED_TASK_STK[LED_STK_SIZE];    //任务堆栈	
void led_task(void *pdata);	            //任务函数

#define EMWIN_DEMO_TASK_PRIO    8        //开始任务的优先级设置为最低
#define EMWIN_STK_SIZE  				3096        //设置任务堆栈大小
OS_STK EMWIN_TASK_STK[EMWIN_STK_SIZE];    //任务堆栈	
void emwin_demo_task(void *pdata);	            //任务函数

#define TOUCH_TASK_PRIO      		10        //开始任务的优先级设置为最低
#define TOUCH_STK_SIZE  				64        //设置任务堆栈大小
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];    //任务堆栈	
void touch_task(void *pdata);	            //任务函数

void BSP_Init(void)
{
	NVIC_Configuration();	 
	delay_init();	    			 //延时函数初始化	  
	uart_init(115200);	 		//串口初始化为9600
	LED_Init();		  				//初始化与LED连接的硬件接口
	TFTLCD_Init();			   	//初始化LCD		 
	tp_dev.init();
//	tp_dev.adjust();
	mem_init();				//初始化内存池

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
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);//创建起始任务
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
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
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
