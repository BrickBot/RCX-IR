#################################################################
# Makefile                                                     
#                            
# Description:                                  
# This makefile builds the GuiCommandServer.jxe application. A
# lircwrapper.h headerfile is also generated. This file is the
# JNI interface to the librcx.so library.
#
# Notes:
# - Dont forget to set the IVE path of the J9 installation,
#   for example IVEHOME=/opt/wsdd/wsdd4.0/ive
# - If the interface of lircwrapper.h is changed, it is also 
#   necessay to modify lircwrapper.c in the librcx directory.
#   After these modifications the library librcx.so must be
#   rebuild.
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
# v 0.1   Oct 13 2002    Henk Dekker <henk_dekker@planet.nl>    
#         Initial makefile
#################################################################
topdir = ..
include $(topdir)/config 

BOOTCLASSPATH = $(IVEHOME)/lib/jclFoundation/classes.zip
JNI_INCLUDES=-I$(IVEHOME)/bin/include

all: $(BLD) lircwrapper.h GuiCommandServer.jxe

$(BLD):
	mkdir -p $(BLD)

finish:
	cp -f lircwrapper.h  ../include
	
clean:
	rm -rf $(BLD)
	rm -f *.log
	rm -f *.sym

proper: clean
	rm -f lircwrapper.h
	rm -f GuiCommandServer.jxe
	
lircwrapper.h: Lirchandler.class 
	$(IVEHOME)/bin/javah -o lircwrapper.h -bootclasspath $(BOOTCLASSPATH) -classpath $(BLD) lirc.Lirchandler

Lirchandler.class: $(BLD) Lirchandler.java
	$(JCC) -d $(BLD) -bootclasspath $(BOOTCLASSPATH) Lirchandler.java

Lircsend.class: Lircsend.java Lirchandler.class
	$(JCC) -d $(BLD) -bootclasspath $(BOOTCLASSPATH) Lircsend.java -classpath $(BLD)

GuiCommandServer.class: GuiCommandServer.java Lircsend.class
	$(JCC) -d $(BLD) -bootclasspath $(BOOTCLASSPATH) GuiCommandServer.java -classpath $(BLD)
 
GuiCommandServer.jxe: GuiCommandServer.class
	jxelink -d $(BLD) -o GuiCommandServer -cp $(BOOTCLASSPATH) -startupClass lirc.GuiCommandServer $(BLD)/lirc/GuiCommandServer.class $(BLD)/lirc/Lircsend.class $(BLD)/lirc/Lirchandler.class

