
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void algo(int client_fd)
{
    char flag = 'F';
    char esc = 'E';
    char stuffedData[1024];
    char orgData[1024] = "";
    int p = 0;

    // Receive stuffed data
    int bytes = recv(client_fd, stuffedData,
                     sizeof(stuffedData) - 1, 0);

    if(bytes <= 0) {
        printf("No data received.\n");
        return;
    }

    stuffedData[bytes] = '\0';

    printf("Received Stuffed Data: %s\n", stuffedData);

    int n = strlen(stuffedData);

    // Check minimum length and flags
    if(n < 2) {
        printf("Invalid frame\n");
        return;
    }

    if(stuffedData[0] != flag) {
        printf("Invalid starting flag\n");
        return;
    }

    if(stuffedData[n - 1] != flag) {
        printf("Invalid ending flag\n");
        return;
    }

    printf("Both flags are valid\n");

    // Remove flags and perform byte destuffing
    for(int i = 1; i < n - 1; i++) {
        if(stuffedData[i] == esc) {
            i++;
            orgData[p] = stuffedData[i];
            p++;
        }
        else {
            orgData[p] = stuffedData[i];
            p++;
        }
    }

    orgData[p] = '\0';

    printf("Original Data: %s\n", orgData);
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

    // 4. Byte destuffing algorithm
    algo(client_fd);

    // 5. Close socket
    close(client_fd);

    return 0;
}
