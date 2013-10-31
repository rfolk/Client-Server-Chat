/**-----------------------------------------------------------------------------
	*
	* Header file for BOOL
	* Defines the boolean data types for convenience.
	*
	* Created: 2013-10-18
	* Revised:
	*
	*---------------------------------------------------------------------------*/

#ifndef __CHAT_MESSAGE_H__
	#define __CHAT_MESSAGE_H__

	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>

	/**---------------------------------------------------------------------------
		*	Message Types
		*		-2 - Chat Denied
		*		-1 - Invalid message
		*		 0 - Petition for Username
		*		 1 - Username accepted
		*		 2 - List of available users
		*		 3 - Request Chat Connection
		*		 4 - Chat Started
		*		 5 - Message to Peer
		*		 6 - End Chat
		*		 7 - Disconnect
		*-------------------------------------------------------------------------*/
	typedef struct
	{
		int msg_type ;
		char payload[ 512 ] ;
	} message ;

#endif
