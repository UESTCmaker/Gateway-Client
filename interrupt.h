#ifndef __INTERRUPT_H
#define __INTERRUPT_H

/*****************************************************************************
Copyright:     2015-2016, Firework.
File name:     interrupt.h
Description:   此文件主要完成家庭网关系统按键触发中断和消抖的相关操作。
Author:        YU SHAO YANG
Version:       1.0
Date:          2016.04.02
History:       NONE
*****************************************************************************/


/*设置按键消抖时间为100毫秒*/
#define DebouncingTime 100

extern void Voice_Thread(void);
extern void create_emergency_LV1(void);
extern void create_emergency_LV2(void);
extern void create_emergency_LV3(void);

void key_interrupt_1(void);
void key_interrupt_2(void);
void key_interrupt_3(void);
void key_interrupt_4(void);

#endif
