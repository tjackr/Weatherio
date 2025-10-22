#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include "tcp/tcp_server.h"

#define JSON_DIR "data/cache"

// Global flag for signal handler
volatile sig_atomic_t keep_running = 1;

void handle_shutdown(int sig) {
    keep_running = 0;
    
    if (sig == SIGINT) {
        printf("\n[SERVER] Received SIGINT (Ctrl+C), shutting down gracefully...\n");
    }
}

void ensure_json_directory(void) {
    struct stat st = {0};
    
    if (stat(JSON_DIR, &st) == -1) {
        printf("[SERVER] Creating directory: %s/\n", JSON_DIR);
        if (mkdir(JSON_DIR, 0755) == -1) {
            perror("ERROR: Failed to create data directory");
            exit(1);
        }
    } else {
        printf("[SERVER] JSON files directory: %s/\n", JSON_DIR);
    }
}

int main(void) {
    // Register signal handlers
    signal(SIGINT, handle_shutdown);   // Ctrl+C
    
    
    printf("========================================\n");
    printf("FILE TRANSFER SERVER\n");
    printf("========================================\n");
    printf("Server PID: %d\n", getpid());
    printf("Press Ctrl+C to stop\n");
    
    // Ensure JSON directory exists
    ensure_json_directory();
    
    int result = server_run(PORT);
    
    if (result == FT_SUCCESS) {
        printf("\nServer completed successfully\n");
        return 0;
    } else {
        printf("\nServer failed with error code: %d\n", result);
        return 1;
    }
}