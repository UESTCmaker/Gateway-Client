/*****************************************************************************
Copyright:     2015-2016, FireWork.
File name:     readWav.h
Description:   readWav.c的头文件
Author:        YuZhou Guo
Version:       v3.0
Date:          2016-05-15
History:       2016-05-06 v1.0使用LINUX系统调用实现读文本
               2016-05-14 v2.0实现输出参数分配动态内存，大小为文件字节数
               2016-05-15 v3.0添加getFileLength(bfile)，修改二进制读文件读到空字符返回的bug
*****************************************************************************/
#ifndef READWAV_H_INCLUDED
#define READWAV_H_INCLUDED

char *read_wav(char *bfile);
int get_file_length(char *bfile);

#endif // READWAV_H_INCLUDED
