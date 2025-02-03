#ifndef FILE_PATH
#define FILE_PATH "/home/kali/CODE/SAE302/RT3SAE02/fiches/"
#endif

#ifndef SSH_TUNNEL_H
#define SSH_TUNNEL_H

#include <libssh/libssh.h>
#include <libssh/server.h>

// Déclarations des fonctions du tunnel SSH
void handle_client(ssh_session session, char* username, char* password);
void handle_session_input(ssh_channel channel);
void send_message(ssh_channel channel);
void handle_channel_requests(ssh_session session, ssh_channel channel);
void send_file_content(ssh_channel channel, const char* filename);
int is_valid_filename(const char* filename);

#endif
