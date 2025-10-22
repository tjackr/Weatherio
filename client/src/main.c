#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcp/tcp_client.h"

int main(void) {
    printf("========================================\n");
    printf("TCP CLIENT\n");
    printf("========================================\n");
    
    int choice;
    int running = 1;
    
    while (running) {
        printf("\n--- MENU ---\n");
        printf("1. Send message to server\n");
        printf("2. Download file from server\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline
        
        if (choice == 3) {
            printf("Exiting...\n");
            break;
        }
        
        ClientConfig config;
        int result;
        
        // Initialize and connect
        result = client_init(&config, SERVER_IP, PORT);
        if (result != FT_SUCCESS) {
            printf("Failed to initialize. Try again.\n");
            continue;
        }
        
        result = client_connect(&config);
        if (result != FT_SUCCESS) {
            client_cleanup(&config);
            printf("Failed to connect. Is server running?\n");
            continue;
        }
        
        switch(choice) {
            case 1: {
                // Send custom message
                char message[1024];
                printf("Enter message: ");
                fgets(message, sizeof(message), stdin);
                message[strcspn(message, "\n")] = 0; // remove newline
                
                result = client_send_message(&config, message);
                if (result == FT_SUCCESS) {
                    printf("Message sent successfully!\n");
                } else {
                    printf("Failed to send message.\n");
                }
                break;
            }
            
            case 2: {
                // Download file
                char remote_file[256];
                char local_file[256];
                
                printf("Enter remote filename: ");
                fgets(remote_file, sizeof(remote_file), stdin);
                remote_file[strcspn(remote_file, "\n")] = 0;
                
                printf("Enter local filename to save: ");
                fgets(local_file, sizeof(local_file), stdin);
                local_file[strcspn(local_file, "\n")] = 0;
                
                // Send message first
                result = client_send_message(&config, "File download request");
                if (result != FT_SUCCESS) {
                    printf("Failed to send request.\n");
                    break;
                }
                
                // Request and download file
                result = client_request_file(&config, remote_file);
                if (result != FT_SUCCESS) {
                    printf("Failed to request file.\n");
                    break;
                }
                
                result = client_receive_file(&config, local_file);
                if (result == FT_SUCCESS) {
                    printf("File downloaded successfully!\n");
                } else {
                    printf("Failed to download file.\n");
                }
                break;
            }
            
            default:
                printf("Invalid choice! Please enter 1, 2, or 3.\n");
                break;
        }
        
        client_cleanup(&config);
    }
    
    printf("\nGoodbye!\n");
    return 0;
}