/******************************************************************************
*
* File                	: mqtt.c
* Hardware Environment	: 
* Build Environment   	: 
* Porject				: 
* Version             	: V1.0.0
* By                  	: 
* Date					: 2020. 02. 06
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "mqtt.h"

/* Defines -------------------------------------------------------------------*/
/* Typedef -------------------------------------------------------------------*/
/* Variable ------------------------------------------------------------------*/
MQTT_OPT_s opts =
{
//	.clientid	= MQTT_CLIENT_ID, 
	.host		= MQTT_BROKER_IP, 
	.port		= MQTT_BROKER_PORT,
	.qos		= QOS0,
	.topic		= MQTT_NEW_TOPIC,
	.delimiter	= (char*)"\n",  
	.showtopics	= 1,
};

Network			n;
MQTTClient		c;

MQTT_s			MQTT;
MQTT_FRAME_s	mqtt_frame;

uint08			MQTT_buf[MQTT_BUFSIZE];
uint08			MQTT_readbuf[MQTT_BUFSIZE];

uint08			publish_ID[MQTT_BUFSIZE];

/* Functions -----------------------------------------------------------------*/
void messageArrived(MessageData* md);

int32 MQTT_Init(void)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer; 

	int32 	rc = 0;
	
	NetworkInit(&n);
	NetworkConnect(&n, opts.host, opts.port);
	MQTTClientInit(&c, &n, 1000, MQTT_buf, MQTT_BUFSIZE, MQTT_readbuf, MQTT_BUFSIZE);
    
	data.willFlag			= 0;
	data.MQTTVersion		= 3;
	data.clientID.cstring	= opts.clientid;
	data.username.cstring	= opts.username;
	data.password.cstring	= opts.password;
	data.keepAliveInterval	= 10;
	data.cleansession		= 1;
	
	printf("[INFO ] MQTT Connecting to %s %d\n", opts.host, opts.port);
	
	rc = MQTTConnect(&c, &data);
	
	printf("[INFO ] MQTT Connected %d\n", rc);
    printf("[INFO ] MQTT Subscribing to %s\n", opts.topic);
	
	rc = MQTTSubscribe(&c, opts.topic, opts.qos, messageArrived);
	
	printf("[INFO ] MQTT Subscribed %d\n", rc);
	
	return rc;
}

void MQTT_DeInit(void)
{
	MQTTDisconnect(&c);
	NetworkDisconnect(&n);
	
	printf("[INFO ] MQTT Dis-Connected\n");
}

void messageArrived(MessageData* md)
{
	MQTTMessage*	message = md->message;

	uint08			buf[MQTT_BUFSIZE];
	uint08			len = 0;
	
	memset(message, '\0', sizeof(message));
	memset(&buf,	'\0', sizeof(buf));
	
	printf("%.*s/", md->topicName->lenstring.len, md->topicName->lenstring.data);
	printf("%.*s%s", (int)message->payloadlen, (char*)message->payload, opts.delimiter);
	
	sprintf(&buf[len], "%.*s/", md->topicName->lenstring.len, md->topicName->lenstring.data);
	len += (uint08)md->topicName->lenstring.len + 1;
	sprintf(&buf[len], "%.*s\n", (int)message->payloadlen, (char*)message->payload);
	len += (uint08)message->payloadlen + 1;
	
	MQTT_rxpush(buf, len);
}

void MQTT_YieldProc(void)
{
	MQTTYield(&c, 1000);
}

void MQTT_rxpop(void)
{
	uint08 data = 0;
	
	while(MQTT.q.RX_Push_cnt != MQTT.q.RX_Pop_cnt)
	{
		data = MQTT.q.RX_Buf[MQTT.q.RX_Pop_cnt++];
		MQTT_Parsing(data);

		MQTT.q.RX_Pop_cnt %= BUF_LEN;
	}
}

