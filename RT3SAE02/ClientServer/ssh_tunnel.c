#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include "server.h"
#include "engine.h"  // Ajout de l'inclusion pour sendfile

// Variables globales définies dans server.h
extern char validFichesGlobal[256];
extern char fichesDirGlobal[256];

// Définir BUFFER_SIZE s'il n'est pas déjà défini
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

// Ajouter cette fonction de validation en haut du fichier
int is_valid_filename(const char* filename) {
    // Copier la liste globale pour le parsing
    char copy[256];
    strcpy(copy, validFichesGlobal);

    // Séparer par virgules
    char *token = strtok(copy, ",");
    while (token) {
        // Supprimer d’éventuels espaces
        while (*token == ' ') token++;
        if (strcmp(filename, token) == 0) {
            return 1;
        }
        token = strtok(NULL, ",");
    }
    return 0;
}

void send_file_content(ssh_channel channel, const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] Cannot open file: %s\n", filename);
        ssh_channel_write(channel, "ERROR: File not found\n", 22);
        return;
    }

    printf("[DEBUG] Sending file %s\n", filename);
    ssh_channel_write(channel, "BEGIN_FILE\n", 11);
    
    char buffer[BUFFER_SIZE];
    // Skip header
    if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
        fclose(file);
        ssh_channel_write(channel, "ERROR: Empty file\n", 17);
        return;
    }
    
    // Send content
    while (fgets(buffer, BUFFER_SIZE, file)) {
        size_t len = strlen(buffer);
        if (ssh_channel_write(channel, buffer, len) != len) {
            fprintf(stderr, "[ERROR] Failed to send file content\n");
            fclose(file);
            return;
        }
    }
    
    fclose(file);
    ssh_channel_write(channel, "END_FILE\n", 9);
    ssh_channel_write(channel, "ACK\n", 4);
    printf("[DEBUG] File sent successfully\n");
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

        // Vérifier si le nom de fichier est valide
        if (!is_valid_filename(buffer)) {
            ssh_channel_write(channel, "ERROR: Invalid filename. Please choose from the available files.\n", 61);
            return;
        }

        // Construire le chemin complet du fichier - NOUVELLE VÉRIFICATION
        char filepath[BUFFER_SIZE];
        if (buffer[0] == '/') {
            // L'utilisateur a envoyé un chemin absolu
            strcpy(filepath, buffer);
        } else {
            // On ajoute le répertoire fichesDirGlobal
            strcpy(filepath, fichesDirGlobal);
            // Vérifier le slash
            if (filepath[strlen(filepath) - 1] != '/') {
                strcat(filepath, "/");
            }
            strcat(filepath, buffer);
        }
        printf("[DEBUG] Chemin complet du fichier: %s\n", filepath);

        // Utiliser send_file_content pour envoyer le fichier
        send_file_content(channel, filepath);

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
            // Création du fichier avec timestamp
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
            }
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
    send_message(channel); 
    // Lire les données tant que le canal n’est pas fermé
    while (!ssh_channel_is_closed(channel) && !ssh_channel_is_eof(channel)) {
        handle_session_input(channel);
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
