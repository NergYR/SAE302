#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "5959"
#define USERNAME "root"
#define PASSWORD "password"
#define WELCOME_MSG "Bienvenue sur le serveur SSH personnalisé.\n"

// Fonction pour envoyer un message via un canal SSH
void send_message(ssh_channel channel) {
    if (!channel) {
        fprintf(stderr, "[ERROR] Canal SSH invalide\n");
        return;
    }

    ssh_channel_write(channel, WELCOME_MSG, strlen(WELCOME_MSG));
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
}

// Fonction pour gérer un client
void handle_client(ssh_session session) {
    int auth = 0;
    ssh_channel channel = NULL;

    if (ssh_handle_key_exchange(session) != SSH_OK) {
        fprintf(stderr, "[ERROR] Échec de l'échange de clés : %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        return;
    }

    while (!auth) {
        ssh_message message = ssh_message_get(session);
        if (!message) break;

        if (ssh_message_type(message) == SSH_REQUEST_AUTH &&
            ssh_message_subtype(message) == SSH_AUTH_METHOD_PASSWORD) {
            const char *user = ssh_message_auth_user(message);
            const char *pass = ssh_message_auth_password(message);
            if (strcmp(user, USERNAME) == 0 && strcmp(pass, PASSWORD) == 0) {
                ssh_message_auth_reply_success(message, 0);
                auth = 1;
            } else {
                ssh_message_reply_default(message);
            }
        } else {
            ssh_message_reply_default(message);
        }
        ssh_message_free(message);
    }

    if (!auth) {
        fprintf(stderr, "[ERROR] Authentification échouée\n");
        ssh_disconnect(session);
        return;
    }

    while ((channel = ssh_channel_new(session)) != NULL) {
        if (ssh_channel_open_session(channel) == SSH_OK) {
            send_message(channel);
            break;
        } else {
            fprintf(stderr, "[ERROR] Échec de l'ouverture du canal : %s\n", ssh_get_error(session));
            ssh_channel_free(channel);
            break;
        }
    }

    ssh_disconnect(session);
}

int main() {
    ssh_bind sshbind;
    ssh_session session;
    int rc;

    sshbind = ssh_bind_new();
    if (!sshbind) {
        fprintf(stderr, "[ERROR] Impossible de créer ssh_bind\n");
        return EXIT_FAILURE;
    }

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, "0.0.0.0");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR, PORT);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, "ssh-rsa");

    if (ssh_bind_listen(sshbind) != SSH_OK) {
        fprintf(stderr, "[ERROR] Échec de l'écoute : %s\n", ssh_get_error(sshbind));
        ssh_bind_free(sshbind);
        return EXIT_FAILURE;
    }

    printf("[INFO] Serveur SSH en écoute sur le port %s\n", PORT);

    while (1) {
        session = ssh_new();
        if (!session) continue;

        rc = ssh_bind_accept(sshbind, session);
        if (rc == SSH_OK) {
            printf("[INFO] Client connecté\n");
            handle_client(session);
        } else {
            fprintf(stderr, "[ERROR] Échec de la connexion client : %s\n", ssh_get_error(sshbind));
        }

        ssh_free(session);
    }

    ssh_bind_free(sshbind);
    return EXIT_SUCCESS;
}
