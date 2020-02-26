/***************************************************************
*                                                              *
* lircfile.c                                                   *
*                                                              *
* Description:                                                 *
* Reads and writes to the LIRC driver. See www.lirc.org        *
*                                                              *
* Debug flags:                                                 *
* APP_PRINT_DEBUG   Show debug data and errors                 *
* APP_PRINT_ERROR   Show errors                                *
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
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <time.h>
#include <sys/time.h>

#include "verbose.h"
#include "lirc.h"
#include "lircfile.h"

/* LIRC_DRIVER_DEVICE filename of the lirc device */
#define LIRC_DRIVER_DEVICE    "/dev/lirc"

/* Time to wait before data arrives */
#define REPLY_TIME            350

/* Globals */
static int lirc_driver = 0;

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
int lirc_open(void)
{
    unsigned long mode;
    char *device=LIRC_DRIVER_DEVICE;

    APP_DEBUG("");

    if (lirc_driver)
    {
        APP_ERROR("Device already open");
        return LIRC_E_DEVICE_IS_OPEN;
    }
    
    lirc_driver = open(device,O_RDONLY);
    if (lirc_driver == -1)
    {
        lirc_driver = 0;
        APP_ERROR("Device not found");
        return LIRC_E_DEVICE_NOT_FOUND;
    }
    else
    {
        close(lirc_driver);
    }

    lirc_driver = open(device,O_RDWR);
    if (lirc_driver == -1)
    {
        lirc_driver = 0;
        APP_ERROR("Device is read only");
        return LIRC_E_DEVICE_READONLY;
    }


    if (ioctl(lirc_driver,LIRC_GET_REC_MODE,&mode)==-1 || mode!=LIRC_MODE_MODE2 )
    {
        close(lirc_driver);
        lirc_driver = 0;
        APP_ERROR("Device is not a LIRC driver");
        return LIRC_E_DEVICE_NO_LIRC;
    }

    return LIRC_OK;
}


/*************************************************************
* lirc_close: Closes the LIRC device.                        *
*                                                            *
* Global: lirc_driver    File descriptor to the lirc device  *
*                                                            *
* Return: errorcodes                                         *
*     LIRC_OK            Device closed succesfully           *
*************************************************************/
int lirc_close(void)
{
    APP_DEBUG("");

    if (lirc_driver)
    {
        close(lirc_driver);
        lirc_driver = 0;
    }

    return LIRC_OK;
}



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
int lirc_reset(void)
{
    int result;
    int errorcode;
    struct timeval tv;
    lirc_t dummy;
    fd_set fds;

    errorcode = LIRC_OK;

    if (lirc_driver == 0)
    {
        APP_ERROR("Device is not open");
        return LIRC_E_DEVICE_NOT_OPEN;
    }

    while(1)
    {
        /* Clear bits */
        FD_ZERO(&fds);
        FD_SET(lirc_driver, &fds);

        /* The maximum 'wait for data' time is 1 second */
        tv.tv_sec = 0;
        tv.tv_usec = REPLY_TIME*1000;

        /* Wait until data received or timeout */
        result = select(FD_SETSIZE, &fds, NULL, NULL, &tv);
        if (result == -1)
        {
            APP_ERROR("Function select() failed");
            errorcode = LIRC_E_DEVICE_ERROR;
            break;
        }

        /* If timeout occured then break this while loop */
        if (!FD_ISSET(lirc_driver, &fds))
        {
            /* No data to be read anymore */
            break;
        }

        /* Data seems to be available, so read it */
        result = read(lirc_driver, &dummy, sizeof(lirc_t));
        if (result != sizeof(lirc_t))
        {
            APP_ERROR("Function read() failed, wrong number of bytes received");
            errorcode = LIRC_E_DEVICE_ERROR;
            break;
        }
    }

    APP_PRINT("\n");

    return errorcode;
}



/*************************************************************
* lirc_receive reads data from the lirc device. It stops     *
* reading if nothing is received for a certain period        *
*                                                            *
* Globals: lirc_driver  File descriptor of the lirc device   *
*                                                            *
* Input:   items_max    Size of the list, in lirct_t items   *
*                                                            *
* Output:  list         List with received lirc_t items      *
*                                                            *
* Return:                                                    *
*   >= 0                     Number of items received        *
*   LIRC_E_DEVICE_NOT_OPEN   Device has not been opened      *
*   LIRC_E_DEVICE_ERROR      Lirc device errors              *
*   LIRC_E_BUFFERSIZE        Number of items exceed items_max*
*************************************************************/
int lirc_receive(lirc_t* list, int items_max)
{
    int result;
    int item_count;
    int errorcode;
    struct timeval tv;
    fd_set fds;

    item_count = 0;
    errorcode = LIRC_OK;

    if (lirc_driver == 0)
    {
        APP_ERROR("Device is not open");
        return LIRC_E_DEVICE_NOT_OPEN;
    }

    APP_PRINT("DEBUG:" APP_SOURCE "receiving data: ");
    APP_FLUSH

    /* Decrease maximum number of items, because we always */
    /* append a dummy item, at the end of this function.   */
    items_max--;

    while(1)
    {
        if (item_count==items_max)
        {
            APP_ERROR("Buffersize exceeded");
            return LIRC_E_BUFFERSIZE;
        }

        /* Clear bits */
        FD_ZERO(&fds);
        FD_SET(lirc_driver, &fds);

        /* The maximum 'wait for data' time is 1 second */
        tv.tv_sec = 0;
        tv.tv_usec = REPLY_TIME*1000;

        /* Wait until data received or timeout */
        if (select(FD_SETSIZE, &fds, NULL, NULL, &tv) == -1)
        {
            APP_ERROR("Function select() failed");
            errorcode = LIRC_E_DEVICE_ERROR;
            break;
        }

        /* If timeout occured then break this while loop */
        if (!FD_ISSET(lirc_driver, &fds))
        {
            /* No data to be read anymore */
            break;
        }

        /* Data seems to be available, so read it */
        result = read(lirc_driver, &list[item_count++], sizeof(lirc_t));
        if (result != sizeof(lirc_t))
        {
            APP_ERROR("Function read() failed, wrong number of bytes received");
            errorcode = LIRC_E_DEVICE_ERROR;
            break;
        }
    }

    /* Put in some extra mark bits, to complete a half    */
    /* received character. This can happen if a byte ends */
    /* ends with a mark, which cannot be detected in the  */
    /* LIRC driver.                                       */
    if (item_count>0)
    {
        list[item_count++] = 417U*10U;
    }

    APP_PRINT("\n");

    return (errorcode==LIRC_OK) ? item_count : errorcode;
}



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
int lirc_send(lirc_t* list, int item_count)
{
    int n;
    int done;
    int todo;
    int result;

    if (lirc_driver == 0)
    {
        APP_ERROR("Device is not open");
        return LIRC_E_DEVICE_NOT_OPEN;
    }

    /* Send the complete list in one go to the driver */
    done = 0;
    while (done<item_count)
    {
        todo = item_count - done;

        result = write(lirc_driver, &list[done],
                       todo*sizeof(lirc_t));

        if (result>0)
        {
            /*
            APP_PRINT("DEBUG:" APP_SOURCE);
            for (n=done; n<(done+result); n++)
            {
                APP_PRINT2("  0x%08x", list[done]);
            }
            APP_PRINT("\n");
            */

            /* Increase number of bytes sent */
            done += result;
        }
        else
        {
            APP_ERROR("Function write() failed");
            return LIRC_E_DEVICE_ERROR;
        }
    }

    return LIRC_OK;
}

