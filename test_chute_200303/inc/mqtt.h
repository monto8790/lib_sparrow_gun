/******************************************************************************
*
* File                	: mqtt.h
* Hardware Environment	: 
* Build Environment   	: 
* Porject				: 
* Version             	: V1.0.0
* By                  	: 
* Date					: 2020. 02. 06
*
*******************************************************************************/
#ifndef _MQTT_H
#define _MQTT_H

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"

#include "protocol.h"

#include "../MQTT/MQTTClient.h"

/* Defines -------------------------------------------------------------------*/
#define MQTT_BUFSIZE						256

#define	MQTT_BROKER_IP						(char*)"localhost"
#define MQTT_BROKER_PORT					1883

#define MQTT_NEW_TOPIC						(char*)"MUV/+/lib_sparrow_gun/MICRO"
#define MQTT_LIBRARY_NAME					(char*)"lib_sparrow_gun"
#define MQTT_CONTAIN_NAME					(char*)"MICRO"

/* Typedef -------------------------------------------------------------------*/
typedef enum
{
	MQTT_STATUS_HEADER,	
	MQTT_STATUS_TYPE,
	MQTT_STATUS_LIB,
	MQTT_STATUS_CONTAIN,
	MQTT_STATUS_MESSAGE,
}MQTT_STATUS_e;

typedef struct
{
	char*		clientid;
	char*		username;
	char*		password;
	char*		host;
	int			port;
	enum QoS	qos;
	
	char*		topic;
	
	int			nodelimiter;
	char*		delimiter;
	
	int			showtopics;
} MQTT_OPT_s;

typedef struct
{
	QUEUE_s		q;
	
	pthread_t	rx_tid;
	pthread_t	tx_tid;
}MQTT_s;

typedef struct
{
	MQTT_STATUS_e		status;
	uint08				index;
	
	uint08				header[MQTT_BUFSIZE];
	uint08				lib[MQTT_BUFSIZE];
	uint08				contain[MQTT_BUFSIZE];
	uint08				cmd[MQTT_BUFSIZE];
	uint08				data[MQTT_BUFSIZE];
}MQTT_FRAME_s;

/* Variable ------------------------------------------------------------------*/
extern MQTT_FRAME_s		mqtt_frame;	

/* Functions -----------------------------------------------------------------*/
int32	MQTT_Init(void);
void 	MQTT_DeInit(void);

void	MQTT_YieldProc(void);
void	MQTT_Parsing(uint08 data);

void	MQTT_Command(void);

void	MQTT_ResponseAlive(void);
void	MQTT_ResponseOpen(uint08 x, uint08 y, uint08 result);
void	MQTT_ResponseStatus(uint08* result);
void	MQTT_Packing(uint08* buf, size_t len);

// Read & Write
void	MQTT_rxpop(void);
void	MQTT_rxpush(uint08* buf, uint08 len);
void	MQTT_txpop(void);
void	MQTT_txpush(uint08* buf, uint08 len);

void*	MQTT_ProcThread(void* args);

#endif

