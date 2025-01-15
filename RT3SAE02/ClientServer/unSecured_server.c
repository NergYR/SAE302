#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

#define BUFFER_SIZE 1024

void doprocessing(int sock) {
    int n;
    char buffer[BUFFER_SIZE];
    DIR *d;
    struct dirent *dir;
    char file_list[BUFFER_SIZE] = "";

    // Liste des fichiers
    d = opendir("fiches");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                strcat(file_list, dir->d_name);
                strcat(file_list, "\n");
            }
        }
        closedir(d);
    } else {
        perror("ERROR opening directory");
        exit(1);
    }

    // Envoi de la liste
    n = write(sock, file_list, strlen(file_list));
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    // Lecture du fichier demandé
    bzero(buffer, BUFFER_SIZE);
    n = read(sock, buffer, BUFFER_SIZE-1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    buffer[strcspn(buffer, "\r\n")] = 0;

    // Ouverture et envoi du fichier demandé
    char filepath[BUFFER_SIZE];
    snprintf(filepath, sizeof(filepath), "fiches/%s", buffer);
    
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        char *error_msg = "File not found\n";
        write(sock, error_msg, strlen(error_msg));
        return;
    }

    while ((n = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        write(sock, buffer, n);
    }
    fclose(file);
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

    /* Start listening for the clients */
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
    return 0;
}