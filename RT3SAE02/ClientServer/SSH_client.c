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

    // Modification de la partie réception et traitement du fichier
    char fileContent[BUFFER_SIZE * 10] = {0};
    char tempBuffer[BUFFER_SIZE] = {0};
    int totalContent = 0;
    int fileStarted = 0;

    // Réception du fichier
    while ((nbytes = ssh_channel_read(channel, tempBuffer, BUFFER_SIZE - 1, 0)) > 0) {
        tempBuffer[nbytes] = '\0';
        printf("[DEBUG] Received chunk: %d bytes\n", nbytes);

        if (strstr(tempBuffer, "BEGIN_FILE") != NULL) {
            fileStarted = 1;
            char* start = strstr(tempBuffer, "BEGIN_FILE\n");
            if (start != NULL) {
                start += strlen("BEGIN_FILE\n");
                size_t len = strlen(start);
                memcpy(fileContent + totalContent, start, len);
                totalContent += len;
            }
            continue;
        }

        if (fileStarted) {
            if (strstr(tempBuffer, "END_FILE") != NULL) {
                char* end = strstr(tempBuffer, "END_FILE");
                size_t len = end - tempBuffer;
                if (len > 0) {
                    memcpy(fileContent + totalContent, tempBuffer, len);
                    totalContent += len;
                }
                break;
            }

            size_t len = strlen(tempBuffer);
            if (totalContent + len < BUFFER_SIZE * 10) {
                memcpy(fileContent + totalContent, tempBuffer, len);
                totalContent += len;
            }
        }
    }

    if (totalContent > 0) {
        fileContent[totalContent] = '\0';
        printf("\n[DEBUG] Total content received: %d bytes\n", totalContent);
        printf("[DEBUG] First few bytes of content:\n%.100s\n", fileContent);

        Student students[MAX_STUDENTS];
        int nbStudents = 0;

        // Traitement de l'appel
        processAttendance(fileContent, students, &nbStudents);

        if (nbStudents > 0) {
            // Création du CSV modifié
            char* modifiedCSV = createModifiedCSV(students, nbStudents);
            if (modifiedCSV) {
                printf("\nEnvoi du fichier modifié au serveur...\n");

                // Préparation et envoi du fichier modifié
                char* finalBuffer = malloc(strlen(modifiedCSV) + 20);
                if (finalBuffer) {
                    sprintf(finalBuffer, "%s\nEND_OF_FILE\n", modifiedCSV);
                    ssh_channel_write(channel, finalBuffer, strlen(finalBuffer));
                    free(finalBuffer);
                }
                free(modifiedCSV);
            }
        }
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
