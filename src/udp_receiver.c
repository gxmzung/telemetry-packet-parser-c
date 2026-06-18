#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "telemetry.h"

#define UDP_PORT 9000
#define BUFFER_SIZE 512
#define WARNING_LOG_PATH "logs/warning_events.csv"

static void init_warning_log(void) {
    FILE *file = fopen(WARNING_LOG_PATH, "w");

    if (file == NULL) {
        fprintf(stderr, "Warning: failed to initialize warning log: %s\n", WARNING_LOG_PATH);
        return;
    }

    fprintf(file, "packet_id,latitude,longitude,altitude,velocity,battery,signal_strength,risk_score\n");
    fclose(file);
}

int main(void) {
    int server_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_fd < 0) {
        perror("socket failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDP_PORT);

    if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }

    init_warning_log();

    printf("========================================\n");
    printf(" UDP Telemetry Receiver - C\n");
    printf("========================================\n");
    printf(" Listening on port %d...\n", UDP_PORT);
    printf(" Warning log: %s\n\n", WARNING_LOG_PATH);

    while (1) {
        TelemetryPacket packet;

        ssize_t received = recvfrom(
            server_fd,
            buffer,
            BUFFER_SIZE - 1,
            0,
            (struct sockaddr *)&client_addr,
            &client_len
        );

        if (received < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[received] = '\0';

        printf("Received raw packet: %s\n", buffer);

        if (!parse_telemetry_line(buffer, &packet)) {
            fprintf(stderr, "Invalid telemetry packet\n");
            printf("----------------------------------------\n");
            continue;
        }

        print_packet(&packet);
        print_diagnostic(&packet);

        if (is_packet_warning(&packet)) {
            append_warning_event(WARNING_LOG_PATH, &packet);
            printf("  Event Log : SAVED\n");
        } else {
            printf("  Event Log : SKIPPED\n");
        }

        printf("----------------------------------------\n");
    }

    close(server_fd);

    return 0;
}
