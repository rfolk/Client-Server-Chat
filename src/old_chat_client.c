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
//int write_flag ;
int sock ;

int
main ( int argc , char ** argv )
{
	int port ;
	sock = -1 ;
	struct sockaddr_in address ;
	struct hostent * host ;
	int user = FALSE ;
	message * msg = malloc ( sizeof ( message ) ) ;
	char str[ 512 ] ;
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
	message * msg = malloc ( sizeof ( message ) ) ;
	sock = *((int *)s);

	choice = menu () ;
	//write_flag = 1 ;

	while ( TRUE )
	{
		recv ( sock , msg , sizeof ( message ) , 0 ) ;
		switch ( msg->msg_type )
		{
			case 1 :
				printf ( "%s\n" , msg->payload ) ;
				choice = menu () ;
				//write_flag = 1 ;
				break ;

			case 3 :
				//write_flag = 1;
				choice = 4;
				printf ( "Would you like to chat with " ) ;
				printf ( "%s" , msg->payload ) ;
				printf ( ": y/n\n" ) ;
				break;
			case 2 :
				printf ( "%s\n" , msg->payload ) ;
				//write_flag = 0;
				break;
			/*
			case 4 :
				printf ( "%s\n" , msg->payload ) ;
				break ;

			case 5 :
				write_flag = 1;
				choice = 5;
			*/
			//case 8 :
			//	send ( sock, msg, sizeof ( message ) , 0 ) ;
			//	break ;

			case 6 :
				printf ( "%s\n" , msg->payload ) ;
				//write_flag = 1;
				choice = 5;
				break;

			case -3 :
				printf ( "That is not a registered user.\n" ) ;
				//write_flag = 1;
				choice = 1;
				break;
			case -2 :
				printf ( "%s does not want to chat.\n" , msg->payload ) ;
				//write_flag = 1;
				choice = menu();
				break;

			default :
				break ;
		}
	}
}

void *
client_write ( void * s )
{
	message * msg = malloc ( sizeof ( message ) ) ;
	sock = *((int *)s);

	//char buffer[ 512 ] ;
	while ( TRUE )
	{
		if ( choice == 1 )
		{
			msg->msg_type = 2 ;
			msg->payload[ 0 ] = '\0' ;
			send ( sock , msg , sizeof ( message ) , 0 ) ;
			//printf ( "request list" ) ;
			//write_flag = 0 ;
		}
		if ( choice == 2 )
		{
			msg->msg_type = 2 ;
			msg->payload[ 0 ] = '\0' ;
			send ( sock , msg , sizeof ( message ) , 0 ) ;
			printf( "Who do you wish to chat with?\n" ) ;
			msg->msg_type = 3 ;
			getline ( msg->payload , 512 , stdin ) ;
			send ( sock , msg , sizeof ( message ) , 0 ) ;
		}
		if ( choice == 3 )
		{
			msg->msg_type = 9 ;
			msg->payload[ 0 ] = '\0' ;
			send ( sock , msg , sizeof ( message ) , 0 ) ;
			exit ( 0 ) ;
		}
		if ( choice == 4 )
		{
			//if yes send 4 if no send -2
			char c = getchar();
			if ( c == 'y' || c == 'Y' )
			{
				msg->msg_type = 4 ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
				//write_flag = 0 ;
			}
			else
			{
				msg->msg_type = -2 ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
				//write_flag = 0 ;
			}
		}
		if ( choice == 5 )
		{
			getline ( msg->payload , 512 , stdin ) ;
			send ( sock , msg , sizeof ( message ) , 0 ) ;
		}

		// switch ( write_flag )
		// {
		// 	case 1 :
		// 		break ;
		// 	case 2 :
		// 		msg->msg_type = 3 ;
		// 		printf ( "User to chat with: " ) ;
		// 		scanf ( "%s" , buffer ) ;
		// 		clear () ;
		// 		strcpy ( msg->payload , buffer ) ;
		// 		send ( sock , msg , sizeof ( message ) , 0 ) ;
		// 		recv ( sock , msg , sizeof ( message ) , 0 ) ;
		// 		if ( msg->msg_type == -1 )
		// 			break ;
		// 		while ( TRUE )
		// 		{
		// 			scanf ( "%s" , buffer ) ;
		// 			clear () ;
		// 			strcpy ( msg->payload , buffer ) ;
		// 			msg->msg_type = 4 ;
		// 			send ( sock , msg , sizeof ( message ) , 0 ) ;
		// 		}
		// 		break ;
		// 	case 3 :
		// 		pthread_exit ( ( void * ) 0 ) ;
		// 		break ;
		// 	default :
		// 		break ;
		// }
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
	else if ( choice == 4 )
	{
		msg->msg_type = 7 ;
		msg->payload[ 0 ] = '\0' ;
		send ( sock , msg , sizeof ( message ) , 0 ) ;
	}
	else
	{
		msg->msg_type = 9 ;
		msg->payload[ 0 ] = '\0' ;
		send ( sock , msg , sizeof ( message ) , 0 ) ;
		exit ( 0 ) ;
	}
}




