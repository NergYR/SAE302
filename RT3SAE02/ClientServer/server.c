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
void getConf(const char *filename, int *port, char* serverName, char* isSecured, char* username, char* password, char* ssh_key) {
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
        } else if (strncmp(line, "Username", 8) == 0) {
            sscanf(line, "Username = \"%[^\"]\"", username);
        } else if (strncmp(line, "Password", 8) == 0) {
            sscanf(line, "Password = \"%[^\"]\"", password);
        } else if (strncmp(line, "ssh_key", 7) == 0) {
            sscanf(line, "ssh_key = \"%[^\"]\"", ssh_key);
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
    char username[256];
    char password[256];
    char ssh_key[256];
    getConf("/home/kali/CODE/SAEDEV/SAE302/config.conf", &port, serverName, isSecured, username, password, ssh_key);
    printf("Server port: %d\n", port);
    printf("Server name: %s\n", serverName);
    printf("Is secured: %s\n", isSecured);
    printf("Username: %s\n", username);

    if(strncmp(isSecured, "true", 4)  == 0) {
        printf("Secured server\n");
        secured_server(port, ssh_key, username, password);
    } else {
        printf("Unsecured server\n");
        unSecured_server(port);

    }


} 
