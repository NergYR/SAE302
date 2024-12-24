#include <libssh/server.h>
#include <libssh/libssh.h>
#define PORT "5959"
#define USERNAME "root"
#define PASSWORD "password"
#define BUFFER_SIZE 1024
#define HOST_KEY_PATH "ssh_host_rsa_key"

// Function to get configuration from file
void getServerPort(const char *filename, int *port, char* serverName, char* isSecured);


// Unsecured Server functions
void doprocessing (int sock);
int unSecured_server(int port);


// SSH Server functions
void handle_session_input(ssh_channel channel);
void send_message(ssh_channel channel);
void handle_channel_requests(ssh_session session, ssh_channel channel);
void handle_client(ssh_session session);
