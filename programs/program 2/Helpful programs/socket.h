#ifndef SOCKETS_H
#define	SOCKETS_H

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>

void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
int send(int &clientSocket, std::string &buffer);
int recvtimeout(int s, char *buf, int len, int timeout);
int receive(int &clientSocket, std::string &buffer);
int createserver(int backLog, const char* port, int &sockfd);

#endif	/* SOCKETS_H */