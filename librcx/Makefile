#################################################################
# Makefile                                                     
#                                                              
# This Makefile builds the  dynamic library 'librcxir.so'. This 
# library does the RCX commands transfer via the lirc_sir
# device driver.
# It is also possible to create a static library, librcxir.a
#
# Dependencies:
# - The ARM toolchain must be installed. 
#   Download from: ftp://ftp.handhelds.org/pub/linux/arm/toolchain
# - The cross compile binaries must be in the PATH,
#   so that arm-linux-gcc, arm-linux-ar, etc can be found.
#
# Known problems:
# - The 'timeval' struct is not defined in toolchain's 'time.h'
#   To make things compile, read the compile errors and define
#   your own struct. Cannot be that difficult
#
# Defines:
#   APP_PRINT_DEBUG       Show debug data and errors
#   APP_PRINT_ERROR       Show errors
#   LIRC_TARGET_PC        Set timing values for laptop
#   LIRC_TARGET_IPAQ      Set timing values for iPAQ
#
# Author:                                                      
# begin      Tue Nov 13 2002                                    
# copyright  (C) 2002 by Henk Dekker                           
# email      henk_dekker@planet.nl                             
#                                                              
# License:                                                     
# This program is free software; you can redistribute it       
# and/or modify it under the terms of the GNU General Public   
# License as published by the Free Software Foundation; either 
# version 2 of the License, or (at your option) any later      
# version.                                                     
#                                                              
# History:                                                     
# v 0.1   Nov 13 2002    Henk Dekker <henk_dekker@planet.nl>    
#         Initial version. 
#################################################################
topdir = ..
include $(topdir)/config

#DEBUG_FLAGS = -DAPP_PRINT_ERROR
DEBUG_FLAGS = -DAPP_PRINT_DEBUG
DEBUG_FLAGS += -DLIRC_TARGET_IPAQ

#CFLAGS = -O2 -g -Wall $(DEBUG_FLAGS)
CFLAGS = -g -Wall $(DEBUG_FLAGS)
INCLUDES = -I../include

CC := $(TARGET)$(CC)
objects := $(patsubst %.c, %.o, $(wildcard *.c))

.SUFFIXES: .c .o


all: librcxir.so

librcxir.so: $(objects)
	$(CC) -shared -o $@ $(objects)

.c.o:
	$(CC) -fPIC -c $(INCLUDES) $(CFLAGS) -o $@ $<

install: all
	cp -f librcxir.so /lib

remove: uninstall clean
     
uninstall: 
	rm -f /lib/librcxir.so

proper: clean

clean:
	rm -f *.so *.o
