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
#include "serial.h"

/* Defines -------------------------------------------------------------------*/
/* Typedef -------------------------------------------------------------------*/
/* Variable ------------------------------------------------------------------*/
SERIAL_s	serial[USART_MAX];

/* Functions -----------------------------------------------------------------*/
int Serial_Init(e_SRIAL_PORT port, speed_t baud)
{
	struct	termios new_setting;

	char* device;
	
	switch(port)
	{
		case usart_1 :
		
			device = USART1;
		
			break;
			
		case usart_2 :
		
			device = USART2;
			
			break;
			
		case usart_4 :
		
			device = USART4;
			
			break;
			
		case usart_5 :
		
			device = USART5;
			
			break;
			
		default :
		
			printf("ERROR : Nonexistent Port \n");
			
			return ERROR;
			
			break;
	}
	
	serial[port].fd = open(device, O_RDWR| O_NOCTTY | O_NDELAY);									// 읽기/쓰기 모드로 모뎀 장치를 연다.(O_RDWR)
	
	if (serial[port].fd < 0)
	{
		printf("[ERROR] Serial Connected Fail to %s \n", device);
	
		return ERROR;
	}
	
	// Check file descriptor
	if(!isatty(serial[port].fd))
	{
		printf("[ERROR] file descriptor %d is NOT a Serial port \n", serial[port].fd);
		
		return ERROR;
	}
	
	tcgetattr(serial[port].fd, &new_setting);														// save current serial port settings
		
	/*
		< Control modes >
		CSIZE	: Character size : 
					- CS5 : 5 bits. 
					CS66 bits. 
					CS77 bits. 
					CS88 bits. 
		CSTOPB	: Send two stop bits, else one. 
		CREAD	: Enable receiver.		
		PARENB	: Parity enable. 
		PARODD	: Odd parity, else even. 
		HUPCL	: Hang up on last close. 
		CLOCAL	: Ignore modem status lines. 
	*/
	new_setting.c_cflag = baud | CS8 | CLOCAL | CREAD;	
		
	/*
		< Input modes >
		BRKINT	: Signal interrupt on break.
		ICRNL	: Map CR to NL on input.
		IGNBRK	: Ignore break condition.
		IGNCR	: Ignore CR.
		IGNPAR	: Ignore characters with parity errors.
		INLCR	: Map NL to CR on input. 
		INPCK	: Enable input parity check. 
		ISTRIP	: Strip character
		IUCLC	: Map upper-case to lower-case on input (LEGACY).
		IXANY	: Enable any character to restart output.
		IXOFF	: Enable start/stop input control.
		IXON	: Enable start/stop output control.
		PARMRK	: Mark parity errors. 
	*/
	new_setting.c_iflag = IGNPAR;
	
	/*
		< Output modes >
		OPOST	: Post-process output. 
		OLCUC	: Map lower-case to upper-case on output (LEGACY).
		ONLCR	: Map NL to CR-NL on output.
		OCRNL	: Map CR to NL on output.
		ONOCR	: No CR output at column 0.
		ONLRET	: NL performs CR function.
		OFILL	: Use fill characters for delay.
		NLDLY	: Select newline delays : 
					- NL0 : Newline character type 0.
					- NL1 : Newline character type 1.
		CRDLY	: Select carriage-return delays :
					- CR0 : Carriage-return delay type 0. 
					- CR1 : Carriage-return delay type 1. 
					- CR2 : Carriage-return delay type 2. 
					- CR3 : Carriage-return delay type 3. 
		TABDLY	: Select horizontal-tab delays : 
					- TAB0 : Horizontal-tab delay type 0. 
					- TAB1 : Horizontal-tab delay type 1.
					- TAB2 : Horizontal-tab delay type 2. 
					- TAB3 : Expand tabs to spaces. 
		BSDLY	: Select backspace delays : 
					- BS0 : Backspace-delay type 0. 
					- BS1 : Backspace-delay type 1. 
		VTDLY	: Select vertical-tab delays : 
					- VT0 : Vertical-tab delay type 0. 
					- VT1 : Vertical-tab delay type 1. 
		FFDLY	: Select form-feed delays :
					- FF0 : Form-feed delay type 0.
					- FF1 : Form-feed delay type 1. 
	*/
	new_setting.c_oflag = 0;

	/*
		< Local Modes >
		ECHO	: Enable echo.
		ECHOE	: Echo erase character as error-correcting backspace.
		ECHOK	: Echo KILL. 
		ECHONL	: Echo NL. 
		ICANON	: Canonical input (erase and kill processing). 
		IEXTEN	: Enable extended input character processing.
		ISIG	: Enable signals. 
		NOFLSH	: Disable flush after interrupt or quit.
		TOSTOP	: Send SIGTTOU for background output.
		XCASE	: Canonical upper/lower presentation (LEGACY). 
	*/
	new_setting.c_lflag = 0;

	// One input byte is enough to return from read()
	// Inter-character timer off
	new_setting.c_cc[VMIN]  = 1;
	new_setting.c_cc[VTIME] = 0; // was 0
	
	tcflush(serial[port].fd, TCIFLUSH);
	tcsetattr(serial[port].fd, TCSANOW, &new_setting);
		
	printf("[INFO ] Connected to %s with 115200 baud, 8 data bits, no parity, 1 stop bit (8N1)\n", device);
		
	// Serial Read Thread Create
	if(pthread_create(&serial[port].rx_tid, NULL, &Serial_rxThread, (void*)port))	
	{
		printf("[ERROR] Failed Creat Pthread for Serial_RX. \n");
		
		return ERROR;
	}
	
	// Serial Write Thread Create
	if(pthread_create(&serial[port].tx_tid, NULL, &Serial_txThread, (void*)port))	
	{
		printf("[ERROR] Failed Creat Pthread for SerialTX. \n");
		
		return ERROR;
	}
	
	serial[port].connect = 0x01;
	
	return 0;
}

