#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

#include "telemetry.h"

#define UDP_PORT 9000
#define BUFFER_SIZE 512
#define WARNING_LOG_PATH "logs/warning_events.csv"
#define SESSION_REPORT_PATH "logs/udp_session_report.txt"

static volatile sig_atomic_t running = 1;
static TelemetrySummary session_summary;

static void handle_sigint(int signal_number) {
    (void)signal_number;
    running = 0;
}

static void init_warning_log(void) {
    FILE *file = fopen(WARNING_LOG_PATH, "w");

    if (file == NULL) {
        fprintf(stderr, "Warning: failed to initialize warning log: %s\n", WARNING_LOG_PATH);
        return;
    }

    fprintf(file, "packet_id,latitude,longitude,altitude,velocity,battery,signal_strength,risk_score,risk_level\n");
    fclose(file);
}

static void write_session_report(void) {
    finalize_summary(&session_summary);
    print_summary(&session_summary);
    write_diagnostic_report(SESSION_REPORT_PATH, &session_summary);

    printf("\nUDP session report saved to: %s\n", SESSION_REPORT_PATH);
    printf("Warning event log saved to : %s\n", WARNING_LOG_PATH);
}

int main(void) {
    int server_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    signal(SIGINT, handle_sigint);
    init_summary(&session_summary);

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
    printf(" Warning log: %s\n", WARNING_LOG_PATH);
    printf(" Press Ctrl+C to stop and print summary.\n\n");

    while (running) {
        TelemetryPacket packet;

        ssize_t received = recvfrom(
            server_fd,
            buffer,
            BUFFER_SIZE - 1,
            0,
            (struct sockaddr *)&client_addr,
            &client_len
        );

        if (!running) {
            break;
        }

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

        update_summary(&session_summary, &packet);

        if (is_packet_warning(&packet)) {
            append_warning_event(WARNING_LOG_PATH, &packet);
            printf("  Event Log : SAVED\n");
        } else {
            printf("  Event Log : SKIPPED\n");
        }

        printf("  Session Count : %d packets\n", session_summary.total_count);
        printf("----------------------------------------\n");
    }

    close(server_fd);

    printf("\nReceiver stopped by user.\n");

    if (session_summary.total_count > 0) {
        write_session_report();
    } else {
        printf("No telemetry packets received.\n");
    }

    return 0;
}
