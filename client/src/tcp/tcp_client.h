#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "tcp_file_transfer.h"

// Client configuration structure
typedef struct {
    int socket_fd;
    char server_ip[16];
    int port;
} ClientConfig;

// Initialize client
// Returns: FT_SUCCESS on success, error code otherwise
int client_init(ClientConfig* config, const char* server_ip, int port);

// Connect to server
// Returns: FT_SUCCESS on success, error code otherwise
int client_connect(ClientConfig* config);

// Send message to server
// Returns: FT_SUCCESS on success, error code otherwise
int client_send_message(ClientConfig* config, const char* message);

// Send filename request to server
// Returns: FT_SUCCESS on success, error code otherwise
int client_request_file(ClientConfig* config, const char* filename);

// Receive file from server
// Returns: FT_SUCCESS on success, error code otherwise
int client_receive_file(ClientConfig* config, const char* local_filename);

// Cleanup and close client
void client_cleanup(ClientConfig* config);

// Run complete client workflow (convenience function)
int client_run(const char* server_ip, int port, 
               const char* remote_file, const char* local_file);

#endif