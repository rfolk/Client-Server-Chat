#ifndef __CHAT_SERVER__
	#define __CHAT_SERVER__

	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <sys/socket.h>
	#include <linux/in.h>
	#include <unistd.h>

	typedef struct
	{
		int sock ;
		struct sockaddr address ;
		int addr_len ;
	} connection_t ;

	void
	* process ( void * ptr ) ;

	int
	main ( int argc , char ** argv ) ;

#endif
