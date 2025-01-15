#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "client_back.h"

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    int portno;
    int sockfd;
    char buffer[BUFFER_SIZE];
    int n;

    // Vérification des arguments
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    // Conversion du port en entier
    portno = atoi(argv[2]);

    // Connexion au serveur
    if (connect_to_server(argv[1], portno, &sockfd) != 0) {
        exit(1);
    }
    
    // Lecture du message initial du serveur
    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        error("[ERROR] reading from socket");
    }
    printf("%s\n", buffer);

    // Demande du fichier à l'utilisateur
    printf("File Selected : ");
    bzero(buffer, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE - 1, stdin);

    // Envoi du nom de fichier au serveur
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("[ERROR] writing to socket");
    }

    // Lecture de la réponse du serveur
    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        error("[ERROR] reading from socket");
    }

    // Affichage de la réponse
    printf("%s\n", buffer);

    // Fermeture de la connexion
    close(sockfd);
    return 0;
}