
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void algo(int client_fd)
{
    char pattern[] = "10110111000";
    char inverse[20] = "";
    char spreadData[1024];
    char orgData[100] = "";
    char temp[20];
    int p = 0;

    int n = strlen(pattern);

    // Generate inverse pattern
    for(int i = 0; i < n; i++) {
        if(pattern[i] == '0') {
            inverse[i] = '1';
        }
        else {
            inverse[i] = '0';
        }
    }
    inverse[n] = '\0';

    // Receive spread data
    int bytes = recv(client_fd, spreadData, sizeof(spreadData) - 1, 0);

    if(bytes <= 0) {
        return;
    }

    spreadData[bytes] = '\0';

    printf("Received spread data: %s\n", spreadData);

    // Decode spread data in pattern-sized groups
    int len = strlen(spreadData);

    for(int i = 0; i < len; i = i + n) {
        p = 0;

        for(int j = i; j < i + n; j++) {
            temp[p] = spreadData[j];
            p++;
        }
        temp[p] = '\0';

        if(strcmp(temp, pattern) == 0) {
            strcat(orgData, "1");
        }
        else {
            strcat(orgData, "0");
        }
    }

    printf("Original data: %s\n", orgData);

}

int main()
{
    // 1. Create socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd == -1) {
        perror("socket");
        return 1;
    }

    // 2. Configure server address
    struct sockaddr_in server_addr = {0};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Connect to sender
    if (connect(client_fd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_fd);
        return 1;
    }

    printf("Connected to sender!\n");

    // 4. DSSS algorithm
    algo(client_fd);

    // 5. Close socket
    close(client_fd);

    return 0;
}
