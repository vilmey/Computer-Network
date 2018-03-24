//	CSCI 446
//	Program 1 - pcounter	
//	Vilmey Filho
//	Nicolas Peres
//

//Library memory, string, error
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

//Library to use the socket
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

// lib to conver the host in ip
#include <netdb.h>

//Code definitions
#define REQUEST "GET /~kkredo/file.html HTTP/1.0\n\n"
#define SERVER_NAME "www.ecst.csuchico.edu"

//#define MAXSIZE 800
#define MAX 500

#define MAXIMO 1024
#define SERVER_PORT 80
#define SEARCH_TAG "<p>"


int main(int argc, char *argv[]) {
  int	sockfd,n;
  char 	*url;
  char	host[MAX] = SERVER_NAME;	
  char	page[MAX] = REQUEST;
  
  
  //Argumentos de entrada
  if(argc != 2)
  {
	system("clear");
	printf("Not enough arguments <len>\n\n");
	exit(1);
  }
		
  //Conversion of the size of the chunck to integer
  //packet_size = atoi(argv[1]);
	int MAXSIZE = atoi(argv[1]);
	
	char	buff[MAXSIZE],	sBuff[MAXSIZE];	

// struct to convert the host to ip
  struct hostent *ip;
// socket's struct
  struct sockaddr_in end;

// função da lib netdb.h para converter ai nosso host para IP
  ip = gethostbyname(host);
  url=inet_ntoa(*(struct in_addr *)ip->h_addr);
  
/* 
separamos buff para pegar resposta do server
mas antes preenchemos a memoria com \0
usamos sbuff para armazenar dados do envio
*/
  memset(buff,'\0',sizeof(char)*MAXSIZE);
  memset(sBuff,'\0',sizeof(char)*MAXSIZE);
  //strcpy(sBuff,getpage);
	strcpy(sBuff,page);
// condições simples em caso de erro
  if( (sockfd=socket(AF_INET,SOCK_STREAM,0)) < 0 ){
    perror("socket");
    return errno;
  }

// definimos a porta usado no caso 80

  end.sin_family=AF_INET; 
  end.sin_port=htons(SERVER_PORT);

  if( inet_pton(AF_INET,url,&end.sin_addr) < 0){
    perror("inet_pton");
    return errno;
  }

  memset(end.sin_zero,'\0',8);

  if(connect(sockfd,(struct sockaddr*)&end,sizeof(struct sockaddr)) < 0){
    perror("connect");
    return errno;
  }

  if( (send(sockfd, sBuff, strlen(sBuff), 0)) < 0){
    perror("send");
    close(sockfd);
    return errno;
  }
  
  int count = 0, i;
  
  
  
// recebe a resposta e mostra saida
  while((n=read(sockfd,buff,MAXSIZE)) > 0)
  {
  	//fprintf(stdout,"%s",buff);
  	 for (i = 0; i<MAXSIZE; i++) {	//for (i = 0; buff[i] != '\0'; i++) {
   	   if (buff[i] == '<'){
   	   	if (buff[i+1] == 'p'){
   	   		if (buff[i+2] == '>'){
   	   			count++;
   	   		}
   	   	}
   	   }
   	}
  }
  
  system("clear");
  printf("\n============================\n");
  printf("Number of <p> flags = %d\n", count);
  printf("============================\n\n");
  
  
  close(sockfd);
  return 0;
}

