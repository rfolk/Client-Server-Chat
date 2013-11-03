

#include "chat_server.h"

void
* process ( void * ptr )
{
	connection_t * conn ;
	conn = ( connection_t * ) ptr ;
	message * msg = malloc ( sizeof ( message ) ) ;
	message * tmp_msg = malloc ( sizeof ( message ) ) ;
	user * this_user ;
	int has_user = FALSE ;
	int i = 0 ;
	if ( !ptr )
		pthread_exit ( 0 ) ;

	while ( !has_user )
	{
		recv ( conn->sock , msg , sizeof ( message ) , 0 ) ;
		if ( msg->msg_type == 0 && check_user ( msg->payload ) )
		{
			this_user = new_user ( ptr , msg->payload ) ;
			has_user = TRUE ;
			//print_user ( this_user ) ;
			msg->msg_type = 1 ;
			msg->payload[ 0 ] = '\0' ;
			send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
			free ( conn ) ;
		}
		else
		{
			msg->msg_type = -1 ;
			msg->payload[ 0 ] = '\0' ;
			connection_t * conn = (connection_t *) ptr ;
			send ( conn->sock , msg , sizeof ( message ) , 0 ) ;
		}
	}

	while ( TRUE )
	{
		printf ( "%d:\n" , i ++ ) ;
		print_user ( this_user ) ;
		recv ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
		switch ( msg->msg_type )
		{
			case -2 :
				strcpy ( msg->payload , this_user->name ) ;
				send ( this_user->partner->socket , msg , sizeof ( message ) , 0 ) ;
				this_user->partner->partner = NULL ;
				this_user->partner = NULL ;
				break ;
			case 2 :
				list_user ( this_user->socket , msg ) ;
				msg->msg_type = 1 ;
				msg->payload[ 0 ] = '\0' ;
				send ( sock , msg , sizeof ( message ) , 0 ) ;
				break ;
			case 3 :
				this_user->partner = add_partner ( msg->payload ) ;
				if ( !this_user->partner )
				{
					msg->msg_type = -3 ;
					msg->payload[ 0 ] = '\0' ;
					//strcat ( msg->payload , " is not a registered user.\n" ) ;
					send ( this_user->socket, msg , sizeof ( message ) , 0 ) ;
					list_user ( this_user->socket , msg ) ;
					print_user ( this_user ) ;
				}
				else
				{
					this_user->partner->partner = this_user ;
					strcpy ( msg->payload , this_user->name ) ;
					send ( this_user->partner->socket , msg , sizeof ( message ) , 0 ) ;
				}
				break ;
			case 4 : // chat accepted
				remove_from_file ( this_user->name ) ;
				remove_from_file ( this_user->partner->name ) ;
				msg->msg_type = 6 ;
				strcpy ( msg->payload , "Now chatting with " ) ;
				strcat ( msg->payload , this_user->partner->name ) ;
				strcat ( msg->payload , "." ) ;
				send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
				strcpy ( msg->payload , "Now chatting with " ) ;
				strcat ( msg->payload , this_user->name ) ;
				strcat ( msg->payload , "." ) ;
				send ( this_user->partner->socket , msg , sizeof ( message ) , 0 ) ;
				break ;
			case 6 :
				tmp_msg->msg_type = 6 ;
				// write "user: "
				strcpy ( tmp_msg->payload , this_user->name ) ;
				strcat ( tmp_msg->payload , ": " ) ;
				send ( this_user->partner->socket , tmp_msg , sizeof ( message ) , 0 ) ;
				// write message
				send ( this_user->partner->socket , msg , sizeof ( message ) , 0 ) ;
				break ;
			case 7 :
				// add user to file
				add_user_to_file ( this_user ) ;
				add_user_to_file ( this_user->partner ) ;
				msg->msg_type = 1 ;
				strcpy ( msg->payload , "Chat ended." ) ;
				send ( this_user->socket , msg , sizeof ( message ) , 0 ) ;
				send ( this_user->partner->socket , msg , sizeof ( message ) , 0 ) ;
				// remove partner
				this_user->partner->partner = NULL ;
				this_user->partner = NULL ;
				break ;
			case 9 :
				close ( this_user->socket ) ;
				remove_user ( this_user ) ;
				pthread_exit ( 0 ) ;
				break ;
		}

	}

	/* close socket and clean up */
	close ( this_user->socket ) ;
	free ( this_user ) ;
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
	FILE * fp ;
	fp = fopen ( CLIENT_LIST , "r" ) ;
	char buffer[ 512 ] ;
	int pos = 0 ;
	while ( fscanf ( fp , "%s" , buffer ) == 1 )
	{
		pos ++ ;
		if ( pos % 4 == 1 )
		{
			msg->msg_type = 2 ;
			strcpy ( msg->payload , buffer ) ;
			send ( sock , msg , sizeof ( message ) , 0 ) ;
		}
	}
	fclose ( fp ) ;
}

user *
add_partner ( char * str )
{
	user * u = head ;
	while ( u )
	{
		if ( strcmp ( u->name , str ) != 0 )
			u = u->next ;
		else
			return u ;
	}
	return FALSE ;
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
remove_user ( user * u )
{
	if ( u != head )
	{
		user * cursor = head ;
		while ( cursor->next != u )
		{
			cursor = cursor->next ;
		}
		cursor->next = u->next ;
	}
	else
	{
		head = head->next ;
	}
	remove_from_file ( u->name ) ;
	free ( u ) ;
}

void
remove_from_file ( char * str )
{
	FILE * fp ;
	FILE * tmp ;
	fp = fopen ( CLIENT_LIST , "r" ) ;
	tmp = fopen ( "temp" , "w" ) ;
	char buffer[ 512 ] ;
	int pos = 1 ;
	while ( fscanf ( fp , "%s" , buffer ) == 1 )
	{
		if ( ( pos % 4 == 1 ) && strcmp ( str , buffer ) != 0 )
		{
			fprintf ( tmp , "%s " , buffer ) ;
			fscanf ( fp , "%s" , buffer ) ;
			fprintf ( tmp , "%s " , buffer ) ;
			fscanf ( fp , "%s" , buffer ) ;
			fprintf ( tmp , "%s " , buffer ) ;
			fscanf ( fp , "%s" , buffer ) ;
			fprintf ( tmp , "%s " , buffer ) ;
		}
		pos ++ ;
	}
	fclose ( fp ) ;
	fclose ( tmp ) ;
	remove ( CLIENT_LIST ) ;
	rename ( "temp" , CLIENT_LIST ) ;
	//fseek ( fp , 0 , SEEK_SET ) ;
}

user *
new_user ( void * ptr , char * str )
{
	connection_t * conn ;
	conn = ( connection_t * ) ptr ;
	user * this_user ;
	if ( !head )
		head = malloc ( sizeof ( user ) ) ;

	this_user = head ;
	while ( this_user->next )
		this_user = this_user->next ;
	this_user->next = malloc ( sizeof ( user ) ) ;
	this_user = this_user->next ;

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

	add_user_to_file ( this_user ) ;
	return this_user ;
}

void
add_user_to_file ( user * u )
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

void
print_user ( user * u )
{
	printf ( "Name:   %s\n" , u->name ) ;
	printf ( "IP:     %s\n" , u->ip ) ;
	printf ( "Port:   %d\n" , u->port ) ;
	printf ( "Socket: %d\n" , u->socket ) ;
}

void
print_message ( message * m )
{
	printf ( "Message: %d\n" , m->msg_type ) ;
	printf ( "\"%s\"\n" , m->payload ) ;
}