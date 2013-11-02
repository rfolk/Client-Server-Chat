/**-----------------------------------------------------------------------------
	*
	* Header file for Chat Client
	* Sets up a client for the chat program.
	*
	* Created: 2013-10-23
	* Revised:
	*	http://www.mario-konrad.ch/wiki/doku.php?id=programming:multithreading:tutorial-04
	*
	*---------------------------------------------------------------------------*/

#include "chat_client.h"

message * msg ;

int
main ( int argc , char ** argv )
{
	int port ;
	int sock = -1 ;
	struct sockaddr_in address ;
	struct hostent * host ;
	int user = FALSE ;
	msg = malloc ( sizeof ( message ) ) ;
	char str[ 512 ] ;
	pthread_t thread_read ;
	pthread_t thread_write ;
	connection_t * connection ;

	/* checking commandline parameter */
	if ( argc != 3 )
	{
		printf ( "Run using: %s hostname port\n" , argv[ 0 ] ) ;
		return -1 ;
	}

	/* obtain port number */
	if ( sscanf ( argv[ 2 ] , "%d" , &port ) <= 0 )
	{
		fprintf ( stderr , "%s -- ERROR -- Wrong parameter: port #\n" , argv[ 0 ] ) ;
		return -2 ;
	}

	/* create socket */
	sock = socket ( AF_INET , SOCK_STREAM , IPPROTO_TCP ) ;

	if ( sock <= 0 )
	{
		fprintf ( stderr , "%s -- ERROR -- Cannot create socket\n" , argv[ 0 ] ) ;
		return -3 ;
	}

	/* connect to server */
	address.sin_family = AF_INET ;
	address.sin_port = htons ( port ) ;
	host = gethostbyname( argv[ 1 ] ) ;
	if ( !host )
	{
		fprintf ( stderr , "%s -- ERROR -- Unknown host: %s\n" , argv[ 0 ] , argv[ 1 ] ) ;
		return -4 ;
	}
	memcpy ( &address.sin_addr , host->h_addr_list[ 0 ] , host->h_length ) ;

	if ( connect ( sock , ( struct sockaddr * ) &address , sizeof ( address ) ) )
	{
		fprintf ( stderr , "%s -- ERROR -- Cannot connect to host: %s\n" , argv[ 0 ] , argv[ 1 ] ) ;
		return -5 ;
	}

	while ( user == FALSE )
	{
		msg->msg_type = 0 ;
		printf ( "Choose an user name: " ) ;
		scanf ( "%s" , str ) ;
		clear () ;
		if ( strcmp( str , "-1" ) == 0 )
			break ;
		strcpy ( msg->payload , str ) ;
		//printf ( "%s\n" , str ) ;
		write ( sock , msg , sizeof ( message ) ) ;
		read ( sock , msg , sizeof ( message ) ) ;
		if ( msg->msg_type == 1 )
			user = TRUE ;
		else if ( msg->msg_type == -1 )
			printf ( "User already in use.\n" ) ;
	}

	pthread_create ( &thread_read , NULL , client_read , &sock ) ;
	//pthread_detach ( thread_read );

	pthread_create ( &thread_write , NULL , client_write , &sock ) ;
	//pthread_detach ( thread_write );



	/* send text to server */
	/*len = strlen ( argv[ 3 ] ) ;
	write ( sock , &len , sizeof ( int ) ) ;
	write ( sock , argv[ 3 ] , len ) ;*/
  while (TRUE) ;
	/* close socket */
	//close( sock ) ;

	//return 0 ;
}

void *
client_read (void * s )
{
	int sock=*((int *)s);

	while ( TRUE )
	{
		read ( sock , msg , sizeof ( message ) ) ;
		if ( msg->msg_type == 4 )
			printf ( "%s\n" , msg->payload ) ;
		else if ( msg->msg_type == 5 )
		{
			pthread_exit ( ( void * ) 0 ) ;
			return ;
		}
	}
}

void *
client_write ( void * s )
{
	int sock=*((int *)s);

	int choice ;
	char buffer[ 512 ] ;
	while ( TRUE )
	{
		choice = menu () ;
		switch ( choice )
		{
			case 1 :
				msg->msg_type = 2 ;
				msg->payload[ 0 ] = '\0' ;
				write ( sock , msg , sizeof ( message ) ) ;
				printf ( "request list" ) ;
				break ;
			case 2 :
				msg->msg_type = 3 ;
				printf ( "User to chat with: " ) ;
				scanf ( "%s" , buffer ) ;
				clear () ;
				strcpy ( msg->payload , buffer ) ;
				write ( sock , msg , sizeof ( message ) ) ;
				read ( sock , msg , sizeof ( message ) ) ;
				if ( msg->msg_type == -1 )
					break ;
				while ( TRUE )
				{
					scanf ( "%s" , buffer ) ;
					clear () ;
					strcpy ( msg->payload , buffer ) ;
					msg->msg_type = 4 ;
					write ( sock , msg , sizeof ( message ) ) ;
				}
				break ;
			case 3 :
				pthread_exit ( ( void * ) 0 ) ;
				return ;
				break ;
		}
		return ;
	}
}
