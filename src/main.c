#include <stdio.h>
#include <string.h>
#include "telemetry.h"

int main(int argc, char *argv[]) {
    const char *file_path = "data/sample_packets.csv";
    const char *report_path = "logs/diagnostic_report.txt";

    if (argc >= 2) {
        file_path = argv[1];
    }

    FILE *file = fopen(file_path, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: failed to open file: %s\n", file_path);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    TelemetrySummary summary;

    init_summary(&summary);

    printf("========================================\n");
    printf(" Telemetry Packet Parser - C\n");
    printf("========================================\n");
    printf(" Input file: %s\n\n", file_path);

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

        update_summary(&summary, &packet);
    }

    fclose(file);

    finalize_summary(&summary);
    print_summary(&summary);
    write_diagnostic_report(report_path, &summary);

    printf("\nReport saved to: %s\n", report_path);

    return 0;
}
