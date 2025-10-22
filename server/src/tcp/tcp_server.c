#include "tcp_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <limits.h>

#define JSON_DIR "data/cache"  // base directory for JSON

// Check if file has .json extension
int is_json_file(const char *filename) {
    size_t len = strlen(filename);
    
    // Check of minimal lenght (x.json = 6 symbols)
    if (len < 6) {
        return 0;
    }
    
    // Check file extension
    if (strcmp(filename + len - 5, ".json") == 0) {
        return 1;
    }
    
    return 0;
}

// Check if requested file is safe (no path traversal)
int is_safe_path(const char *filename) {
    // Reject empty filename
    if (filename == NULL || filename[0] == '\0') {
        printf("[SERVER] SECURITY: Empty filename\n");
        return 0;
    }
    
    // Reject absolute paths
    if (filename[0] == '/') {
        printf("[SERVER] SECURITY: Rejected absolute path: %s\n", filename);
        return 0;
    }
    
    // Reject path traversal attempts
    if (strstr(filename, "..") != NULL) {
        printf("[SERVER] SECURITY: Rejected path traversal: %s\n", filename);
        return 0;
    }
    
    // Reject paths with multiple slashes
    if (strstr(filename, "//") != NULL) {
        printf("[SERVER] SECURITY: Rejected invalid path: %s\n", filename);
        return 0;
    }
    
    // Check if it's a JSON file
    if (!is_json_file(filename)) {
        printf("[SERVER] SECURITY: Only .json files allowed: %s\n", filename);
        return 0;
    }
    
    return 1;  // Safe
}

int server_init(ServerConfig *config, int port) {
    config->port = port;
    config->client_fd = -1;
    
    // Create socket
    config->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (config->socket_fd < 0) {
        perror("ERROR: Socket creation failed");
        return FT_ERROR_SOCKET;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(config->socket_fd, SOL_SOCKET, SO_REUSEADDR, 
                   &opt, sizeof(opt)) < 0) {
        perror("WARNING: setsockopt failed");
    }
    
    // Bind
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(config->socket_fd, (struct sockaddr*)&server_addr, 
             sizeof(server_addr)) < 0) {
        perror("ERROR: Bind failed");
        close(config->socket_fd);
        return FT_ERROR_BIND;
    }
    
    // Listen
    if (listen(config->socket_fd, 1) < 0) {
        perror("ERROR: Listen failed");
        close(config->socket_fd);
        return FT_ERROR_LISTEN;
    }
    
    printf("[SERVER] Initialized on port %d\n", port);
    return FT_SUCCESS;
}

int server_accept_client(ServerConfig *config) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    printf("[SERVER] Waiting for client...\n");
    
    config->client_fd = accept(config->socket_fd, 
                               (struct sockaddr*)&client_addr, 
                               &client_len);
    
    if (config->client_fd < 0) {
        perror("ERROR: Accept failed");
        return FT_ERROR_ACCEPT;
    }
    
    printf("[SERVER] Client connected: %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));
    
    return FT_SUCCESS;
}

int server_receive_message(ServerConfig *config) {
    char message[1024];
    memset(message, 0, sizeof(message));
    
    ssize_t n = read(config->client_fd, message, sizeof(message) - 1);
    if (n <= 0) {
        perror("ERROR: Failed to receive message");
        return FT_ERROR_RECV;
    }
    
    printf("[SERVER] Client message: '%s'\n", message);
    
    // Send acknowledgment
    const char *ack = "Message received!";
    if (send_all(config->client_fd, ack, strlen(ack)) < 0) {
        perror("ERROR: Failed to send acknowledgment");
        return FT_ERROR_SEND;
    }
    
    return FT_SUCCESS;
}

int server_receive_filename(ServerConfig *config, char *filename, size_t max_len) {
    memset(filename, 0, max_len);
    
    ssize_t n = read(config->client_fd, filename, max_len - 1);
    if (n <= 0) {
        perror("ERROR: Failed to receive filename");
        return FT_ERROR_RECV;
    }
    
    filename[strcspn(filename, "\n")] = 0;
    printf("[SERVER] Client requested: '%s'\n", filename);
    
    return FT_SUCCESS;
}