void MQTT_rxpush(uint08* buf, uint08 len)
{
	uint08 i;
	
	if(len > 0)
	{		
		for(i = 0; i < len; i++)
		{
			MQTT.q.RX_Buf[MQTT.q.RX_Push_cnt++] = buf[i];
					
			MQTT.q.RX_Push_cnt %= BUF_LEN;
		}
	}
}

void MQTT_txpop(void)
{
	uint08	buf[BUF_LEN];
	uint32	len = 0;
	
	while(MQTT.q.TX_Push_cnt != MQTT.q.TX_Pop_cnt)
	{
		buf[len++] = MQTT.q.TX_Buf[MQTT.q.TX_Pop_cnt++];
		
		MQTT.q.TX_Pop_cnt %= BUF_LEN;
	}
}

void MQTT_txpush(uint08* buf, uint08 len)
{
	int32 i;
	
	for(i = 0; i < len ; i++)
	{
		MQTT.q.TX_Buf[MQTT.q.TX_Push_cnt++] = buf[i];

		MQTT.q.TX_Push_cnt %= BUF_LEN;
	}
}

void MQTT_Parsing(uint08 data)
{
	switch(mqtt_frame.status)
	{
		case MQTT_STATUS_HEADER :
		
			if(data != '/')
			{
				if((data == 'M') && (mqtt_frame.index == 0))
				{
					memset(&mqtt_frame, 0x00, sizeof(mqtt_frame));
					
					mqtt_frame.header[mqtt_frame.index ++] = data;
				}
				else if((data == 'U') && (mqtt_frame.index == 1))
				{
					mqtt_frame.header[mqtt_frame.index ++] = data;
				}
				else if((data == 'V') && (mqtt_frame.index == 2))
				{
					mqtt_frame.header[mqtt_frame.index ++] = data;
				}
				else
				{
					memset(&mqtt_frame, 0x00, sizeof(mqtt_frame));
				}
				
				mqtt_frame.status = MQTT_STATUS_HEADER;
			}
			else
			{
				if((mqtt_frame.index == 3) && (strcmp(mqtt_frame.header, (uint08*)"MUV") == 0))
				{
					mqtt_frame.index = 0;
					
//					mqtt_frame.status = MQTT_STATUS_FROM;
					mqtt_frame.status = MQTT_STATUS_TYPE;
				}
				else
				{
					memset(&mqtt_frame, 0x00, sizeof(mqtt_frame));
					
					mqtt_frame.status = MQTT_STATUS_HEADER;
				}
			}
			
			break;

		case MQTT_STATUS_TYPE :
			
			if(data != '/')
			{
				mqtt_frame.cmd[mqtt_frame.index ++] = data;
				
				mqtt_frame.status = MQTT_STATUS_TYPE;
			}
			else
			{
				mqtt_frame.index = 0;
				
				mqtt_frame.status = MQTT_STATUS_LIB;
			}
				
			break;
			
		case MQTT_STATUS_LIB :
		
			if(data != '/')
			{
				mqtt_frame.lib[mqtt_frame.index ++] = data;
				
				mqtt_frame.status = MQTT_STATUS_LIB;
			}
			else
			{
				mqtt_frame.index = 0;
				
				mqtt_frame.status = MQTT_STATUS_CONTAIN;
			}
		
			break;
			
		case MQTT_STATUS_CONTAIN :
		
			if(data != '/')
			{
				mqtt_frame.contain[mqtt_frame.index ++] = data;
				
				mqtt_frame.status = MQTT_STATUS_CONTAIN;
			}
			else
			{
				mqtt_frame.index = 0;
				
				mqtt_frame.status = MQTT_STATUS_MESSAGE;
			}
		
			break;

		case MQTT_STATUS_MESSAGE :
		
			mqtt_frame.data[mqtt_frame.index ++] = data;
			
			if(data != '\n')
			{
				mqtt_frame.contain[mqtt_frame.index ++] = data;
				
				mqtt_frame.status = MQTT_STATUS_MESSAGE;
			}
			else
			{
				MQTT_Command();
				
				mqtt_frame.index = 0;
				
				mqtt_frame.status = MQTT_STATUS_HEADER;
			}
			
			break;

		default :
		
			mqtt_frame.status = MQTT_STATUS_HEADER;
			
			break;
	}
}

