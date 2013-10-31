#ifndef __CHAT_SERVER__
	#define __CHAT_SERVER__

	#include "bool.h"
	#include "chat_message.h"
	#include "connection_t.h"

	#include <pthread.h>
	#include <linux/in.h>
	#include <unistd.h>

	#define CLIENT_LIST "connected_clients.txt"

	void
	* process ( void * ptr ) ;

	int
	main ( int argc , char ** argv ) ;

	int
	check_user ( char * str ) ;

	void
	list_user ( int sock ) ;


#endif
