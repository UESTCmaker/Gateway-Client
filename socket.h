/*****************************************************************************
Copyright:     2015-2016, FireWork.
File name:     socket.c
Description:   本文件包含与JSON数据包的生成有关的函数，属于工具函数文件
Author:        ShaoYang Yu
Version:       v3.0
Date:          2016-05-15
History:       2016-04-26 v1.0
               2016-05-8  v2.0
               2016-05-11 v3.0
*****************************************************************************/
#ifndef __SOCKET_H
#define __SOCKET_H

int create_socket(void);
void recv_json_packet(void *arg);
void send_json_packet(void *arg);
void send_tick(void*arg);
void resolve_json_packet(void* arg);

#endif
