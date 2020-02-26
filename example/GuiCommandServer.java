/***************************************************************
*                                                              *
* GuiCommandServer.java                                        *
*                                                              *
* Description:                                                 *
* This class starts a server socket and reads bytes send by a  *
* GUI application.                                             *
* This class uses the Lircsend class                           *
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

import java.net.ServerSocket;
import java.net.Socket;
import java.io.*;

public class GuiCommandServer {
	
	final static int SERVER_SOCKET_PORT       = 2222;
	
	private ServerSocket     serverSocket     = null;
	private Socket           clientSocket     = null;
	private DataInputStream  dis              = null;
	private byte             byteMessage      = 0;
	
	private Lircsend         infrared; 
	
		
	public GuiCommandServer() {
		
		try {
			serverSocket = new ServerSocket(SERVER_SOCKET_PORT);
			System.out.println("Server socket created...");
		} catch (IOException e) {
			System.out.println("Error: Server cannot connect to port!");
			System.exit(1);
		}
		
		try {
			clientSocket = serverSocket.accept();
			System.out.println("Client socket accepted...");	
		} catch (IOException e) {
			System.out.println("Error: Client socket accept failed!");
			System.exit(1);
		}
		
		try {
			dis = new DataInputStream(this.clientSocket.getInputStream());
		} catch (IOException e) {
			System.out.println("Error: Cannot create socket DataInputStream!");
			System.exit(1);
		}
		
		infrared = new Lircsend();			
	}
	
	
	public void readMessage() {
		
		System.out.println("Start reading input from socket...");
		try { 
			while (true)
			{
				byteMessage = dis.readByte();
				// System.out.println("Read " + byteMessage + " from socket.");
				infrared.sendLircCommand(byteMessage);
			}
			} catch (IOException e) {
				System.out.println("Error: Cannot read byte from socket!");
			}
	}
	
	
	public void finalize() {
		
		try {
			dis.close();
			} catch (IOException e) {}
	}
	
	
	public static void main(String[] args) {
		
		GuiCommandServer server = new GuiCommandServer();
		server.readMessage();
	}
}

