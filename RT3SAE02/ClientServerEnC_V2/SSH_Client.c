#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"  // Adresse IP du serveur
#define SERVER_PORT 5959       // Port personnalisé
#define USERNAME "user"        // Nom d'utilisateur
#define PASSWORD "password"    // Mot de passe

int main() {
    ssh_session session;
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;

    // Initialisation de la session SSH
    session = ssh_new();
    if (session == NULL) {
        fprintf(stderr, "Erreur : Impossible de créer une session SSH\n");
        return -1;
    }
    int port;
    port = SERVER_PORT;

    // Configuration de la session
    ssh_options_set(session, SSH_OPTIONS_HOST, SERVER_IP);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, USERNAME);

    // Connexion au serveur
    rc = ssh_connect(session);
    if (rc != SSH_OK) {
        fprintf(stderr, "Erreur de connexion : %s\n", ssh_get_error(session));
        ssh_free(session);
        return -1;
    }
    printf("Connecté au serveur SSH %s:%d\n", SERVER_IP, SERVER_PORT);

    // Authentification par mot de passe
    rc = ssh_userauth_password(session, NULL, PASSWORD);
    if (rc != SSH_AUTH_SUCCESS) {
        fprintf(stderr, "Erreur d'authentification : %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        ssh_free(session);
        return -1;
    }
    printf("Authentification réussie\n");

    // Création d'un canal
    channel = ssh_channel_new(session);
    if (channel == NULL) {
        fprintf(stderr, "Erreur : Impossible de créer un canal SSH\n");
        ssh_disconnect(session);
        ssh_free(session);
        return -1;
    }

    // Ouverture du canal
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        fprintf(stderr, "Erreur d'ouverture de canal : %s\n", ssh_get_error(session));
        ssh_channel_free(channel);
        ssh_disconnect(session);
        ssh_free(session);
        return -1;
    }

    // Lecture des données envoyées par le serveur
    printf("Lecture des données envoyées par le serveur...\n");
    while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[nbytes] = '\0';  // Null-terminate the string
        printf("%s", buffer);
    }

    if (nbytes < 0) {
        fprintf(stderr, "Erreur lors de la lecture : %s\n", ssh_get_error(session));
    }

    // Fermeture et nettoyage
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);

    printf("\nDéconnexion du serveur SSH\n");
    return 0;
}
