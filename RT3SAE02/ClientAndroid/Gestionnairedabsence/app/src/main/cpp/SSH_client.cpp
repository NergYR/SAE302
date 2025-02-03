#include <libssh/libssh.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Client.h"  // Inclure le header avec les définitions des fonctions
#include "client_back.h"

#define BUFFER_SIZE 4096
#define MAX_STUDENTS 100

void error_ssh(ssh_session session, const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, ssh_get_error(session));
    ssh_disconnect(session);
    ssh_free(session);
    exit(1);
}

// Renommer la fonction main en ssh_client_main
int ssh_client_main(int argc, char *argv[]) {
    ssh_session session;
    ssh_channel channel;
    char buffer[BUFFER_SIZE];
    int rc;

    if (argc < 5) {
        fprintf(stderr, "Usage: %s hostname port username password\n", argv[0]);
        exit(1);
    }

    // Initialisation de la session SSH
    session = ssh_new();
    if (session == NULL) {
        exit(1);
    }

    // Configuration de la session
    ssh_options_set(session, SSH_OPTIONS_HOST, argv[1]);
    ssh_options_set(session, SSH_OPTIONS_PORT_STR, argv[2]);
    ssh_options_set(session, SSH_OPTIONS_USER, argv[3]);

    // Connexion au serveur
    rc = ssh_connect(session);
    if (rc != SSH_OK) {
        error_ssh(session, "Connection failed");
    }

    // Authentification
    rc = ssh_userauth_password(session, NULL, argv[4]);
    if (rc != SSH_AUTH_SUCCESS) {
        error_ssh(session, "Authentication failed");
    }

    // Création du canal
    channel = ssh_channel_new(session);
    if (channel == NULL) {
        error_ssh(session, "Channel creation failed");
    }

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        ssh_channel_free(channel);
        error_ssh(session, "Channel opening failed");
    }

    // Lecture du message initial du serveur
    int nbytes = ssh_channel_read(channel, buffer, BUFFER_SIZE - 1, 0);
    if (nbytes > 0) {
        buffer[nbytes] = '\0';
        printf("%s", buffer);
    }

    // Sélection du fichier
    printf("File Selected : ");
    fgets(buffer, BUFFER_SIZE - 1, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    // Envoi du nom de fichier
    ssh_channel_write(channel, buffer, strlen(buffer));

    // Lecture du contenu du fichier
    Student students[MAX_STUDENTS];
    int nbStudents = 0;
    char fileContent[BUFFER_SIZE * 10] = {0};
    int totalReceived = 0;

    while ((nbytes = ssh_channel_read(channel, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[nbytes] = '\0';
        
        // Vérifier si on a reçu le marqueur de fin
        if (strstr(buffer, "\nENDOFFILE\n") != NULL) {
            char* end = strstr(buffer, "\nENDOFFILE\n");
            *end = '\0';
            if (end > buffer) {
                strcat(fileContent + totalReceived, buffer);
            }
            break;
        }

        strcat(fileContent + totalReceived, buffer);
        totalReceived += nbytes;

        // Vérifier si le buffer est plein
        if (totalReceived >= (BUFFER_SIZE * 10) - BUFFER_SIZE) {
            fprintf(stderr, "File too large\n");
            return 1;
        }
    }

    if (totalReceived == 0) {
        fprintf(stderr, "No data received\n");
        return 1;
    }

    // Traitement de l'appel
    printf("\nDébut de l'appel...\n");
    processAttendance(fileContent, students, &nbStudents);

    // Création du CSV modifié
    char* modifiedCSV = createModifiedCSV(students, nbStudents);
    printf("\nEnvoi du fichier modifié au serveur...\n");
    printf("\n[DEBUG] Contenu du CSV à envoyer :\n%s\n", modifiedCSV);

    // Préparation et envoi du fichier modifié
    char finalBuffer[BUFFER_SIZE * 10];
    snprintf(finalBuffer, sizeof(finalBuffer), "%s\nEND_OF_FILE\n", modifiedCSV);
    
    size_t total_length = strlen(finalBuffer);
    size_t sent = 0;
    while (sent < total_length) {
        nbytes = ssh_channel_write(channel, finalBuffer + sent,
                                 (total_length - sent < BUFFER_SIZE) ? total_length - sent : BUFFER_SIZE);
        if (nbytes < 0) {
            error_ssh(session, "Error sending modified file");
        }
        sent += nbytes;
    }

    // Lecture de la confirmation
    nbytes = ssh_channel_read(channel, buffer, BUFFER_SIZE - 1, 0);
    if (nbytes > 0) {
        buffer[nbytes] = '\0';
        printf("%s", buffer);
    }

    // Nettoyage
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);

    return 0;
}

// Nouvelle fonction main qui sera le point d'entrée
int main(int argc, char *argv[]) {
    return ssh_client_main(argc, argv);
}
