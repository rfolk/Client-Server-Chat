//------------------------------------------------------------------------------
//
// Header file to src/menu.c
//
// Created: 2013-10-18
// Revised:
//
//------------------------------------------------------------------------------

#ifndef __MENU_H__
	#define __MENU_H__

	#include <ctype.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#include "bool.h"

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
