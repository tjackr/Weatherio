#include "tcp_file_transfer.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

long get_file_size(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

ssize_t send_all(int sock, const void* buffer, size_t length) {
    const char* ptr = (const char*)buffer;
    size_t remaining = length;
    
    while (remaining > 0) {
        ssize_t sent = write(sock, ptr, remaining);
        
        if (sent < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        
        remaining -= sent;
        ptr += sent;
    }
    
    return length;
}

ssize_t recv_all(int sock, void* buffer, size_t length) {
    char* ptr = (char*)buffer;
    size_t remaining = length;
    
    while (remaining > 0) {
        ssize_t received = read(sock, ptr, remaining);
        
        if (received < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        
        if (received == 0) {
            return 0;
        }
        
        remaining -= received;
        ptr += received;
    }
    
    return length;
}