/* multipleServerSocket.c */ 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include "server.h"


// Function to get configuration from file 
void getServerPort(const char *filename, int *port, char* serverName, char* isSecured) {
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
        } else if (strncmp(line, "IsSecured", 9) == 0) {
            sscanf(line, "IsSecured = \"%[^\"]\"", isSecured);
        }
    }

    fclose(file);
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
    char isSecured[10];
    getServerPort("/home/energetiq/Bureau/CODE/SAE302/config.conf", &port, serverName, isSecured);
    printf("Server port: %d\n", port);
    printf("Server name: %s\n", serverName);
    printf("Is secured: %s\n", isSecured);

    if(strncmp(isSecured, "true", 4)  == 0) {
        printf("Secured server\n");
        //secured_server(port);
    } else {
        printf("Unsecured server\n");
        unSecured_server(port);

    }


} 
