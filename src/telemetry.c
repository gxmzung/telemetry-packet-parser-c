#include <stdio.h>
#include "telemetry.h"

int parse_telemetry_line(const char *line, TelemetryPacket *packet) {
    if (line == NULL || packet == NULL) {
        return 0;
    }

    int parsed = sscanf(
        line,
        "%d,%lf,%lf,%lf,%lf,%lf,%d",
        &packet->packet_id,
        &packet->latitude,
        &packet->longitude,
        &packet->altitude,
        &packet->velocity,
        &packet->battery,
        &packet->signal_strength
    );

    return parsed == 7;
}

void print_packet(const TelemetryPacket *packet) {
    if (packet == NULL) {
        return;
    }

    printf("Packet #%d\n", packet->packet_id);
    printf("  Position : %.6f, %.6f\n", packet->latitude, packet->longitude);
    printf("  Altitude : %.2f m\n", packet->altitude);
    printf("  Velocity : %.2f m/s\n", packet->velocity);
    printf("  Battery  : %.2f %%\n", packet->battery);
    printf("  Signal   : %d dBm\n", packet->signal_strength);
}

int is_packet_warning(const TelemetryPacket *packet) {
    if (packet == NULL) {
        return 0;
    }

    if (packet->battery < 25.0) {
        return 1;
    }

    if (packet->signal_strength < -85) {
        return 1;
    }

    if (packet->altitude > 1000.0) {
        return 1;
    }

    if (packet->velocity > 80.0) {
        return 1;
    }

    return 0;
}

void print_diagnostic(const TelemetryPacket *packet) {
    if (packet == NULL) {
        return;
    }

    printf("  Diagnostic: ");

    if (!is_packet_warning(packet)) {
        printf("NORMAL\n");
        return;
    }

    printf("WARNING - ");

    if (packet->battery < 25.0) {
        printf("LOW_BATTERY ");
    }

    if (packet->signal_strength < -85) {
        printf("WEAK_SIGNAL ");
    }

    if (packet->altitude > 1000.0) {
        printf("HIGH_ALTITUDE ");
    }

    if (packet->velocity > 80.0) {
        printf("HIGH_SPEED ");
    }

    printf("\n");
}
