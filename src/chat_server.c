

#include "chat_server.h"

void
* process ( void * ptr )
{
	message * msg = malloc ( sizeof ( message ) ) ;
	connection_t * conn ;
	int i = 1 ;
	long addr = 0 ;
	char ip_str[ 16 ] ;
	char partner_name[ 80 ] ;
	int partner_socket ;
	if ( !ptr )
		pthread_exit ( 0 ) ;
	conn = ( connection_t * ) ptr ;

	while ( TRUE )
	{
			/* read length of message */
		read ( conn->sock , msg , sizeof ( message ) ) ;

		switch ( msg->msg_type )
		{
			case 0 :
				if ( check_user ( msg->payload ) )
				{
					addr = ( long ) ( ( struct sockaddr_in * )&conn->address )->sin_addr.s_addr ;
					sprintf ( ip_str , "%d.%d.%d.%d" ,
										( int ) ( ( addr       ) & 0xff ) ,
										( int ) ( ( addr >>  8 ) & 0xff ) ,
										( int ) ( ( addr >> 16 ) & 0xff ) ,
										( int ) ( ( addr >> 24 ) & 0xff ) ) ;
					FILE * fp = fopen ( CLIENT_LIST , "a" ) ;
					/* print name of user */
					fprintf ( fp , "%s " , msg->payload ) ;
					/* print ip address of user */
					fprintf ( fp , "%s ", ip_str ) ;
					/* print port # of user */
					fprintf ( fp , "%d " , ((struct sockaddr_in *)&conn->address)->sin_port) ;
					/* print socket # of user */
					fprintf ( fp , "%d\n", &conn->sock ) ;
					fclose ( fp ) ;
					msg->msg_type = 1 ;
				}
				else
					msg->msg_type = -1 ;
				msg->payload[ 0 ] = '\0' ;
				write ( conn->sock , msg , sizeof ( message ) ) ;
				printf ( "Run through: %d" , i++ ) ;
				printf ( "weird\n" ) ;
				break ;
			case -2 :
				strcpy( msg->payload , partner_name ) ;
				strcat( msg->payload , " does not wish to chat with you.\n" ) ;
				write ( conn->sock , msg , sizeof ( message ) ) ;
			case 2 :
				list_user ( conn->sock ) ;
				break ;
		}

	}

	/* close socket and clean up */
	close ( conn->sock ) ;
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
	FILE * fp ;
	fp = fopen( CLIENT_LIST , "r" ) ;
	char buffer[ 512 ] ;
	int pos = 0 ;
	while ( fscanf ( fp , "%s" , buffer ) == 1 )
	{
		pos ++ ;
		if ( ( pos % 4 == 1 ) && strcmp ( str , buffer ) == 0 )
			return FALSE ;
	}
	return TRUE ;
}

void
list_user ( int sock )
{
	message * msg = malloc ( sizeof ( message ) ) ;
	FILE * fp ;
	fp = fopen( CLIENT_LIST , "r" ) ;
	char buffer[ 512 ] ;
	int pos = 0 ;
	while ( fscanf ( fp , "%s" , buffer ) == 1 )
	{
		pos ++ ;
		if ( pos % 4 == 1 )
		{
			msg->msg_type = 2 ;
			strcpy ( msg->payload , buffer ) ;
			write ( sock , msg , sizeof ( message ) ) ;
		}
	}
	return ;
}