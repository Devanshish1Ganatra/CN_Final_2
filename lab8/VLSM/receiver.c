
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void algorithm(char ip[], int prefix, int subnet, int hosts[]) {
    int i, h, newPrefix;
    int group, cnt, val;
    int arr[] = {128, 64, 32, 16, 8, 4, 2, 1};
    char ans[20];

    printf("\nIP: %s\n", ip);

    for (i = 0; i < subnet; i++) {
        h = 0;

        while ((1 << h) - 2 < hosts[i]) {
            h++;
        }

        newPrefix = 32 - h;

        if (newPrefix < prefix) {
            printf("Subnet %d: Not enough address space!\n", i + 1);
            continue;
        }

        group = newPrefix / 8;
        cnt = newPrefix % 8;

        strcpy(ans, "");

        for (int j = 0; j < group; j++) {
            strcat(ans, "255");

            if (j < 3) {
                strcat(ans, ".");
            }
        }

        if (cnt > 0) {
            val = 0;

            for (int j = 0; j < cnt; j++) {
                val += arr[j];
            }

            if (group > 0) {
                strcat(ans, ".");
            }

            sprintf(ans + strlen(ans), "%d", val);
        }

        int total = group;

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

        printf("\nSubnet %d\n", i + 1);
        printf("Required hosts: %d\n", hosts[i]);
        printf("Prefix: /%d\n", newPrefix);
        printf("Subnet Mask: %s\n", ans);
    }
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;

    char ip[20];
    int prefix, subnet, hosts[20], i;
    int received;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    connect(client_fd, (struct sockaddr *)&server_addr,
            sizeof(server_addr));

    recv(client_fd, ip, sizeof(ip), MSG_WAITALL);
    recv(client_fd, &prefix, sizeof(prefix), MSG_WAITALL);
    recv(client_fd, &subnet, sizeof(subnet), MSG_WAITALL);

    for (i = 0; i < subnet; i++) {
        recv(client_fd, &hosts[i], sizeof(hosts[i]), MSG_WAITALL);
    }

    algorithm(ip, prefix, subnet, hosts);

    close(client_fd);

    return 0;
}