void MQTT_Command(void)
{
	uint08 command[MQTT_BUFSIZE];
	uint08 x, y;
	
	uint08 tmp[MQTT_BUFSIZE];
	uint08 i, index, len, status;
		
	memset(command, 0x00, sizeof(command));
	x = 0;
	y = 0;
/*	
	for(i = 0, index = 0, len = 0, status = 0; mqtt_frame.data[i] != '\n'; i++)
	{
		switch(index)
		{
			case 0 :
				
				if((data == 'c') && (index == 0))
				{
					memset(&tmp, 0x00, sizeof(tmp));
					
					index ++;
				}
				else if((data == 'o') && (index == 1))
				{
					index ++;
				}
				else if((data == 'n') && (mqtt_frame.index == 2))
				{
					index ++;
				}
				else
				{
					memset(&tmp, 0x00, sizeof(tmp));
					
					index = 0;
				}
				
				status = 1;
			
				break;
				
			case 1 :
			
				x = x << 4;
				x +=  mqtt_frame.data[i] - '0';
			
				break;
				
			case 2 :
			
				y = y << 4;
				y +=  mqtt_frame.data[i] - '0';
			
				break;
		}
	}

	if	(strcmp(mqtt_frame.cmd, MOTT_TOPIC_HANDSHAKE) == 0)
	{
		
	}
	else if(strcmp(mqtt_frame.cmd, MOTT_TOPIC_COMMAND) == 0)
	{
		if(strcmp(command, MOTT_CMD_ALIVE) == 0)
		{
			Protocol_RequestAlive();
		}
		else if(strcmp(command, MOTT_CMD_OPEN) == 0)
		{
			Protocol_RequestEnable(x, y);
		}
		else if(strcmp(command, MOTT_CMD_STATUS) == 0)
		{
			Protocol_RequestStatus();
		}
		else
		{
			
		}
	}
	else
	{
		
	}
*/
}

void MQTT_ResponseAlive(void)
{
	uint08 buf[MQTT_BUFSIZE];	
	size_t len = 0;
	
//	sprintf(buf, "%s", MOTT_CMD_ALIVE);
	len = strlen(buf);
	
	MQTT_Packing(buf, len);
}

void MQTT_ResponseOpen(uint08 x, uint08 y, uint08 result)
{
	uint08 buf[MQTT_BUFSIZE];	
	size_t len = 0;
	
	sprintf(buf, "%s %02d %02d %02d", MOTT_CMD_OPEN, x, y, result);
	len = strlen(buf);
	
	MQTT_Packing(buf, len);
}

void MQTT_ResponseStatus(uint08* result)
{
	uint08 buf[MQTT_BUFSIZE];	
	size_t len = 0;
	uint08 i;
	
//	sprintf(&buf[len], "%s ", MOTT_CMD_STATUS);
	len += strlen(buf);

	for(i = 0; i < 8; i ++)
	{
		if(i != 7)	sprintf(&buf[len++], "%02d ",	result[i]);
		else		sprintf(&buf[len++], "%02d",	result[i]);
		
		len += 2;
	}
	len = strlen(buf);
	
	MQTT_Packing(buf, len);
}

void MQTT_Packing(uint08* buf, size_t len)
{
	MQTTMessage	message;
	uint08 		topic[MQTT_BUFSIZE];
	
	memset(&message, 0x00, sizeof(message));
	memset(topic,	0x00, sizeof(topic));

//	sprintf(topic, "MUV/%s/%s/%s",	MOTT_TOPIC_DATA, 
//									MQTT_LIBRARY_NAME, 
//									MQTT_CONTAIN_NAME);

	message.payload = buf;
	message.payloadlen = (size_t)len;
	
//	printf("%s/",	topic);
//	printf("%.*s [%d] \n", (int)message.payloadlen, (char*)message.payload, len);

	MQTTPublish(&c, topic, &message);
}