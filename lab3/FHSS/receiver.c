
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void algo(int client_fd)
{
    int k, groups, i, j;
    int hoppingFreq[100];
    char bits[100][20];
    int freq[100], count;
    char data[1024] = "";

    // Receive k from sender
    recv(client_fd, &k, sizeof(k), 0);

    // Receive number of frequencies
    recv(client_fd, &groups, sizeof(groups), 0);

    // Receive frequency hopping sequence
    recv(client_fd, hoppingFreq, groups * sizeof(int), 0);

    count = 1 << k;

    // Generate binary combinations and frequencies
    for(i = 0; i < count; i++) {
        for(j = k - 1; j >= 0; j--) {
            bits[i][k - 1 - j] = ((i >> j) & 1) + '0';
        }

        bits[i][k] = '\0';
        freq[i] = 200 + 100 * i;
    }

    printf("Received frequency sequence:\n");

    // Find each frequency in the mapping
    for(i = 0; i < groups; i++) {
        printf("%d kHz\n", hoppingFreq[i]);

        for(j = 0; j < count; j++) {
            if(hoppingFreq[i] == freq[j]) {
                strcat(data, bits[j]);
                break;
            }
        }
    }

    printf("Decoded binary data: %s\n", data);
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

    // 4. FHSS algorithm
    algo(client_fd);

    // 5. Close socket
    close(client_fd);

    return 0;
}
