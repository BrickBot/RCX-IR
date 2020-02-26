#################################################################
# Makefile                                                     
#                                                              
# This Makefile builds the libJniRcxIr.so dynamic library.
# 'libJniRcxIr.so' is a JNI library, to communicate between
# Java and librcx
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
# v 0.1   Nov 13 2002    Henk Dekker <henk_dekker@planet.nl>    
#         Initial version. 
#################################################################
topdir = ..
include $(topdir)/config

CC := $(subst $CC, $(TARGET)$(CC), $CC)

CFLAGS = -O2 -fno-exceptions -g -Wall 

INCLUDES = -I../include

AR = $(TARGET)ar
RANLIB = $(TARGET)ranlib
NM = $(TARGET)nm

NAME := jnilirc_JniRcxIr

all: $(NAME).so

$(NAME).so: $(NAME).o
	$(CC) -shared -o $(NAME).so $(NAME).o -L../build -lrcxir -lc
        
$(NAME).o:
	$(CC) -fPIC -c -DLINUX $(INCLUDES) $(NAME).c

install: all
	cp -f $(NAME).so /lib

remove: uninstall clean
     
uninstall: 
	rm -f /lib/$(NAME).so

proper: clean

clean:
	rm -f $(NAME).o $(NAME).so 

