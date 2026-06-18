#include <stdio.h>
#include <string.h>
#include "telemetry.h"

int main(int argc, char *argv[]) {
    const char *file_path = "data/sample_packets.csv";

    if (argc >= 2) {
        file_path = argv[1];
    }

    FILE *file = fopen(file_path, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: failed to open file: %s\n", file_path);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int total_count = 0;
    int warning_count = 0;

    printf("========================================\n");
    printf(" Telemetry Packet Parser - C\n");
    printf("========================================\n\n");

    while (fgets(line, sizeof(line), file) != NULL) {
        TelemetryPacket packet;

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (!parse_telemetry_line(line, &packet)) {
            fprintf(stderr, "Invalid packet line: %s\n", line);
            continue;
        }

        print_packet(&packet);
        print_diagnostic(&packet);
        printf("----------------------------------------\n");

        total_count++;

        if (is_packet_warning(&packet)) {
            warning_count++;
        }
    }

    fclose(file);

    printf("\nSummary\n");
    printf("  Total packets   : %d\n", total_count);
    printf("  Warning packets : %d\n", warning_count);
    printf("  Normal packets  : %d\n", total_count - warning_count);

    return 0;
}
