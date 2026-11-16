#include "embedded_profile.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Parse exactly five CSV scalar fields, with optional CRLF, and reject
   partial numeric tokens, NAN/Inf, extra columns, and out-of-range integers. */
int embedded_parse_csv(const char *line, EmbeddedSample *out) {
    if (!line || !out) return 0;
    size_t n = 0;
    while (n < 256 && line[n]) ++n;
    if (n == 256) return 0;
    if (n && line[n - 1] == '\n') --n;
    if (n && line[n - 1] == '\r') --n;

    size_t starts[5] = {0}, widths[5] = {0};
    size_t part = 0, begin = 0;
    for (size_t i = 0; i <= n; ++i) {
        if (i != n && line[i] != ',') continue;
        if (part >= 5 || i == begin || i - begin >= 64) return 0;
        starts[part] = begin;
        widths[part] = i - begin;
        ++part;
        begin = i + 1;
    }
    if (part != 5) return 0;

    double parsed[5];
    for (size_t i = 0; i < 5; ++i) {
        char token[64] = {0};
        memcpy(token, line + starts[i], widths[i]);
        for (size_t j = 0; j < widths[i]; ++j) {
            if (isspace((unsigned char)token[j])) return 0;
        }
        errno = 0;
        char *end = NULL;
        if (i == 0 || i == 4) {
            long value = strtol(token, &end, 10);
            if (errno || end == token || *end || value < INT_MIN || value > INT_MAX) return 0;
            parsed[i] = (double)value;
        } else {
            double value = strtod(token, &end);
            if (errno || end == token || *end || !isfinite(value)) return 0;
            parsed[i] = value;
        }
    }
    EmbeddedSample verified = {
        .packet_id = (int)parsed[0],
        .voltage = parsed[1],
        .temperature = parsed[2],
        .signal_strength = parsed[3],
        .recorded_crc = (int)parsed[4],
    };
    *out = verified;
    return 1;
}

unsigned embedded_anomaly_flags(const EmbeddedSample *sample) {
    if (!sample) return 0;
    unsigned flags = 0;
    if (sample->voltage < 10.5 || sample->voltage > 13.0)
        flags |= EMBEDDED_VOLTAGE_ANOMALY;
    if (sample->temperature < -10.0 || sample->temperature > 75.0)
        flags |= EMBEDDED_TEMPERATURE_ANOMALY;
    if (sample->signal_strength < 35.0)
        flags |= EMBEDDED_SIGNAL_ANOMALY;
    return flags;
}

void embedded_summary_init(EmbeddedSummary *summary) {
    if (!summary) return;
    memset(summary, 0, sizeof *summary);
}

void embedded_summary_add(EmbeddedSummary *summary, const EmbeddedSample *sample) {
    if (!summary || !sample) return;
    ++summary->total;
    if (embedded_anomaly_flags(sample)) ++summary->anomaly_packets;
    summary->voltage_sum += sample->voltage;
    summary->temperature_sum += sample->temperature;
}

double embedded_average_voltage(const EmbeddedSummary *summary) {
    return summary && summary->total ? summary->voltage_sum / (double)summary->total : 0.0;
}
double embedded_average_temperature(const EmbeddedSummary *summary) {
    return summary && summary->total ? summary->temperature_sum / (double)summary->total : 0.0;
}
