/******************************************************************************
*
* File                	: serial.h
* Hardware Environment	: 
* Build Environment   	: 
* Porject				: 
* Version             	: V1.0.0
* By                  	: 
* Date					: 2020. 02. 03
*
*******************************************************************************/
#ifndef _SERIAL_H_
#define _SERIAL_H_

/* Includes ------------------------------------------------------------------*/
#include <termios.h>
#include <fcntl.h>

#include "typedef.h"

#include "protocol.h"

/* Defines -------------------------------------------------------------------*/
#define USART_MAX												1

#define USART1													"/dev/ttyS1"
#define USART2													"/dev/ttyO2"
#define USART4													"/dev/ttyO4"
#define USART5													"/dev/ttyO5"

/* Typedef -------------------------------------------------------------------*/
typedef enum
{
	usart_1,
	usart_2,
	usart_4,
	usart_5,
}e_SRIAL_PORT;

typedef struct
{
	int32		fd;
	uint08		connect; 	

	QUEUE_s		q;
	pthread_t	rx_tid;
	pthread_t	tx_tid;
}SERIAL_s;

/* Functions -----------------------------------------------------------------*/
// Initialize
int		Serial_Init(e_SRIAL_PORT port, speed_t baud);
int 	Serial_DeInit(e_SRIAL_PORT port);

// Read & Write
void	Serial_rxpop(void);
void	Serial_rxpush(e_SRIAL_PORT port);
void	Serial_txpop(e_SRIAL_PORT port);
void	Serial_txpush(e_SRIAL_PORT port, uint08* buf, uint08 len);

// Thread
void*	Serial_rxThread(void*);
void*	Serial_txThread(void*);

#endif
/* End Of File ---------------------------------------------------------------*/