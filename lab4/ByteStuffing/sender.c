
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void algo(int client_fd)
{
    char flag = 'F';
    char esc = 'E';
    char data[100];
    char stuffedData[1024] = "";
    int p = 0;

    printf("Data: ");
    scanf("%s", data);

    // Add starting flag
    stuffedData[p] = flag;
    p++;

    // Perform byte stuffing
    for(int i = 0; i < strlen(data); i++) {
        if(data[i] == flag) {
            stuffedData[p] = esc;
            p++;
            stuffedData[p] = flag;
            p++;
        }
        else if(data[i] == esc) {
            stuffedData[p] = esc;
            p++;
            stuffedData[p] = esc;
            p++;
        }
        else {
            stuffedData[p] = data[i];
            p++;
        }
    }

    // Add ending flag
    stuffedData[p] = flag;
    p++;

    stuffedData[p] = '\0';

    printf("Stuffed Data: %s\n", stuffedData);

    // Send stuffed data to receiver
    send(client_fd, stuffedData, strlen(stuffedData) + 1, 0);

    printf("Stuffed data sent to receiver.\n");
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

    // 6. Byte stuffing algorithm
    algo(client_fd);

    // 7. Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
