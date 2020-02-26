/***************************************************************
*                                                              *
* rcxcode.c                                                    *
*                                                              *
* Description:                                                 *
* Converts between RCX packets and data byte arrays            *
*                                                              *
* Author:                                                      *
* begin      Sun 20 Nov 2002                                   *
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
* v 0.1   20 Nov 2002   Henk Dekker <henk.dekker@ordina.nl>    *
*         Initial version                                      *
***************************************************************/
#include "verbose.h"
#include "rcxcode.h"



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
               unsigned char* databuf, int datasize)
{
    int sum;
    int count;
    int datalen;
    unsigned char *pend;
    unsigned char *prcx;

    count = 0;
    datalen = (rcxlen-5) / 2;
    if (datalen>datasize)
    {
        APP_ERROR("Data buffer length exceeds buffer size");
        return RCX_E_BUFFER;    
    }
    prcx = &rcxbuf[0];
    pend = &rcxbuf[rcxlen];

    /* Check RCX packet header, start if 0x55 found */
    if ((prcx<pend) && (*prcx==0x55))
    {
        prcx++;
    }
    if ((prcx<pend) && (*prcx==0xff))
    {
        prcx++;
    }
    if ((prcx<pend) && (*prcx==0x00))
    {
        prcx++;
    }
    if (prcx != &rcxbuf[3])
    {
        APP_ERROR("RCX packet header not correct");
        return RCX_E_NO_RCX;
    }

    /* Check received data, and fill 'pbuf' buffer */
    for ( sum=0; prcx<(pend-2); prcx+=2 )
    {
        /* Check complements value */
        if (*prcx != (~*(prcx+1)&0xff) )
        {
            APP_ERROR("RCX packet data value complement not correct");
            return RCX_E_NO_RCX;                
        }
        databuf[count++] = *prcx;
        if (count==datasize)
        {
            APP_ERROR("RCX number of data bytes exceed buffer size");
            return RCX_E_BUFFER;    
        }
        sum += *prcx;
    }

    /* Check RCX packet checksum */
    if (*prcx != (sum&0xff))
    {
        APP_ERROR("RCX packet checksum not correct");
        return RCX_E_NO_RCX;                
    }

    /* Check RCXs checksum complement */
    if (*prcx != (~*(prcx+1)&0xff) )
    {
        APP_ERROR("RCX packet checksum complement not correct");
        return RCX_E_NO_RCX;                            
    }

    /* Return number of data bytes */
    return count;
}



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
               unsigned char* rcxbuf, int rcxsize)
{
    int sum;
    int rcxlen;
    unsigned char *pdata;
    unsigned char *prcx;

    rcxlen = (datalen * 2) + 5;
    if ( rcxlen>rcxsize )
    {
        APP_ERROR("RCX buffer length exceeds buffer size");
        return RCX_E_BUFFER;    
    }
    sum = 0;
    prcx = &rcxbuf[0];
    pdata = &databuf[0];
    
    /* Fill buffer with packet header */
    *prcx++ = 0x55;
    *prcx++ = 0xff;
    *prcx++ = 0x00;

    /* Fill buffer with data bytes */
    while ( datalen-- )
    {
        *prcx++ = *pdata;
        *prcx++ = (~ *pdata) & 0xff;
        sum += *pdata++;
    }

    /* Fill buffer with checksum */
    *prcx++ = sum;
    *prcx++ = ~sum;

    /* Return number of RCX bytes */
    return (int) (prcx - &rcxbuf[0]);
}

