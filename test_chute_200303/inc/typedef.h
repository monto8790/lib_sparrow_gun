/******************************************************************************
*
* File                	: typedef.h
* Hardware Environment	: 
* Build Environment   	: 
* Porject				: 
* Version             	: V1.0.0
* By                  	: 
* Date					: 2002. 02. 03
*
*******************************************************************************/
#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

/* Defines -------------------------------------------------------------------*/
#define MQTTCLIENT_PLATFORM_HEADER					MQTTLinux.h

#define OPEN										1
#define CLOSED										0
#define ERROR										-1

#define BUF_LEN										1024

/* Typedef -------------------------------------------------------------------*/
typedef unsigned char												uint08;
typedef unsigned short												uint16;
typedef unsigned int												uint32;

typedef char														int08;
typedef short														int16;
typedef int															int32;

typedef struct
{
	uint08 RX_Buf[BUF_LEN];
	uint16 RX_Push_cnt;
	uint16 RX_Pop_cnt;
	
	uint08 TX_Buf[BUF_LEN];
	uint16 TX_Push_cnt;
	uint16 TX_Pop_cnt;
}QUEUE_s;

/* Variable ------------------------------------------------------------------*/
extern bool stop_f;

/* Functions -----------------------------------------------------------------*/

#endif
/* End Of File ---------------------------------------------------------------*/