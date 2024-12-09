#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define LOG_SERVER_PORT 9000
#define BUFFER_SIZE 1024
#define LOG_FILE "echo.log"

void log_to_file(const char *log_message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        perror("Failed to open log file");
        return;
    }
    fprintf(log_file, "%s\n", log_message);
    fclose(log_file);
}

int main() {
    int log_udp_socket;
    struct sockaddr_in log_server_addr, log_client_addr;
    socklen_t log_addr_len = sizeof(log_client_addr);
    char log_buffer[BUFFER_SIZE];

    log_udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (log_udp_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&log_server_addr, 0, sizeof(log_server_addr));
    log_server_addr.sin_family = AF_INET;
    log_server_addr.sin_port = htons(LOG_SERVER_PORT);
    log_server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(log_udp_socket, (struct sockaddr *)&log_server_addr, sizeof(log_server_addr)) < 0) {
        perror("Bind failed");
        close(log_udp_socket);
        exit(EXIT_FAILURE);
    }

    printf("Log server started on port %d\n", LOG_SERVER_PORT);

    while (1) {
        memset(log_buffer, 0, sizeof(log_buffer));
        recvfrom(log_udp_socket, log_buffer, sizeof(log_buffer), 0, (struct sockaddr *)&log_client_addr, &log_addr_len);
        printf("Received log: %s\n", log_buffer);
        log_to_file(log_buffer);
    }

    close(log_udp_socket);
    return 0;
}