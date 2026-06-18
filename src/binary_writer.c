#include <stdio.h>
#include "telemetry.h"

int main(void) {
    const char *output_path = "data/sample_packets.bin";

    BinaryTelemetryPacket packets[] = {
        {TELEMETRY_MAGIC, 1, 36.350411, 127.384548, 120.5, 14.2, 87.5, -62},
        {TELEMETRY_MAGIC, 2, 36.351002, 127.385010, 130.1, 16.8, 72.3, -68},
        {TELEMETRY_MAGIC, 3, 36.352120, 127.386200, 210.0, 22.5, 51.0, -74},
        {TELEMETRY_MAGIC, 4, 36.353450, 127.387800, 450.2, 35.1, 24.5, -79},
        {TELEMETRY_MAGIC, 5, 36.354900, 127.388300, 980.0, 65.7, 38.2, -88},
        {TELEMETRY_MAGIC, 6, 36.355100, 127.389000, 1050.4, 84.3, 19.8, -91}
    };

    FILE *file = fopen(output_path, "wb");

    if (file == NULL) {
        fprintf(stderr, "Error: failed to create binary file: %s\n", output_path);
        return 1;
    }

    size_t packet_count = sizeof(packets) / sizeof(packets[0]);

    fwrite(packets, sizeof(BinaryTelemetryPacket), packet_count, file);
    fclose(file);

    printf("Binary telemetry file created: %s\n", output_path);
    printf("Packet count: %zu\n", packet_count);

    return 0;
}
