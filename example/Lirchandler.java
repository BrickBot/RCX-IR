/***************************************************************
*                                                              *
* Lirchandler.java                                             *
*                                                              *
* Description:                                                 *
* This class defines native RCX calls and loads the native     *
* library liblircwrapper.so                                    * 
* This class uses the methods in the Lircwrapper.c file.       *
*                                                              *
* Author:    Keno Vrisekoop                                    *
* begin      Tue Okt 9 2002                                    *
* copyright  (C) 2002 by Keno Vrisekoop                        *
* email      keno.vrisekoop@ordina.nl                          *
*                                                              *
* License:                                                     *
* -                                                            *
*                                                              *
* History:                                                     *
* v 0.1   Oct 9 2002    Keno Vrisekoop                         *
*         Initial version                                      *
*                                                              *
***************************************************************/

package lirc;

public class Lirchandler {
	
	static {
		System.loadLibrary("lircwrapper");
	}

	public native int rcx_open();
	public native int rcx_close();
	public native int rcx_send(int len, int i0, int i1, int i2);
}

