#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>
#include <sys/types.h>

#define LOG_SERVER_PORT 9000
#define BUFFER_SIZE 1024

void log_message_to_server(const char *client_ip, const char *message) {
    int udp_socket;
    struct sockaddr_in log_server_addr;
    char log_message[BUFFER_SIZE];

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[BUFFER_SIZE];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    snprintf(log_message, sizeof(log_message), "%s \"%s\" was received from %s", timestamp, message, client_ip);

    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        perror("Socket creation failed");
        return;
    }

    memset(&log_server_addr, 0, sizeof(log_server_addr));
    log_server_addr.sin_family = AF_INET;
    log_server_addr.sin_port = htons(LOG_SERVER_PORT);
    log_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(udp_socket, log_message, strlen(log_message), 0, (struct sockaddr *)&log_server_addr, sizeof(log_server_addr));
    close(udp_socket);
}

void handle_client(int client_socket, struct sockaddr_in client_addr, int is_udp) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            close(client_socket);
            break;
        }

        send(client_socket, buffer, bytes_received, 0);

        char *client_ip = inet_ntoa(client_addr.sin_addr);
        log_message_to_server(client_ip, buffer);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int tcp_socket, udp_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    fd_set read_fds;

    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (tcp_socket < 0 || udp_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(tcp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ||
        bind(udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(tcp_socket, 5);

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(tcp_socket, &read_fds);
        FD_SET(udp_socket, &read_fds);

        int max_fd = tcp_socket > udp_socket ? tcp_socket : udp_socket;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Select failed");
            continue;
        }

        if (FD_ISSET(tcp_socket, &read_fds)) {
            new_socket = accept(tcp_socket, (struct sockaddr *)&client_addr, &addr_len);
            if (fork() == 0) {
                handle_client(new_socket, client_addr, 0);
                exit(EXIT_SUCCESS);
            }
            close(new_socket);
        }

        if (FD_ISSET(udp_socket, &read_fds)) {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, sizeof(buffer));
            recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
            log_message_to_server(inet_ntoa(client_addr.sin_addr), buffer);
        }
    }

    close(tcp_socket);
    close(udp_socket);
    return 0;
}