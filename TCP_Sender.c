// TCP_Sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 2048

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <IP> <PORT> <ALGO>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip = argv[2];
    int port = atoi(argv[3]);
    const char *algo = argv[4];

    // Read the file
    FILE *file = fopen("file.txt", "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set congestion control algorithm
    if (setsockopt(sockfd, IPPROTO_TCP, 0, algo, strlen(algo)) == -1) {
        perror("Error setting congestion control algorithm");
        exit(EXIT_FAILURE);
    }

    // Connect to the receiver
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    // Send the file
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(sockfd, buffer, bytes_read, 0) == -1) {
            perror("Error sending file");
            exit(EXIT_FAILURE);
        }
    }

    // User decision to send file again
    char choice;
    do {
        printf("Send the file again? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'y') {
            rewind(file);
            while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
                if (send(sockfd, buffer, bytes_read, 0) == -1) {
                    perror("Error sending file");
                    exit(EXIT_FAILURE);
                }
            }
        }
    } while (choice == 'y');

    // Send exit message
    const char *exit_msg = "exit";
    if (send(sockfd, exit_msg, strlen(exit_msg), 0) == -1) {
        perror("Error sending exit message");
        exit(EXIT_FAILURE);
    }

    // Close the TCP connection and file
    fclose(file);
    close(sockfd);

    return 0;
}

	
