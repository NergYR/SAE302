#ifndef SERVER_H
#define SERVER_H

#include <libssh/server.h>
#include <libssh/libssh.h>

// Définir BUFFER_SIZE s'il n'est pas déjà défini
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

// Function to get configuration from file
void getConf(const char *filename, int *port, char* serverName, char* isSecured, char* hostKeyPath, char* username, char* password);


// Unsecured Server functions
void doprocessing (int sock);
int unSecured_server(int port);


// SSH Server functions
int secured_server(int port, char* hostKeyPath, char* username, char* password);
void handle_session_input(ssh_channel channel);
void send_message(ssh_channel channel);
void handle_channel_requests(ssh_session session, ssh_channel channel);
void handle_client(ssh_session session, char* username, char* password);  // Ajout de la déclaration

// Engines functions

int process(char* buffer);
char* sendfile(const char* filename);

extern char validFichesGlobal[256]; // Liste des fichiers autorisés
extern char fichesDirGlobal[256]; // Nouvelle variable globale pour stocker le chemin des fiches

#endif