/*****************************************************************************
Copyright:     2015-2016, FireWork.
File name:     makeJson.h
Description:   makeJson.c的头文件
Author:        YuZhou Guo,ShaoYang Yu
Version:       v3.0
Date:          2016-05-11
History:       2016-04-26 v1.0实现make_json_raw函数
               2016-05-8  v2.0联调测试，修正JSON数据包长度字段为byte类型
               2016-05-11 v3.0重构文件包含的函数，添加resolveJson.c到工程
*****************************************************************************/
#ifndef __MAKEJSON_H
#define __MAKEJSON_H

/*C语言第三方函数库，用于处理JSON数据格式*/
#include "cJSON.h"

char *create_json_packet(char *code, char *msg, char *serial, cJSON *data);
cJSON *generate_data(char *keyA, char *valueA, char *keyB, char *valueB);

unsigned char *pack_json_packet(char *item);
void int_to_byte(int i,  unsigned char *bytes);
void set_json_body(unsigned char *msg, char *item, int len);

#endif
