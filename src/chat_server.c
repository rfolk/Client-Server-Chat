

#include "chat_server.h"

void
* process ( void * ptr )
{
	connection_t * conn ;
	conn = ( connection_t * ) ptr ;
	message * msg = malloc ( sizeof ( message ) ) ;
	message * tmp_msg = malloc ( sizeof ( message ) ) ;
	user * this_user ;
	char partner_name[ 512 ] ;
	int partner_socket ;
	if ( !ptr )
		pthread_exit ( 0 ) ;

	while ( TRUE )
	{
			/* read length of message */
		read ( conn->sock , msg , sizeof ( message ) ) ;

		switch ( msg->msg_type )
		{
			case 0 :
				if ( check_user ( msg->payload ) )
				{
					this_user = new_user ( ptr , msg->payload ) ;
					msg->msg_type = 1 ;
					msg->payload[ 0 ] = '\0' ;
					write ( this_user->socket , msg , sizeof ( message ) ) ;
				}
				else
				{
					msg->msg_type = -1 ;
					msg->payload[ 0 ] = '\0' ;
					printf("set msg_type\n");
					connection_t * conn = (connection_t *) ptr ;
					write ( conn->sock , msg , sizeof ( message ) ) ;
				}
				break ;
			case -2 :
				strcpy ( msg->payload , partner_name ) ;
				strcat ( msg->payload , " does not wish to chat with you.\n" ) ;
				partner_name[ 0 ] = '\0' ;
				partner_socket = -1 ;
				write ( this_user->socket , msg , sizeof ( message ) ) ;
			case 2 :
				printf ( "huh? %d %p\n" , this_user->socket , (void *)msg ) ;
				list_user ( this_user->socket , msg ) ;
				printf( "%s\n listed" , this_user->name ) ;
				break ;
			case 3 :
				partner_socket = get_partner_socket( msg->payload ) ;
				if ( partner_socket == -1 )
				{
					msg->msg_type = -3 ;
					strcat ( msg->payload , " is not a registered user.\n" ) ;
					write ( this_user->socket, msg , sizeof ( message ) ) ;
					list_user ( this_user->socket , msg ) ;
				}
				else
				{
					strcpy ( partner_name , msg->payload ) ;
					// record partner name
					// get partner socket
					// record partner socket
					// check with potential partner
					// write payload is "initiator potential_partner"
					strcpy ( msg->payload , this_user->name ) ;
					write ( partner_socket , msg, sizeof ( message ) ) ;
				}
				break ;
			case 4 : // chat accepted
				// record partner name
				strcpy ( partner_name , msg->payload ) ;
				// get partner socket
				// record partner socket
				partner_socket = get_partner_socket ( partner_name ) ;
				// need to pass 5 to partner then have it passed back.
				tmp_msg->msg_type = 5 ;
				write ( partner_socket , tmp_msg , sizeof ( message ) ) ;
			case 5 :
				// remove user from file
				remove_user ( this_user->name ) ;
				// write now chatting with user to partner
				strcpy ( msg->payload , "Now chatting with " ) ;
				strcat ( msg->payload , this_user->name ) ;
				strcat ( msg->payload , ".\n" ) ;
				write ( partner_socket , msg , sizeof ( message ) ) ;
				break ;
			case 6 :
				tmp_msg->msg_type = 6 ;
				// write "user: "
				strcpy ( tmp_msg->payload , this_user->name ) ;
				strcat ( tmp_msg->payload , ": " ) ;
				write ( partner_socket , tmp_msg , sizeof ( message ) ) ;
				// write message
				strcat ( msg->payload , "\n" ) ;
				write ( partner_socket , msg , sizeof ( message ) ) ;
				break ;
			case 7 :
				// need to pass 8 to partner then have it passed back.
				tmp_msg->msg_type = 8 ;
				write ( partner_socket , tmp_msg , sizeof ( message ) ) ;
			case 8 :
				// add user to file
				// remove partner
				//	set partner name to null
				//	set partner socket to -1
				break ;
			case 9 :
				close ( this_user->socket ) ;
				free ( conn ) ;
				pthread_exit ( 0 ) ;
				break ;
		}

	}

	/* close socket and clean up */
	close ( this_user->socket ) ;
	free ( conn ) ;
	pthread_exit ( 0 ) ;
}

int
main ( int argc , char ** argv )
{
	/* clear out the client list at every run */
	FILE * fp = fopen ( CLIENT_LIST , "w" );
	fclose ( fp ) ;

	int sock = -1 ;
	struct sockaddr_in address ;
	int port ;
	connection_t * connection ;
	pthread_t thread ;

	/* check for command line arguments */
	if ( argc != 2 )
	{
		fprintf ( stderr , "usage: %s port\n" , argv[ 0 ] ) ;
		return -1 ;
	}

	/* obtain port number */
	if ( sscanf ( argv[ 1 ] , "%d" , &port ) <= 0 )
	{
		fprintf ( stderr , "%s: error: wrong parameter: port\n" , argv[ 0 ] ) ;
		return -2 ;
	}

	/* create socket */
	sock = socket ( AF_INET , SOCK_STREAM , IPPROTO_TCP ) ;
	if ( sock <= 0 )
	{
		fprintf ( stderr , "%s: error: cannot create socket\n" , argv[ 0 ] ) ;
		return -3 ;
	}

	/* bind socket to port */
	address.sin_family = AF_INET ;
	address.sin_addr.s_addr = INADDR_ANY ;
	address.sin_port = htons ( port ) ;
	if ( bind ( sock , ( struct sockaddr * )&address , sizeof ( struct sockaddr_in ) ) < 0 )
	{
		fprintf ( stderr , "%s: error: cannot bind socket to port %d\n" , argv[ 0 ] , port ) ;
		return -4 ;
	}

	/* listen on port */
	if ( listen ( sock , 5 ) < 0 )
	{
		fprintf ( stderr , "%s: error: cannot listen on port\n" , argv[ 0 ] ) ;
		return -5 ;
	}

	printf ( "%s: ready and listening\n" , argv[ 0 ] ) ;

	while ( TRUE )
	{
		/* accept incoming connections */
		connection = ( connection_t * ) malloc ( sizeof ( connection_t ) ) ;
		connection->sock = accept ( sock , &connection->address , &connection->addr_len ) ;
		if ( connection->sock <= 0 )
		{
			free ( connection ) ;
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create ( &thread , 0 , process , ( void * )connection ) ;
			pthread_detach ( thread ) ;
		}
	}

	return 0 ;
}

