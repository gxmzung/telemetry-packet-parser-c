#include <stdio.h>
#include "uart_log_parser.h"
#include "diagnostic_engine.h"
#include "device_report.h"

int main(void) {
    FILE *file = fopen("data/sample_uart.log", "r");

    if (file == NULL) {
        printf("Failed to open UART log file.\n");
        return 1;
    }

    char line[256];
    UartLogEntry entry;
    DiagnosticSummary summary;

    init_diagnostic_summary(&summary);

    while (fgets(line, sizeof(line), file)) {
        if (parse_uart_log_line(line, &entry)) {
            printf("[%s] %s\n", log_level_to_string(entry.level), entry.event);
            update_diagnostic_summary(&summary, &entry);
        }
    }

    fclose(file);

    print_diagnostic_report(&summary);

    return 0;
}