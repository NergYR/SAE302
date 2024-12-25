#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"

void doprocessing(int sock) {
    int n;
    char buffer[256];

    bzero(buffer, 256);

    n = read(sock, buffer, 255);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    // Nettoyer le nom du fichier reçu
    buffer[strcspn(buffer, "\r\n")] = 0; // Supprimer les caractères de nouvelle ligne
    printf("Received filename: '%s'\n", buffer);

    char* file_content = sendfile(buffer);
    if (file_content != NULL) {
        size_t total_sent = 0;
        size_t file_size = strlen(file_content);
        printf("[DEBUG] File size: %ld\n", file_size);
        while (total_sent < file_size) {
            n = write(sock, file_content + total_sent, file_size - total_sent);
            if (n < 0) {
                perror("ERROR writing to socket");
                free(file_content);
                exit(1);
            }
            total_sent += n;
        }
        free(file_content);

        // Envoyer l'ACK après l'envoi complet du fichier
        n = write(sock, "ACK\n", 4);
        if (n < 0) {
            perror("ERROR writing ACK to socket");
            exit(1);
        }
        printf("[DEBUG] ACK sent\n");
    } else {
        n = write(sock, "ERROR: File not found\n", 22);
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }
}

int unSecured_server(int port) {
    int sockfd, newsockfd;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    pid_t pid;

    /* First call to socket() function */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    /* Start listening for the clients, here process will
       go in sleep mode and will wait for the incoming connection */
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        /* Create child process */
        pid = fork();
        if (pid < 0) {
            perror("ERROR on fork");
            exit(1);
        }

        if (pid == 0) {
            /* This is the client process */
            close(sockfd);
            doprocessing(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
    }
}