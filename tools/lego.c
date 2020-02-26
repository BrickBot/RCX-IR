/***************************************************************
*                                                              *
* Description:                                                 *
* LEGO communicates with the Lego RCX controller. If arguments *
* are given, then a RCX packet is composed and send to the RCX *
* by means of the LIRC device driver. Without arguments the    *
* application starts receiving from the RCX. From the received *
* RCX packet the data bytes are extracted and displayed to     *
* console.                                                     *
*                                                              *
* Dependencies:                                                *
* - 'librcx.so' must be installed on your system               *
* - Driver 'lirc_sir.o' must be installed and loaded. For      *
*   details see LIRC project's website www.lirc.org            *
*                                                              *
* Author:                                                      *
* begin      Fri Oct 4 2002                                    *
* copyright  (C) 2002 by Henk Dekker                           *
* email      henk_dekker@planet.nl                             *
*                                                              *
* License:                                                     *
* This program is free software; you can redistribute it       *
* and/or modify it under the terms of the GNU General Public   *
* License as published by the Free Software Foundation; either *
* version 2 of the License, or (at your option) any later      *
* version.                                                     *
*                                                              *
* History:                                                     *
* v 0.1   Oct 8  2002   Henk Dekker <henk.dekker@ordina.nl>    *
*         Initial version                                      *
*                                                              *
***************************************************************/

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include "rcx.h"

#define LEGO_BUFFER_LENGTH   1024

/* Prototypes */
int parse_cmd_line(unsigned char* pbuf, int argc, char** argv);
void display_rcx_reply(unsigned char* sbuf, int slen);


/***************************************************************
* main:                                                        *
*                                                              *
* Input:  argc      Number of arguments                        *
*         argv      List of pointers to the arguments          *
*                                                              *
* Return: EXIT_SUCCESS on success even if received data corrupt*
*         EXIT_FAILURE on failure accessing the device driver  *
*                                                              *
***************************************************************/
int main(int argc,char **argv)
{
    int count;
    int result;
    unsigned char buffer[LEGO_BUFFER_LENGTH];

    /* Pre-parse command arguments */	
    if ((argc==2) && (argv[1][0]=='-'))
    {
        printf("Usage: %s [byte ...]  (bytes in hex)\n", argv[0]);
        printf("Note: If no bytes are given, then nothing is transmitted.\n");
    	return EXIT_SUCCESS;
    }

    /* Open the LIRC driver */
    result = rcx_open();
    switch (result)
    {
        case RCX_OK:
        printf("%s ok: LIRC device opened succesfully.\n",argv[0]);
        break;
        
        case RCX_E_DEVICE_IS_OPEN:
        printf("%s error: LIRC device is already open.\n",argv[0]);
        break;
        
        case RCX_E_DEVICE_NOT_FOUND:
        printf("%s error: LIRC device cannot be opened!\n",argv[0]);
        return EXIT_FAILURE;

        case RCX_E_DEVICE_READONLY:
        printf("%s error: LIRC device is read-only!\n",argv[0]);
        return EXIT_FAILURE;

        case RCX_E_DEVICE_NO_LIRC:
        printf("%s error: Device is not a LIRC device!\n",argv[0]);
        return EXIT_FAILURE;

        default:
        printf("%s error: Unknown return code of rcx_open() call!\n",argv[0]);
        return EXIT_FAILURE;
    }

    /* Open the LIRC driver */
    result = rcx_reset();
    switch (result)
    {
        case RCX_OK:
        printf("%s ok: LIRC device reset succesfully.\n",argv[0]);
        break;

        case RCX_E_DEVICE_NOT_OPEN:
        printf("%s error: LIRC device is not open!\n",argv[0]);
        break;

        case RCX_E_DEVICE_ERROR:
        printf("%s error: Error in LIRC device!\n",argv[0]);
        return EXIT_FAILURE;

        case RCX_E_PROGRAM_FAILURE:
        printf("%s error: RCX reported a program failure!\n",argv[0]);
        return EXIT_FAILURE;

        default:
        printf("%s error: Unknown return code of rcx_reset() call!\n",argv[0]);
        return EXIT_FAILURE;
    }


    /* Parse command line and send data to RCX */
    count = parse_cmd_line(buffer, argc, argv);
    if (count>0)
    {
        /* Send bytes to RCX */
        switch (rcx_command(buffer, LEGO_BUFFER_LENGTH, &count))
        {
            case RCX_OK:
            printf("%s ok: RCX command processed succesfully.\n",argv[0]);
            break;

            case RCX_E_DEVICE_NOT_OPEN:
            printf("%s error: Lirc device not opened!\n",argv[0]);
            return EXIT_FAILURE;

            case RCX_E_DEVICE_ERROR:
            printf("%s error: Lirc device driver error!\n",argv[0]);
            return EXIT_FAILURE;

            case RCX_E_RECV_NOTHING:
            printf("%s error: RCX command sent, but no reply received!\n",argv[0]);
            return EXIT_FAILURE;

            case RCX_E_RECV_ERROR:
            printf("%s error: RCX reply received with errors!\n",argv[0]);
            return EXIT_FAILURE;
            
            default:
            printf("%s error: Unknown return code of rcx_send() call!\n",argv[0]);
            return EXIT_FAILURE;
        }
        
        /* print rcx reply */
        display_rcx_reply(buffer, count);
    }

    switch (rcx_close())
    {
        case RCX_OK:
        printf("%s ok: LIRC device closed succesfully.\n",argv[0]);
        break;
        
        default:
        printf("%s error: Unknown return code of rcx_close() call!\n",argv[0]);
        return EXIT_FAILURE;
    }
    
	return(EXIT_SUCCESS);
}



/*************************************************************
* display_rcx_reply dumps data in hex notation to console    *
*                                                            *
* Input:  sbuf      Buffer containing data                   *
*         slen      Length of the buffer                     *
*                                                            *
* Return: none                                               *
*                                                            *
*************************************************************/
void display_rcx_reply(unsigned char* sbuf, int slen)
{
    printf("Received RCX data: ");
    while (slen-- > 0)
    {
        printf("%02x ", *sbuf++);
    }
    printf("\n");
}



/*************************************************************
* parse_cmd_line converts the hex bytes given on the command *
* line to an list of bytes                                   *
*                                                            *
* Input:  argc      Number of arguments                      *
*         argv      List of pointers to the arguments        *
*                                                            *
* In/Out: pbuf      Address of byte input buffer.            *
*                                                            *
* Return: number of arguments (without command itself)       *
*                                                            *
*************************************************************/
int parse_cmd_line(unsigned char* pbuf, int argc, char** argv)
{
    int n;
    
    for (n=1;n<argc;n++)
    {
        *pbuf++ = strtol(argv[n], NULL, 16);
    }

    return (argc-1);
}

