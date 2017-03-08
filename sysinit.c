/*****************************************************************************
Copyright:     2015-2016, Firework.
File name:     sysinit.h
Description:   此文件主要完成家庭网关系统按键初始化的相关操作，使用了树莓派中
               的wiringPi库作为GPIO控制库
Author:        YU SHAO YANG
Version:       1.0
Date:          2016.04.02
History:       NONE
*****************************************************************************/
#include<stdio.h>
#include<wiringPi.h>
#include <unistd.h>
#include"sysinit.h"
#include"interrupt.h"

/*************************************************
Function:       gateway_init
Description:    该函数用于执行四个按键的初始化和中断
                设置工作。
Calls:          wiringPi.h(
                            int wiringPiSetup(void);
                            void pinMode(int pin, int mode);
                            void pullUpDnControl(int pin, int pud);
                            int wiringPiISR(int pin, int edgeType,
                                       void(*function)(void));
                          )
Called By:      main.c
Input:          NONE
Output:         void KeyInterrupt1(void);
                void KeyInterrupt2(void);
                void KeyInterrupt3(void);
                void KeyInterrupt4(void);
Return:         NONE
Others:         NONE
*************************************************/
void gateway_init()
{

    /*wiringPi库初始化函数，需要ROOT权限执行*/
    wiringPiSetup();

    /*将四个按键模式均设置为输入捕获模式*/
	pinMode(Key1,INPUT);
	pinMode(Key2,INPUT);
	pinMode(Key3,INPUT);
	pinMode(Key4,INPUT);

	/*将四个按键所对应的端口设置为有上拉电阻模式，内部电压上拉至3.3V*/
	pullUpDnControl(Key1, PUD_UP);
	pullUpDnControl(Key2, PUD_UP);
	pullUpDnControl(Key3, PUD_UP);
	pullUpDnControl(Key4, PUD_UP);

	/*设置四个按键为下降沿触发中断，并设置中断服务程序名*/
	wiringPiISR(Key1,INT_EDGE_FALLING,&key_interrupt_1);
	wiringPiISR(Key2,INT_EDGE_FALLING,&key_interrupt_2);
	wiringPiISR(Key3,INT_EDGE_FALLING,&key_interrupt_3);
	wiringPiISR(Key4,INT_EDGE_FALLING,&key_interrupt_4);
}


/*************************************************
Function:       delay_us
Description:    该函数用于微秒延时。
Calls:          unistd.h(
                            void usleep(int micro_seconds);
                        )
Called By:      main.c
                interrupt.c
                //
Input:          unsigned int useconds
Output:         NONE
Return:         NONE
Others:         NONE
*************************************************/
void delay_us(unsigned int useconds)
{
    usleep(useconds);
}

/*************************************************
Function:       delay_ms
Description:    该函数用于毫秒延时。
Calls:          unistd.h(
                            void usleep(int micro_seconds);
                        )
Called By:      main.c
                interrupt.c
                //
Input:          unsigned int mseconds
Output:         NONE
Return:         NONE
Others:         NONE
*************************************************/
void delay_ms(unsigned int mseconds)
{
    usleep(mseconds*1000);
}

/*************************************************
Function:       delay_s
Description:    该函数用于毫秒延时。
Calls:          unistd.h(
                             unsigned sleep(unsigned int seconds);
                        )
Called By:      main.c
                interrupt.c
                //
Input:          unsigned int seconds
Output:         NONE
Return:         NONE
Others:         NONE
*************************************************/
void delay_s(unsigned int seconds)
{
    sleep(seconds);
}
