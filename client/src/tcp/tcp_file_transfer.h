#ifndef TCP_FILE_TRANSFER_H
#define TCP_FILE_TRANSFER_H

#include <stddef.h>
#include <sys/types.h>

// Configuration
#define PORT 8080
#define BUFFER_SIZE 4096
#define FILENAME_SIZE 256
#define SERVER_IP "127.0.0.1"

// Error codes
#define FT_SUCCESS 0
#define FT_ERROR_SOCKET -1
#define FT_ERROR_BIND -2
#define FT_ERROR_LISTEN -3
#define FT_ERROR_ACCEPT -4
#define FT_ERROR_CONNECT -5
#define FT_ERROR_FILE_NOT_FOUND -6
#define FT_ERROR_FILE_OPEN -7
#define FT_ERROR_SEND -8
#define FT_ERROR_RECV -9
#define FT_ERROR_FILE_WRITE -10

// Utility functions
long get_file_size(const char* filename);
ssize_t send_all(int sock, const void* buffer, size_t length);
ssize_t recv_all(int sock, void* buffer, size_t length);

#endif // FILE_TRANSFER_H
