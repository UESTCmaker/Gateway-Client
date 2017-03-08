#include<stdio.h>
#include<wiringPi.h>
#include<stdlib.h>
#include"sysinit.h"
#include"makeJson.h"
#include"resolveJson.h"

//设置每轮中断响应时间
#define InterruptDelay 150

//char *json;
unsigned char *pack;
char *unpack;
//char *bu;

int main ()
{
    //char *json;
    //char *pack;
    //json = get_date();
    //pack = get_time();
    //printf("%s\n%s",json,pack);


    gateway_init();
    create_socket();
	while(1){delay(InterruptDelay);}

	/*cJSON * data = generate_data("level", "2", "fileType", "VOLT");
    json = make_json_raw("1100","tick","20160001",data);
    printf("%s\n", json);
    //printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    bu = getJsonStringValue(json,"data", "fileType");
    printf("%s",bu);*/

    /*pack = pack_json(json);
    unpack = unpack_json(pack);
    printf("%s",json);
    printf("\n==========pack===========\n");
    printf("%s",pack+4);
    printf("\n=========unpack==========\n");
    printf("%s",unpack);*/

    return 0;
}


