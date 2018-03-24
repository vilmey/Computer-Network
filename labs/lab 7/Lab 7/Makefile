all : client server

client : stream-talk-client.c
	gcc -Wall -Werror -o client stream-talk-client.c

server : stream-talk-server.c
	gcc -Wall -Werror -o server stream-talk-server.c

clean :
	rm *.o