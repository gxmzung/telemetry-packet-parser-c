#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdio.h>
#include <stddef.h>

#define MAX_LINE_LENGTH 256
#define TELEMETRY_MAGIC 0x54504B54

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
    unsigned int magic;
    int packet_id;
    double latitude;
    double longitude;
    double altitude;
    double velocity;
    double battery;
    int signal_strength;
} BinaryTelemetryPacket;

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
const char *get_risk_level(int risk_score);
void print_diagnostic(const TelemetryPacket *packet);

void init_summary(TelemetrySummary *summary);
void update_summary(TelemetrySummary *summary, const TelemetryPacket *packet);
void finalize_summary(TelemetrySummary *summary);
void print_summary(const TelemetrySummary *summary);
void write_diagnostic_report(const char *path, const TelemetrySummary *summary);

int read_binary_packet(FILE *file, TelemetryPacket *packet);
void convert_binary_to_telemetry(const BinaryTelemetryPacket *binary, TelemetryPacket *packet);

void append_warning_event(const char *path, const TelemetryPacket *packet);

#endif
