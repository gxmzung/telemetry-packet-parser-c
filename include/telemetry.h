#ifndef TELEMETRY_H
#define TELEMETRY_H

#define MAX_LINE_LENGTH 256

typedef struct {
    int packet_id;
    double latitude;
    double longitude;
    double altitude;
    double velocity;
    double battery;
    int signal_strength;
} TelemetryPacket;

int parse_telemetry_line(const char *line, TelemetryPacket *packet);
void print_packet(const TelemetryPacket *packet);
int is_packet_warning(const TelemetryPacket *packet);
void print_diagnostic(const TelemetryPacket *packet);

#endif
