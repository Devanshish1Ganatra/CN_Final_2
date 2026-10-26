
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void algo(int client_fd)
{
    char pattern[] = "10110111000";
    char inverse[20] = "";
    char data[100];
    char spreadData[1024] = "";
    int p = 0;

    printf("Data: ");
    scanf("%s", data);

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

    // Generate spread data
    for(int i = 0; i < strlen(data); i++) {
        if(data[i] == '1') {
            for(int j = 0; j < n; j++) {
                spreadData[p] = pattern[j];
                p++;
            }
        }
        else {
            for(int j = 0; j < n; j++) {
                spreadData[p] = inverse[j];
                p++;
            }
        }
    }

    spreadData[p] = '\0';

    printf("Spread Data: %s\n", spreadData);

    // Send spread data to receiver
    send(client_fd, spreadData, strlen(spreadData) + 1, 0);

    printf("Spread data sent to receiver.\n");

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

    // 5. Accept one receiver
    client_fd = accept(server_fd, NULL, NULL);

    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    // 6. DSSS algorithm
    algo(client_fd);

    // 7. Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
