/**-----------------------------------------------------------------------------
	*
	* Header file to src/menu.c
	*
	* Created: 2013-10-18
	* Revised:
	*
	*---------------------------------------------------------------------------*/

#ifndef __MENU_H__
	#define __MENU_H__

	#include <ctype.h>
	#ifndef __STDIO_H__
		#define __STDIO_H__
		#include <stdio.h>
	#endif
	#ifndef __STDLIB_H__
		#define __STDLIB_H__
		#include <stdlib.h>
	#endif
	#ifndef __STRING_H__
		#define __STRING_H__
		#include <string.h>
	#endif

	#include "bool.h"
	#include "clear.h"

	#define BUFF_SIZE 32

int
get_input ( int * input ) ;
int
menu ( void ) ;
void
print_menu ( void ) ;
int
validate_input ( char * str ) ;

#endif
