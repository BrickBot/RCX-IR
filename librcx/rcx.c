/***************************************************************
*                                                              *
* rcx.c                                                        *
*                                                              *
* Description:                                                 *
* Communicates at RCX packet level with a Lego RCX controller. *
* For the infrared communication the LIRC device driver is     *
* used, see www.lirc.org.                                      *
*                                                              *
* Latest version allows the transmission of raw bytes, to      *
* support low-level communication.                             *
*                                                              *
* Debug flags:                                                 *
* APP_PRINT_DEBUG   Show debug data amd errors                 *
* APP_PRINT_ERROR   Show errors                                *
*                                                              *
* Author:                                                      *
* begin      Mon Jul 8 2002                                    *
* copyright  (C) 2002 by Henk Dekker                           *
* email      henk.dekker@ordina.nl                             *
*                                                              *
* License:                                                     *
* This program is free software; you can redistribute it       *
* and/or modify it under the terms of the GNU General Public   *
* License as published by the Free Software Foundation; either *
* version 2 of the License, or (at your option) any later      *
* version.                                                     *
*                                                              *
* History:                                                     *
* v 0.1   Jul 15 2002   Henk Dekker <henk.dekker@ordina.nl>    *
*         - First functional version                           *
* V 0.2   Oct 4 2002    Henk Dekker <henk.dekker@ordina.nl>    *
*         - Make a library instead of an executable            *
* V 0.3   Oct 8 2002    Henk Dekker <henk.dekker@ordina.nl>    *
*         - Add debug/error messages                           *
* V 0.4   Nov 26 2002   Henk Dekker <henk.dekker@ordina.nl>    *
*         - Split up layers in several files                   *
*         - Add low-level communication functions              *
***************************************************************/
#include "rcx.h"
#include "lirc.h"
#include "verbose.h"
#include "rcxcode.h"
#include "lirccode.h"
#include "lircfile.h"

/* Defines */
#define BUFFERSIZE            1024

/* Prototypes */
int raw_receive(unsigned char* buf, int buf_size);
int raw_send(unsigned char tx_byte);



/***************************************************************
* rcx_open:   Initializes the LIRC driver.                     *
*                                                              *
* Input:   none                                                *
* Output:  none                                                *
* Return:  RCX_OK                 Device is opened normally    *
*          RCX_E_DEVICE_NOT_FOUND Cannot open the LIRC driver  *
*          RCX_E_DEVICE_READONLY  No permissions to write to   *
*                                 the LIRC driver              *
*          RCX_E_DEVICE_NO_LIRC   Device is not a LIRC driver  *
*          RCX_E_DEVICE_IS_OPEN   Device is already open       *
*          RCX_E_PROGRAM_FAILURE  Internal error               *
***************************************************************/
int rcx_open(void)
{
    APP_DEBUG("");
    APP_FLUSH

    switch (lirc_open())
    {
    case LIRC_OK: /* Device has been opened succesfully */
        return RCX_OK;

    case LIRC_E_DEVICE_IS_OPEN: /* Device is already open */
        return RCX_E_DEVICE_IS_OPEN;

    case LIRC_E_DEVICE_NOT_FOUND: /* Device cannot be opened */
        return RCX_E_DEVICE_NOT_FOUND;

    case LIRC_E_DEVICE_READONLY: /* Device is read-only */
        return RCX_E_DEVICE_READONLY;

    case LIRC_E_DEVICE_NO_LIRC: /* Device not a lirc_sir driver */
        return RCX_E_DEVICE_NO_LIRC;

    default:
        return RCX_E_PROGRAM_FAILURE;
    }
}



