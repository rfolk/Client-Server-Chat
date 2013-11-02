/**-----------------------------------------------------------------------------
	*
	* Header file for Chat Client
	* Sets up a client for the chat program.
	*
	* Created: 2013-10-23
	* Revised:
	*
	*---------------------------------------------------------------------------*/

#ifndef __CHAT_CLIENT__
	#define __CHAT_CLIENT__

	#include "bool.h"
	#include "chat_message.h"
	#include "connection_t.h"
	#include "menu.h"
	#include "clear.h"

	/*
	#include <pthread.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <linux/in.h>
	*/

	#include<stdio.h>
	#include<stdlib.h>
	#include<unistd.h>
	#include<string.h>
	#include<sys/types.h>
	#include<errno.h>
	#include<sys/socket.h>
	#include<netinet/in.h>
	#include<arpa/inet.h>
	#include<netdb.h>
	#include<pthread.h>

	int
	main ( int argc , char ** argv ) ;

	void *
	client_read ( void * s ) ;

	void *
	client_write ( void * s ) ;

#endif
