# compiles

CC = gcc -std=c99 -pipe -pedantic -ggdb -Wall
CFLAGS = -Iinclude -lpthread

server: src/chat_server.c
	$(CC) $(CFLAGS) -o server src/chat_server.c

client: src/chat_client.c src/menu.c src/clear.c
	$(CC) $(CFLAGS) -o bin/client src/chat_client.c src/menu.c src/clear.c


#gcc -std=c99 -pipe -pedantic -ggdb -Iinclude -Wall -o bin/server src/chat_server.c -lpthread
#gcc -std=c99 -pipe -pedantic -ggdb -Iinclude -Wall -o bin/client src/chat_client.c src/menu.c src/clear.c -lpthread