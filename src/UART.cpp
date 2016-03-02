/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "UART.h"
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>


int uart0_filestream = -1;


bool missionStart = false;


bool UART::getMissionStart() 
{
    return missionStart;
}

void UART::setMissionStart(bool val) 
{
    missionStart = val;
}

void UART::init()
{
    printf("Trying to open UART...\n");
    uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
                
	}
    struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
        
    printf("UART is open\n");
}

/*
 * used for sending commands to regbot
 
 */
void UART::send(char *str)
{
    if (uart0_filestream != -1)
	{
		int count = write(uart0_filestream, str, strlen(str));
		if (count < 0)
		{
			printf("UART TX error\n");
		}
	}
    
   
}


/*
 * used for receiving data from regbot
 */
void UART::receive()
{
    if (uart0_filestream != -1)
        {
            // Read up to 10 characters from the port if they are there
            char rx_buffer[10];
            int rx_length = read(uart0_filestream, (void*)rx_buffer, 10);		//Filestream, buffer to store in, number of bytes to read (max)
            if (rx_length < 0)
            {
                    // error                   
            }
            else if (rx_length == 0)
            {
                    //No data waiting
            }
            else
            {
                rx_buffer[rx_length] = '\0';

                char* str = rx_buffer;
                
                printf("%s",str);

                if (strncmp(str,"start\n",6)==0)
                {
                    missionStart = true;
                    printf("Starting the mission!\n");
                }

            }
        }
}
