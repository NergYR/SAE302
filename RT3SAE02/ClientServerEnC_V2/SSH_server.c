#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "5959" // Port personnalisé pour le serveur SSH
#define MESSAGE "Bienvenue sur le serveur SSH personnalisé\n"

// Fonction pour gérer une session client
void handle_client(ssh_session session) {
    ssh_message message;
    int auth = 0;

    if (session == NULL) {
        fprintf(stderr, "[ERROR] Session SSH invalide\n");
        return;
    }

    printf("[DEBUG] Début de la gestion du client\n");

    if(ssh_handle_key_exchange(session) != SSH_OK) {
        fprintf(stderr, "[ERROR] Erreur lors de l'échange de clés: %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        return;
    }

    // Authentification simple avec nom d'utilisateur et mot de passe
    while ((message = ssh_message_get(session)) != NULL) {
        printf("[DEBUG] Reçu un message d'authentification\n");
        if (ssh_message_type(message) == SSH_REQUEST_AUTH &&
            ssh_message_subtype(message) == SSH_AUTH_METHOD_PASSWORD) {
            const char* user = ssh_message_auth_user(message);
            const char* pass = ssh_message_auth_password(message);
            printf("[DEBUG] Tentative d'authentification - Utilisateur: %s\n", user);

            // Vérification des informations d'authentification
            if (strcmp(user, "user1") == 0 && strcmp(pass, "password") == 0) {
                printf("[DEBUG] Authentification réussie\n");
                auth = 1;
                ssh_message_auth_reply_success(message, 0);
            } else {
                printf("[DEBUG] Authentification échouée\n");
                ssh_message_auth_set_methods(message, SSH_AUTH_METHOD_PASSWORD);
                ssh_message_reply_default(message);
            }
        } else {
            ssh_message_reply_default(message);
        }
        ssh_message_free(message);
    }

    if (!auth) {
        fprintf(stderr, "[ERROR] Échec de l'authentification\n");
        ssh_disconnect(session);
    }

    printf("[DEBUG] Fin de la gestion du client\n");
}

int main() {
    ssh_bind sshbind;
    ssh_session session;
    int rc;

    sshbind = ssh_bind_new();
    if (sshbind == NULL) {
        fprintf(stderr, "[ERROR] Erreur lors de la création de ssh_bind\n");
        return EXIT_FAILURE;
    }

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, "0.0.0.0");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR, PORT);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, "ssh-rsa");

    rc = ssh_bind_listen(sshbind);
    if (rc < 0) {
        fprintf(stderr, "[ERROR] Erreur lors de l'écoute sur le port %s: %s\n", PORT, ssh_get_error(sshbind));
        ssh_bind_free(sshbind);
        return EXIT_FAILURE;
    }

    printf("[DEBUG] Serveur SSH en écoute sur le port %s\n", PORT);

    while (1) {
        session = ssh_new();
        if (session == NULL) {
            fprintf(stderr, "[ERROR] Erreur lors de la création de la session SSH\n");
            continue;
        }

        rc = ssh_bind_accept(sshbind, session);
        if (rc == SSH_ERROR) {
            fprintf(stderr, "[ERROR] Erreur lors de l'acceptation de la connexion: %s\n", ssh_get_error(sshbind));
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