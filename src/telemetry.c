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

void convert_binary_to_telemetry(const BinaryTelemetryPacket *binary, TelemetryPacket *packet) {
    if (binary == NULL || packet == NULL) {
        return;
    }

    packet->packet_id = binary->packet_id;
    packet->latitude = binary->latitude;
    packet->longitude = binary->longitude;
    packet->altitude = binary->altitude;
    packet->velocity = binary->velocity;
    packet->battery = binary->battery;
    packet->signal_strength = binary->signal_strength;
}

int read_binary_packet(FILE *file, TelemetryPacket *packet) {
    if (file == NULL || packet == NULL) {
        return 0;
    }

    BinaryTelemetryPacket binary_packet;

    size_t read_count = fread(
        &binary_packet,
        sizeof(BinaryTelemetryPacket),
        1,
        file
    );

    if (read_count != 1) {
        return 0;
    }

    if (binary_packet.magic != TELEMETRY_MAGIC) {
        fprintf(stderr, "Invalid binary packet magic: 0x%X\n", binary_packet.magic);
        return 0;
    }

    convert_binary_to_telemetry(&binary_packet, packet);

    return 1;
}


const char *get_risk_level(int risk_score) {
    if (risk_score >= 80) {
        return "CRITICAL";
    }

    if (risk_score >= 60) {
        return "HIGH";
    }

    if (risk_score >= 40) {
        return "MEDIUM";
    }

    if (risk_score >= 1) {
        return "LOW";
    }

    return "NORMAL";
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
    int risk_score = calculate_risk_score(packet);
    printf("  Risk     : %d / 100\n", risk_score);
    printf("  Level    : %s\n", get_risk_level(risk_score));
}

int is_packet_warning(const TelemetryPacket *packet) {
    if (packet == NULL) {
        return 0;
    }

    return calculate_risk_score(packet) >= 40;
}

int calculate_risk_score(const TelemetryPacket *packet) {
    int score = 0;

    if (packet == NULL) {
        return 0;
    }

    if (packet->battery < 25.0) {
        score += 35;
    } else if (packet->battery < 40.0) {
        score += 15;
    }

    if (packet->signal_strength < -85) {
        score += 30;
    } else if (packet->signal_strength < -75) {
        score += 10;
    }

    if (packet->altitude > 1000.0) {
        score += 25;
    } else if (packet->altitude > 800.0) {
        score += 10;
    }

    if (packet->velocity > 80.0) {
        score += 25;
    } else if (packet->velocity > 60.0) {
        score += 10;
    }

    if (score > 100) {
        score = 100;
    }

    return score;
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

void init_summary(TelemetrySummary *summary) {
    if (summary == NULL) {
        return;
    }

    summary->total_count = 0;
    summary->warning_count = 0;
    summary->average_altitude = 0.0;
    summary->average_velocity = 0.0;
    summary->average_battery = 0.0;
    summary->max_altitude = 0.0;
    summary->max_velocity = 0.0;
    summary->min_battery = 100.0;
}

void update_summary(TelemetrySummary *summary, const TelemetryPacket *packet) {
    if (summary == NULL || packet == NULL) {
        return;
    }

    summary->total_count++;

    if (is_packet_warning(packet)) {
        summary->warning_count++;
    }

    summary->average_altitude += packet->altitude;
    summary->average_velocity += packet->velocity;
    summary->average_battery += packet->battery;

    if (packet->altitude > summary->max_altitude) {
        summary->max_altitude = packet->altitude;
    }

    if (packet->velocity > summary->max_velocity) {
        summary->max_velocity = packet->velocity;
    }

    if (packet->battery < summary->min_battery) {
        summary->min_battery = packet->battery;
    }
}

void finalize_summary(TelemetrySummary *summary) {
    if (summary == NULL || summary->total_count == 0) {
        return;
    }

    summary->average_altitude /= summary->total_count;
    summary->average_velocity /= summary->total_count;
    summary->average_battery /= summary->total_count;
}

void print_summary(const TelemetrySummary *summary) {
    if (summary == NULL) {
        return;
    }

    printf("\n========================================\n");
    printf(" Mission Summary\n");
    printf("========================================\n");
    printf("  Total packets      : %d\n", summary->total_count);
    printf("  Warning packets    : %d\n", summary->warning_count);
    printf("  Normal packets     : %d\n", summary->total_count - summary->warning_count);
    printf("  Average altitude   : %.2f m\n", summary->average_altitude);
    printf("  Average velocity   : %.2f m/s\n", summary->average_velocity);
    printf("  Average battery    : %.2f %%\n", summary->average_battery);
    printf("  Max altitude       : %.2f m\n", summary->max_altitude);
    printf("  Max velocity       : %.2f m/s\n", summary->max_velocity);
    printf("  Min battery        : %.2f %%\n", summary->min_battery);
}

void write_diagnostic_report(const char *path, const TelemetrySummary *summary) {
    if (path == NULL || summary == NULL) {
        return;
    }

    FILE *file = fopen(path, "w");

    if (file == NULL) {
        fprintf(stderr, "Warning: failed to write diagnostic report: %s\n", path);
        return;
    }

    fprintf(file, "Telemetry Packet Parser C - Diagnostic Report\n");
    fprintf(file, "=============================================\n\n");
    fprintf(file, "Total packets      : %d\n", summary->total_count);
    fprintf(file, "Warning packets    : %d\n", summary->warning_count);
    fprintf(file, "Normal packets     : %d\n", summary->total_count - summary->warning_count);
    fprintf(file, "Average altitude   : %.2f m\n", summary->average_altitude);
    fprintf(file, "Average velocity   : %.2f m/s\n", summary->average_velocity);
    fprintf(file, "Average battery    : %.2f %%\n", summary->average_battery);
    fprintf(file, "Max altitude       : %.2f m\n", summary->max_altitude);
    fprintf(file, "Max velocity       : %.2f m/s\n", summary->max_velocity);
    fprintf(file, "Min battery        : %.2f %%\n", summary->min_battery);

    fclose(file);
}

void append_warning_event(const char *path, const TelemetryPacket *packet) {
    if (path == NULL || packet == NULL) {
        return;
    }

    FILE *file = fopen(path, "a");

    if (file == NULL) {
        fprintf(stderr, "Warning: failed to write warning event log: %s\n", path);
        return;
    }

    int risk_score = calculate_risk_score(packet);

    fprintf(
        file,
        "%d,%.6f,%.6f,%.2f,%.2f,%.2f,%d,%d,%s\n",
        packet->packet_id,
        packet->latitude,
        packet->longitude,
        packet->altitude,
        packet->velocity,
        packet->battery,
        packet->signal_strength,
        risk_score,
        get_risk_level(risk_score)
    );

    fclose(file);
}
