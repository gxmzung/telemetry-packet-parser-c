#ifndef DIAGNOSTIC_ENGINE_H
#define DIAGNOSTIC_ENGINE_H

#include "uart_log_parser.h"

typedef struct {
    int info_count;
    int warn_count;
    int error_count;
    int sensor_count;
    int low_voltage_count;
    int high_temperature_count;
    int low_signal_count;
} DiagnosticSummary;

void init_diagnostic_summary(DiagnosticSummary *summary);
void update_diagnostic_summary(DiagnosticSummary *summary, const UartLogEntry *entry);
int is_device_healthy(const DiagnosticSummary *summary);

#endif