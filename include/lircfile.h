/***************************************************************
*                                                              *
* lircfile.h                                                   *
*                                                              *
* Description:                                                 *
* Reads and writes to the LIRC driver. See www.lirc.org        *
*                                                              *
* Author:                                                      *
* begin      Tue Nov 26 2002                                   *
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
* v 0.1   Nov 26 2002   Henk Dekker <henk.dekker@ordina.nl>    *
*         Initial version                                      *
***************************************************************/
#ifndef _LIRCFILE_H
#define _LIRCFILE_H



/**************************************************************/
/************************ Defines  ****************************/
/**************************************************************/
/* External error codes */
#define LIRC_OK                  (   0)
#define LIRC_E_DEVICE_NOT_FOUND  (-100)
#define LIRC_E_DEVICE_READONLY   (-101)
#define LIRC_E_DEVICE_NO_LIRC    (-102)
#define LIRC_E_DEVICE_NOT_OPEN   (-103)
#define LIRC_E_DEVICE_IS_OPEN    (-104)
#define LIRC_E_DEVICE_ERROR      (-105)
#define LIRC_E_BUFFERSIZE        (-106)


/**************************************************************/
/*********************** Prototypes ***************************/
/**************************************************************/

/*************************************************************
* lirc_open: Opens the LIRC device. In Linux and Unix        *
* communication with drivers can be done by means of reading *
* and writing to a '/dev/...' file.                          *
* Note: Be sure the /dev/lirc file has the proper rights,    *
* otherwise modify the rights with 'chmod 666 /dev/lirc'     *
*                                                            *
* For detailed decription, see headerfile rcx.h              *
*                                                            *
* Global: lirc_driver        File descriptor to lirc device  *
*                                                            *
* Return:                                                    *
*   LIRC_OK                  Device opened succesfully       *
*   LIRC_E_DEVICE_IS_OPEN    Device is already open          *
*   LIRC_E_DEVICE_NOT_FOUND  Device cannot be opened         *
*   LIRC_E_DEVICE_READONLY   Device is read-only             *
*   LIRC_E_DEVICE_NO_LIRC    Device is not a lirc_sir driver *
*************************************************************/
int lirc_open(void);




/*************************************************************
* lirc_close: Closes the LIRC device.                        *
*                                                            *
* Global: lirc_driver    File descriptor to the lirc device  *
*                                                            *
* Return: errorcodes                                         *
*     LIRC_OK            Device closed succesfully           *
*************************************************************/
int lirc_close(void);




/*************************************************************
* lirc_reset resets the lirc device, and clears the receive  *
* buffer.                                                    *
*                                                            *
* Globals: lirc_driver  File descriptor of the lirc device   *
*                                                            *
* Input:   none                                              *
*                                                            *
* Output:  none                                              *
*                                                            *
* Return:                                                    *
*   LIRC_OK                  Device reset successful         *
*   LIRC_E_DEVICE_NOT_OPEN   Device has not been opened      *
*   LIRC_E_DEVICE_ERROR      Lirc device errors              *
*************************************************************/
int lirc_reset(void);




/***************************************************************
* lirc_receive reads data from the lirc device. It stops       *
* reading if nothing is received for a certain period          *
*                                                              *
* Globals: lirc_driver  File descriptor of the lirc device     *
*                                                              *
* Input:   items_max    Size of the list, in lirct_t items     *
*                                                              *
* Output:  list         List with received lirc_t items        *
*                                                              *
* Return:                                                      *
*   >= 0                     Number of items received          *
*   LIRC_E_DEVICE_NOT_OPEN   Device has not been opened        *
*   LIRC_E_DEVICE_ERROR      Lirc device errors                *
*   LIRC_E_BUFFERSIZE        Number of items exceed items_max  *
***************************************************************/
int lirc_receive(lirc_t* list, int items_max);



/***************************************************************
* lirc_send sends a lirc_t list to the LIRC device driver.     *
*                                                              *
* Global: lirc_driver  The file descriptor of the LIRC driver  *
*                                                              *
* Input:  list         An array that contains lirc_t items     *
*         item_count   Number of items to send                 *
*                                                              *
* Output: none                                                 *
*                                                              *
* Return:                                                      *
*   LIRC_OK                  All items sent succesfully        *
*   LIRC_E_DEVICE_NOT_OPEN   Device has not been opened        *
*   LIRC_E_DEVICE_ERROR      Lirc device errors                *
***************************************************************/
int lirc_send(lirc_t* list, int item_count);


#else
#error -- lircfile.h -- included twice, or more...
#endif /* _LIRCFILE_H */
