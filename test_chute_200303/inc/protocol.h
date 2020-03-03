/******************************************************************************
*
* File                	: protocol.h
* Hardware Environment	: 
* Build Environment   	: 
* Porject				: 
* Version             	: V1.0.0
* By                  	: 
* Date					: 2020. 02. 03
*
*******************************************************************************/
#ifndef _PROTOCOL_H
#define _PROTOCOL_H

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"

#include "serial.h"
#include "mqtt.h"

/* Defines -------------------------------------------------------------------*/
#define PROTOCOL_BUFSIZE												256
#define PROTOCOL_PAYLOADSIZE											8

#define PROTOCOL_STX													0xA2			// Start
#define PROTOCOL_ETX													0xA3			// End

#define PROTOCOL_REQ_ALIVE												0x01
#define PROTOCOL_RES_ALIVE												0x02
#define PROTOCOL_REQ_ENABLE												0x03
#define PROTOCOL_RES_ENABLE												0x04
#define PROTOCOL_REQ_STATUS												0x05
#define PROTOCOL_RES_STATUS												0x06

#define PROTOCOL_RESERVE												0xFF

/* Typedef -------------------------------------------------------------------*/
typedef enum
{
	PROTOCOL_SEQ_INIT,
	PROTOCOL_SEQ_ALIVE,
	PROTOCOL_SEQ_ENABLE,
	PROTOCOL_SEQ_STATUS,
}PROTOCOL_SEQ_e;

typedef enum
{
	PROTOCOL_STATUS_STX,
	PROTOCOL_STATUS_COMMAND,
	PROTOCOL_STATUS_PAYLOAD,
	PROTOCOL_STATUS_CHECKSUM,
	PROTOCOL_STATUS_ETX,
}PROTOCOL_STATUS_e;

typedef struct
{
	PROTOCOL_STATUS_e	status;
	uint08				index;
	
	uint08				cmd;
	uint08				data[PROTOCOL_BUFSIZE];
	uint08				crc;

	pthread_t			proc_tid;
}PROTOCOL_FRAME_s;

/* Variable ------------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
int08	Protocol_Init(void);

void	Protocol_Parsing(uint08 data);
void	Protocol_CalCRC(uint08 data);
uint08	Protocol_CheckCRC(void);

void	Protocol_Response(void);

void	Protocol_RequestAlive(void);
void	Protocol_RequestEnable(uint08 x, uint08 y);
void	Protocol_RequestStatus(void);
void	Protocol_Packing(uint08* frame);

#endif

