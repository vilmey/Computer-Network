# Nathaniel Murphy, Vilmey Filho
all : file_client file_server

file_client : client.c
	gcc -Wall -o file_client client.c

file_server : server.c
	gcc -Wall -o file_server server.c

clean :
	rm *.o