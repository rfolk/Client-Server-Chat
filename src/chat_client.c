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

int choice ;
int block_flag ;
int sock ;
message * msg ;
char str[ 512 ] ;
char input ;

int
main ( int argc , char ** argv )
{
	int port ;
	sock = -1 ;
	struct sockaddr_in address ;
	struct hostent * host ;
	int user = FALSE ;
	msg = malloc ( sizeof ( message ) ) ;
	pthread_t thread_read ;
	pthread_t thread_write ;
	//connection_t * connection ;

	//ctrl c signal handling
	( void ) signal ( SIGINT, catch_ctrlc );

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
		send ( sock , msg , sizeof ( message ) , 0 ) ;
		recv ( sock , msg , sizeof ( message ) , 0 ) ;
		if ( msg->msg_type == 1 )
			user = TRUE ;
		else if ( msg->msg_type == -1 )
			printf ( "User already in use.\n" ) ;
	}

	pthread_create ( &thread_read , NULL , client_read , &sock ) ;
	//pthread_detach ( thread_read );

	pthread_create ( &thread_write , NULL , client_write , &sock ) ;
	//pthread_detach ( thread_write );

  while (TRUE) ;

	return 0 ;
}

void *
client_read (void * s )
{
	sock = *((int *)s);

	while ( TRUE )
	{
		recv ( sock , msg , sizeof ( message ) , 0 ) ;
		switch ( msg->msg_type )
		{
			case 0 :
				printf ( "%s\n" , msg->payload ) ;
				break ;
			case -1 :
				block_flag = FALSE ;
				break ;
			case -2 :
				printf ( "Would you like to chat with " );
				printf ( "%s: y/n\n" , msg->payload ) ;
				scanf ( "%c" , &input ) ;
/*				block_flag = TRUE ;
				msg->payload[ 0 ] = '\0' ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
				block_flag = FALSE ;*/
				break ;
			case -3 :
				printf ( "Chat denied.\n" ) ;
				block_flag = FALSE ;
				break ;
			case -4 :
				printf( "%s\n" , msg->payload ) ;
				break ;
			case -5 :
				printf( "%s" , msg->payload ) ;
				block_flag = FALSE ;
				break ;
			case -6 :
				printf ( "Chat ended.\n" ) ;
				print_menu () ;
				scanf ( "%c" , &input ) ;
				break ;

		}
	}
}

void *
client_write ( void * s )
{
	sock = *((int *)s);

	print_menu () ;
	scanf ( "%c" , &input ) ;

	while ( TRUE )
	{
		handle_input ();
		switch ( choice )
		{
			case 1 :
				printf ("made it.");
				msg->msg_type = 1 ;
				msg->payload[ 0 ] = '\0' ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
				block_flag = TRUE ;
				while ( block_flag ) ;
				print_menu () ;
				scanf ( "%c" , &input ) ;
				break ;

			case 3 :
				msg->msg_type = 10 ;
				msg->payload[ 0 ] = '\0' ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
				exit ( 0 ) ;
				break ;

			case 2 :
				msg->msg_type = 1 ;
				msg->payload[ 0 ] = '\0' ;
				block_flag = TRUE ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
				while ( block_flag ) ;
				block_flag = TRUE ;
				printf ( "before type change \n");
				msg->msg_type = 2 ;
				printf ( "after type change \n");
				printf ( "Choose a user to chat with: \n" ) ;
				scanf ( "%s" , msg->payload ) ;
				//getline ( msg->payload , 512 , stdin ) ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
				while ( block_flag ) ;
				//chat denied go back to menu
				if ( msg->msg_type == 3 )
				{
					print_menu () ;
					scanf ( "%c" , &input ) ;
					break ;
				}
				choice = 5 ;
				break ;
			case 5 :
				fgets ( msg->payload , 512 , stdin ) ;
				//getline ( msg->payload , 512 , stdin ) ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
				break ;
			case 6 :
				msg->payload[ 0 ] = '\0' ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
			default :
				break ;
		}
	}
}

void
catch_ctrlc ( int sig )
{
	message * msg = malloc ( sizeof ( message ) ) ;

	if ( !choice )
	{
		exit ( 0 );
	}
	else if ( choice == 5 )
	{
		msg->msg_type = 6 ;
		msg->payload[ 0 ] = '\0' ;
		send ( sock , msg , sizeof ( message ) , 0 ) ;
		printf ( "Chat ended.\n" ) ;
		print_menu () ;
		scanf ( "%c" , &input ) ;
	}
	else
	{
		msg->msg_type = 10 ;
		msg->payload[ 0 ] = '\0' ;
		send ( sock , msg , sizeof ( message ) , 0 ) ;
		exit ( 0 ) ;
	}
}

void
print_menu ( void )
{
	//printf ( "\n" ) ;
	printf ( "1. List users\n" ) ;
	printf ( "2. Chat\n" ) ;
	printf ( "3. Exit\n" ) ;
}

void
handle_input ( void )
{
	if ( input == '1' )
		choice = 1 ;
	else if ( input == '2' )
		choice = 2 ;
	else if ( input == '3' )
		choice = 3 ;
	else if ( input == 'y' || input == 'Y' )
	{
		msg->msg_type = 4 ;
		choice = 6 ;
	}
	else
	{
		msg->msg_type = 3 ;
		choice = 6 ;
	}
}