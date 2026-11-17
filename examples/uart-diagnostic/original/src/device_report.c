#include <stdio.h>
#include "device_report.h"

void print_diagnostic_report(const DiagnosticSummary *summary) {
    printf("\n=== UART Diagnostic Report ===\n");
    printf("INFO events: %d\n", summary->info_count);
    printf("WARN events: %d\n", summary->warn_count);
    printf("ERROR events: %d\n", summary->error_count);
    printf("Sensor samples: %d\n", summary->sensor_count);
    printf("Low voltage events: %d\n", summary->low_voltage_count);
    printf("High temperature events: %d\n", summary->high_temperature_count);
    printf("Low signal events: %d\n", summary->low_signal_count);

    if (is_device_healthy(summary)) {
        printf("Device status: HEALTHY\n");
    } else {
        printf("Device status: NEEDS ATTENTION\n");
    }
}