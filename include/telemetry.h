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

typedef struct {
    int total_count;
    int warning_count;
    double average_altitude;
    double average_velocity;
    double average_battery;
    double max_altitude;
    double max_velocity;
    double min_battery;
} TelemetrySummary;

int parse_telemetry_line(const char *line, TelemetryPacket *packet);
void print_packet(const TelemetryPacket *packet);
int is_packet_warning(const TelemetryPacket *packet);
int calculate_risk_score(const TelemetryPacket *packet);
void print_diagnostic(const TelemetryPacket *packet);

void init_summary(TelemetrySummary *summary);
void update_summary(TelemetrySummary *summary, const TelemetryPacket *packet);
void finalize_summary(TelemetrySummary *summary);
void print_summary(const TelemetrySummary *summary);
void write_diagnostic_report(const char *path, const TelemetrySummary *summary);

#endif
