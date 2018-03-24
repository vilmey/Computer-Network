/*
* Nathaniel Murphy, Vilmey Filho
* CSCI 446
* Spring 2015
* 
* This template provides the framework you need for program 1. Complete the
* functions defined and create any you feel are necessary. stream-talk-client.c
* on Learn will prove useful.
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFF_MAX 256

ssize_t recvx(int sockfd, void *buf, size_t length);
int open_socket(const char *host, const char *port);

int main(int argc, char *argv[])
{
    int open_fd;
    int out_fd;
    char buffer[BUFF_MAX];
    ssize_t bytes_read = 0;
    size_t nbytes;
    
    // Store the arguments entered by user
    const char *server = argv[1];
    const char *port = argv[2];
    const char *out_file_name = argv[3];
    
    if( (open_fd = open_socket(server, port)) == -1 )
    {
        perror("open()");
        return 1;
    }
    
    send(open_fd, out_file_name, strlen(out_file_name), 0);
    
    nbytes = sizeof(buffer);
    out_fd = open(out_file_name, O_RDWR | O_CREAT, S_IRWXU);
    
    do
    {
        if( (recvx(open_fd, buffer, nbytes)) == -1 )
        {
            perror("recx()");
            return 1;
        }
        
        if(bytes_read > 0)
        {
            if(write(out_fd, buffer, bytes_read) == -1)
            {
                perror("write()");
                return 1;
            }
        }
    } while(bytes_read != 0);
    
    close(open_fd);
    close(out_fd);
    
    return 0;
}

ssize_t recvx(int sockfd, void *buf, size_t length)
{
    char *ptr = (char*)buf;
    ssize_t length_recv;
    ssize_t length_total;
    
    while(length_total < length)
    {
        length_recv = recv(sockfd, ptr, length - length_total, MSG_DONTWAIT);
        
        // Look at what errors if any were received
        if(length_recv == -1)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                continue;
            }
            else if(errno == ECONNABORTED || errno == ECONNRESET)
            {
                break;
            }
            else
            {
                return -1;
            }
        }
        else if(length_recv == 0)
        {
            break;
        }
        else
        {
            ptr += length_recv;
            length_total += length_recv;
        }
    }
    
    return length_total;
}

int open_socket(const char *host, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *addr_base = NULL, *addr_cur = NULL;
    int retval = 0;
    int open_fd;
    
    memset(&host, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    
    printf("Get address.");
    retval = getaddrinfo(host, port, &hints, &addr_base);
    
    if(retval)
    {
        perror("getaddrinfo()");
        exit(1);
    }
    
    for(addr_cur = addr_base; addr_cur != NULL; addr_cur = addr_cur->ai_next)
    {
        if( ( open_fd = socket(addr_cur->ai_family, addr_cur->ai_socktype, addr_cur->ai_addrlen) ) == -1)
        {
            continue;
        }
        
        if(connect(open_fd, addr_cur->ai_addr, addr_cur->ai_addrlen) != -1)
        {
            break;
        }
        close(open_fd);
    }
    freeaddrinfo(addr_base);
    
    return open_fd;
}
