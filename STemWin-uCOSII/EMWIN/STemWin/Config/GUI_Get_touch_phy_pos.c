#include "GUI.h"
#include "delay.h"
#include "led.h"
#include "ILI93xx.h"


#define TOUCH_AD_TOP		190  	//���´������Ķ�����д�� Y ��ģ������ֵ��
#define TOUCH_AD_BOTTOM		4096 	//���´������ĵײ���д�� Y ��ģ������ֵ��
#define TOUCH_AD_LEFT 		290		//���´���������࣬д�� X ��ģ������ֵ��
#define TOUCH_AD_RIGHT		3800	//���´��������Ҳ࣬д�� X ��ģ������ֵ��

static int _AD2X(int adx) {
  I32 r = adx - TOUCH_AD_LEFT;
  r *=  lcddev.width- 1;
  return r / (TOUCH_AD_RIGHT - TOUCH_AD_LEFT);    
}

/*********************************************************************
*
*       _AD2Y
*
* Purpose:
*   Convert physical value into (logical) coordinates.
*/
static int _AD2Y(int ady) {
  I32 r = ady - TOUCH_AD_TOP;
  r *= lcddev.height - 1;
  return r/(TOUCH_AD_BOTTOM - TOUCH_AD_TOP);    
}
#if 1
#define GetMeasureValue
#endif
extern void _MY_GetTouchPos(void)
{
	GUI_PID_STATE TouchState;
	int xphys;
	int yphys;

	GUI_SetFont(&GUI_Font20_ASCII);
	GUI_CURSOR_Show();//��ʾ�α�
	GUI_CURSOR_Select(&GUI_CursorArrowM);//�α���ʾ��ʽ
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_BLACK);
	GUI_Clear();
	GUI_DispString("Measurement of\nA/Dconverter values");

	while(1)
	{
		GUI_TOUCH_GetState(&TouchState);
//		if(TouchState.Pressed)
			{
			xphys = GUI_TOUCH_X_MeasureX();//GUI_TOUCH_GetxPhys(); //���� 
			yphys = GUI_TOUCH_X_MeasureY();//GUI_TOUCH_GetyPhys(); //���� 
#ifdef GetMeasureValue
			TouchState.x = _AD2X(xphys);
			TouchState.y = _AD2Y(yphys);
#endif
		}

		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt("Analog input:\n",0,40);
		GUI_GotoY(GUI_GetDispPosY()+2);
		GUI_DispString("x:");
		GUI_DispDec(xphys,4);
		GUI_DispString(",y:");
		GUI_DispDec(yphys,4);
		GUI_SetColor(GUI_RED);
		GUI_GotoY(GUI_GetDispPosY()+4);
		GUI_DispString("\nPosition:\n");
		GUI_GotoY(GUI_GetDispPosY()+2);
		GUI_DispString("x:");
		GUI_DispDec(TouchState.x,4);
		GUI_DispString(",y:");
		GUI_DispDec(TouchState.y,4);		 
		LED0 = !LED0;
		GUI_Delay(10);
		LED0 = !LED0;
		xphys = 0;yphys = 0;
		TouchState.Pressed = 0000;
	}
}
