/***************************************************************
*                                                              *
* lirccode.c                                                   *
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
#include "verbose.h"
#include "lirc.h"
#include "lirccode.h"

/* LIRC_DRIVER define sets the platform. Options "ipaq" or "sir" */
#define LIRC_DRIVER           "ipaq"   

#define BIT_PERIOD            417

/* Prototypes */
void pc_adjust(int bit, int* lirc_t);
void ipaq_adjust(int bit, int* lirc_t);
int lirc_byte_encode(unsigned char data, lirc_t* list);
int lirc_byte_decode(lirc_t data, unsigned char* pbuf);
void add_to_list(unsigned int bit, unsigned int* signal_ptr,
                 int* index_ptr,  lirc_t* list);

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
                int data_size)
{
    int n;
    int item_cnt = 0;

    for (n=0; n<buf_len; n++)
    {
        /* Check in advance if data_size is large */
        /* enough to accept the next character    */
        if ((item_cnt+12)>data_size)
        {
            return LIRC_E_BUF_SIZE;
        }

        /* Convert the byte into lirc_t elements */
        item_cnt += lirc_byte_encode(buf[n], &data[item_cnt]);
    }

    return item_cnt;
}




/*************************************************************
* lirc_decode parses 'space' and 'mark' times and regenerates*
* an 2400baud 8odd1 modem signal.                            *
*                                                            *
* Input:  data      List of composed datatypes with a 'space'*
*                   or 'mark' signal, plus the length of it. *
*         items     Number of items in 'data' array          *
*         buf_size  Size of the buf character array          *
*                                                            *
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
                int buf_size)
{
    int n;
    int result;
    int error = 0;
    int byte_cnt = 0;

    for (n=0; n<items; n++)
    {
        /* Check if data_size is large enough */
        /* to accept the next character       */
        if (byte_cnt==buf_size)
        {
            APP_ERROR("Number of bytes exceed buf_size");
            return LIRC_E_BUF_SIZE;
        }

        /* Convert the byte into lirc_t elements */
        result = lirc_byte_decode(data[n], &buf[byte_cnt]);
        switch (result)
        {
        case (-1):
            /* A decode error occured */
            error = 1;
            break;

        case 0:
            /* No errors, and no byte filled yet */
            break;

        case 1:
            /* A byte filled, without errors */
            byte_cnt++;
            break;

        default:
            /* This should never occure */
            APP_ERROR("Internal program error");
        }
    }

    return (error==1) ? LIRC_E_NO_RS232 : byte_cnt;
}


/*************************************************************
* lirc_byte_encode generates from a character a pattern of   *
* 'mark' and 'space' signals. The pattern simulates a        *
* 2400baud 8O1 modem signal.                                 *
*                                                            *
* Input:  data      The character to decode to lirc elements *
*                                                            *
* Output: list      Composed datatype with a 'space' or      *
*                   'mark' signal, plus the length of it.    *
*                                                            *
* Return:           Number of lirc_t elements                *
*                                                            *
*************************************************************/
int lirc_byte_encode(unsigned char data, lirc_t* list)
{
    int n;
    int parity;
    int item_index;
    unsigned int bit;
    unsigned int signal;

    bit = 0;
    parity = 0;

    /* Preset 'signal' to 1 to force creating */
    /* a new item that gets index 0           */
    signal = 1;
    item_index = -1;

    /* First bit, the startbit, is a 'space' */
    add_to_list(0, &signal, &item_index, list);

    /* Then append data bits */
    for (n=0;n<8;n++)
    {
        bit = data&0x01U;
        data >>= 1;

        if (bit)
        {
            parity++;
        }

        add_to_list(bit, &signal, &item_index, list);
    }

    /* Append the ODD parity bit */
    add_to_list((parity+1)%2, &signal, &item_index, list);

    /* At last append the 'mark' stop bit */
    add_to_list(1, &signal, &item_index, list);

    /* Return item count */
    return item_index+1;
}


/***************************************************************
* add_to_list adds a single bit to the 'pulse' and 'space'     *
* list, which is sent directly to the LIRC device driver.      *
* The duration of 'pulse's and 'space's is simply increased if *
* equal bits are successive.                                   *
*                                                              *
* Input:  bit        The bit to add to the list                *
*                                                              *
* In/Out: signal_ptr The current signal. If the bit differs    *
*                    from the signal then a new item is added  *
*                    and the signal changes to the bit value.  *
*         index_ptr  Index to the current item in the list,    *
*                    this value increases after a bit change.  *
*         list       List of 'space' and 'pulse' durations,    *
*                    first item in list is always a 'pulse'.   *
*                                                              *
* Return: none                                                 *
*                                                              *
***************************************************************/
void add_to_list(unsigned int bit, unsigned int* signal_ptr,
                 int* index_ptr,  lirc_t* list)
{
    if (*signal_ptr == bit)
    {
        /* If no signal change, then only increase the */
        /* signal period.                              */
        list[*index_ptr] += BIT_PERIOD;
    }
    else
    {
        /* If signal change, then append the signal list    */
        /* with a new item                                  */
        (*index_ptr)++;

#ifdef LIRC_TARGET_IPAQ
        ipaq_adjust(bit, &list[*index_ptr]);
#endif /* LIRC_TARGET_IPAQ */

#ifdef LIRC_TARGET_PC
        pc_adjust(bit, &list[*index_ptr]);
#endif /* LIRC_TARGET_PC */

        /* Remember value of last bit */
        *signal_ptr = bit;

        /* To indicate the last item, clear the next item */
        list[(*index_ptr+1)] = 0;
    }
}


