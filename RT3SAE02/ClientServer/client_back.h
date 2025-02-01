#ifndef CLIENT_BACK_H
#define CLIENT_BACK_H

#include "server.h"  // Pour BUFFER_SIZE
#include "Client.h"  // Pour Student

void error(const char *msg);
int connect_to_server(const char *hostname, int portno, int *sockfd);

#endif