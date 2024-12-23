#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

#define PORT "5959"
#define USERNAME "root"
#define PASSWORD "password"
#define BUFFER_SIZE 1024
#define HOST_KEY_PATH "ssh_host_rsa_key"

void handle_session_input(ssh_channel channel) {
    char buffer[BUFFER_SIZE];
    int nbytes;

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    if (nbytes > 0) {
        printf("[DEBUG] Reçu: %.*s\n", nbytes, buffer);
        ssh_channel_write(channel, buffer, nbytes); // Echo
    }
}

void send_message(ssh_channel channel) {
    const char *message = "Bienvenue sur le canal SSH !\nEntrez vos commandes:\n";
    if (ssh_channel_write(channel, message, strlen(message)) < 0) {
        fprintf(stderr, "[ERROR] Échec envoi message\n");
    }
}

void handle_channel_requests(ssh_session session, ssh_channel channel) {
    struct pollfd fds[1];
    fds[0].fd = ssh_get_fd(session);
    fds[0].events = POLLIN;

    send_message(channel);

    while (!ssh_channel_is_eof(channel)) {
        if (poll(fds, 1, 1000) > 0) {
            if (fds[0].revents & POLLIN) {
                handle_session_input(channel);
            }
        }
    }
}

void handle_client(ssh_session session) {
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
            if (strcmp(ssh_message_auth_user(message), USERNAME) == 0 &&
                strcmp(ssh_message_auth_password(message), PASSWORD) == 0) {
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
