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

    printf("[DEBUG] Début de la gestion du client\n");

    // Authentification simple avec nom d'utilisateur et mot de passe
    while ((message = ssh_message_get(session)) != NULL) {
        printf("[DEBUG] Reçu un message d'authentification\n");
        if (ssh_message_type(message) == SSH_REQUEST_AUTH &&
            ssh_message_subtype(message) == SSH_AUTH_METHOD_PASSWORD) {
            const char* user = ssh_message_auth_user(message);
            const char* pass = ssh_message_auth_password(message);
            printf("[DEBUG] Tentative d'authentification - Utilisateur: %s\n", user);
            if (strcmp(user, "user") == 0 && strcmp(pass, "password") == 0) {
                ssh_message_auth_reply_success(message, 0);
                auth = 1;
                printf("[DEBUG] Authentification réussie\n");
                break;
            } else {
                printf("[DEBUG] Authentification échouée\n");
                ssh_message_reply_default(message);
            }
        }
        ssh_message_free(message);
    }

    if (!auth) {
        fprintf(stderr, "Authentification échouée\n");
        return;
    }

    // Création et ouverture d'un canal
    printf("[DEBUG] Création d'un canal\n");
    ssh_channel channel = ssh_channel_new(session);
    if (ssh_channel_open_session(channel) != SSH_OK) {
        fprintf(stderr, "Erreur d'ouverture de canal : %s\n", ssh_get_error(session));
        ssh_channel_free(channel);
        return;
    }

    // Envoi d'un message au client
    printf("[DEBUG] Envoi du message au client\n");
    if (ssh_channel_write(channel, MESSAGE, strlen(MESSAGE)) == SSH_ERROR) {
        fprintf(stderr, "Erreur d'envoi des données : %s\n", ssh_get_error(session));
    }

    // Fermeture du canal
    printf("[DEBUG] Fermeture du canal\n");
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
}

int main() {
    ssh_bind sshbind;
    ssh_session session;
    int rc;

    printf("[DEBUG] Initialisation du serveur SSH\n");

    // Initialisation du serveur SSH
    sshbind = ssh_bind_new();
    if (sshbind == NULL) {
        fprintf(stderr, "Erreur lors de l'initialisation de ssh_bind\n");
        exit(1);
    }

    // Configuration des options du serveur
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, "127.0.0.1");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, PORT);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, "ssh_keys");

    // Lancement de l'écoute du serveur SSH
    printf("[DEBUG] Lancement de l'écoute sur le port %s\n", PORT);
    if (ssh_bind_listen(sshbind) < 0) {
        fprintf(stderr, "Erreur d'écoute : %s\n", ssh_get_error(sshbind));
        ssh_bind_free(sshbind);
        exit(1);
    }
    printf("Serveur SSH en écoute sur le port %s\n", PORT);

    while (1) {
        // Acceptation d'une nouvelle connexion
        printf("[DEBUG] Attente d'une nouvelle connexion\n");
        session = ssh_new();
        if (session == NULL) {
            fprintf(stderr, "Erreur lors de la création de la session SSH\n");
            continue;
        }

        rc = ssh_bind_accept(sshbind, session);
        if (rc != SSH_OK) {
            fprintf(stderr, "Erreur d'acceptation de connexion : %s\n", ssh_get_error(sshbind));
            ssh_free(session);
            continue;
        }

        printf("[DEBUG] Nouvelle connexion acceptée\n");

        // Gérer la session client
        handle_client(session);

        // Libération des ressources de la session
        printf("[DEBUG] Fermeture de la session client\n");
        ssh_disconnect(session);
        ssh_free(session);
    }

    // Libération des ressources du serveur
    printf("[DEBUG] Fermeture du serveur SSH\n");
    ssh_bind_free(sshbind);
    ssh_finalize();
    return 0;
}
