#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "tcp_file_transfer.h"
#include <signal.h>

// Global flag for graceful shutdown (defined in main.c)
extern volatile sig_atomic_t keep_running;

// Server configuration structure
typedef struct {
    int port;
    int socket_fd;
    int client_fd;
} ServerConfig;

// Initialize server
// Returns: FT_SUCCESS on success, error code otherwise
int server_init(ServerConfig* config, int port);

// Wait for client connection
// Returns: FT_SUCCESS on success, error code otherwise
int server_accept_client(ServerConfig* config);

// Receive message from client
// Returns: FT_SUCCESS on success, error code otherwise
int server_receive_message(ServerConfig* config);

// Receive filename from client
// Returns: FT_SUCCESS on success, error code otherwise
int server_receive_filename(ServerConfig*  config, char* filename, size_t max_len);

// Send file to client
// Returns: FT_SUCCESS on success, error code otherwise
int server_send_file(ServerConfig* config, const char* filename);

// Cleanup and close server
void server_cleanup(ServerConfig* config);

// Run complete server workflow (convenience function)
int server_run(int port);

#endif