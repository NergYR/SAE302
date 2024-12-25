#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>
#include "server.h"

// Structure pour les données client
typedef struct {
    ssh_session session;
    char *username;
    char *password;
} client_data_t;

// Fonction pour gérer chaque client dans un thread
void *handle_client_thread(void *arg) {
    client_data_t *data = (client_data_t *)arg;
    
    handle_client(data->session, data->username, data->password);
    
    ssh_disconnect(data->session);
    ssh_free(data->session);
    free(data);
    pthread_exit(NULL);
}

int secured_server(int port, char* hostKeyPath, char* username, char* password) {
    ssh_bind sshbind;
    ssh_session session;
    int rc;
    char port_str[7];

    sshbind = ssh_bind_new();
    if (sshbind == NULL) {
        fprintf(stderr, "[ERROR] Échec initialisation ssh_bind\n");
        return EXIT_FAILURE;
    }

    printf("[DEBUG] Initialisation serveur SSH\n");
    printf("[DEBUG] Port : %d\n", port);

    snprintf(port_str, sizeof(port_str), "%d", port);
    printf("[DEBUG] Port : %s\n", port_str);

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, "0.0.0.0");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR, port_str);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, hostKeyPath);

    rc = ssh_bind_listen(sshbind);
    if (rc < 0) {
        fprintf(stderr, "[ERROR] Échec écoute sur le port : %s\n", ssh_get_error(sshbind));
        ssh_bind_free(sshbind);
        return EXIT_FAILURE;
    }

    printf("[DEBUG] Serveur SSH démarré sur le port %d\n", port);

    while (1) {
        session = ssh_new();
        if (session == NULL) {
            fprintf(stderr, "[ERROR] Échec création session\n");
            continue;
        }

        rc = ssh_bind_accept(sshbind, session);
        if (rc != SSH_OK) {
            fprintf(stderr, "[ERROR] Échec acceptation connexion : %s\n", 
                    ssh_get_error(sshbind));
            ssh_free(session);
            continue;
        }

        printf("[DEBUG] Nouvelle connexion acceptée\n");

        // Préparation des données pour le thread
        client_data_t *client_data = malloc(sizeof(client_data_t));
        client_data->session = session;
        client_data->username = username;
        client_data->password = password;

        // Création du thread
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client_thread, client_data) != 0) {
            fprintf(stderr, "[ERROR] Échec création thread\n");
            free(client_data);
            ssh_disconnect(session);
            ssh_free(session);
            continue;
        }

        // Détachement du thread
        pthread_detach(thread);
    }

    ssh_bind_free(sshbind);
    return EXIT_SUCCESS;
}