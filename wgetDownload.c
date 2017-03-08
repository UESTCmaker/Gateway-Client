/*****************************************************************************
Copyright:     2015-2016, FireWork.
File name:     wgetDownload.c
Description:   当收到服务器发送的code为2112的json包时，从服务器指定的路径上下载相应文件，并保存到指定目录
Author:        YuZhou Guo
Version:       v3.0
Date:          2016-05-16
History:       2016-05-12 v1.0实现基本下载功能
               2016-05-16 v2.0添加安静模式，并指定输出路径
               2016-05-15 v3.0修复枚举变量的bug，添加下载失败提示
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "wgetDownload.h"

/*全局变量说明：文件序号，文件通过序号命名
取值范围：number >= 1，整型
调用函数：char* wgetDownload(char* filepath, QuietMode qmode, ConMode cmode, char* type)
*/
int number = 1;

/*************************************************
Function:       char* wget_download(char* filepath, QuietMode qmode, ConMode cmode, char* type)
Description:    利用wget命令下载服务器指定路径上的文件，其中-P指示文件名，-q指定安静模式
Calls:          NONE
Called By:      resolveJson.c(
                         void Download_file(void *arg)
                        )
Input:          filepath-服务器文件下载路径，qmode-指定安静模式的枚举变量，type-指定文件类型
Output:         存储在本地/home/pi/fileServer/的文件
Return:         本地文件的绝对路径对应的字符串
Others:         NONE
*************************************************/
char* wget_download(char* filepath, QuietMode qmode, ConMode cmode, char* type)
{
	/*形成Url字符串*/
	const char *IP = "http://121.42.196.94:8080/";  /*服务器外网IP地址*/
	char *url = (char *)malloc(strlen(filepath) + strlen(IP) + 1);
	char *command;
	int fileType = atoi(type);
	if(url == NULL){
		perror("Malloc error");
		exit(1);
	}
    strcpy(url, IP);
    strcat(url, filepath);

    /*形成filedir文件绝对路径字符串*/
    char *filedir;

    /*计算number的位数,保存在i中*/
    int t = number, i;
    for(i = 0; t != 0; i++){
        t/=10;
    }
    char *filename = malloc(i + 1);
    if(filename == NULL){
        perror("Malloc error");
        exit(1);
    }
    /*代替函数itoa()，实现int转换为char类型*/
    sprintf(filename, "%d", number);
    number++;

    /*实际上，两种类型文件存放目录的字符串长度相同*/
    char *dir = "/home/pi/fileServer/";
    filedir =  (char *)malloc(strlen(dir) + strlen("volt/") + strlen(filename) + strlen(".wav") + 1);
    if(filedir == NULL){
        perror("Malloc error");
        exit(1);
    }
    strcpy(filedir, dir);

    /*根据文件类型分别存储到不同的目录：*.wav存储到volt/下，.conf存储到conf/下*/
    if(fileType == VOLT){
        strcat(filedir, "volt/");   /*filedir = "/home/cookissea/fileServer/volt/"*/
    }else if(fileType == CONF){
        strcat(filedir, "conf/");   /*filedir = "/home/cookissea/fileServer/conf/"*/
    }
    strcat(filedir, filename);
    if(fileType == VOLT){
        strcat(filedir, ".wav");    /*filedir = "/home/cookissea/fileServer/volt/"*/
    }else if(fileType == CONF){
        strcat(filedir, ".cfg");    /*filedir = "/home/cookissea/fileServer/conf/"*/
    }/* end of if(fileType == VOLT)*/

    /*形成command命令行字符串*/
    command = malloc(strlen("wget -O ") + strlen(filedir) + strlen(" -q ") + strlen(url) +  1);
	if(command == NULL){
		perror("Malloc error");
		exit(1);
	}
	strcpy(command, "wget -O ");
	strcat(command, filedir);
	if(qmode == SILENT)
        strcat(command, " -q ");
    //if(cmode == CONTINUE)
    //    strcat(command, " -c ");
	strcat(command, url);

	/*执行下载命令*/
	errno = 0;
	system(command);

	/*exception handling*/
	/*switch(errno){
		case :
	}*/
	/*通知下载情况*/
	if(errno == 0){
        printf("Download successful!\n");
	}
	if(errno > 0){
        printf("Download failed!\n");
	}

	free(url);
	url = NULL;
    free(filename);
    filename = NULL;
	free(command);
	command = NULL;
	return filedir;
}

/*************************************************
Function:       bool md5check(char *md5, char *filname)
Description:    对比服务器文件的MD5值，判断是否下载到原始文件
Calls:          NONE
Called By:      resolveJson.c(
                         void Download_file(void *arg)
                        )
Input:          函数构建中
Output:         函数构建中
Return:         函数构建中
Others:         NONE
*************************************************/
//bool md5check(char *md5, char *filname)
//{

//}
