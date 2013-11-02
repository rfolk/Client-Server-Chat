


#ifndef __CHAT_USER_H__
  #define __CHAT_USER_H__

	typedef struct user
	{
		char name[ 512 ] ;
		char ip[ 16 ] ;
		int  port ;
		int  socket ;
		struct user * partner ;
		struct user * next ;
	} user ;

#endif