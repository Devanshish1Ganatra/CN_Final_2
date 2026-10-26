
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void algorithm(int client_fd)
{
    int window, frames, tt, tp;

    printf("Enter window size: ");
    scanf("%d", &window);

    printf("Enter number of frames: ");
    scanf("%d", &frames);

    printf("Enter Tt (ms): ");
    scanf("%d", &tt);

    printf("Enter Tp (ms): ");
    scanf("%d", &tp);

    // Send inputs to receiver
    send(client_fd, &window, sizeof(window), 0);
    send(client_fd, &frames, sizeof(frames), 0);
    send(client_fd, &tt, sizeof(tt), 0);
    send(client_fd, &tp, sizeof(tp), 0);

    // Receive completion time
    int totalTime;

    recv(client_fd, &totalTime, sizeof(totalTime), MSG_WAITALL);

    printf("\nTotal completion time: %d ms\n", totalTime);
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr;

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind
    bind(server_fd, (struct sockaddr*)&server_addr,
         sizeof(server_addr));

    // 4. Listen
    listen(server_fd, 5);

    printf("Waiting for receiver...\n");

    // 5. Accept
    client_fd = accept(server_fd, NULL, NULL);

    // 6. Run algorithm
    algorithm(client_fd);

    // 7. Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
