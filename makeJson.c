/*****************************************************************************
Copyright:     2015-2016, FireWork.
File name:     makeJson.c
Description:   本文件包含与JSON数据包的生成有关的函数，属于工具函数文件
Author:        YuZhou Guo,ShaoYang Yu
Version:       v3.0
Date:          2016-05-11
History:       2016-04-26 v1.0实现make_json_raw函数
               2016-05-8  v2.0联调测试，修正JSON数据包长度字段为byte类型
               2016-05-11 v3.0重构文件包含的函数，添加resolveJson.c到工程
*****************************************************************************/
#include"cJSON.h"
#include"makeJson.h"
#include<stdio.h>
//#include <sys/types.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

#define VOLT 1 /*文件类型：语音*/
#define CONF 2 /*文件类型：配置*/

/*************************************************
Function:       char* create_json_packet(char* code, char* msg, char* serial, cJSON *data)
Description:    生成原生JSON包
Calls:          NONE
Called By:      thread.c(
                         void create_wav_thread()
                         void create_emergency_LV1()
                         void create_emergency_LV2()
                         void create_emergency_LV3()
                        )
                socket.c(
                         void send_tick(void*arg)
                        )
Input:          code-JSON编号，msg-JSON包消息描述，serial-JSON包序列号，data-JSON包数据子对象
Output:         生成的JSON包对应的字符串
Return:         与Output相同
Others:         NONE
*************************************************/
char* create_json_packet(char* code, char* msg, char* serial, cJSON *data)
{
    /*创建gatewayMessage对象并添加项*/
	cJSON *gatewayMessage = cJSON_CreateObject();
	cJSON_AddStringToObject(gatewayMessage, "code", code);
	cJSON_AddStringToObject(gatewayMessage, "msg", msg);

    /*获取当前时间，格式：XXXX-XX-XX XX:XX:XX*/
    time_t cur_time = time(NULL);
    char cur[20];
    struct tm *ptr = localtime(&cur_time);
    strftime(cur, sizeof(cur) + 1, "%Y-%m-%d %H:%M:%S", ptr);
    cJSON_AddStringToObject(gatewayMessage, "time", cur);

    cJSON_AddStringToObject(gatewayMessage, "serial", serial);

    /*如果data不为空则装入data子对象*/
    if(data != NULL){
        cJSON_AddItemToObject(gatewayMessage, "data", data);
    }

    return cJSON_Print(gatewayMessage);
}

/*************************************************
Function:       cJSON *generate_data(char *keyA, char *valueA, char *keyB, char *valueB)
Description:    生成JSON数据包中的data对象，最多在data对象中添加两条数据字段。如果keyA == NULL那么不添加该项
Calls:          NONE
Called By:      thread.c(
                         void create_wav_thread()
                         void create_emergency_LV1()
                         void create_emergency_LV2()
                         void create_emergency_LV3()
                        )
Input:          keyA-添加的第一个数据项名，keyB-添加的第二个数据项名，valueA-添加的第一个数据内容，valueB-添加的第二个数据内容
Output:         生成data对象
Return:         与Output相同
Others:         NONE
*************************************************/
cJSON *generate_data(char *keyA, char *valueA, char *keyB, char *valueB)
{
    cJSON *data = cJSON_CreateObject();
    if(keyA != NULL)
        cJSON_AddStringToObject(data, keyA, valueA);
    if(keyB != NULL)
        cJSON_AddStringToObject(data, keyB, valueB);

    return data;
}

/*************************************************
Function:       unsigned char* pack_json_packet(char* item)
Description:    生成带有数据包的字节数为头部的JSON数据包
Calls:          makeJson.c(
                         void int_to_byte(int i,unsigned char*bytes)
                         void set_json_body(unsigned char* msg, char* item, int len)
                        )
Called By:      socket.c(
                         void send_tick(void*arg)
                         void send_json_packet(void *arg)
                        )
Input:          原生JSON数据包
Output:         生成带有数据包的字节数为头部的JSON数据包
Return:         与Output相同
Others:         NONE
*************************************************/
unsigned char *pack_json_packet(char *item)
{
    unsigned char * msg;
    int  len;
    len = strlen(item); /*获取原生包字符长度*/
    msg = (unsigned char *)malloc(len + 4); /*动态分配数组空间*/
    int_to_byte(len + 4, msg);  /*将json打包长度信息写入msg头4字节*/
    set_json_body(msg, item, len);  /*将json包内容部分写入msg余下字节*/

    return  msg;
}

/*************************************************
Function:       void int_to_byte(int i,unsigned char*bytes)
Description:    将int转换成byte类型（C语言中没有byte类型，用unsigned char代替）
Calls:          NONE
Called By:      makeJson.c(
                         unsigned char *pack_json_packet(char *item)
                        )
Input:          bytes-输出字符串，i-需被转换的整型
Output:         输出byte类型字符
Return:         void
Others:         NONE
*************************************************/
void int_to_byte(int i, unsigned char *bytes)
{
    memset(bytes,0,sizeof(unsigned char) *4);
    bytes[0] = (unsigned char) (0xff & i);
    bytes[1] = (unsigned char) ((0xff00 & i) >> 8);
    bytes[2] = (unsigned char) ((0xff0000 & i) >> 16);
    bytes[3] = (unsigned char) ((0xff000000 & i) >> 24);
    return;
}

/*************************************************
Function:       void set_json_body(unsigned char* msg, char* item, int len)
Description:    将原生JSON数据包的每个字符向后移动四个字符，即重新设置JSON数据包数据字段位置
Calls:          NONE
Called By:      makeJson.c(
                         unsigned char *pack_json_packet(char *item)
                        )
Input:          msg-重新设置后的JSON数据包，item-原生数据包，len-原声数据包长度
Output:         输出byte类型字符
Return:         void
Others:         NONE
*************************************************/
void set_json_body(unsigned char* msg, char* item, int len)
{
    int i;
    for(i=0;i<len;i++)
    {
        msg[4+i]=item[i];
    }
}