int server_send_file(ServerConfig *config, const char *filename) {
    // Security check: validate filename
    if (!is_safe_path(filename)) {
        printf("[SERVER] ERROR: Unsafe file path rejected\n");
        long error_size = -1;
        send_all(config->client_fd, &error_size, sizeof(error_size));
        return FT_ERROR_FILE_NOT_FOUND;
    }
    
    // Build full path in JSON directory
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", JSON_DIR, filename);
    
    // Check file
    long file_size = get_file_size(filepath);
    
    if (file_size < 0) {
        printf("[SERVER] ERROR: File not found\n");
        long error_size = -1;
        send_all(config->client_fd, &error_size, sizeof(error_size));
        return FT_ERROR_FILE_NOT_FOUND;
    }
    
    printf("[SERVER] File path: %s\n", filepath);
    printf("[SERVER] File size: %ld bytes", file_size);
    if (file_size < 1024) {
        printf("\n");
    } else if (file_size < 1048576) {
        printf(" (%.2f KB)\n", file_size / 1024.0);
    } else {
        printf(" (%.2f MB)\n", file_size / 1048576.0);
    }
    
    // Send file size
    if (send_all(config->client_fd, &file_size, sizeof(file_size)) < 0) {
        perror("ERROR: Failed to send file size");
        return FT_ERROR_SEND;
    }
    
    // Open file
    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        perror("ERROR: Cannot open file");
        return FT_ERROR_FILE_OPEN;
    }
    
    // Send file content
    printf("[SERVER] Sending file...\n");
    
    char buffer[BUFFER_SIZE];
    long total_sent = 0;
    ssize_t bytes_read;
    
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        if (send_all(config->client_fd, buffer, bytes_read) < 0) {
            perror("ERROR: Failed to send file data");
            close(file_fd);
            return FT_ERROR_SEND;
        }
        
        total_sent += bytes_read;
        
        int percent = (int)((total_sent * 100) / file_size);
        printf("\r[SERVER] Progress: %d%% (%ld/%ld bytes)", 
               percent, total_sent, file_size);
        fflush(stdout);
    }
    
    printf("\n");
    close(file_fd);
    
    if (total_sent == file_size) {
        printf("[SERVER] SUCCESS: File sent (%ld bytes)\n", total_sent);
        return FT_SUCCESS;
    } else {
        printf("[SERVER] WARNING: Partial transfer\n");
        return FT_ERROR_SEND;
    }
}

void server_cleanup(ServerConfig *config) {
    if (config->client_fd >= 0) {
        close(config->client_fd);
        config->client_fd = -1;
    }
    if (config->socket_fd >= 0) {
        close(config->socket_fd);
        config->socket_fd = -1;
    }
    printf("[SERVER] Cleanup complete\n");
}

// External flag for graceful shutdown
extern volatile sig_atomic_t keep_running;

int server_run(int port) {
    ServerConfig config;
    char filename[FILENAME_SIZE];
    int result;
    
    result = server_init(&config, port);
    if (result != FT_SUCCESS) {
        return result;
    }
    
    // Server loop - handle multiple clients
    while (keep_running) {
        result = server_accept_client(&config);
        if (result != FT_SUCCESS) {
            continue;
        }
        
        // Receive message first
        result = server_receive_message(&config);
        if (result != FT_SUCCESS) {
            if (config.client_fd >= 0) {
                close(config.client_fd);
                config.client_fd = -1;
            }
            continue;
        }
        
        // Try to receive filename (might fail if client only sent message)
        result = server_receive_filename(&config, filename, FILENAME_SIZE);
        if (result != FT_SUCCESS) {
            printf("[SERVER] Client closed connection (message only)\n");
            if (config.client_fd >= 0) {
                close(config.client_fd);
                config.client_fd = -1;
            }
            continue;
        }
        
        // Send file
        result = server_send_file(&config, filename);
        
        // Close client connection
        if (config.client_fd >= 0) {
            close(config.client_fd);
            config.client_fd = -1;
        }
        
        printf("[SERVER] Ready for next client...\n\n");
    }
    
    server_cleanup(&config);
    return FT_SUCCESS;
}