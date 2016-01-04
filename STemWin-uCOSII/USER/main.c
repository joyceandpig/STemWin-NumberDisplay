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
void gui_disp_dec(void)
{
	GUI_DispStringAt("GUI_DispStringAt()",0,20);
	GUI_DispDec(123,4);
	GUI_DispString("   ");
	GUI_DispDec(-123,4);
	
	GUI_DispStringAt("GUI_DispDecAt()",0,35);
	GUI_DispDecAt(100,100,35,3);
	GUI_DispDecAt(-564,130,35,4);
	
	GUI_DispStringAt("GUI_DispDecShift(): ", 0, 50);
	GUI_DispDecShift(1236,5,2);
	GUI_DispString("   ");
	GUI_DispDecShift(-1236, 6, 2);
	GUI_DispStringAt("GUI_DispDecSpace(): ", 0, 65);
	GUI_DispDecSpace(3265,5);
	GUI_DispStringAt("GUI_DispSDec():     ", 0, 80);
	GUI_DispSDec(200,4);
	GUI_DispStringAt("GUI_DispSDecShift():     ", 0, 95);
	GUI_DispSDecShift(12345,7,3);
}
void gui_disp_float(void)
{
	float f = 123.45678;
	GUI_DispStringAt("GUI_DispFloat():\n", 0, 20);
	GUI_DispFloat(f, 9);
	GUI_GotoX(100);
	GUI_DispFloat(-f, 9);
	GUI_DispStringAt("GUI_DispFloatFix():\n", 0, 45);
	GUI_DispFloatFix(f, 9, 2);
	GUI_GotoX(100);
	GUI_DispFloatFix(f, 9, 2);
	GUI_DispStringAt("GUI_DispSFloatFix():\n", 0, 70);
	GUI_DispSFloatFix(f, 9, 2);
	GUI_GotoX(100);
	GUI_DispSFloatFix(-f, 9, 2);
	GUI_DispStringAt("GUI_DispFloatMin():\n", 0, 95);
	GUI_DispFloatMin(f, 3);
	GUI_GotoX(100);
	GUI_DispFloatMin(-f, 3);
	GUI_DispStringAt("GUI_DispSFloatMin():\n", 0, 120);
	GUI_DispSFloatMin(f, 3);
	GUI_GotoX(100);
	GUI_DispSFloatMin(-f, 3);
}
void gui_disp_bin_hex(void)
{
	int dec = 20;
	char* emwinversion;
	GUI_DispStringAt("GUI_DispBin():      ",0,20);
	GUI_DispBin(dec,6);
	GUI_DispStringAt("GUI_DispBinAt():  ", 0, 35);
	GUI_DispBinAt(dec,87,35,6);
	//???????? API ????
	GUI_DispStringAt("GUI_DispHex():     ", 0, 50);
	GUI_DispHex(dec,2);
	GUI_DispStringAt("GUI_DispHexAt(): ", 0, 65);
	GUI_DispHexAt(dec, 87,65,2);

	emwinversion = (char*)GUI_GetVersionString();    //???? emwin ???
GUI_DispStringAt(emwinversion,200,100);    //?? emwin ???
}
void gui_num_disp(void)
{
//	gui_disp_dec();
//	gui_disp_float();
	gui_disp_bin_hex();

}
int main(void)
{
	BSP_Init();
//	main_ui();
	gui_num_disp();
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