/***************************************************************
* rcx_reset:   Resets LIRC driver, and clears input buffers.   *
*                                                              *
* Input:   none                                                *
* Output:  none                                                *
* Return:  RCX_OK                 Device is opened normally    *
*          RCX_E_DEVICE_NOT_OPEN  Device has not been opened   *
*          RCX_E_DEVICE_ERROR     Error in LIRC device         *
*          RCX_E_PROGRAM_FAILURE  Internal error               *
***************************************************************/
int rcx_reset(void)
{
    APP_DEBUG("");
    APP_FLUSH

    /* Reset the LIRC driver */
    switch (lirc_reset())
    {
    case LIRC_E_DEVICE_NOT_OPEN: /* Device not open */
        return RCX_E_DEVICE_NOT_OPEN;

    case LIRC_E_DEVICE_ERROR: /* Lirc device errors */
        return RCX_E_DEVICE_ERROR;

    case LIRC_OK: /* Lirc reset ok */
        return RCX_OK;

    default: /* Internal error, result code unknown */
        return RCX_E_PROGRAM_FAILURE;
    }
}



/***************************************************************
* rcx_close:    Closes the LIRC driver.                        *
*                                                              *
* Input:   none                                                *
* Output:  none                                                *
* Return:  RCX_OK                 Device is closed succesfully *
***************************************************************/
int rcx_close(void)
{
    APP_DEBUG("");

    lirc_close();

    APP_FLUSH

    return RCX_OK;
}



/***************************************************************
* rcx_command: Send a command to the LIRC driver, and receive  *
*              its reply.                                      *
*                                                              *
*              The buffer holds the RCX opcode, plus arguments *
*              to be sent. The same buffer is also used for    *
*              receiving input.                                *
*                                                              *
*              This is a non-blocking function, but it can     *
*              take a second before the call returns.          *
*                                                              *
*              ------------------Example---------------------- *
*              int           errorcode;                        *
*              int           length                            *
*              unsigned char buffer[BUF_SIZE] = "\010";        *
*                                                              *
*              length = 1;                                     *
*              errorcode = rcx_send(buffer, BUF_SIZE, &length);*
*              ----------------------------------------------- *
*                                                              *
* Input:   buf_len                Number of bytes to send      *
*          buf                    Send and receive buffer      *
*          buf_size               Size of send/receive buffer  *
* Output:  buf_len                Number of bytes received     *
* Return:  RCX_OK                 Command and reply has been   *
*                                 send and received succesfully*
*          RCX_E_DEVICE_NOT_OPEN  Device has not been opened   *
*          RCX_E_DEVICE_ERROR     Cannot send to LIRC driver   *
*          RCX_E_RECV_NOTHING     No data received             *
*          RCX_E_RECV_ERROR       Data received, with errors   *
***************************************************************/
int rcx_command(unsigned char* buf, int buf_size, int* buf_len)
{
    int result;
    unsigned char opcode;

    APP_DEBUG("");

    /* Send data bytes as RCX packet to the LIRC driver */
    opcode = buf[0];
    result = rcx_send(buf, *buf_len);
    if (result!=RCX_OK)
    {
        return result;
    }

    /* Receive reply from LIRC and parse RCX packet */
    result = rcx_receive(buf, buf_size, buf_len);

    /* Check if data contains inverted opcode, to */
    /* check if it complies with the RCX protocol */
    if (opcode != (unsigned char) (~buf[0]&0xffU) )
    {
        result = RCX_E_RECV_ERROR;
    }

    APP_FLUSH

    return result;
}




/***************************************************************
* rcx_send:    Send a RCX packet to the LIRC driver            *
*                                                              *
* Input:   buf_len                Number of bytes to send      *
*          buf                    Send buffer                  *
* Output:                                                      *
* Return:  RCX_OK                 Command has been sent        *
*          RCX_E_DEVICE_NOT_OPEN  Device has not been opened   *
*          RCX_E_DEVICE_ERROR     Cannot send to LIRC driver   *
*          RCX_E_PROGRAM_FAILURE  Internal error               *
***************************************************************/
int rcx_send(unsigned char* buf, int buf_len)
{
    int index = 0;
    int result = RCX_OK;
    int rcxlen = 0;
    unsigned char send_byte_buf[BUFFERSIZE];

    /* Convert byte array to a RCX packet */
    rcxlen = rcx_encode(buf, buf_len, send_byte_buf, BUFFERSIZE);
    if (rcxlen==RCX_E_BUFFER)
    {
        result = RCX_E_PROGRAM_FAILURE;
    }

    /* Send a byte at a time to the LIRC driver */
    while ((result==RCX_OK) && (index<rcxlen))
    {
        result = rcx_send_byte(send_byte_buf[index]);
        index++;
    }

    APP_FLUSH

    return result;
}




