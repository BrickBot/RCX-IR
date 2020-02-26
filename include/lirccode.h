/***************************************************************
*                                                              *
* lirccode.h                                                   *
*                                                              *
* Description:                                                 *
* Converts between byte arrays and LIRC driver codes.          *
*                                                              *
* Flags:                                                       *
* LIRC_TARGET_PC    Set timing values for laptop               *
* LIRC_TARGET_IPAQ  Set timing values for iPAQ                 *
* APP_PRINT_DEBUG   Show debug data amd errors                 *
* APP_PRINT_ERROR   Show errors                                *
*                                                              *
* Author:                                                      *
* begin      Wed Nov 27 2002                                   *
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
* v 0.1   Nov 27 2002   Henk Dekker <henk.dekker@ordina.nl>    *
*         Initial version                                      *
***************************************************************/
#ifndef _LIRCCODE_H
#define _LIRCCODE_H


/**************************************************************/
/************************ Defines  ****************************/
/**************************************************************/

/* Error codes */
#define LIRC_OK                (   0)
#define LIRC_E_BUF_SIZE        (-100)
#define LIRC_E_NO_RS232        (-101)

/**************************************************************/
/*********************** Prototypes ***************************/
/**************************************************************/


/*************************************************************
* lirc_encode generates a pattern of 'mark' and 'space'      *
* signals from a byte array. The pattern simulates a         *
* 2400baud 8O1 modem signal.                                 *
*                                                            *
* Input:  buf       The characters to convert                *
*         buf_len   Number of characters in buffer           *
*         data_size Maximum number of lirc_t elements that   *
*                   the 'data' array can store               *
*                                                            *
* Output: data      Array of lirc_t datatypes that contains  *
*                   marks and spaces, plus the lengths of it *
*                                                            *
* Return: >=0       Number of lirc_t items                   *
          LIRC_E_BUF_SIZE   data elements exceeds data_size  *
*                                                            *
*************************************************************/
int lirc_encode(unsigned char* buf, int buf_len, lirc_t* data,
                int data_size);




/*************************************************************
* lirc_decode parses 'space' and 'mark' times and regenerates*
* an 2400baud 8odd1 modem signal. The output are received    *
* characters put in buffer.                                  *
*                                                            *
* Input:  data      List of composed datatypes with a 'space'*
*                   or 'mark' signal, plus the length of it. *
*         buf_len   Size of the buf character array          *
* Output: buf       Array of decoded characters              *
*                   character to write the next char in.     *
*                                                            *
* Return: >=0             Number of chars in buffer          *
*         LIRC_E_NO_RS232 Decoded signal does not comply     *
*                         with a 2400 8O1 signal             *
*         LIRC_E_BUF_SIZE Number of chars exceed buf_size    *
*                                                            *
*************************************************************/
int lirc_decode(lirc_t* data, int items, unsigned char* buf,
                int buf_size);

#else
#error -- lirccode.h -- included twice, or more...
#endif /* _LIRCCODE_H */

