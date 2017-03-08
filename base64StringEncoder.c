/*****************************************************************************
Copyright:     2015-2016, FireWork.
File name:     base64StringEncoder.c
Description:   本文件用base64为readWav()函数返回的字符串编码
Author:        YuZhou Guo
Version:       v3.0
Date:          2016-05-15
History:       2016-04-25 v1.0实现base64编码逻辑
               2016-05-14 v2.0修改指针bug，实现编码无误
               2016-05-15 v3.0进行单元测试，实现无损文件上传
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "base64StringEncoder.h"

static const char *codes =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*************************************************
Function:       unsigned char* base64_encode(const unsigned char *in,  unsigned long len)
Description:    用base64为readWav()函数返回的字符串编码
Calls:          NONE
Called By:      thread.c(
                         void Voice_Thread()
                        )
Table Accessed: NONE
Table Updated:  NONE
Input:          in-音频文件输入字符串,len-字符串in的长度
Output:         base64编码后的字符串
Return:         与Output相同
Others:         NONE
*************************************************/
unsigned char* encode_base64(const unsigned char *in,  unsigned long len)
{
    unsigned long i, j = 0, leven;
    leven = 3*(len / 3);
    unsigned char *p = malloc((len / 3 + 1) * 4 + 1);  /*p为编码生成的字符串*/
    if(p == NULL){
        printf("Malloc error.");
        exit(1);
    }
    p[0] = "";

    for (i = 0; i < leven; i += 3) {
        /*通过指针为数组p赋值，三个字符一组，编码生成四个字符*/
        p[j++] = codes[in[0] >> 2]; /*对输入字符串的第一个字母的ASCII码右移两位，得到第一个编码字符*/
        p[j++] = codes[((in[0] & 3) << 4) + (in[1] >> 4)];  /*in[0]&3可得出一个字节的后两个比特位，左移4位之后补上第二个字符的ASCII码前四位*/
        p[j++] = codes[((in[1] & 0xf) << 2) + (in[2] >> 6)];/*生成第三个编码字符*/
        p[j++] = codes[in[2] & 0x3];/*生成第三个编码字符*/
        in += 3;
    }
    /*对输入数组in剩下的字符编码，编码方式与三字符编码大同小异。两字符编码得到三个字符，并在结尾append'='
    一字符编码生成两个字符，并在结尾append两个'='。'='作为缺省标志，只能在生成字符串结尾出现。*/
    if (i < len) {
        unsigned char a = in[0];
        unsigned char b = (i+1 < len) ? in[1] : 0;
        unsigned char c = 0;

        p[j++] = codes[a >> 2];
        p[j++] = codes[((a & 3) << 4) + (b >> 4)];
        p[j++] = (i+1 < len) ? codes[((b & 0xf) << 2) + (c >> 6)] : '=';
        p[j++] = '=';
    }

    /*形成完整C字串*/
    p[j] = '\0';
    //printf("%d\n", strlen(p));

    return p;
}
