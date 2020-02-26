/***************************************************************
*                                                              *
* JniRcxIr.c                                                   *
*                                                              *
* Description:                                                 *
* Java Native Interface to RCX command communicator.           *
* When rebuilding jnilirc_JniRcxIr.so first remove 			   * 
* jnilirc_JniRcxIr.o 										   *
* and liblircwrapper.so from the command line!                 *
*                                                              *
* Author:    Bas Soons		                                   *
* begin      Tue Okt 9 2002                                    *
* copyright  (C) 2002 by Keno Vrisekoop                        *
* email      Bas.Soons@ordina.nl                               *
*                                                              *
* License:                                                     *
+ -                                                            *
*                                                              *
* History:                                                     *
* v 0.1   Nov 22 2002    Bas Soons		                       *
*         Initial version                                      *
*                                                              *
***************************************************************/

#include<stdio.h>
#include<string.h>
#include <jni.h>

#include "jnilirc_JniRcxIr.h"
#include "rcx.h"

// *** Use this function in case of testing without RCX ***
int rcx_sendTest(int* len, char* buf)
{
	//printf("SendTest id=%d.\n", buf[3]);
	if (buf[3] == 0x64)
	{
		
		buf[0] = 0xff; 
		buf[1] = 5;     // length
			
		buf[2] = 0x64;  // message id RCX Battery request
		buf[3] = 1;     // data length
		buf[4] = 0x31;  // Value 0 - 100
			
		buf[5] = 0x62;  // message id CollisionBack
		buf[6] = 0;     // data length
			
		*len   = 7;
	
		return RCX_OK;
	}
	if (buf[3] == 0x21)
	{
	
		buf[0] = 0xff; 
		buf[1] = 7;     // length
			
		buf[2] = 0x61;  // message id CollisionFront
		buf[3] = 0;     // data length
			
		buf[4] = 0x21;  // message id  forward
		buf[5] = 1;     // data length
		buf[6] = 0x00;  // OK
			
		buf[7] = 0x60;  // message id RCX battery low
		buf[8] = 0;     // data length
			
		*len    = 9;
			
		return RCX_OK;
	}
	
	if (buf[3] == 0x50)
	{
		//printf("0x50");
		buf[0] = 0xff; 
		buf[1] = 2;     // length
			
		buf[2] = 0x00;  // message id CollisionFront
		buf[3] = 0;     // data length

		*len    = 4;
			
		return RCX_OK;
	}
	
	return 1;	
}

JNIEXPORT jint JNICALL Java_jnilirc_JniRcxIr_open
(JNIEnv * env, jobject object, jstring aString)
{
	int result = 0;
	printf("JNI lirc: rcx_init called.\n");
	
	result = rcx_open();
	result = 1;
	
	return (jint)result;	
}

JNIEXPORT jint JNICALL Java_jnilirc_JniRcxIr_close
  (JNIEnv * env, jobject object)
{
	int result = 0;
	printf("JNI lirc: rcx_close called.\n");
	
	result = rcx_close();
	result = 1;
	
	return result;	
}

JNIEXPORT jint JNICALL Java_jnilirc_JniRcxIr_send
  (JNIEnv * env, jobject object, jbyteArray aArray, jint aInt)
{
	
}

JNIEXPORT jint JNICALL Java_jnilirc_JniRcxIr_write
  (JNIEnv * env, jobject object, jbyteArray aArray, jint aInt) 
{
  	int result = 0;
	
	const char *cbuf;
	int i;	
	// *** RCX len[0] = Java jlen ***;
	//(*env)->GetIntArrayRegion(env, jlen, 0, 1, len); 
	// Typecast the given length
	int givenLength = ((int) aInt);
	// *** RCX bufffer = Java jbuffer ***
	cbuf = (*env)->GetByteArrayElements(env, aArray, 0); 
   
    //printf("JNI lirc: rcx_send called, with messageID=%d.and length %d\n", cbuf[3], aInt);
    
	
	for (i=0; i < givenLength; i++)
	{
		result = rcx_send_byte(cbuf[i]);  
	}
		
	/*printf("\nJNI lirc: TEST MODUS CALL\n");
	result = rcx_sendTest(len, buffer);*/
	
	// *** Store RCX results in Java references ***
	if (result == RCX_OK)
	{
		printf("Send successful\n");
	}
	 
	
	return (jint)1;		
}

JNIEXPORT jint JNICALL Java_jnilirc_JniRcxIr_read
  (JNIEnv * env, jobject object, jbyteArray aArray)
{

	char buffer[3];
	
	const char *cbuf;
	cbuf = (*env)->GetByteArrayElements(env, aArray, 0);
	strcpy(buffer, cbuf);
	/*
	buffer[0] = 0x7; //has to return
	printf("JNI:: Set ACKBYTE: %d", buffer[0]);
	(*env)->SetByteArrayRegion(env, aArray, 0, 1, buffer);
	
    printf("Reading byte:%d.\n", buffer[0]);*/
    
    rcx_receive_byte(&buffer[0]);
 	(*env)->SetByteArrayRegion(env, aArray, 0, 1, buffer);   
    return 1; 	
}

/*
 * Class:     jnilirc_JniRcxIr
 * Method:    intTest
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jnilirc_JniRcxIr_intTest
  (JNIEnv * env, jobject obj)
{
	printf("INTINTINT INTINT\n");
	return 1;
}

/*
 * Class:     jnilirc_JniRcxIr
 * Method:    message
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_jnilirc_JniRcxIr_message
  (JNIEnv * env, jobject obj, jstring str)
{
	printf("MESSAGE MESSAGE\n");
	return 1;	
}
  
/*
 * Class:     jnilirc_JniRcxIr
 * Method:    test
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jnilirc_JniRcxIr_test
  (JNIEnv * env, jobject obj)
{
  	printf("TEST TEST TEST\n");  	
}
  
