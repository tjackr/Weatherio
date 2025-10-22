#include "tcp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

int client_init(ClientConfig* config, const char* server_ip, int port) {
    config->port = port;
    config->socket_fd = -1;
    strncpy(config->server_ip, server_ip, 15);
    config->server_ip[15] = '\0';
    
    // Create socket
    config->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (config->socket_fd < 0) {
        perror("ERROR: Socket creation failed");
        return FT_ERROR_SOCKET;
    }
    
    printf("[CLIENT] Socket created\n");
    return FT_SUCCESS;
}

int client_connect(ClientConfig* config) {
    struct sockaddr_in server_addr;
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->port);
    
    if (inet_pton(AF_INET, config->server_ip, &server_addr.sin_addr) <= 0) {
        perror("ERROR: Invalid address");
        return FT_ERROR_CONNECT;
    }
    
    printf("[CLIENT] Connecting to %s:%d...\n", config->server_ip, config->port);
    
    if (connect(config->socket_fd, (struct sockaddr*)&server_addr, 
                sizeof(server_addr)) < 0) {
        perror("ERROR: Connection failed (is server running?)");
        return FT_ERROR_CONNECT;
    }
    
    printf("[CLIENT] Connected successfully\n");
    return FT_SUCCESS;
}

int client_send_message(ClientConfig* config, const char* message) {
    printf("[CLIENT] Sending message: '%s'\n", message);
    
    if (send_all(config->socket_fd, message, strlen(message)) < 0) {
        perror("ERROR: Failed to send message");
        return FT_ERROR_SEND;
    }
    
    // Receive acknowledgment
    char ack[256];
    ssize_t n = read(config->socket_fd, ack, sizeof(ack) - 1);
    if (n > 0) {
        ack[n] = '\0';
        printf("[CLIENT] Server response: '%s'\n", ack);
    }
    
    return FT_SUCCESS;
}

int client_request_file(ClientConfig* config, const char* filename) {
    printf("[CLIENT] Requesting file: '%s'\n", filename);
    
    // Send filename with newline (server expects it)
    char request[FILENAME_SIZE];
    snprintf(request, FILENAME_SIZE, "%s\n", filename);
    
    if (send_all(config->socket_fd, request, strlen(request)) < 0) {
        perror("ERROR: Failed to send filename");
        return FT_ERROR_SEND;
    }
    
    return FT_SUCCESS;
}

int client_receive_file(ClientConfig* config, const char* local_filename) {
    // Receive file size
    long file_size;
    if (recv_all(config->socket_fd, &file_size, sizeof(file_size)) <= 0) {
        perror("ERROR: Failed to receive file size");
        return FT_ERROR_RECV;
    }
    
    // Check for error from server
    if (file_size < 0) {
        printf("[CLIENT] ERROR: Server could not find file\n");
        return FT_ERROR_FILE_NOT_FOUND;
    }
    
    printf("[CLIENT] File size: %ld bytes", file_size);
    if (file_size < 1024) {
        printf("\n");
    } else if (file_size < 1048576) {
        printf(" (%.2f KB)\n", file_size / 1024.0);
    } else {
        printf(" (%.2f MB)\n", file_size / 1048576.0);
    }
    
    // Open local file for writing
    int file_fd = open(local_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0) {
        perror("ERROR: Cannot create local file");
        return FT_ERROR_FILE_OPEN;
    }
    
    printf("[CLIENT] Receiving file...\n");
    
    // Receive file content
    char buffer[BUFFER_SIZE];
    long total_received = 0;
    
    while (total_received < file_size) {
        size_t to_read = BUFFER_SIZE;
        if (file_size - total_received < BUFFER_SIZE) {
            to_read = file_size - total_received;
        }
        
        ssize_t bytes_received = read(config->socket_fd, buffer, to_read);
        
        if (bytes_received < 0) {
            perror("ERROR: Failed to receive data");
            close(file_fd);
            return FT_ERROR_RECV;
        }
        
        if (bytes_received == 0) {
            printf("[CLIENT] ERROR: Connection closed prematurely\n");
            close(file_fd);
            return FT_ERROR_RECV;
        }
        
        if (write(file_fd, buffer, bytes_received) != bytes_received) {
            perror("ERROR: Failed to write to file");
            close(file_fd);
            return FT_ERROR_FILE_WRITE;
        }
        
        total_received += bytes_received;
        
        int percent = (int)((total_received * 100) / file_size);
        printf("\r[CLIENT] Progress: %d%% (%ld/%ld bytes)", 
               percent, total_received, file_size);
        fflush(stdout);
    }
    
    printf("\n");
    close(file_fd);
    
    if (total_received == file_size) {
        printf("[CLIENT] SUCCESS: File saved as '%s'\n", local_filename);
        return FT_SUCCESS;
    } else {
        printf("[CLIENT] WARNING: Partial transfer\n");
        return FT_ERROR_RECV;
    }
}

void client_cleanup(ClientConfig* config) {
    if (config->socket_fd >= 0) {
        close(config->socket_fd);
        config->socket_fd = -1;
    }
    printf("[CLIENT] Cleanup complete\n");
}

int client_run(const char* server_ip, int port, 
               const char* remote_file, const char* local_file) {
    ClientConfig config;
    int result;
    
    result = client_init(&config, server_ip, port);
    if (result != FT_SUCCESS) {
        return result;
    }
    
    result = client_connect(&config);
    if (result != FT_SUCCESS) {
        client_cleanup(&config);
        return result;
    }
    
    // Send message first
    result = client_send_message(&config, "Hello Server! I want to download a file.");
    if (result != FT_SUCCESS) {
        client_cleanup(&config);
        return result;
    }
    
    result = client_request_file(&config, remote_file);
    if (result != FT_SUCCESS) {
        client_cleanup(&config);
        return result;
    }
    
    result = client_receive_file(&config, local_file);
    
    client_cleanup(&config);
    
    return result;
}