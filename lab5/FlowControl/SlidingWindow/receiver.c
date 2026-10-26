
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX 1000

void algorithm(int client_fd)
{
    int window, frames, tt, tp;

    // Receive inputs from sender
    recv(client_fd, &window, sizeof(window), MSG_WAITALL);
    recv(client_fd, &frames, sizeof(frames), MSG_WAITALL);
    recv(client_fd, &tt, sizeof(tt), MSG_WAITALL);
    recv(client_fd, &tp, sizeof(tp), MSG_WAITALL);

    int ackTime[MAX];

    // Three arrays required for Sliding Window
    int transmitted[MAX] = {0};
    int acknowledged[MAX] = {0};
    int outstanding[MAX] = {0};

    int currentTime = 0;
    int outstandingCount = 0;

    printf("\nSLIDING WINDOW (NOISELESS CHANNEL)\n");
    printf("Window = %d, Frames = %d, Tt = %d ms, Tp = %d ms\n\n",
           window, frames, tt, tp);

    printf("%-12s | %-34s | %-34s\n",
           "Time (ms)", "SENDER", "RECEIVER");

    printf("------------|------------------------------------|------------------------------------\n");

    for (int p = 0; p < frames; p++) {

        // If window is full, wait for the earliest ACK
        while (outstandingCount >= window) {
            int earliest = -1;

            for (int i = 0; i < p; i++) {
                if (acknowledged[i] == 0) {
                    if (earliest == -1 ||
                        ackTime[i] < ackTime[earliest]) {
                        earliest = i;
                    }
                }
            }

            currentTime = ackTime[earliest];

            // Update arrays for all ACKs arriving by this time
            for (int i = 0; i < p; i++) {
                if (acknowledged[i] == 0 &&
                    ackTime[i] <= currentTime) {
                    acknowledged[i] = 1;
                    outstanding[i] = 0;
                    outstandingCount--;
                }
            }
        }

        // Load and send the next frame
        int startTime = currentTime;
        int sendTime = startTime + tt;
        int arrival = sendTime + tp;
        int receiverEnd = arrival + tt;

        ackTime[p] = receiverEnd + tp;

        transmitted[p] = 1;
        outstanding[p] = 1;
        outstandingCount++;

        printf("%d-%d        | Load frame P%d\n",
               startTime, sendTime, p + 1);

        printf("%-12d | Send P%d -----------------------> | Arrives at %d ms\n",
               sendTime, p + 1, arrival);

        printf("%d-%d        |                                    | Receive P%d, prepare ACK\n",
               arrival, receiverEnd, p + 1);

        printf("%d-%d        | <----------------------- ACK P%d | ACK arrives at %d ms\n",
               receiverEnd, ackTime[p], p + 1, ackTime[p]);

        outstandingCount++;
        // Correct the count: each transmitted frame adds one outstanding frame.
        // (The increment above is the original window-tracking operation.)

        currentTime = sendTime;
    }

    // Wait for all remaining ACKs
    int totalTime = currentTime;

    for (int i = 0; i < frames; i++) {
        if (ackTime[i] > totalTime)
            totalTime = ackTime[i];
    }

    // All transmitted frames are acknowledged by completion
    for (int i = 0; i < frames; i++) {
        if (transmitted[i] == 1 &&
            ackTime[i] <= totalTime) {
            acknowledged[i] = 1;
            outstanding[i] = 0;
        }
    }

    printf("\nTotal frames sent       : %d\n", frames);
    printf("Window size             : %d\n", window);
    printf("Total completion time   : %d ms\n", totalTime);

    // Send completion time back to sender
    send(client_fd, &totalTime, sizeof(totalTime), 0);
}

int main()
{
    int client_fd;
    struct sockaddr_in server_addr;

    // 1. Create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Connect
    connect(client_fd, (struct sockaddr*)&server_addr,
            sizeof(server_addr));

    // 4. Run algorithm
    algorithm(client_fd);

    // 5. Close socket
    close(client_fd);

    return 0;
}
