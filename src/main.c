#include <stdio.h>
#include <string.h>
#include "telemetry.h"

static int has_bin_extension(const char *file_path) {
    const char *extension = strrchr(file_path, '.');

    if (extension == NULL) {
        return 0;
    }

    return strcmp(extension, ".bin") == 0;
}

static void process_packet(const TelemetryPacket *packet, TelemetrySummary *summary) {
    print_packet(packet);
    print_diagnostic(packet);
    printf("----------------------------------------\n");

    update_summary(summary, packet);
}

int main(int argc, char *argv[]) {
    const char *file_path = "data/sample_packets.csv";
    const char *report_path = "logs/diagnostic_report.txt";

    if (argc >= 2) {
        file_path = argv[1];
    }

    int binary_mode = has_bin_extension(file_path);

    FILE *file = fopen(file_path, binary_mode ? "rb" : "r");

    if (file == NULL) {
        fprintf(stderr, "Error: failed to open file: %s\n", file_path);
        return 1;
    }

    TelemetrySummary summary;

    init_summary(&summary);

    printf("========================================\n");
    printf(" Telemetry Packet Parser - C\n");
    printf("========================================\n");
    printf(" Input file: %s\n", file_path);
    printf(" Mode      : %s\n\n", binary_mode ? "BINARY" : "CSV");

    if (binary_mode) {
        TelemetryPacket packet;

        while (read_binary_packet(file, &packet)) {
            process_packet(&packet, &summary);
        }
    } else {
        char line[MAX_LINE_LENGTH];

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

            process_packet(&packet, &summary);
        }
    }

    fclose(file);

    finalize_summary(&summary);
    print_summary(&summary);
    write_diagnostic_report(report_path, &summary);

    printf("\nReport saved to: %s\n", report_path);

    return 0;
}