/* Adjust time values for PC platform */
void pc_adjust(int bit, int* lirc_t)
{
    /* Fine tuning, signal duration */
    if (bit)
    {
        /* Actually these bits are sent as spaces by    */
        /* the drivers. Fine tune by measuring pulse    */
        /* and space times by the lirc mode2 tool       */
        *lirc_t = BIT_PERIOD + 30;
    }
    else
    {
        *lirc_t = BIT_PERIOD + 145;
    }
}


/* Adjust time values for iPAQ platform */
void ipaq_adjust(int bit, int* lirc_t)
{
    if (bit)
    {
        *lirc_t = BIT_PERIOD;
    }
    else
    {
        *lirc_t = BIT_PERIOD - 20;
    }
}


/*************************************************************
* lirc_decode parses 'space' and 'mark' times and regenerates*
* an 2400baud 8odd1 modem signal. The output are received    *
* characters printed to stdout, and in case of errors RS232  *
* compliant error messages.                                  *
*                                                            *
* Input:  data      Composed datatype with a 'space' or      *
*                   'mark' signal, plus the length of it.    *
*                                                            *
* In:     pbuf      Pointer to receive character in          *
*                                                            *
* Return: 1         Success, and a character received        *
*         0         Success, but no character received yet   *
*         -1        Decoded signal does not comply with      *
*                   an 2400 8O1 signal                       *
*                                                            *
*************************************************************/
int lirc_byte_decode(lirc_t data, unsigned char* pbuf)
{
    int is_mark;
    int is_space;
    int no_bits;
    int period;
    int returncode;

    static int total_bits = 0;
    static int parity_bit = 0;
    static unsigned char data_byte = 0;

    returncode = 0;
    is_mark = (data&PULSE_BIT) ? 0 : 1;
    is_space = (is_mark) ? 0 : 1;
    period = (int) data&PULSE_MASK;

    /* A single bit lasts 417 us. Round to nearest integer.*/
    no_bits = (period+(BIT_PERIOD/2))/BIT_PERIOD;

    /* Optimalization and error-recovery routine, in case  */
    /* a long row of equal bits are received               */
    if (no_bits>10)
    {
        if (is_mark)
        {
            /* The first 10 bits, could be the data, parity*/
            /* and stopbits of the last received byte.     */
            no_bits = 10;
        }

        if (is_space)
        {
            /* Never more then 10 spaces in a row can be   */
            /* received. Print error and clear buffer.     */
            APP_ERROR("Break error");
            returncode = -1;

            data_byte = 0;
            parity_bit = 0;
            total_bits = 0;

            /* Last space bit can be the startbit of the   */
            /* next character to be received. Assume this  */
            /* and trust the layer above will detect this  */
            /* by checking parity and framing errors.      */
            no_bits = 1;
        }

    }


    /* Parse bit stream (A mark is an '1' and a space '0')  */
    /* bit0=start       Should be a space                   */
    /* bit1..bit8=data  Can be a mark or space              */
    /* bit9=parity      Total number of marks should be odd */
    /* bit10=stop       Should be a mark                    */
    while (no_bits>0)
    {
        no_bits--;

        if (total_bits==0) /* Start bit */
        {
            if (is_space)
            {
                /* If startbit received, then clear buffer  */
                /* and start filling the byte with databits.*/
                total_bits = 1;
                data_byte = 0;
                parity_bit = 0;
            }
        }
        else if (total_bits<9) /* Data bits */
        {
            total_bits++;
            /* Low databits are received first. Put bit in  */
            /* byte and update the parity bit counter.      */
            data_byte >>= 1;
            if (is_mark) {
                data_byte += 0x80;
                parity_bit++;
            }
        }
        else if (total_bits==9) /* Parity bit */
        {
            total_bits++;

            /* At this point we received 8 bits of data. */
            /* Display it in advance, without knowing if */
            /* the parity bit is correct.                */
            APP_PRINT2("0x%02x ", data_byte);

            /* Add received byte to receive buffer */
            *pbuf = data_byte;
            returncode = 1;

            /* Only update the parity bit counter */
            if (is_mark) {
                parity_bit++;
            }
            /* The parity bit counter should be ODD */
            if ((parity_bit%2)==0)
            {
                /* Received byte not ODD */
                APP_ERROR("Parity error");
                returncode = -1;
            }
        }
        else  /* Stopbit */
        {
            /* The stopbit should be a mark */
            if (is_space)
            {
                /* No stopbit received */
                APP_ERROR("Framing error");
                returncode = -1;
            }
            total_bits = 0;
        }
    }

    /* Force updating the console */
    APP_FLUSH
    return returncode;
}


