/***************************************************************
*                                                              *
* Lircsend.java                                                *
*                                                              *
* Description:                                                 *
* This class implements the RCX protocol interface.            *
* It translates GUI commands into a RCX byte buffer.           *
* This class uses the Lirchandler class .                      *
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

public class Lircsend {
	
	final static int  RCX_BUFFER_LENGTH = 1024;
	
	// *** GUI command codes ***
	final static byte RCX_MV_STOP     = 0x24;
	final static byte RCX_MV_FORWARD  = 0x21;
	final static byte RCX_MV_BACKWARD = 0x27;
	final static byte RCX_MV_LEFT     = 0x23;
	final static byte RCX_MV_RIGHT    = 0x25;
	
	final static byte RCX_CMD_ALIVE   = 0x10;
	
	// *** RCX command codes ***
	// byte 0 values:
	final static byte RCX_ALIVE      = 0x10;
	final static byte MOTOR_ONOFF    = 0x21;
	final static byte MOTOR_POWER    = 0x13;
	
	// byte 1 values:
	final static byte MOTOR_BACKWARD = 0x00;
	final static byte MOTOR_STOP     = 0x04;
	final static byte MOTOR_FORWARD  = 0x08;
	
	final static byte MOTOR_NR_0     = 0x00; // left
	final static byte MOTOR_NR_1     = 0x10; // not used
	final static byte MOTOR_NR_2     = 0x20; // right
	
	final static byte MOTOR_PWR_0    = 0x00; // left
	final static byte MOTOR_PWR_1    = 0x01; // not used
	final static byte MOTOR_PWR_2    = 0x02; // right
	
	// *** class variables ***
	private byte[]      buffer;
	private Lirchandler lirc;


	public Lircsend() {
		
		buffer = new byte[RCX_BUFFER_LENGTH];
		lirc = new Lirchandler();
		
		if (lirc.rcx_open() != 0)
		{
			System.out.println("lirc: rcx_open failed!");	
		}	
	}
	
	
	public void sendLircCommand(byte command) {
		
		int result = 0;
		int length = 0;
		
		switch(command) {
			case RCX_MV_STOP:
				buffer[0] = MOTOR_ONOFF;
				buffer[1] = MOTOR_STOP + MOTOR_NR_0;
				length    = 2;
				send(length); // send 2X to stop 2 engines
				buffer[1] = MOTOR_STOP + MOTOR_NR_2;
				break;
			case RCX_MV_FORWARD:
				buffer[0] = MOTOR_ONOFF;
				buffer[1] = MOTOR_FORWARD + MOTOR_NR_0;
				length    = 2;
				send(length);
				buffer[1] = MOTOR_FORWARD + MOTOR_NR_2;
				break;
			case RCX_MV_BACKWARD:
				buffer[0] = MOTOR_ONOFF;
				buffer[1] = MOTOR_BACKWARD + MOTOR_NR_0;
				length    = 2;
				send(length);
				buffer[1] = MOTOR_BACKWARD + MOTOR_NR_2;
				break;
			case RCX_MV_LEFT:
				buffer[0] = MOTOR_ONOFF;
				buffer[1] = MOTOR_STOP + MOTOR_NR_0;
				length    = 2;
				send(length);
				buffer[1] = MOTOR_FORWARD + MOTOR_NR_2;
				break;
			case RCX_MV_RIGHT:
				buffer[0] = MOTOR_ONOFF;
				buffer[1] = MOTOR_FORWARD + MOTOR_NR_0;
				length    = 2;
				send(length);
				buffer[1] = MOTOR_STOP + MOTOR_NR_2;
				break;
			case RCX_CMD_ALIVE:
				buffer[0] = RCX_CMD_ALIVE;
				length    = 1;
				break;	
			default:
				System.out.println("Invalid command received from GUI! STOP");
				buffer[0] = MOTOR_ONOFF;
				buffer[1] = MOTOR_STOP + MOTOR_NR_0;
				length    = 2;
				send(length);
				buffer[1] = MOTOR_STOP + MOTOR_NR_2;
		}
		
		send(length);
	}
	
	
	private void send(int len) {
		
		int buf0 = buffer[0];
		int buf1 = buffer[1];
		int buf2 = buffer[2];
		
		int result = lirc.rcx_send(len, buf0, buf1, buf2);
	}
	
	
	public void finalize() {
		
		int dummy = lirc.rcx_close();
	}
	
}

