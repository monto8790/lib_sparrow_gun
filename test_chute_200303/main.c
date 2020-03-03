/******************************************************************************
*
* File                	: main.c
* Hardware Environment	: 
* Build Environment   	: 
* Project				: 
* Version             	: V1.0.0
* By                  	: 
* Date					: 2020. 02. 03
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"

#include "protocol.h"
#include "mqtt.h"

/* Defines -------------------------------------------------------------------*/
/* Typedef -------------------------------------------------------------------*/
/* Variable ------------------------------------------------------------------*/


pthread_t	time_tid = 0;

bool 		stop_f = false;

/* Functions -----------------------------------------------------------------*/
void main_quit(int sig);

int main(int argc, char** argv)
{
	uint32 port;
	
	signal(SIGINT, main_quit);
	
	// Serial INIT
	for(port = 0; port < USART_MAX; port ++)
	{
		if(Serial_Init(port, B9600) == ERROR)
		{
			return ERROR;
		}
	}

	MQTT_Init();
	
	Protocol_Init();

	while(!stop_f)
	{
		Serial_rxpop();
		MQTT_rxpop();
		
		MQTT_YieldProc();
	}

	return 0;
}

void main_quit(int sig)																// this function is called when you press Ctrl-C
{
	stop_f = true;
	
	int port;
	
	printf("[INFO ] Terminating at User Request \n");
	
	for(port = 0; port < USART_MAX; port ++)
	{
		MQTT_DeInit();
		
		Serial_DeInit(port);
	}
}