// TCP_Receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 2048

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <PORT> <ALGO>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);
    const char *algo = argv[3];

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

    // Bind socket
    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 1) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    // Accept connection
    socklen_t client_len = sizeof(client_addr);
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sockfd == -1) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    // Receive the file
    FILE *file = tmpfile();
    if (!file) {
        perror("Error creating temporary file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    clock_t start_time, end_time;
    start_time = clock();
    while ((bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }
    end_time = clock();
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    // Send response to the sender
    const char *response_msg = "ack";
    if (send(client_sockfd, response_msg, strlen(response_msg), 0) == -1) {
        perror("Error sending response message");
        exit(EXIT_FAILURE);
    }

    // Close connection and file
    close(client_sockfd);
    fclose(file);
    close(sockfd);

    // Calculate and print statistics
    double total_bandwidth = (2.0 / (1024 * 1024)) / elapsed_time; // 2MB file size
    printf("----------------------------------\n");
    printf("- * Statistics * -\n");
    printf("- Run #1 Data: Time=%.2fms; Speed=%.2fMB/s\n", elapsed_time * 1000, total_bandwidth);
    printf("- Average time: %.2fms\n", elapsed_time * 1000);
    printf("- Average bandwidth: %.2fMB/s\n", total_bandwidth);
    printf("----------------------------------\n");

    return 0;
}


