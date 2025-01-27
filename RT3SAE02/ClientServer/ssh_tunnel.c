#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include "server.h"



void send_file_content(ssh_channel channel, const char* filename) {
    char* file_content = sendfile(filename);
    if (file_content != NULL) {
        size_t total_sent = 0;
        size_t file_size = strlen(file_content);
        printf("[DEBUG] File size: %ld\n", file_size);
        while (total_sent < file_size) {
            int nbytes = ssh_channel_write(channel, file_content + total_sent, file_size - total_sent);
            if (nbytes < 0) {
                fprintf(stderr, "[ERROR] Échec envoi contenu fichier\n");
                free(file_content);
                return;
            }
            total_sent += nbytes;
        }
        free(file_content);

        // Envoyer l'ACK après l'envoi complet du fichier
        ssh_channel_write(channel, "\nACK\n", 4);
        printf("[DEBUG] ACK sent\n");
    } else {
        ssh_channel_write(channel, "ERROR: File not found\n", 22);
    }
}


void handle_session_input(ssh_channel channel) {
    char buffer[BUFFER_SIZE];
    char complete_buffer[BUFFER_SIZE * 10] = {0};
    int nbytes;

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    if (nbytes > 0) {
        buffer[nbytes] = '\0';
        printf("[DEBUG] Reçu: %s\n", buffer);

        // Nettoyer le nom du fichier reçu
        buffer[strcspn(buffer, "\r\n")] = 0;
        printf("Received filename: '%s'\n", buffer);

        // Construire le chemin complet du fichier
        char filepath[BUFFER_SIZE];
        snprintf(filepath, sizeof(filepath), "%s%s", FILE_PATH, buffer);

        // Envoyer le contenu du fichier
        FILE *file = fopen(filepath, "r");
        if (file != NULL) {
            while ((nbytes = fread(buffer, 1, sizeof(buffer) - 1, file)) > 0) {
                ssh_channel_write(channel, buffer, nbytes);
            }
            fclose(file);

            // Envoyer un marqueur de fin de fichier explicite
            const char* end_marker = "\nENDOFFILE\n";
            ssh_channel_write(channel, end_marker, strlen(end_marker));

            // Attendre la réception du fichier modifié
            int total_received = 0;
            memset(complete_buffer, 0, sizeof(complete_buffer));
            
            while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer) - 1, 0)) > 0) {
                buffer[nbytes] = '\0';
                
                if (strstr(buffer, "END_OF_FILE") != NULL) {
                    strncat(complete_buffer + total_received, buffer, nbytes);
                    char* end = strstr(complete_buffer, "\nEND_OF_FILE");
                    if (end != NULL) {
                        *end = '\0';
                        break;
                    }
                }
                
                if (total_received + nbytes >= sizeof(complete_buffer)) {
                    ssh_channel_write(channel, "Erreur: fichier trop grand\n", 27);
                    return;
                }
                
                memcpy(complete_buffer + total_received, buffer, nbytes);
                total_received += nbytes;
            }

            if (total_received > 0) {
                // Création du nom de fichier horodaté
                time_t now = time(NULL);
                struct tm *tm_info = localtime(&now);
                char timestamp[20];
                strftime(timestamp, 20, "%Y%m%d_%H%M%S", tm_info);
                
                char new_filepath[BUFFER_SIZE];
                char *dot = strrchr(filepath, '.');
                if (dot != NULL) {
                    strncpy(new_filepath, filepath, dot - filepath);
                    new_filepath[dot - filepath] = '\0';
                    snprintf(new_filepath + strlen(new_filepath), BUFFER_SIZE - strlen(new_filepath), 
                            "_presence_%s.csv", timestamp);
                }

                // Écriture du fichier modifié
                FILE *newfile = fopen(new_filepath, "w");
                if (newfile != NULL) {
                    fprintf(newfile, "%s", complete_buffer);
                    fclose(newfile);
                    printf("[DEBUG] Fichier sauvegardé : %s\n", new_filepath);
                    ssh_channel_write(channel, "Fichier d'appel sauvegardé avec succès.\n", 40);
                } else {
                    ssh_channel_write(channel, "Erreur lors de la sauvegarde du fichier\n", 39);
                }
            } else {
                ssh_channel_write(channel, "Erreur lors de la réception du fichier\n", 39);
            }
        } else {
            ssh_channel_write(channel, "ERROR: File not found\n", 22);
        }
    }
}

void send_message(ssh_channel channel) {
    const char *message = "Communication sécurisé en cours...\nVeuillez choisir un fichier à télécharger\nFichiers disponibles :\n- RT1FA.csv\n- RT1FI.csv\n- RT2FA.csv\n- RT2FI.csv\n";
    

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
            const char* auth_password = ssh_message_auth_password(message);
            if (strcmp(ssh_message_auth_user(message), username) == 0 &&
                auth_password != NULL && strcmp(auth_password, password) == 0) {
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
