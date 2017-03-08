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
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<pthread.h>
#include"makeJson.h"
#include"resolveJson.h"
#include"socket.h"

/*缓冲区最大字符长度*/
#define MAX 65535

/*服务器IP地址及端口号设置*/
const char* IP = "121.42.196.94";
const int SERVERPORT = 10101;

unsigned char * raw_send_buffer_tick;   /*原生tick数据包预发送缓冲区*/
unsigned char * send_buffer_tick;   /*tick数据包发送缓冲区*/

unsigned char * raw_send_buffer = NULL;    /*原生数据送缓冲区*/
unsigned char raw_recv_buffer[MAX];    /*数据预接收缓冲区*/

unsigned char * send_buffer;    /*数据发送缓冲区*/

extern int number;  /*外部调用wgetDownload.c文件中的全局变量*/

/*************************************************
Function:       int create_socket()
Description:    初始化socket，并与服务器交互，通过发送或接收JSON数据包
Calls:          socket.c(
                        void send_tick(void*arg)
                        void send_json_packet(void *arg)
                        void recv_json_packet(void *arg)
                        )
Called By:      main.c(
                      int main()
                    )
Input:          NONE
Output:         创建套接字，发送并接收JSON数据包
Return:         0
Others:         NONE
*************************************************/
int create_socket()
{
    int ser_socketfd;   /*创建套接字标记符*/
    struct sockaddr_in remote_addr;     /*创建socket结构体*/
    pthread_t send_socket_tick; /*创建发送tick线程描述符*/
    pthread_t send_socket;  /*创建发送线程描述符*/
    pthread_t recv_socket;  /*创建接收线程描述符*/

    memset(&remote_addr, 0, sizeof(struct sockaddr_in));    /*初始化结构体*/
    remote_addr.sin_family = AF_INET;   /*设置使用网际IPv4协议*/
    remote_addr.sin_addr.s_addr = inet_addr(IP);    /*设置IP地址*/
    remote_addr.sin_port = htons(SERVERPORT);   /*设置服务器端口*/

    /*创建套接字*/
    if((ser_socketfd = socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        perror("socket failed.");
        return 1;
    }

    /*将套接字与结构体进行连接，向服务器端发送三次握手信号*/
    if((connect(ser_socketfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))) < 0)
    {
        perror("connect failed.");
        return 1;
    }

    /*创建套接字tick数据发送线程*/
    if((pthread_create(&send_socket_tick, NULL, send_tick,(void*)ser_socketfd)) != 0)
    {
        perror("send thread create failed.");
        return 1;
    }

    /*创建套接字数据发送线程*/
    if((pthread_create(&send_socket, NULL, send_json_packet,(void*)ser_socketfd)) != 0)
    {
        perror("send thread create failed.");
        return 1;
    }

    /*创建套接字数据接收线程*/
    if((pthread_create(&recv_socket, NULL, recv_json_packet,(void*)ser_socketfd)) != 0)
    {
        perror("recv thread create failed.");
        return 1;
    }

    while(1);/*进入死循环，不执行关闭套接字命令*/
    close(ser_socketfd);
    return 0;
}


/*************************************************
Function:       void send_tick(void*arg)
Description:    发送心跳包
Calls:          makeJson.c(
                        char* create_json_packet(char* code, char* msg, char* serial, cJSON *data)
                        unsigned char *pack_json_packet(char *item)
                        )
Called By:      socket.c(
                      int create_socket()
                    )
Input:          arg-套接字标记符（作为线程入口函数，必须以void *类型穿参）
Output:         创建心跳包，并通过套接字发送
Return:         void
Others:         NONE
*************************************************/
void send_tick(void*arg)
{
    int fd = (int)arg;
    int len;
    raw_send_buffer_tick = create_json_packet("1100","tick","20160001",NULL);
    len = strlen(raw_send_buffer_tick) + 4;
    send_buffer_tick = pack_json_packet(raw_send_buffer_tick);
    while(1)
    {
        if((send(fd,send_buffer_tick,len,0))<0)
        {
            perror("send failed.");
            return 1;
        }
        sleep(10);
    }
}

