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

	#include <pthread.h>
	#include <netdb.h>

	int
	main ( int argc , char ** argv ) ;

	void *
	client_read ( int sock ) ;

	void *
	client_write ( int sock ) ;

#endif