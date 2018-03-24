/*
* Nathaniel Murphy, Vilmey Filho
* CSCI 446
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>

#include <netinet/in.h>
#include <netinet/ip.h>

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    const char *port;
    int sock;
    int sock_fd;
    int client_fd;
    int count = 1500;
    int temp;
    
    struct addrinfo addr;
    struct addrinfo *results;
    
    int file_size;
    char filename[256];
    int bytes_sent;
    
    if (argc == 2)
    {
        port = argv[1];
    }
    else
    {
        printf("Need a port number.");
        return 1;
    }
	
	memset(&addr, 0, sizeof(struct addrinfo));
	addr.ai_family = AF_INET;
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_flags = AI_PASSIVE;
	addr.ai_protocol = 0;
	addr.ai_canonname = NULL;
	addr.ai_addr = NULL;
	addr.ai_next = NULL;
	
	if((sock = getaddrinfo(NULL, port, &addr, &results)) != 0)
	{
		perror("getaddrinfo()");
		return 1;
	}
    
    // make a socket, and check for errors
    if( ( sock = socket(results->ai_family, results->ai_socktype, results->ai_protocol) ) == -1 )
    {
        perror("socket()");
        return 1;
    }
    
    if((bind(sock, results->ai_addr, results->ai_addrlen)) == -1)
    {
        perror("bind()");
        return 1;
    }
    
    if(listen(sock, 1) == -1)
    {
        perror("listen()");
        return 1;
    }
    
    if( (sock_fd = accept(sock, results->ai_addr, &(results->ai_addrlen)) ) == -1 )
    {
        perror("accept()");
        return 1;
    }
    
    if((file_size = recv(sock_fd, filename, sizeof(filename), 0) ) == -1)
    {
        perror("recv()");
        return 1;
    }
    
    filename[file_size] = '\0';
    if(filename[strlen(filename) - 1] == '\n')
        filename[strlen(filename) - 1] = '\0';
    if(filename[strlen(filename) - 1] == '\r')
        filename[strlen(filename) - 1] = '\0';
    
    if((client_fd = open(filename, O_RDONLY) ) == -1)
    {
        perror("open()");
        return 1;
    }
    
    temp = htonl(count);
    
    do
    {
        if( (bytes_sent = send(client_fd, &temp, sizeof(temp), 0)) == -1)
        {
            perror("send()");
            return 1;
        }
    }while(bytes_sent > 0);
    
    close(client_fd);
    close(sock_fd);
    
    return 0;
}
