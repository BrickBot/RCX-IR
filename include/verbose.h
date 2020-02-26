/***************************************************************
*                                                              *
* verbose.h                                                    *
*                                                              *
* Description:                                                 *
* Headerfile which is used to display debug and error messages *
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
#ifndef _VERBOSE_H
#define _VERBOSE_H

/* Automatically print errors if debugging is enabled */
#ifdef APP_PRINT_DEBUG
#  define APP_PRINT_ERROR
#endif

/* If debug or error messages are enabled, */
/* then define the print enable flag       */
#ifdef APP_PRINT_ERROR
#  define APP_PRINT_ENABLE
#endif

#define APP_SOURCE            " " __FILE__ ":" __FUNCTION__ "()\t"

#ifdef APP_PRINT_ENABLE
#  include <stdio.h>
#  define APP_FLUSH           fflush(stderr);
#else
#  define APP_FLUSH           ;
#endif

#ifdef APP_PRINT_ERROR
#  define APP_ERROR(str)      fprintf(stderr, "ERROR:" \
                                      APP_SOURCE str "!\n");APP_FLUSH
#else
#  define APP_ERROR(str)      ;
#endif

#ifdef APP_PRINT_DEBUG
#  define APP_PRINT(str)      fprintf(stderr, str)
#  define APP_PRINT2(str,arg) fprintf(stderr, str, arg)
#  define APP_DEBUG(str)      fprintf(stderr, "DEBUG:" \
                                      APP_SOURCE str ".\n");APP_FLUSH
#else
#  define APP_PRINT(str)      ;
#  define APP_PRINT2(str,arg) ;
#  define APP_DEBUG(str)      ;
#endif

#else
#error -- verbose.h -- included twice, or more...
#endif /* _VERBOSE_H */
