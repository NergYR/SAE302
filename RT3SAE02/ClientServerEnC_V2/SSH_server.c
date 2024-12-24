#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include "server.h"

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

    if (ssh_bind_listen(sshbind) != SSH_OK) {
        fprintf(stderr, "[ERROR] Échec écoute : %s\n", ssh_get_error(sshbind));
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

        printf("[DEBUG] Connexion acceptée\n");

        handle_client(session, username, password);

        ssh_disconnect(session);
        ssh_free(session);
    }

    ssh_bind_free(sshbind);
    return EXIT_SUCCESS;
}