
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void algorithm(char ip[], int subnet, int prefix) {
    int r = 0;
    int newPrefix;
    int group, cnt;
    int arr[] = {128, 64, 32, 16, 8, 4, 2, 1};
    int val = 0;
    int i, total;

    char ans[20] = "";

    while ((1 << r) < subnet) {
        r++;
    }

    newPrefix = prefix + r;

    if (newPrefix > 32 || newPrefix < 0) {
        printf("Invalid prefix for IPv4 subnetting!\n");
        return;
    }

    group = newPrefix / 8;
    cnt = newPrefix % 8;

    for (i = 0; i < group; i++) {
        strcat(ans, "255");

        if (i < 3) {
            strcat(ans, ".");
        }
    }
    
    if (cnt > 0) {
        val = 0;

        for (i = 0; i < cnt; i++) {
            val = val + arr[i];
        }


        sprintf(ans + strlen(ans), "%d", val);
    }

    total = group;

    if (cnt > 0) {
        total++;
    }

    while (total < 4) {
        if (strlen(ans) > 0) {
            strcat(ans, ".");
        }

        strcat(ans, "0");
        total++;
    }

    printf("\nIP: %s\n", ip);
    printf("Number of subnets: %d\n", subnet);
    printf("Original prefix: /%d\n", prefix);
    printf("New prefix: /%d\n", newPrefix);
    printf("Subnet mask: %s\n", ans);
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;

    char ip[20];
    int subnet, prefix;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    connect(client_fd, (struct sockaddr *)&server_addr,
            sizeof(server_addr));

    recv(client_fd, ip, sizeof(ip), MSG_WAITALL);
    recv(client_fd, &subnet, sizeof(subnet), MSG_WAITALL);
    recv(client_fd, &prefix, sizeof(prefix), MSG_WAITALL);

    algorithm(ip, subnet, prefix);

    close(client_fd);

    return 0;
}
