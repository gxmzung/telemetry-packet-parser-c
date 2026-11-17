#include "diagnostic_engine.h"

void init_diagnostic_summary(DiagnosticSummary *summary) {
    summary->info_count = 0;
    summary->warn_count = 0;
    summary->error_count = 0;
    summary->sensor_count = 0;
    summary->low_voltage_count = 0;
    summary->high_temperature_count = 0;
    summary->low_signal_count = 0;
}

void update_diagnostic_summary(DiagnosticSummary *summary, const UartLogEntry *entry) {
    if (entry->level == LOG_INFO) {
        summary->info_count++;
    }

    if (entry->level == LOG_WARN) {
        summary->warn_count++;
    }

    if (entry->level == LOG_ERROR) {
        summary->error_count++;
    }

    if (entry->level == LOG_SENSOR) {
        summary->sensor_count++;

        if (entry->voltage >= 0 && entry->voltage < 10.5) {
            summary->low_voltage_count++;
        }

        if (entry->temperature > 75.0) {
            summary->high_temperature_count++;
        }

        if (entry->signal >= 0 && entry->signal < 35.0) {
            summary->low_signal_count++;
        }
    }
}

int is_device_healthy(const DiagnosticSummary *summary) {
    if (summary->error_count > 0) {
        return 0;
    }

    if (summary->low_voltage_count > 0) {
        return 0;
    }

    if (summary->high_temperature_count > 0) {
        return 0;
    }

    if (summary->low_signal_count > 1) {
        return 0;
    }

    return 1;
}