/*****************************************************************************
Copyright:     2015-2016, Firework.
File name:     interrupt.h
Description:   此文件主要完成家庭网关系统按键触发中断和消抖的相关操作。
Author:        YU SHAO YANG
Version:       1.0
Date:          2016.04.02
History:       NONE
*****************************************************************************/
#include<stdio.h>
#include<wiringPi.h>
#include<pthread.h>
#include<stdlib.h>
#include"sysinit.h"
#include"interrupt.h"
#include"thread.h"

/*定义四个线程描述符作为有效中断触发的线程入口*/
pthread_t Thread1;
pthread_t Thread2;
pthread_t Thread3;
pthread_t Thread4;

/*************************************************
Function:       key_interrupt
Description:    该函数用于执行四个按键的消抖和线程
                创建工作。
Calls:          sysinit.h(
                            void delay_ms(unsigned int mseconds);
                         )
                wiringPi.h(
                            void digitalRead(int pin);
                          )
                pthread.h(
                            int pthread_create(pthread_t*restrict tidp,const pthread_attr_t *restrict_attr,
                                               void*（*start_rtn)(void*),void *restrict arg);
                         )
Called By:      thread.c
Input:          NONE
Output:         void Voice_Thread(void);
                void Emergency_Thread_Level1(void);
                void Emergency_Thread_Level2(void);
                void Emergency_Thread_Level3(void);
Return:         NONE
Others:         NONE
*************************************************/

void key_interrupt_1(void)
{
	delay_ms(DebouncingTime);

	/*若读取为高电平说明此次中断为按键抖动，从中断服务程序返回*/
	if(digitalRead(Key1)==1)
    {
        return;
    }

	//printf("已进入按键1对应的程序\n");

	/*进入语音对讲线程*/
	pthread_create(&Thread1,NULL,create_wav_thread,NULL);
}

void key_interrupt_2(void)
{
	delay_ms(DebouncingTime);

	/*若读取为高电平说明此次中断为按键抖动，从中断服务程序返回*/
	if(digitalRead(Key2)==1)
    {
        return;
    }

	//printf("已进入按键2对应的程序\n");

	/*进入一级紧急呼叫线程*/
	pthread_create(&Thread2,NULL,create_emergency_LV1,NULL);
}

void key_interrupt_3(void)
{
	delay_ms(DebouncingTime);

	/*若读取为高电平说明此次中断为按键抖动，从中断服务程序返回*/
	if(digitalRead(Key3)==1)
    {
        return;
    }

	//printf("已进入按键3对应的程序\n");

    /*进入二级紧急呼叫线程*/
	pthread_create(&Thread3,NULL,create_emergency_LV2,NULL);
}

void key_interrupt_4(void)
{
	delay_ms(DebouncingTime);

	/*若读取为高电平说明此次中断为按键抖动，从中断服务程序返回*/
	if(digitalRead(Key4)==1)
    {
        return;
    }

	//printf("已进入按键4对应的程序\n");

	/*进入三级紧急呼叫线程*/
	pthread_create(&Thread4,NULL,create_emergency_LV3,NULL);
}
