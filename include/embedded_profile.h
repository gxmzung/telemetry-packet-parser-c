#ifndef EMBEDDED_PROFILE_H
#define EMBEDDED_PROFILE_H

#include <stddef.h>

/* Independent research CSV profile adapted from embedded-telemetry-lab-c.
   Kept distinct from GPS/flight TelemetryPacket to avoid schema collisions.
   Input CRC is a recorded value only; provenance of the protected bytes is unknown. */
typedef struct {
    int packet_id;
    double voltage;
    double temperature;
    double signal_strength;
    int recorded_crc;
} EmbeddedSample;

typedef struct {
    size_t total;
    size_t anomaly_packets;
    double voltage_sum;
    double temperature_sum;
} EmbeddedSummary;

enum {
    EMBEDDED_VOLTAGE_ANOMALY = 1,
    EMBEDDED_TEMPERATURE_ANOMALY = 2,
    EMBEDDED_SIGNAL_ANOMALY = 4
};

int embedded_parse_csv(const char *line, EmbeddedSample *out);
unsigned embedded_anomaly_flags(const EmbeddedSample *sample);
void embedded_summary_init(EmbeddedSummary *summary);
void embedded_summary_add(EmbeddedSummary *summary, const EmbeddedSample *sample);
double embedded_average_voltage(const EmbeddedSummary *summary);
double embedded_average_temperature(const EmbeddedSummary *summary);

#endif
