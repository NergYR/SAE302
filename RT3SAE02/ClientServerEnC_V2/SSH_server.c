#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include "server.h"



int main() {
    ssh_bind sshbind;
    ssh_session session;
    int rc;

    sshbind = ssh_bind_new();
    if (sshbind == NULL) {
        fprintf(stderr, "[ERROR] Échec initialisation ssh_bind\n");
        return EXIT_FAILURE;
    }

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, "0.0.0.0");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR, PORT);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, HOST_KEY_PATH);

    if (ssh_bind_listen(sshbind) != SSH_OK) {
        fprintf(stderr, "[ERROR] Échec écoute : %s\n", ssh_get_error(sshbind));
        ssh_bind_free(sshbind);
        return EXIT_FAILURE;
    }

    printf("[DEBUG] Serveur SSH démarré sur le port %s\n", PORT);

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
        handle_client(session);
        ssh_disconnect(session);
        ssh_free(session);
    }

    ssh_bind_free(sshbind);
    return EXIT_SUCCESS;
}
