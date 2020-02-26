package jnilirc;
/***************************************************************
*                                                              *
* JniRcxIr.java                                                *
*                                                              *
* Description:                                                 *
* This class defines native RCX calls and loads the native     *
* library liblircwrapper.so.                                   *
* Be sure liblircwrapper.so path is set in LD_LIBRARY_PATH.    * 
* This class uses the methods in the JniRcxIr.c file.          *
*                                                              *
* Author:    Bas Soons                                         *
* begin      Tue Nov 22 2002                                   *
* copyright  (C) 2002 by Bas Soons		                       *
* email      Bas.Soons@ordina.nl	                           *
*                                                              *
* License:                                                     *
* -                                                            *
*                                                              *
* History:                                                     *
* v 0.1   Oct 9 2002   Bas Soons		                       *
*         Initial version                                      *
*                                                              *
***************************************************************/
public class JniRcxIr 
{
	/**
	 * Private members
	 */	
	private int usbFlag		= 0;
	private int err			= 0;
	/**
	 * -----------------------------
	 */
			
	/**
	 * Load library
	 */	
	static 
	{
		try
		{
			//System.loadLibrary("JniRcxIr.so");
			System.loadLibrary("jnilirc_JniRcxIr.so");
		}
		catch (UnsatisfiedLinkError e)
		{
			System.out.println("Library not loaded."+ e);
		}
	}
	/**
	 * -----------------------------
	 */
	
	/**
	 * Native functions
	 */
	
	/** Open the tower
   	 * @param port port to use, e.g. usb or COM1
   	 */
	public native int open(String p);
	
	/** Close the tower
   	 *@return error number or zero for success
   	 */
    public native int close();
    
    /** send a packet to the RCX, e.g 0x10 for ping
   	 * @param b packet to send
   	 * @param n number of bytes
   	 * @return error number
   	 */
    public native int send(byte b[], int n);
    
    /** Write low-level bytes to the tower, e.g 0xff550010ef10ef for ping
   	 * @param b bytes to send
   	 * @param n number of bytes
   	 * @return error number
   	 */
    public native int write(byte b[], int n);
    
    /** Low-level read
   	 * @param b buffer to receive bytes
   	 * @return number of bytes read
   	 */
 	public native int read(byte b[]);
 	
 	public native void test();
 	
 	public native int intTest();
 	public native int message(String msg);
	/**
	 * -----------------------------
	 */
	
	/**
	 * Constructor
	 */
	public JniRcxIr()
	{
		System.out.println(".JniRcxIr created.");
	}
	
	/**
	 * Member functions
	 */
	public int open() 
	{
		System.out.println(".JniRcxIr try to open");
 	   return open("");
 	 //	return 0; 
 	}
	
	public int getError() 
	{
  	  return err;
  	}
	
	public int getUsbFlag() 
	{
 	   return usbFlag;
  	}  	
}
