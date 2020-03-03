/******************************************************************************
*
* File                	: protocol.c
* Hardware Environment	: 
* Build Environment   	: 
* Porject				: 
* Version             	: V1.0.0
* By                  	: 
* Date					: 2020. 02. 03
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "protocol.h"

/* Defines -------------------------------------------------------------------*/
/* Typedef -------------------------------------------------------------------*/
/* Variable ------------------------------------------------------------------*/
PROTOCOL_FRAME_s protocol_frame;

/* Functions -----------------------------------------------------------------*/
int08 Protocol_Init(void)
{
	memset(&protocol_frame, 0x00, sizeof(protocol_frame));
}

void Protocol_Parsing(uint08 data)
{
	switch(protocol_frame.status)
	{
		case PROTOCOL_STATUS_STX :

			if(data == PROTOCOL_STX)
			{
				memset(&protocol_frame, 0x00, sizeof(protocol_frame));
			
				protocol_frame.status = PROTOCOL_STATUS_COMMAND;
			}
			else
			{
				protocol_frame.status = PROTOCOL_STATUS_STX;
			}
			
			break;

		case PROTOCOL_STATUS_COMMAND :

			protocol_frame.cmd = data;
			Protocol_CalCRC(data);
			
			protocol_frame.status = PROTOCOL_STATUS_PAYLOAD;
			
			break;

		case PROTOCOL_STATUS_PAYLOAD :

			protocol_frame.data[protocol_frame.index++] = data;
			Protocol_CalCRC(data);
			
			if(protocol_frame.index >= PROTOCOL_PAYLOADSIZE)
			{
				protocol_frame.status = PROTOCOL_STATUS_CHECKSUM;
			}

			break;

		case PROTOCOL_STATUS_CHECKSUM :

			if(protocol_frame.crc == data)
			{
				Protocol_Response();
				
				protocol_frame.status = PROTOCOL_STATUS_ETX;
			}
			else
			{
				protocol_frame.status = PROTOCOL_STATUS_STX;
			}
			
			break;

		case PROTOCOL_STATUS_ETX :

			protocol_frame.status = PROTOCOL_STATUS_STX;

			break;

		default :

			protocol_frame.status = PROTOCOL_STATUS_STX;

			break;
	}
}

void Protocol_Response(void)
{
	switch(protocol_frame.cmd)
	{
		case PROTOCOL_RES_ALIVE :
		
			MQTT_ResponseAlive();
		
			break;
			
		case PROTOCOL_RES_ENABLE :
		
			MQTT_ResponseOpen(protocol_frame.data[0], protocol_frame.data[1], protocol_frame.data[2]);
		
			break;
			
		case PROTOCOL_RES_STATUS :
		
			MQTT_ResponseStatus(protocol_frame.data);
		
			break;
	}
}

void Protocol_RequestAlive(void)
{
	uint08 buf[PROTOCOL_BUFSIZE];
	uint08 len = 0;
	
	memset(buf, 0x00, sizeof(buf));
	
	buf[len++] = PROTOCOL_REQ_ALIVE;
	
	Protocol_Packing(buf);
}

void Protocol_RequestEnable(uint08 x, uint08 y)
{
	uint08 buf[PROTOCOL_BUFSIZE];
	uint08 len = 0;
	
	memset(buf, 0x00, sizeof(buf));
	
	buf[len++] = PROTOCOL_REQ_ENABLE;
	
	buf[len++] = x;
	buf[len++] = y;
	
	Protocol_Packing(buf);
}

void Protocol_RequestStatus(void)
{
	uint08 buf[PROTOCOL_BUFSIZE];
	uint08 len = 0;
	
	memset(buf, 0x00, sizeof(buf));
	
	buf[len++] = PROTOCOL_REQ_STATUS;
	
	Protocol_Packing(buf);
}

void Protocol_Packing(uint08* frame)
{
	uint08 buf[PROTOCOL_BUFSIZE];
	uint08 i, crc = 0x00;
	uint08 len = 0;
	
	memset(buf, 0x00, sizeof(buf));
	
	buf[len++] = PROTOCOL_STX;

	for(i = 0; i < PROTOCOL_PAYLOADSIZE + 1; i++, len++)
	{
		buf[len] = frame[i];
		crc ^= frame[i];
	}
	
	buf[len++] = crc;
	buf[len++] = PROTOCOL_ETX;
	
	Serial_txpush(usart_1, buf, len);
}

void Protocol_CalCRC(uint08 data)
{
	protocol_frame.crc ^= data;
}

uint08 Protocol_CheckCRC(void)
{
	int08	result = 0x00;
	uint08	i, crc = 0x00;
	
	crc ^= protocol_frame.cmd;
	for(i = 0; i < PROTOCOL_PAYLOADSIZE; i++)
	{
		crc ^= protocol_frame.data[i];
	}
		
	if(crc == protocol_frame.crc)
	{
		result = 0x01;
	}
	else
	{
		result = 0x00;
	}
	
	printf("%d \n", result);
	
	return result;
}