
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void algo(int client_fd)
{
    int k, i, j, m;
    char data[100], temp[20];
    char bits[100][20];
    int freq[100], count = 1;
    int hoppingFreq[100], groups = 0;

    printf("k: ");
    scanf("%d", &k);

    printf("Data: ");
    scanf("%s", data);

    // Send k to receiver
    send(client_fd, &k, sizeof(k), 0);

    count = 1 << k;

    // Generate binary combinations and frequencies
    for(i = 0; i < count; i++) {
        for(j = k - 1; j >= 0; j--) {
            bits[i][k - 1 - j] = ((i >> j) & 1) + '0';
        }

        bits[i][k] = '\0';
        freq[i] = 200 + 100 * i;
    }

    // Divide data into groups and find frequencies
    int n = strlen(data);

    for(i = 0; i < n; i = i + k) {
        int p = 0;

        for(j = i; j < i + k; j++) {
            temp[p] = data[j];
            p++;
        }

        temp[p] = '\0';

        for(m = 0; m < count; m++) {
            if(strcmp(bits[m], temp) == 0) {
                hoppingFreq[groups] = freq[m];

                printf("%s -> %d kHz\n", temp, freq[m]);

                groups++;
                break;
            }
        }
    }

    // Send number of frequencies
    send(client_fd, &groups, sizeof(groups), 0);

    // Send frequency hopping sequence
    send(client_fd, hoppingFreq, groups * sizeof(int), 0);

    printf("Frequency sequence sent to receiver.\n");

    // Receive decoded data
    char buffer[1024];

    
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr;

    char buffer[1024];

    // 1. Create a TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    // 2. Configure the server address
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 3. Bind the socket to port 8080
    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // 4. Start listening for connections
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Sender waiting on port 8080...\n");

    // 5. Accept receiver connection
    client_fd = accept(server_fd, NULL, NULL);

    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    // 6. FHSS algorithm
    algo(client_fd);

    // 7. Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
