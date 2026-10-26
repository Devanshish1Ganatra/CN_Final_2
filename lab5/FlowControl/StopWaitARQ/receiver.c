
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void lostPacket(int packet, int lost, int *timer, int tt, int tp) {
       int timeout = 3*2*(tp);
    if (packet == lost) {
       printf("Packet is lost\n");
       *timer += timeout;
       printf("Timeout for packet %d at %d ms\n", packet, *timer);
    }
}
void algo(int client_fd) {
    int tt, tp, packet, lost;

    // Receive inputs from sender
    recv(client_fd, &tt, sizeof(tt), MSG_WAITALL);
    recv(client_fd, &tp, sizeof(tp), MSG_WAITALL);
    recv(client_fd, &packet, sizeof(packet), MSG_WAITALL);
    recv(client_fd, &lost, sizeof(lost), MSG_WAITALL);

    int timer = 0;
    int loadSender = 0, transmit = 0;
    int loadReceiver = 0, ack = 0;
    int timeout = 3*2*(tp);
    printf("\n--- Stop-and-Wait Timeline ---\n");

    for (int i = 1; i <= packet; i++) {

        loadSender = timer + tt;
        timer = loadSender;
        printf("Load packet %d at sender from %d-%d ms\n",
               i, loadSender - tt, loadSender);

               if(lost == i){
                     lostPacket(i, lost, &timer, tt, tp);
              }
        transmit = timer + tp;
        timer = transmit;
              
        printf("Packet %d received at %d ms\n",
               i, transmit);

        loadReceiver = timer + tt;
        timer = loadReceiver;

        printf("Process packet %d at receiver from %d-%d ms\n",
               i, loadReceiver - tt, loadReceiver);

        ack = timer + tp;
        timer = ack;
        printf("ACK for packet %d received at sender at %d ms\n",
               i, ack);

    }
    printf("Completion time: %d ms\n", timer);
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(client_fd, (struct sockaddr*)&server_addr,
            sizeof(server_addr));

    algo(client_fd);

    close(client_fd);

    return 0;
}
