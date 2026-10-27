#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;

    char ip[20];
    int subnet, prefix;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&server_addr,
         sizeof(server_addr));

    listen(server_fd, 1);

    printf("Enter IP: ");
    scanf("%19s", ip);

    printf("Enter number of subnets: ");
    scanf("%d", &subnet);

    printf("Enter original prefix: ");
    scanf("%d", &prefix);

    size = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                       &size);

    send(client_fd, ip, sizeof(ip), 0);
    send(client_fd, &subnet, sizeof(subnet), 0);
    send(client_fd, &prefix, sizeof(prefix), 0);

    printf("Data sent successfully.\n");

    close(client_fd);
    close(server_fd);

    return 0;
}
