/***************************************************************
*                                                              *
* rcx.h                                                        *
*                                                              *
* Description:                                                 *
* Interface to RCX command communicator. This RCX program      *
* relies on the Lirc driver. (www.lirc.org)                    *
*                                                              *
* Author:    Henk Dekker                                       *
* begin      Wed Nov 13 2002                                   *
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
* v 0.1   Nov 13 2002   Henk Dekker <henk.dekker@ordina.nl>    *
*         Initial version                                      *
***************************************************************/
#ifndef _RCX_H
#define _RCX_H



/**************************************************************/
/************************ Defines  ****************************/
/**************************************************************/
/* External error codes */
#define RCX_OK                  (   0)
#define RCX_E_PROGRAM_FAILURE   (-100)
#define RCX_E_DEVICE_NOT_FOUND  (-101)
#define RCX_E_DEVICE_READONLY   (-102)
#define RCX_E_DEVICE_NO_LIRC    (-103)
#define RCX_E_DEVICE_NOT_OPEN   (-104)
#define RCX_E_DEVICE_IS_OPEN    (-105)
#define RCX_E_DEVICE_ERROR      (-106)
#define RCX_E_RECV_NOTHING      (-107)
#define RCX_E_RECV_ERROR        (-108)


/**************************************************************/
/*********************** Prototypes ***************************/
/**************************************************************/


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
int rcx_open(void);




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
int rcx_reset(void);




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
*          RCX_E_PROGRAM_FAILURE  Internal error               *
***************************************************************/
int rcx_command(unsigned char* buf, int buf_size, int* buf_len);




/***************************************************************
* rcx_send:    Send a RCX packet to the LIRC driver            *
*                                                              *
*              The buffer holds the RCX opcode, plus arguments *
*              to be sent                                      *
*                                                              *
* Input:   buf_len                Number of bytes to send      *
*          buf                    Send buffer                  *
* Output:                                                      *
* Return:  RCX_OK                 Command has been sent        *
*          RCX_E_DEVICE_NOT_OPEN  Device has not been opened   *
*          RCX_E_DEVICE_ERROR     Cannot send to LIRC driver   *
*          RCX_E_PROGRAM_FAILURE  Internal error               *
***************************************************************/
int rcx_send(unsigned char* buf, int buf_len);




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
int rcx_receive(unsigned char* buf, int buf_size, int* buf_len);




/***************************************************************
* rcx_close:    Closes the LIRC driver.                        *
*                                                              *
* Input:   none                                                *
* Output:  none                                                *
* Return:  RCX_OK                 Device closed succesfully    *
***************************************************************/
int rcx_close(void);




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
int rcx_send_byte(unsigned char tx_byte);




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
int rcx_receive_byte(unsigned char* rx_byte);


#else
#error -- rcx.h -- included twice, or more...
#endif /* _RCX_H */
