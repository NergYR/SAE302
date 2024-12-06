/* simpleClientSocket.c */ 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 
 
int main(int argc, char *argv[]) 
{ 
    int sockfd, portno, n; 
    struct sockaddr_in serv_addr; 
    struct hostent *server; 
     
    char buffer[256]; 
     
    if (argc < 3 ) { 
        fprintf(stderr,"usage %s hostname port\n", argv[0]); 
        exit(0); 
    } 
    portno = atoi(argv[2]); 
     
    /* Create a socket point */ 
    sockfd = socket(PF_INET, SOCK_STREAM, 0); 
     
    server = gethostbyname(argv[1]); 
     
    //bzero((char *) &serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    bcopy((char *)server->h_addr_list[0], // identique à memcpy 
 	(char *)&serv_addr.sin_addr.s_addr, server->h_length); 
 
    serv_addr.sin_port = htons(portno); 
    
    /* Now connect to the server */ 
    connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)); 
     
    /* Now ask for a message from the user, this message 
     * will be read by server 
     */ 
    printf("Please enter the message: "); 
    bzero(buffer,256); 
    fgets(buffer,255,stdin); 
     
    /* Send message to the server */ 
    n = write(sockfd,buffer,strlen(buffer)); 
     
    /* Now read server response */ 
    bzero(buffer,256); 
    n = read(sockfd,buffer,255); 
     
    printf("%s\n",buffer); 
    return 0; 
} 
