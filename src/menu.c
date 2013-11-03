/**-----------------------------------------------------------------------------
	* Chat Menu Function
	*
	* Creates the user interface for the client-server chat program.
	*
	* Created: 2013-10-18
	* Revised:
	*---------------------------------------------------------------------------*/

#include "menu.h"

/**-----------------------------------------------------------------------------
	* Main Menu
	* Prints the menu, secures and validates the input, returns the input to the
	* main program.
	* @return validated input
	*---------------------------------------------------------------------------*/
int
menu ( void )
{
	clear () ;
	int choice ;
	int valid = FALSE ;
	do
	{
		print_menu () ;
		valid = get_input ( &choice ) ;
		if ( choice > 3 || choice < 1 )
		{
			printf ( "Invalid choice." ) ;
			valid = FALSE ;
		}
		clear () ;
	}
	while ( valid == FALSE ) ;
	return choice ;
}

/**-----------------------------------------------------------------------------
	* Secures Input
	* Gets the input from the command line and returns it to the menu. Will
	* validate that the input is a number
	* @param  pointer to the choice value
	* @return the validity of the input (number or not)
	*---------------------------------------------------------------------------*/
int
get_input ( int * input )
{
	char buffer[ BUFF_SIZE ] ;
	if ( fgets ( buffer , sizeof ( buffer ) , stdin ) != NULL )
	{
		buffer[ strlen( buffer ) - 1 ] = '\0' ;
		if ( validate_input ( buffer ) == TRUE )
		{
			*input = atoi ( buffer ) ;
			return TRUE ;
		}
	}
	return FALSE ;
}

/**-----------------------------------------------------------------------------
	* Validates For Number
	* Checks that every character in the buffer is a digit so that it will be an
	* actual number.
	* @param  pointer to the buffer
	* @return the number in the buffer
	*---------------------------------------------------------------------------*/
int
validate_input ( char * str )
{
	unsigned int index ;
	for ( index = 0 ; index < strlen ( str ) ; index ++ )
	{
		if ( !isdigit ( str[ index ] ) )
			return FALSE ;
	}
	return TRUE;
}

/**-----------------------------------------------------------------------------
	* Prints the Menu
	*---------------------------------------------------------------------------*/
void
print_menu ( void )
{
	//printf ( "\n" ) ;
	printf ( "1. List users\n" ) ;
	printf ( "2. Chat\n" ) ;
	printf ( "3. Exit\n" ) ;
}
