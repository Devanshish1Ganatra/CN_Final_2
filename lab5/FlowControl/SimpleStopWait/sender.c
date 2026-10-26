#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void algo(int client_fd) {
    int tt, tp, packet;

    printf("Tp: ");
    scanf("%d", &tp);

    printf("Tt: ");
    scanf("%d", &tt);

    printf("Enter number of packet: ");
    scanf("%d", &packet);

    // Send inputs to receiver
    send(client_fd, &tt, sizeof(tt), 0);
    send(client_fd, &tp, sizeof(tp), 0);
    send(client_fd, &packet, sizeof(packet), 0);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    listen(server_fd, 5);

    printf("Waiting for receiver...\n");

    client_fd = accept(server_fd, NULL, NULL);

    algo(client_fd);

    close(client_fd);
    close(server_fd);

    return 0;
}