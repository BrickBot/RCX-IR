/***************************************************************
*                                                              *
* rcxcode.h                                                    *
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
* v 0.1   Nov 13 2002   Henk Dekker <henk_dekker@planet.nl>    *
*         Initial version                                      *
***************************************************************/
#ifndef _RCXCODE_H
#define _RCXCODE_H

/**************************************************************/
/************************ Defines  ****************************/
/**************************************************************/


/* Error codes, for rcxcode.c file */
#define RCX_E_NO_RCX            (-100)
#define RCX_E_BUFFER            (-101)


/**************************************************************/
/*********************** Prototypes ***************************/
/**************************************************************/

/*************************************************************
* rcx_encode encodes a RCX packet from a data byte buffer    *
* In otherwords it adds the RCX protocol bytes.              *
*                                                            *
* Important: The buffer is used for input and output         *                  
*                                                            *
* Input:  databuf   Pointer to buffer that contains data     *
*         datalen   Number of data bytes in buffer           *
*         rcxsize   Buffer size (Used for input and output)  *
*                                                            *
* Output: rcxbuf    Pointer to RCX bytes output buffer.      *
*                                                            *
* Return: >= 0          Ok, number of bytes in rcx buffer    *
*         RCX_E_BUFFER  Buffer size too small                *
*************************************************************/
int rcx_encode(unsigned char* databuf, int datalen,
               unsigned char* rcxbuf, int rcxsize);



/*************************************************************
* rcx_decode decodes a RCX packet to a data byte buffer.     *
* In otherwords it stips the RCX protocol bytes.             *
*                                                            *
* Input:  rcxbuf    Pointer to buffer that contains RCX data *
*         rcxlen    Number of RCX bytes in buffer            *
*         datasize  Size of data bytes output buffer         *
*                                                            *
* Output: databuf   Pointer to data bytes output buffer.     *
*                                                            *
* Return: >= 0            Decoding ok, number of data bytes  *
*         RCX_E_NO_RCX    Error, input is not RCX            *
*         RCX_E_BUFFER    Error, buffer size too small       *
*************************************************************/
int rcx_decode(unsigned char* rcxbuf, int rcxlen,
               unsigned char* databuf, int datasize);

#else
#error -- rcxcode.h -- included twice, or more...
#endif /* _RCXCODE_H */