int
check_user ( char * str )
{
	printf("checkuser\n");
	FILE * fp ;
	fp = fopen( CLIENT_LIST , "r" ) ;
	char buffer[ 512 ] ;
	int pos = 0 ;
	while ( fscanf ( fp , "%s" , buffer ) == 1 )
	{
		printf ("%d:%d %s\n" , pos , pos%4 , buffer ) ;
		if ( ( pos % 4 == 0 ) && strcmp ( str , buffer ) == 0 )
		{
			printf ( "isauser\n");
			fclose ( fp ) ;
			return FALSE ;
		}
		pos ++ ;
	}
	printf("checkuserworks\n");
	fclose ( fp ) ;
	return TRUE ;
}

void
list_user ( int sock , message * msg )
{
	printf ( "Socket: %d Msg: %p\n" , sock , (void *)msg ) ;
	FILE * fp ;
	printf ( "File pointer: %p\n" , (void*)fp);
	fp = fopen ( CLIENT_LIST , "r" ) ;
	char buffer[ 512 ] ;
	int pos = 0 ;
	printf ("The file is: %p" , (void *)fp ) ;
	while ( fscanf ( fp , "%s" , buffer ) == 1 )
	{
		pos ++ ;
		printf ( "%d beep" , pos ) ;
		if ( pos % 4 == 1 )
		{
			printf ( "%s" , buffer ) ;
			msg->msg_type = 2 ;
			strcpy ( msg->payload , buffer ) ;
			write ( sock , msg , sizeof ( message ) ) ;
		}
	}
	printf ( "done");
	msg->msg_type = 1 ;
	msg->payload[ 0 ] = '\0' ;
	write ( sock , msg , sizeof ( message ) ) ;
	fclose ( fp ) ;
}

int
get_partner_socket ( char * str )
{
	int socket = -1 ;
	FILE * fp ;
	fp = fopen ( CLIENT_LIST , "r" ) ;
	char buffer [ 512 ] ;
	int pos = 0 ;
	while ( fscanf ( fp , "%s" , buffer ) == 1 )
	{
		pos++ ;
		if ( pos % 4 == 1 && strcmp ( str , buffer ) == 0 )
		{
			int temp1 , temp2 ;
			fscanf ( fp , " %d %d %d" , &temp1 , &temp2 , &socket ) ;
			fclose ( fp ) ;
			return socket ;
		}
	}
	fclose ( fp ) ;
	return socket ;
}

void
remove_user ( char * str )
{
	FILE * fp ;
	fp = fopen ( CLIENT_LIST , "a" ) ;
	fseek ( fp , 0 , SEEK_SET ) ;
	fclose ( fp ) ;
}

user *
new_user ( void * ptr , char * str )
{
	printf ("open new user \n");
	connection_t * conn ;
	conn = ( connection_t * ) ptr ;
	user * this_user ;
	if ( !head )
		head = malloc ( sizeof ( user ) ) ;

	this_user = head ;
	while ( this_user->next )
		this_user = this_user->next ;
	this_user->next = malloc ( sizeof ( user ) ) ;

	strcpy ( this_user->name , str ) ;
	this_user->socket = conn->sock ;
	this_user->port = ((struct sockaddr_in *)&conn->address)->sin_port ;

	long addr = 0 ;
	char ip_str[ 16 ] ;

	addr = ( long ) ( ( struct sockaddr_in * )&conn->address )->sin_addr.s_addr ;
	sprintf ( ip_str , "%d.%d.%d.%d" ,
						( int ) ( ( addr       ) & 0xff ) ,
						( int ) ( ( addr >>  8 ) & 0xff ) ,
						( int ) ( ( addr >> 16 ) & 0xff ) ,
						( int ) ( ( addr >> 24 ) & 0xff ) ) ;
	strcpy ( this_user->ip , ip_str ) ;

	this_user->next = 0 ;
	this_user->partner = 0 ;

	add_user_to_list ( this_user ) ;
	return this_user ;
}

void
add_user_to_list ( user * u )
{
	FILE * fp = fopen ( CLIENT_LIST , "a" ) ;
	/* print name of user */
	fprintf ( fp , "%s " , u->name ) ;
	/* print ip address of user */
	fprintf ( fp , "%s ", u->ip ) ;
	/* print port # of user */
	fprintf ( fp , "%d " , u->port ) ;
	/* print socket # of user */
	fprintf ( fp , "%d\n", u->socket ) ;
	fclose ( fp ) ;
}