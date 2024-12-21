#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "5959"
#define USERNAME "root"
#define PASSWORD "password"
#define MESSAGE "Bienvenue sur le serveur SSH personnalisé\n"

// Fonction pour envoyer un message sur un canal SSH
void send_ssh_message(ssh_channel channel) {
    if (channel == NULL) {
        fprintf(stderr, "[ERROR] Le canal SSH est invalide\n");
        return;
    }

    printf("[DEBUG] Envoi du message sur le canal\n");
    ssh_channel_write(channel, MESSAGE, strlen(MESSAGE));

    // Fermeture du canal
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
}

// Fonction pour gérer une session client
void handle_client(ssh_session session) {
    ssh_channel channel = NULL;
    int auth = 0;

    printf("[DEBUG] Début de la gestion du client\n");

    if (ssh_handle_key_exchange(session) != SSH_OK) {
        fprintf(stderr, "[ERROR] Échange de clés échoué : %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        return;
    }

    // Authentification utilisateur
    while (1) {
        ssh_message message = ssh_message_get(session);
        if (message == NULL) break;

        if (ssh_message_type(message) == SSH_REQUEST_AUTH &&
            ssh_message_subtype(message) == SSH_AUTH_METHOD_PASSWORD) {
            const char *user = ssh_message_auth_user(message);
            const char *pass = ssh_message_auth_password(message);
            printf("[DEBUG] Tentative d'authentification - Utilisateur: %s\n", user);

            if (strcmp(user, USERNAME) == 0 && strcmp(pass, PASSWORD) == 0) {
                printf("[DEBUG] Authentification réussie\n");
                auth = 1;
                ssh_message_auth_reply_success(message, 0);
            } else {
                printf("[DEBUG] Authentification échouée\n");
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

    // Ouverture du canal principal
    channel = ssh_channel_new(session);
    if (channel == NULL) {
        fprintf(stderr, "[ERROR] Impossible de créer un canal SSH\n");
        ssh_disconnect(session);
        return;
    }

    if (ssh_channel_open_session(channel) != SSH_OK) {
        fprintf(stderr, "[ERROR] Impossible d'ouvrir une session de canal : %s\n", ssh_get_error(session));
        ssh_channel_free(channel);
        ssh_disconnect(session);
        return;
    }

    // Envoi d'un message au client
    send_ssh_message(channel);

    printf("[DEBUG] Fin de la gestion du client\n");
    ssh_disconnect(session);
}

int main() {
    ssh_bind sshbind;
    ssh_session session;
    int rc;

    printf("[DEBUG] Initialisation du serveur SSH\n");

    sshbind = ssh_bind_new();
    if (sshbind == NULL) {
        fprintf(stderr, "[ERROR] Impossible d'initialiser ssh_bind\n");
        return EXIT_FAILURE;
    }

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, "0.0.0.0");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR, PORT);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, "ssh-rsa");

    rc = ssh_bind_listen(sshbind);
    if (rc != SSH_OK) {
        fprintf(stderr, "[ERROR] Échec de l'écoute : %s\n", ssh_get_error(sshbind));
        ssh_bind_free(sshbind);
        return EXIT_FAILURE;
    }

    printf("[DEBUG] Serveur SSH en écoute sur le port %s\n", PORT);

    while (1) {
        session = ssh_new();
        if (session == NULL) {
            fprintf(stderr, "[ERROR] Impossible de créer une session SSH\n");
            continue;
        }

        rc = ssh_bind_accept(sshbind, session);
        if (rc != SSH_OK) {
            fprintf(stderr, "[ERROR] Échec de l'acceptation de la connexion : %s\n", ssh_get_error(sshbind));
            ssh_free(session);
            continue;
        }

        printf("[DEBUG] Connexion acceptée\n");

        handle_client(session);
        ssh_free(session);
    }

    ssh_bind_free(sshbind);
    return EXIT_SUCCESS;
}
