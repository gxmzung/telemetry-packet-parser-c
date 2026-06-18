#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define UDP_PORT 9000
#define SERVER_IP "127.0.0.1"

int main(void) {
    int sock_fd;
    struct sockaddr_in server_addr;

    const char *packets[] = {
        "1,36.350411,127.384548,120.5,14.2,87.5,-62",
        "2,36.351002,127.385010,130.1,16.8,72.3,-68",
        "4,36.353450,127.387800,450.2,35.1,24.5,-79",
        "3,36.352120,127.386200,210.0,22.5,51.0,-74",
        "6,36.355100,127.389000,1050.4,84.3,19.8,-91"
    };

    size_t packet_count = sizeof(packets) / sizeof(packets[0]);

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock_fd < 0) {
        perror("socket failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    printf("========================================\n");
    printf(" UDP Telemetry Anomaly Sender - C\n");
    printf("========================================\n");
    printf(" This sender intentionally skips and reorders packets.\n\n");

    for (size_t i = 0; i < packet_count; i++) {
        sendto(
            sock_fd,
            packets[i],
            strlen(packets[i]),
            0,
            (const struct sockaddr *)&server_addr,
            sizeof(server_addr)
        );

        printf("Sent packet: %s\n", packets[i]);
        sleep(1);
    }

    close(sock_fd);

    printf("\nAnomaly packets sent.\n");

    return 0;
}
