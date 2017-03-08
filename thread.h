#ifndef __THREAD_H
#define __THREAD_H

/*****************************************************************************
Copyright:     2016-2017, Firework.
File name:     thread.h
Description:   此文件主要完成家庭网关系统按键录音上传和紧急呼叫功能。
Author:        YU SHAO YANG
Version:       2.0
Date:          2016.05.15
History:       2016.04.30 修改函数名TEST为Voice_Thread,其他同前
               2016.05.15 修改Voice_Thread函数，实现声音上传功能
                          修改Emergency_Thread_Level函数，实现紧急呼叫功能
*****************************************************************************/

void create_wav_thread(void);
void create_emergency_LV1(void);
void create_emergency_LV2(void);
void create_emergency_LV3(void);

int set_priority(const int pri);

#endif
