/*****************************************************************************
Copyright:     2016-2017, Firework.
File name:     thread.h
Description:   此文件主要完成家庭网关系统按键录音上传和紧急呼叫功能。
Author:        YU SHAO YANG
Version:       2.0
Date:          2016.05.15
History:       2016.04.30 修改函数名TEST为Voice_Thread,其他同前
               2016.05.15 修改Voice_Thread函数，实现声音上传功能
                          修改Emergency_Thread_Level函数，实现紧急呼叫功能
*****************************************************************************/
#include"thread.h"
#include"cJSON.h"
#include"makeJson.h"
#include"base64StringEncoder.h"
#include"readWav.h"
#include"socket.h"
#include<stdio.h>
#include<pthread.h>
#include<sched.h>
#include<stdlib.h>

/*设置互斥量，避免多线程同时访问*/
pthread_mutexattr_t mutex;

/*设置记录型信号量，记录当前状态*/
volatile int recordmutex = 0;

/*设置状态量，记录system函数的反馈值*/
int RC_status = 0;
int KL_status = 0;
//int RC_pid=0;

/*设置文件名，记录录音文件个数*/
int filenumber = 1;

/*数据预发送缓冲区，引用自socket.c*/
extern unsigned char * raw_send_buffer;



/*************************************************
Function:       create_wav_thread
Description:    该函数用于执行按键录音和上传工作。
Calls:          pthread.h(
                            int pthread_mutex_init(pthread_mutex_t *restrict mutex,
                                                   const pthread_mutexattr_t *restrict attr);
                            int pthread_mutex_lock(pthread_mutex_t *mutex);
                            int pthread_mutex_unlock(pthread_mutex_t *mutex);
                         )
                stdio.h(
                            int sprintf( char *buffer, const char *format, [ argument] … );
                            void perror ( const char * str );
                       )
                stdlib.h(
                            int system(const char * string);
                            void *malloc(unsigned int num_bytes);
                            void free(void *ptr);

                        )
Called By:      thread.c
Input:          NONE
Output:
Return:         NONE
Others:         NONE
*************************************************/
void create_wav_thread(){

    char buffer[100] = {0};/*设置命令缓冲区*/
    char *filename;/*存储要创建的音频文件名*/
    unsigned char *input_file_char;/*存储读取的wav文件*/
    unsigned char *output_file_base64;/*存储base64编码后的文件*/
    cJSON* data;/*存储cJSON数据域的内容*/


    /*设置录音线程为最高优先级*/
    (void)set_priority(30);

    /*初始化互斥量*/
    pthread_mutex_init(&mutex,NULL);

	//printf("此次按键信息已经成功创建线程1\n");

	/*此处为首次按下按钮进入录音模式*/
	if(recordmutex==0)
    {

		pthread_mutex_lock (&mutex);
		++recordmutex;/*修改记录型信号量，标记状态*/
		pthread_mutex_unlock (&mutex);

        /*制作语音录制命令：“plughw:1,0” 表示的是使用外置声卡*/
		sprintf(buffer,"cd /home/pi/Voice && sudo arecord -D \"plughw:1,0\" %d.wav",filenumber);

		/*执行录音指令*/
		RC_status = system(buffer);

		/*设置执行错误反馈*/
		if(RC_status==-1)
		{
            perror("arecord open falled:");
            return;
        }
	}

	/*此处为录音结束并对音频文件进行编码和上传*/
	else if(recordmutex==1)
    {
        /*等待音频文件尾部录音完成*/
        delay_ms(1500);

		pthread_mutex_lock (&mutex);
		--recordmutex;/*修改记录型信号量，标记状态*/
		pthread_mutex_unlock (&mutex);

        /*停止录音*/
		KL_status = system("killall arecord");

		/*设置执行错误反馈*/
		if(KL_status==-1){
            perror("arecord kill falled:" );
        }

        /*将要创建的音频文件名写入变量filename*/
        filename = (char*)malloc(25);
        sprintf(filename, "/home/pi/Voice/%d.wav",filenumber);

        /*读取wav文件并将其转换为Byte格式*/
        input_file_char = read_wav(filename);

        /*将Byte格式的文件进行BASE64编码*/
        output_file_base64 =encode_base64(input_file_char, get_file_length(filename));

        /*组装cJSON的数据域*/
        data = generate_data("fileType", "1", "file", output_file_base64);

        /*将制作完成的JSON数据包放入发送缓冲区*/
        raw_send_buffer = create_json_packet("1211", "fileUp", "20160001", data);

        /*设置下次音频文件名*/
        filenumber++;

		/*使用下面的方法也可以实现停止录音*/
		/*RC_pid = system("pidof arecord");
		char buffer[100] = {0};
		sprintf(buffer,"kill -2 %d",RC_pid);*/

		/*malloc收尾工作*/
		free(filename);
		filename = NULL;
	}
}


/*************************************************
Function:       create_emergency_LV1
Description:    该函数用于执行按键紧急呼叫工作。
Calls:
Called By:      thread.c
Input:          NONE
Output:
Return:         NONE
Others:         NONE
*************************************************/
void create_emergency_LV1()
{

	(void)set_priority(10) ;

	//printf("此次按键信息已经成功创建线程2\n");

	/*制作一级紧急呼叫信息包的数据域*/
    cJSON * data = generate_data("level", "1", NULL, NULL);

    /*将制作完成的JSON数据包放入发送缓冲区*/
    raw_send_buffer = create_json_packet("1110", "emergency", "20160001",data);
}

void create_emergency_LV2()
{

	(void)set_priority(10);

	//printf("此次按键信息已经成功创建线程3\n");

    /*制作二级紧急呼叫信息包的数据域*/
    cJSON * data = generate_data("level", "2", NULL, NULL);

    /*将制作完成的JSON数据包放入发送缓冲区*/
    raw_send_buffer = create_json_packet("1110", "emergency", "20160001",data);
}

void create_emergency_LV3()
{

	(void)set_priority(10);

	//printf("此次按键信息已经成功创建线程4\n");

	/*制作三级紧急呼叫信息包的数据域*/
    cJSON * data = generate_data("level", "3", NULL, NULL);

     /*将制作完成的JSON数据包放入发送缓冲区*/
    raw_send_buffer = create_json_packet("1110", "emergency", "20160001",data);

	//system("omxplayer -o local /home/pi/t1.wav");//本地录音测试功能
}


/*************************************************
Function:       set_priority
Description:    该函数用于执行各线程优先级动态修改工作。
Calls:          sched.h
Called By:      thread.c
Input:          const int pri
Output:         NONE
Return:         NONE
Others:         NONE
*************************************************/
int set_priority(const int pri)
{
    struct sched_param sched ;

    memset (&sched, 0, sizeof(sched)) ;

    if (pri > sched_get_priority_max (SCHED_RR))
    {
      sched.sched_priority = sched_get_priority_max (SCHED_RR) ;
    }

    else
    {
      sched.sched_priority = pri ;
    }

    return sched_setscheduler (0, SCHED_RR, &sched) ;
}