int Serial_DeInit(e_SRIAL_PORT port)
{
	char* device;
	
	switch(port)
	{
		case usart_1 :
		
			device = USART1;
		
			break;
			
		case usart_2 :
		
			device = USART2;
			
			break;
			
		case usart_4 :
		
			device = USART4;
			
			break;
			
		case usart_5 :
		
			device = USART5;
			
			break;
			
		default :
		
			printf("[ERROR] Error on Nonexistent Port \n");
			
			return ERROR;
			
			break;
	}
	
	if(serial[port].connect)
	{	
		if(close(serial[port].fd) == ERROR)
		{
			printf("[ERROR] Error on port %s close \n", device);
			
			return ERROR;
		}
		
		serial[port].connect = 0x00;
		printf("[INFO ] Port %s Close \n", device);
	}
	
	return 0;
}

void Serial_rxpop(void)
{
	uint08 data = 0;

	uint08 port;
	
	for(port = 0; port < USART_MAX; port ++)
	{		
		while(serial[port].q.RX_Push_cnt != serial[port].q.RX_Pop_cnt)
		{
			data = serial[port].q.RX_Buf[serial[port].q.RX_Pop_cnt++];
			
			Protocol_Parsing(data);
			
			serial[port].q.RX_Pop_cnt %= BUF_LEN;
		}
	}
}

void Serial_rxpush(e_SRIAL_PORT port)
{
	char data[256];
	int len = 0;
	
	len = read(serial[port].fd, data, 256);
	
	if(len > 0)
	{		
		for(int i = 0; i < len; i++)
		{
			serial[port].q.RX_Buf[serial[port].q.RX_Push_cnt++] = data[i];
					
			serial[port].q.RX_Push_cnt %= BUF_LEN;
		}
	}
}

void Serial_txpop(e_SRIAL_PORT port)
{
	uint08	buf[BUF_LEN];
	uint32	len = 0;
	
	while(serial[port].q.TX_Push_cnt != serial[port].q.TX_Pop_cnt)
	{
		buf[len++] = serial[port].q.TX_Buf[serial[port].q.TX_Pop_cnt++];
		
		serial[port].q.TX_Pop_cnt %= BUF_LEN;
	}
	
	if(len > 0)
	{
		if(write(serial[port].fd, buf, len) < 0)
		{
			printf("[ERROR] \n");
		}
	}
}

void Serial_txpush(e_SRIAL_PORT port, uint08* buf, uint08 len)
{
	int32 i;
	
	if(serial[port].connect)
	{
		for(i = 0; i < len ; i++)
		{
			serial[port].q.TX_Buf[serial[port].q.TX_Push_cnt++] = buf[i];

			serial[port].q.TX_Push_cnt %= BUF_LEN;
		}
	}
}

// Thread
void* Serial_rxThread(void* arg)
{
	int32 port;
	
	port = (int)arg;
	
	while(!stop_f)
	{		
		if(serial[port].connect)
		{
			Serial_rxpush(port);
		}
		
		usleep(500);
	}
}

void* Serial_txThread(void* arg)
{
	int32 port;
	
	port = (int)arg;
	
	while(!stop_f)
	{
		if(serial[port].connect)
		{
			Serial_txpop(port);
		}
		
		usleep(500);
	}
}

/* End Of File ---------------------------------------------------------------*/