/***************************************************************
* rcx_receive: Receive a RCX packet from the LIRC driver       *
*                                                              *
*              This is a non-blocking function, but it can     *
*              take a second before the call returns.          *
*                                                              *
* Input:   buf                    Receive buffer               *
*          buf_size               Size of receive buffer       *
* Output:  buf_len                Number of bytes received     *
* Return:  RCX_OK                 Command and reply has been   *
*                                 send and received succesfully*
*          RCX_E_DEVICE_NOT_OPEN  Device has not been opened   *
*          RCX_E_DEVICE_ERROR     Error in LIRC device         *
*          RCX_E_RECV_NOTHING     No data received             *
*          RCX_E_RECV_ERROR       Data received, with errors   *
***************************************************************/
int rcx_receive(unsigned char* buf, int buf_size, int* buf_len)
{
    int result;
    unsigned char recv_byte_buf[BUFFERSIZE];

    APP_DEBUG("");
    APP_FLUSH

    /* Receive and decode input LIRC driver */
    result = raw_receive(recv_byte_buf, BUFFERSIZE);
    APP_PRINT2("DEBUG:" APP_SOURCE "Function raw_receive() returned %d\n", result);
    if (result<0)
    {
        return result;
    }

    result = rcx_decode(recv_byte_buf, result, buf, buf_size);
    APP_PRINT2("DEBUG:" APP_SOURCE "Function rcx_decode() returned %d\n", result);
    switch (result)
    {
    case RCX_E_NO_RCX: /* Error, input is not RCX */
        return RCX_E_RECV_ERROR;

    case RCX_E_BUFFER: /* Error, buffer size too small */
        return RCX_E_RECV_ERROR;

    case 0: /* No data received */
        return RCX_E_RECV_NOTHING;

    default: /* Succesfully decoded the RCX packet */
        *buf_len = result;
    }

    return RCX_OK;
}






/***************************************************************
* rcx_send_byte:   Send a single byte to the LIRC driver       *
*                                                              *
* Input:   tx_byte                Byte to send                 *
* Output:                                                      *
* Return:  RCX_OK                 Byte has been sent           *
*          RCX_E_DEVICE_NOT_OPEN  Device has not been opened   *
*          RCX_E_DEVICE_ERROR     Cannot send to LIRC driver   *
*          RCX_E_PROGRAM_FAILURE  Internal error               *
***************************************************************/
int rcx_send_byte(unsigned char tx_byte)
{
    int result;

    APP_PRINT2("DEBUG:" APP_SOURCE "0x%02x\n", tx_byte);

    result = raw_send(tx_byte);

    APP_FLUSH
    return result;
}



/***************************************************************
* rcx_receive_byte: Receive a byte from the LIRC driver        *
*                                                              *
*              This is a non-blocking function, but it can     *
*              take a second before the call returns.          *
*                                                              *
* Input:                                                       *
* Output:  rx_byte                Pointer to receive byte in   *
* Return:  RCX_OK                 A byte has been received     *
*          RCX_E_DEVICE_NOT_OPEN  Device has not been opened   *
*          RCX_E_DEVICE_ERROR     Error in LIRC device         *
*          RCX_E_RECV_NOTHING     No data received             *
*          RCX_E_RECV_ERROR       Data received, with errors   *
***************************************************************/
int rcx_receive_byte(unsigned char* rx_byte)
{
    int result = 1;
    static int recv_byte_index = 0;
    static int recv_byte_count = 0;
    static unsigned char recv_byte_buf[BUFFERSIZE];

    APP_DEBUG("");

    /* Receive and decode byte from LIRC driver input */
    if (recv_byte_index==recv_byte_count)
    {
        result = raw_receive(recv_byte_buf, BUFFERSIZE);
        APP_PRINT2("DEBUG:" APP_SOURCE "Function raw_receive() returned %d\n", result);
        if (result>0)
        {
            recv_byte_index = 0;
            recv_byte_count = result;
        }
    }

    /* Write a buffer value in byte */
    if (recv_byte_index<recv_byte_count)
    {
        *rx_byte = recv_byte_buf[recv_byte_index];
        recv_byte_index++;
    }

    APP_FLUSH

    return (result>0) ? RCX_OK : result;
}



