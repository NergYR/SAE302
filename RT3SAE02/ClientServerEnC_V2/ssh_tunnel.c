#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include "server.h"



void handle_session_input(ssh_channel channel) {
    char buffer[BUFFER_SIZE];
    int nbytes;

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    if (nbytes > 0) {
        buffer[nbytes] = '\0'; // Ajouter le caractère de fin de chaîne
        printf("[DEBUG] Reçu: %s\n", buffer);

        // Nettoyer le nom du fichier reçu
        buffer[strcspn(buffer, "\r\n")] = 0; // Supprimer les caractères de nouvelle ligne
        printf("Received filename: '%s'\n", buffer);

        // Construire le chemin complet du fichier
        char full_path[BUFFER_SIZE];
        snprintf(full_path, sizeof(full_path), "%s%s", FILE_PATH, buffer);

        // Vérifier si le fichier existe
        FILE *file = fopen(full_path, "r");
        if (file != NULL) {
            fclose(file);

            char* file_content = sendfile(buffer);
            if (file_content != NULL) {
                size_t total_sent = 0;
                size_t file_size = strlen(file_content);
                printf("[DEBUG] File size: %ld\n", file_size);
                while (total_sent < file_size) {
                    nbytes = ssh_channel_write(channel, file_content + total_sent, file_size - total_sent);
                    if (nbytes < 0) {
                        fprintf(stderr, "[ERROR] Échec envoi contenu fichier\n");
                        free(file_content);
                        return;
                    }
                    total_sent += nbytes;
                }
                free(file_content);

                // Envoyer l'ACK après l'envoi complet du fichier
                ssh_channel_write(channel, "ACK\n", 4);
                printf("[DEBUG] ACK sent\n");
            } else {
                ssh_channel_write(channel, "ERROR: File not found\n", 22);
            }
        } else {
            ssh_channel_write(channel, "ERROR: File not found\n", 22);
        }
    }
}

void send_message(ssh_channel channel) {
    const char *message = "Bienvenue sur le canal SSH !\nEntrez vos commandes:\n";
    if (ssh_channel_write(channel, message, strlen(message)) < 0) {
        fprintf(stderr, "[ERROR] Échec envoi message\n");
    }
}

void handle_channel_requests(ssh_session session, ssh_channel channel) {
    struct pollfd fds[1];
    fds[0].fd = ssh_get_fd(session);
    fds[0].events = POLLIN;

    send_message(channel);

    while (!ssh_channel_is_eof(channel)) {
        if (poll(fds, 1, 1000) > 0) {
            if (fds[0].revents & POLLIN) {
                handle_session_input(channel);
            }
        }
    }
}
void handle_client(ssh_session session, char* username, char* password) {
    ssh_message message;
    ssh_channel channel = NULL;
    int authenticated = 0;

    if (ssh_handle_key_exchange(session) != SSH_OK) {
        fprintf(stderr, "[ERROR] Échec échange clés : %s\n", ssh_get_error(session));
        return;
    }

    while (!authenticated && (message = ssh_message_get(session)) != NULL) {
        if (ssh_message_type(message) == SSH_REQUEST_AUTH &&
            ssh_message_subtype(message) == SSH_AUTH_METHOD_PASSWORD) {
            if (strcmp(ssh_message_auth_user(message), username) == 0 &&
                strcmp(ssh_message_auth_password(message), password) == 0) {
                authenticated = 1;
                ssh_message_auth_reply_success(message, 0);
                printf("[DEBUG] Authentification réussie\n");
            } else {
                ssh_message_reply_default(message);
            }
        } else {
            ssh_message_reply_default(message);
        }
        ssh_message_free(message);
    }

    if (!authenticated) {
        fprintf(stderr, "[ERROR] Authentification échouée\n");
        return;
    }

    while ((message = ssh_message_get(session)) != NULL) {
        if (ssh_message_type(message) == SSH_REQUEST_CHANNEL_OPEN &&
            ssh_message_subtype(message) == SSH_CHANNEL_SESSION) {
            channel = ssh_message_channel_request_open_reply_accept(message);
            if (channel == NULL) {
                fprintf(stderr, "[ERROR] Échec acceptation canal\n");
                ssh_message_free(message);
                continue;
            }

            printf("[DEBUG] Canal accepté\n");
            handle_channel_requests(session, channel);
            ssh_channel_free(channel);
            ssh_message_free(message);
            break;
        } else {
            ssh_message_reply_default(message);
            ssh_message_free(message);
        }
    }
}
