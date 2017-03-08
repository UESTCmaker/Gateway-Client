/*****************************************************************************
Copyright:     2015-2016, FireWork.
File name:     base64StringEncoder.h
Description:   base64StringEncoder.c的头文件
Author:        YuZhou Guo
Version:       v3.0
Date:          2016-05-15
History:       2016-04-25 v1.0实现base64编码逻辑
               2016-05-14 v2.0修改指针bug，实现编码无误
               2016-05-15 v3.0进行单元测试，实现无损文件上传
*****************************************************************************/
#ifndef BASE64STRINGENCODER_H_INCLUDED
#define BASE64STRINGENCODER_H_INCLUDED

/*base64字符映射集。特定指针偏移量与特定字符形成一对映射*/
static const char *codes;

unsigned char* encode_base64(const unsigned char *in,  unsigned long len);

#endif // BASE64STRINGENCODER_H_INCLUDED
