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
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define SERVER_NAME "farnsworth.ecst.csuchico.edu"
#define SERVER_PORT "80"
#define REQUEST "GET /lab_docs/reset_instructions.pdf HTTP/1.0\n\n"

ssize_t recvx(int sockfd, void *buf, size_t len);
int open_ip_socket(const char* host, const char* port);
void print_string(FILE* io, const char* string, size_t string_len);

int main(int argc, char** argv)
{
	int open_fd;
	int out_fd;
	char *out_file_name;
	char buffer[256];
	ssize_t written;
	ssize_t bytes_read;
	size_t nbytes;

	// open connection to SERVER_NAME at port SERVER_PORT
	open_fd = open_ip_socket(SERVER_NAME, SERVER_PORT);

	if (open_fd == -1)
	{
		fprintf(stderr, "Could not open connection to `%s:%s'\n", SERVER_NAME, SERVER_PORT);
		return -2;
	}

	// send REQUEST
	send(open_fd, REQUEST, strlen(REQUEST), 0);
	
	nbytes = sizeof(buffer);
	out_file_name = "local_file";
	
	out_fd = open(out_file_name, O_RDWR | O_CREAT, S_IRWXU);

	do
	{
	if((bytes_read = recvx(open_fd, buffer, nbytes)) == -1)
	{
		printf("Could not read the file\n");
		return 1;
	}

	if(bytes_read > 0)
	{  
		if((written = write(out_fd, buffer, bytes_read)) == -1)
		{
			printf("Unable to write to file\n");
			perror("");
			return 1;
		}
	}

	}while(bytes_read != 0);

	close(open_fd);
	close(out_fd);

	return 0;
}

ssize_t recvx(int sockfd, void *buf, size_t len)
{
  char* ptr = (char*)buf;
  ssize_t len_recv = 0;
  ssize_t len_total = 0;
  
  // Need to loop because signal handlers can interrupt a recv call
  while (len_total < len)
  {
    len_recv = recv(sockfd, ptr, len - len_total, MSG_DONTWAIT);
    
    if (len_recv == -1)
    {
      if (errno == EAGAIN ||
          errno == EWOULDBLOCK)
      {
        // Not actual errors, try again
        continue;
      }
      else if (errno == ECONNABORTED || errno == ECONNRESET)
      {
        // Connection reset or broken
        break;
      }
      else
      {
        // Real error
        return -1;
      }
    }
    else if (len_recv == 0)
    {
      // End of data
      break;
    }
    else
    {
      // Successful read
      ptr += len_recv;
      len_total += len_recv;
    }
  }
  
  return len_total;
}

int open_ip_socket(const char* host, const char* port)
{
	struct addrinfo hints;
	struct addrinfo *addr_base = NULL, *addr_cur = NULL;
	int retval = 0;
	int open_fd = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	// Lookup IP address for `host'
	retval = getaddrinfo(host, port, &hints, &addr_base);

	// If the lookup failed (!= 0)
	if (retval)
	{
    // Spout an error message
		fprintf(stderr, "Could not lookup host '%s': %s.\n", host, gai_strerror(retval));
		exit(1);
	}

	// A DNS lookup can return multiple IP addresses
	for (addr_cur = addr_base; addr_cur != NULL; addr_cur = addr_cur->ai_next)
	{
		// For each one, create a socket
		if ((open_fd = socket(addr_cur->ai_family, addr_cur->ai_socktype, addr_cur->ai_protocol)) == -1 )
		{
			// Could not open socket
			continue;
		}
    
		// And try to connect it
		if (connect(open_fd, addr_cur->ai_addr, addr_cur->ai_addrlen) != -1)
		{
			// Successful connection, start cleanup
			break;
		}
    
		// Good socket, bad connection
		close(open_fd);
	}
  
	// Cleanup time
	freeaddrinfo(addr_base);
  
	return open_fd;
}
