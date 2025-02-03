#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "client_back.h"


void error(const char *msg) {
    perror(msg);
    exit(0);
}

int connect_to_server(const char *hostname, int portno, int *sockfd) {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0) {
        error("[ERROR] opening socket");
        return 1;
    }

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "[ERROR] no such host\n");
        return 1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("[ERROR] connecting");
        return 1;
    }

    return 0;
}