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

int
main ( int argc , char ** argv )
{
	int port ;
	int sock = -1 ;
	struct sockaddr_in address ;
	struct hostent * host ;
	int len ;

	/* checking commandline parameter */
	if ( argc != 4 )
	{
		printf ( "Run using: %s hostname port text\n" , argv[ 0 ] ) ;
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

	/* send text to server */
	len = strlen ( argv[ 3 ] ) ;
	write ( sock , &len , sizeof ( int ) ) ;
	write ( sock , argv[ 3 ] , len ) ;

	/* close socket */
	close( sock ) ;

	return 0 ;
}
