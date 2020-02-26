#################################################################
# Makefile                                                     
#                                                              
# This Makefile builds the `lego` executable. This binary uses
# the librcx.so library. This can also be used for test purposes.          
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
# v 0.1   Nov 13 2002	Henk Dekker <henk_dekker@planet.nl>
#         Initial version
#################################################################
topdir = ..
include $(topdir)/config


CFLAGS = -O2 -g -Wall $(DEBUG_FLAGS)

INCLUDES = -I../include

CC := $(TARGET)$(CC)

all: lego

lego: lego.c 
	$(CC) $(INCLUDES) $(CFLAGS) -L../build -o lego lego.c -lrcxir

install: all
	cp -f lego /usr/local/bin

remove: uninstall clean
     
uninstall: 
	rm -f /usr/local/bin/lego 

proper: clean

clean:
	rm -f lego