/*************************************************
Function:       void send_json_packet(void *arg)
Description:    发送其他JSON数据包，现阶段包括紧急呼叫包、文件上传包
Calls:          makeJson.c(
                        unsigned char *pack_json_packet(char *item)
                        )
Called By:      socket.c(
                      int create_socket()
                      )
Input:          arg-套接字标记符（作为线程入口函数，必须以void *类型穿参）
Output:         创建其他JSON数据包，并通过套接字发送
Return:         void
Others:         NONE
*************************************************/
void send_json_packet(void *arg)
{
    int fd = (int)arg;
    int len;
    while(1)
    {
        if(raw_send_buffer!=NULL)
        {
            len = strlen(raw_send_buffer)+4;
            send_buffer = pack_json_packet(raw_send_buffer);
            if((send(fd,send_buffer,len,0))<0)
            {
                perror("send failed.");
                return 1;
            }
            send_buffer=NULL;
            raw_send_buffer=NULL;
        }
    }
}

/*************************************************
Function:       void recv_json_packet(void *arg)
Description:    接收服务器发送的JSON数据包，包括文件路径包，无命令包等
Calls:          resolveJson.c(
                        char* unpack_json_packet(unsigned char* item)
                        )
Called By:      socket.c(
                      int create_socket()
                      )
Input:          arg-套接字标记符（作为线程入口函数，必须以void *类型穿参）
Output:         接收服务器发送的JSON数据包，包括文件路径包，无命令包等
Return:         void
Others:         NONE
*************************************************/
void recv_json_packet(void *arg)
{
    int fd = (int)arg;

    int wholelenth; /*所有网络JSON数据包长度*/
    int len;    /*单个网络JSON数据包长度*/

    pthread_t resolve_socket;
    char* recv_buffer;  /*拆包后原生JSON数据包*/
    char* recv_head;

    /*循环接收服务器发送的网络JSON数据包*/
    while(1)
    {
        /*每次最多接收MAX个字节数据，返回接收字节数*/
        wholelenth = recv(fd,raw_recv_buffer,MAX,0);
        if(wholelenth<0)
        {
            perror("recv failed.");
            return 1;
        }
        else
        {
            /*进行本轮拆包*/
            recv_buffer = unpack_json_packet(raw_recv_buffer);
            len  = strlen(recv_buffer)+4;
            //printf("%d\n",len);

            /*创建本轮接收数据解析线程*/
             if((pthread_create(&resolve_socket, NULL, resolve_json_packet,(void*)recv_buffer))!=0)
            {
                perror("recv thread create failed.");
                return 1;
            }

             /*等待线程处理结束*/
            pthread_join(resolve_socket,NULL);

            /*本轮接收数据包多于一个*/
            while(len<wholelenth)
            {
                 /*拆包*/
                recv_head = &raw_recv_buffer[len];
                recv_buffer = unpack_json_packet(recv_head);
                len += strlen(recv_buffer)+4;

                /*创建接收数据解析线程*/
                if((pthread_create(&resolve_socket, NULL, resolve_json_packet,(void*)recv_buffer))!=0)
                {
                    perror("recv thread create failed.");
                    return 1;
                }

                /*等待线程处理结束*/
                pthread_join(resolve_socket,NULL);
            }

            /*一轮数据接收完成*/
            printf("1 row recv finished!\n");
            /*进行语音播放和删除操作*/
            play_voice();
        }
    }
}

/*************************************************
Function:       void resolve_json_packet(void* arg)
Description:    接收服务器发送的JSON数据包，包括文件路径包，无命令包等
Calls:          resolveJson.c(
                        int resolve_json(char* json);
                        )
Called By:      socket.c(
                      void recv_json_packet(void *arg)
                      )
Input:          arg-接收JSON包对应的字符串（作为线程入口函数，必须以void *类型穿参）
Output:         接收服务器发送的JSON数据包，包括文件路径包，无命令包等
Return:         void
Others:         NONE
*************************************************/
void resolve_json_packet(void* arg)
{
    char* recv_buffer = (char *)arg;
    resolve_json(recv_buffer);
}




/*************************************************
Function:       void play_voice(void)
Description:    对本轮下载的语音执行播放和删除操作。
Calls:          stdlib.h(
                            int system(const char * string);
                        )
Called By:      socket.c
Input:          NONE
Output:         NONE
Return:         void
Others:         NONE
*************************************************/
void play_voice()
{
    /*DIR* dirp;
    int num = 0;
    dirp = opendir("/home/pi/fileServer/volt/");
    while(dirp){
    if(readdir(dirp)!=NULL)
        {++num;}
        else
        {break;}
    }
    closedir(dirp);
    printf("num is %d\n",num);*/
    system("omxplayer -p -o local /home/pi/fileServer/volt/*.wav");
    system("sudo rm -f /home/pi/fileServer/volt/*.wav");
    number = 1;

}