/***************************************************************
* raw_receive: Receive raw bytes from the LIRC driver          *
*                                                              *
* Note:        This is a non-blocking function, but it can     *
*              take a second before the call returns.          *
*                                                              *
* Input:   buf                    Receive buffer               *
*          buf_size               Size of receive buffer       *
* Output:  buf_len                Number of bytes received     *
* Return:  >0                     No bytes received and decoded*
*          RCX_E_DEVICE_NOT_OPEN  Device has not been opened   *
*          RCX_E_DEVICE_ERROR     Error in LIRC device         *
*          RCX_E_RECV_NOTHING     No data received             *
*          RCX_E_RECV_ERROR       Data received, with errors   *
***************************************************************/
int raw_receive(unsigned char* buf, int buf_size)
{
    int result;
    lirc_t recv_lirc_buf[BUFFERSIZE];

    /* Receive input from LIRC driver */
    result = lirc_receive(recv_lirc_buf, BUFFERSIZE);
    APP_PRINT2("DEBUG:" APP_SOURCE "Function lirc_receive() returned %d\n", result);
    switch (result)
    {
    case LIRC_E_DEVICE_NOT_OPEN: /* Device not open */
        return RCX_E_DEVICE_NOT_OPEN;

    case LIRC_E_DEVICE_ERROR: /* Lirc device errors */
        return RCX_E_DEVICE_ERROR;

    case LIRC_E_BUF_SIZE: /* No items exceed items_max */
        return RCX_E_RECV_ERROR;

    case 0:
        return RCX_E_RECV_NOTHING;

    default:
        ; /* Successful, one or more items received */
    }


    /* Decode received LIRC items */
    result = lirc_decode(recv_lirc_buf, result, buf, buf_size);
    APP_PRINT2("DEBUG:" APP_SOURCE "Function lirc_decode() returned %d\n", result);
    switch (result)
    {
    case LIRC_E_NO_RS232: /* Decoded signal not 2400 8O1 signal */
        return RCX_E_RECV_ERROR;

    case LIRC_E_BUF_SIZE: /* Number of chars exceed buf_size */
        return RCX_E_RECV_ERROR;

    case 0:
        return RCX_E_RECV_NOTHING;

    default:
        ; /* Succesfully decoded, one or more items */
    }

    return result;
}


/***************************************************************
* raw_send:    Send a single byte to the LIRC driver           *
*                                                              *
* Input:   tx_byte                Byte to send                 *
* Output:                                                      *
* Return:  RCX_OK                 Byte has been sent           *
*          RCX_E_DEVICE_NOT_OPEN  Device has not been opened   *
*          RCX_E_DEVICE_ERROR     Cannot send to LIRC driver   *
*          RCX_E_PROGRAM_FAILURE  Internal error               *
***************************************************************/
int raw_send(unsigned char tx_byte)
{
    int ret;
    int result;
    lirc_t list[16];

    /* This function cannot fail. 16 items should */
    /* always be enough for a single byte.        */
    result = lirc_encode(&tx_byte, 1, list, 16);

    /* Send the list */
    result = lirc_send(list, result);
    switch (result)
    {
    case LIRC_OK: /* All items sent succesfully */
        ret = RCX_OK;
        break;

    case LIRC_E_DEVICE_NOT_OPEN: /* Device has not been opened */
        ret = RCX_E_DEVICE_NOT_OPEN;
        break;

    case LIRC_E_DEVICE_ERROR: /* Lirc device errors */
        ret = RCX_E_DEVICE_ERROR;
        break;

    default: /* Unexpected error, this should not occur */
        ret = RCX_E_PROGRAM_FAILURE;
    }

    return ret;
}

