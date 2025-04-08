#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "main.h"
#include "stdbool.h"

// 定义按钮按下的消抖时间
#define BUTTON_DOUBLE_TIME	(50)	// 双击间隔时间（500毫秒）
#define BUTTON_LONG_TIME  	(80)              		// 长按时间间隔（1000毫秒）

/*创建一个结构体用来存储四个按键的信息*/
struct keys
{
	unsigned char judge_sta;	//用于判断按键处于什么状态，用于判断按键进入什么状态了
	bool key_sta;			//用于判断B1 B2 B3 B4四个按键的状态，0按下，1松开
	unsigned char Lcd_sta; 	// 用于判断按键Lcd显示状态，1短按，2长按，3双击
};
struct Some_State
{
	int tim;
	int sta_re;
	bool sta;
};
struct Input_Capure
{
	double frq;
	double duty;
};

int get_key_stat(int );
void set_key_stat(int ,bool);

void LCD_Light_Back(uint8_t *,uint8_t ,uint8_t ,uint8_t );
void LCD_Text_Back (uint8_t *,uint8_t ,uint8_t ,uint8_t );

void LED_Disp(int );

	
#endif
