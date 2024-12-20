/* multipleServerSocket.c */ 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>



// Function to get configuration from file 
void getServerPort(const char *filename, int *port, char* serverName) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "ServerPort", 10) == 0) {
            sscanf(line, "ServerPort = \"%d\"", port);
        } else if (strncmp(line, "Servername", 10) == 0) {
            sscanf(line, "Servername = \"%[^\"]\"", serverName);
        }
    }

    fclose(file);
}
 
void doprocessing (int sock) 
{ 
    int n; 
    char buffer[256]; 
     
    bzero(buffer,256); 
     
    n = read(sock,buffer,255); 
    // Write JSON data into json file
    FILE *file = fopen("data.json", "w");
    if (file == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "{\"data\": \"%s\"}\n", buffer);
    fprintf(file, "%s", buffer);
    fclose(file);
    // End writing JSON data into json file


    printf("Here is the message: %s\n",buffer); 
    n = write(sock,"I got your message",18);      
} 


int main( int argc, char *argv[] ) 
{ 
    int sockfd, newsockfd, portno; 
    unsigned int clilen; 
    char buffer[256]; 
    struct sockaddr_in serv_addr, cli_addr; 
    int  n; 
    pid_t pid; 
    int port;
    char serverName[256];
    getServerPort("/home/kali/CODE/SAE302/config.conf", &port, serverName);
    printf("Server port: %d\n", port);
    printf("Server name: %s\n", serverName);




    /* First call to socket() function */ 
    sockfd = socket(PF_INET, SOCK_STREAM, 0); 
     
    /* Initialize socket structure */ 
    bzero((char *) &serv_addr, sizeof(serv_addr)); 
     
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(port); 
     
    /* Now bind the host address using bind() call.*/ 
    bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); 
     
    /* Now start listening for the clients, here 
     * process will go in sleep mode and will wait 
     * for the incoming connection */ 
    listen(sockfd,5); 
    clilen = sizeof(cli_addr); 
     
    while (1) 
    { 
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,  
                           &clilen); 
         
        /* Create child process */ 
        pid = fork(); 
         
        if (pid == 0) 
        { 
            /* This is the client process */ 
            close(sockfd); 
            doprocessing(newsockfd); 
            exit(0); 
        } 
        else 
        { 
            close(newsockfd); 
        } 
    } /* end of while */ 
} 
