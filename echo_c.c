#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <server_ip> <port> <tcp|udp>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);
    int is_tcp = strcmp(argv[3], "tcp") == 0;

    int client_socket;
    struct sockaddr_in server_addr;

    if (is_tcp) {
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
    } else {
        client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    }

    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    if (is_tcp && connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        if (is_tcp) {
            send(client_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(client_socket, buffer, sizeof(buffer), 0);
        } else {
            sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
            memset(buffer, 0, sizeof(buffer));
            recvfrom(client_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        }

        printf("Echo from server: %s", buffer);
    }

    close(client_socket);
    return 0;
}