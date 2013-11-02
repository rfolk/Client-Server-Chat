#ifndef __CHAT_SERVER__
	#define __CHAT_SERVER__

	#include "bool.h"
	#include "chat_message.h"
	#include "connection_t.h"
	#include "chat_user.h"

	#include <pthread.h>
	#include <netinet/in.h>
	/*#include <linux/in.h>*/
	#include <unistd.h>

	#define CLIENT_LIST "connected_clients.txt"

	user * head ;
	user * tail ;

	void
	* process ( void * ptr ) ;

	int
	main ( int argc , char ** argv ) ;

	int
	check_user ( char * str ) ;

	void
	list_user ( int sock , message * msg ) ;

	int
	get_partner_socket ( char * str ) ;

	void
	remove_user ( char * str ) ;

	user *
	new_user ( void * ptr , char * str ) ;

	void
	add_user_to_list ( user * u ) ;

#endif
