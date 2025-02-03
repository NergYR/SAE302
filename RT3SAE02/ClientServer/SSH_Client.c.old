#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"  // Adresse IP du serveur
#define SERVER_PORT 5959       // Port personnalisé
#define USERNAME "root"        // Nom d'utilisateur
#define PASSWORD "password"    // Mot de passe

int main() {
    ssh_session session;
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;
    const char *message = "\nid;name;age\n1;joe;29\n";

    // Initialisation de la session SSH
    session = ssh_new();
    if (session == NULL) {
        fprintf(stderr, "Erreur lors de la création de la session SSH\n");
        return EXIT_FAILURE;
    }

    // Configuration de la session SSH
    int port = SERVER_PORT;
    ssh_options_set(session, SSH_OPTIONS_HOST, SERVER_IP);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);

    // Connexion au serveur SSH
    rc = ssh_connect(session);
    if (rc != SSH_OK) {
        fprintf(stderr, "Erreur de connexion : %s\n", ssh_get_error(session));
        ssh_free(session);
        return EXIT_FAILURE;
    }

    // Authentification
    rc = ssh_userauth_password(session, USERNAME, PASSWORD);
    if (rc != SSH_AUTH_SUCCESS) {
        fprintf(stderr, "Erreur d'authentification : %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        ssh_free(session);
        return EXIT_FAILURE;
    }

    // Ouverture du canal
    channel = ssh_channel_new(session);
    if (channel == NULL) {
        fprintf(stderr, "Erreur lors de la création du canal\n");
        ssh_disconnect(session);
        ssh_free(session);
        return EXIT_FAILURE;
    }

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        fprintf(stderr, "Erreur lors de l'ouverture du canal : %s\n", ssh_get_error(session));
        ssh_channel_free(channel);
        ssh_disconnect(session);
        ssh_free(session);
        return EXIT_FAILURE;
    }

    // Envoi de la chaîne de caractères
    rc = ssh_channel_write(channel, message, strlen(message));
    if (rc < 0) {
        fprintf(stderr, "Erreur lors de l'envoi du message : %s\n", ssh_get_error(session));
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        ssh_disconnect(session);
        ssh_free(session);
        return EXIT_FAILURE;
    }

    // Lecture de la réponse
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    if (nbytes < 0) {
        fprintf(stderr, "Erreur lors de la lecture de la réponse : %s\n", ssh_get_error(session));
    } else {
        printf("Réponse du serveur : %.*s\n", nbytes, buffer);
    }

    // Fermeture du canal et de la session
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);

    return EXIT_SUCCESS;
}