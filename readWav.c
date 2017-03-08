/*****************************************************************************
Copyright:     2015-2016, FireWork.
File name:     read_wav.c
Description:   读取音频文件，存储在字符串中
Author:        YuZhou Guo
Version:       v3.0
Date:          2016-05-15
History:       2016-05-06 v1.0使用LINUX系统调用实现读文本
               2016-05-14 v2.0实现输出参数分配动态内存，大小为文件字节数
               2016-05-15 v3.0添加getFileLength(bfile)，修改二进制读文件读到空字符返回的bug
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "readWav.h"
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAX 1024

/*************************************************
Function:       char *read_wav(char *bfile)
Description:    读取音频文件，存储在字符指针中
Calls:          NONE
Called By:      thread.c(
                         void Voice_Thread()
                        )
Table Accessed: NONE
Table Updated:  NONE
Input:          bfile-音频文件名的指针
Output:         音频文件内容，存放在字符串中
Return:         与Output相同
Others:         NONE
*************************************************/
char *read_wav(char *bfile)
{

    int i, p = 0, fd, SIZE;

    if((fd = open(bfile, O_RDONLY)) < 0){
        perror("File open error");
        exit(1);
    }
    SIZE = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    //printf("File size: %d\n", SIZE);

    char *buf = (char *)malloc(MAX + 1); /*存放读取的二进制文件内容*/
    char *out = (char *)malloc(SIZE);
    /*循环读取音频文件，每次读取1024个字节，并复制到out指向的内存空间，直到文件末尾*/
    while((i = read(fd, buf, MAX)) > 0){
        memcpy(out + p, buf, i);
        p+=i;
    }

    close(fd);

    /*释放堆内存，防止内存泄漏*/
    free(buf);
    buf = NULL;

    return out;
}

/*************************************************
Function:       int get_file_length(char *bfile)
Description:    获取音频输出字符串的长度，不忽略输出字符串中的空字符
Calls:          NONE
Called By:      thread.c(
                         void Voice_Thread()
                        )
Table Accessed: NONE
Table Updated:  NONE
Input:          bfile-音频文件名的指针
Output:         音频文件输出字符串长度
Return:         与Output相同
Others:         NONE
*************************************************/
int get_file_length(char *bfile)
{

    /*fd：文件描述符，SIZE:文件字节数，也是输出字符串的长度*/
    int fd, SIZE;

    if((fd = open(bfile, O_RDONLY)) < 0){
        perror("File open error");
        exit(1);
    }
    SIZE = lseek(fd, 0, SEEK_END);
    close(fd);

    return SIZE;
}
