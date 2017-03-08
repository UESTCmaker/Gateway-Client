/*****************************************************************************
Copyright:     2015-2016, FireWork.
File name:     socket.c
Description:   本文件包含与JSON数据包的解析有关的函数，属于工具函数文件
Author:        ShaoYang Yu
Version:       v3.0
Date:          2016-05-15
History:       2016-04-26 v1.0
               2016-05-8  v2.0
               2016-05-11 v3.0
*****************************************************************************/
#include "cJSON.h"
#include "resolveJson.h"
#include"wgetDownload.h"
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
/*************************************************
Function:       int resolve_json(char* json)
Description:    解析拆包后的json包
Calls:          NONE
Called By:      socket.c(
                            void resolve_json_packet(void* arg)
                        )
Input:          char*类型的JSON包
Output:         NONE
Return:         命令对应的编号（参见网关协议手册）
Others:         NONE
*************************************************/
int resolve_json(char* json){
    struct recv re;
    int num = 0;
    pthread_t download;
    clear_struct(re);

    re.code = get_json_stringValue(json, "code",NULL);
    if(strcmp(re.code,"2101")==0){  //get tick back noOrder
        printf("code:2101 received!\n");
        num = 13;
    }
    else if(strcmp(re.code,"2113")==0){
        re.type = get_json_stringValue(json, "data","type");
        num = 12;
    }
    else if(strcmp(re.code,"2100")==0){
        re.filePath = get_json_stringValue(json, "data","filePath");
        re.newVersion = get_json_stringValue(json, "data","newvision");
        re.hash = get_json_stringValue(json, "data","hash");
        //printf("%s\n",re.filePath);
        num = 11;
    }
    else if(strcmp(re.code,"2112")==0){
        printf("code:2112 received!\n");
        re.filePath = get_json_stringValue(json, "data","filePath");
        re.fileType = get_json_stringValue(json, "data", "fileType");
          if((pthread_create(&download, NULL, download_file, (void*)&re))!=0)
            {
            perror("recv thread create failed.");
            return 1;
            }
            pthread_join(download,NULL);
        //printf("%s\n",re.filePath);
        num = 10;
    }
    return num;
}

/*************************************************
Function:       void download_file(void *arg)
Description:    执行json包中语音文件url地址的下载工作
Calls:          wgetDownload.c(
                                    char* wget_download(char* filepath, QuietMode qmode, ConMode cmode, char* type)
                              )
Called By:      socket.c(
                            int resolve_json(char* json)
                        )
Input:          char*类型的JSON包
Output:         NONE
Return:         命令对应的编号（参见网关协议手册）
Others:         NONE
*************************************************/
void download_file(void *arg){
    struct recv *re = (struct recv *)arg;
    char buffer[30] = {0};
    char* filedir;
    //printf("good\n");
    printf("filePath:%s\nfileType:%s\n",(*re).filePath,(*re).fileType);

    /*下载文件类型为wav格式*/
    if(strstr((*re).filePath,".wav")!=NULL)
    {

        filedir = wget_download((*re).filePath, 3, 5, (*re).fileType);
        //system("omxplayer -p -o /home/pi/fileServer/volt/*.wav");
        pthread_exit(NULL);

    }
    printf("wrong filePath.\n");
    pthread_exit(NULL);
}


/*************************************************
Function:       void clear_struct(struct recv re)
Description:    清空json包数据接收存储结构体
Calls:          NONE
Called By:      socket.c(
                            int resolve_json(char* json)
                        )
Input:          struct recv 类型的结构体
Output:         NONE
Return:         NONE
Others:         NONE
*************************************************/
void clear_struct(struct recv re){
        re.code = NULL;
        re.filePath = NULL;
        re.fileType = NULL;
        re.hash = NULL;
        re.newVersion = NULL;
        re.type = NULL;
}


/*************************************************
Function:       char *get_json_stringValue(char *string_from_server, char *keyA, char *keyB)
Description:    获取JSON数据包中的数据，keyA表示你想要获得的值对应的键
                如果*keyA != "data"，那么将keyB设为NULL；否则，keyB设置为data对象中想要获取的字段
Calls:          NONE
Called By:      socket.c(
                            int resolve_json(char* json)
                        )
Input:          char*类型的json包，母键名，子键名
Output:         对应的键值
Return:         对应的键值
Others:         NONE
*************************************************/
char *get_json_stringValue(char *string_from_server, char *keyA, char *keyB)
{
    cJSON *JsonFromServer = cJSON_Parse(string_from_server);
    cJSON *stringValue = cJSON_GetObjectItem(JsonFromServer, keyA);
    //printf("%s\n", cJSON_Print(stringValue));
    cJSON *dataValue;
    if(strcmp(keyA,"data")!=0){
        //printf("%s\n", cJSON_Print(stringValue));
        //实际上除了cJSON_String也没有其他类型
        if(stringValue->type == cJSON_String){
            //printf("%s\n", stringValue->valuestring);
            return stringValue->valuestring;
            }
    }else{
        dataValue = cJSON_GetObjectItem(stringValue, keyB);
        //printf("%s\n", cJSON_Print(dataValue));
        //printf("%d\n", dataValue->type);
        if(dataValue->type == cJSON_String){
            //printf("%s\n", dataValue->valuestring);
            return dataValue->valuestring;
            }
    }
    return NULL;
}

/*************************************************
Function:       char* unpack_json_packet(unsigned char* item)
Description:    将包头为数据包的字节数的JSON数据包转换为纯JSON格式数据包
Calls:          resolveJosn.c(
                                int bytes_to_int(unsigned char* bytes)
                             )
Called By:      socket.c(
                            void recv_json_packet(void *arg)
                        )
Input:          包头为数据包的字节数的JSON数据包
Output:         纯json格式数据包
Return:         与Output相同
Others:         NONE
*************************************************/
char* unpack_json_packet(unsigned char* item)
{
    int i;
    int len;
    char* body;
    len = bytes_to_int(item);
    //printf("%d",len);
    body = (char *)malloc(len-4);//动态分配数组空间
    for(i=0;i<len-4;i++){
        body[i]=item[4+i];
    }
    return body;

}


/*************************************************
Function:       int bytes_to_int(unsigned char* bytes)
Description:    将bytes转换成int类型（C语言中没有byte类型，用unsigned char代替）
Calls:          NONE
Called By:      resolveJson.c(
                                char* unpack_json_packet(unsigned char* item)
                             )
Input:          bytes-输入字符串
Output:         输出int类型数字
Return:         与output相同
Others:         NONE
*************************************************/
int bytes_to_int(unsigned char* bytes)
{
    int addr = bytes[0] & 0xFF;
    addr |= ((bytes[1] << 8) & 0xFF00);
    addr |= ((bytes[2] << 16) & 0xFF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
 }
