#include <stdio.h>
#include <string.h>
#include "uart_log_parser.h"

LogLevel detect_log_level(const char *line) {
    if (strstr(line, "[INFO]") != NULL) {
        return LOG_INFO;
    }

    if (strstr(line, "[WARN]") != NULL) {
        return LOG_WARN;
    }

    if (strstr(line, "[ERROR]") != NULL) {
        return LOG_ERROR;
    }

    if (strstr(line, "[SENSOR]") != NULL) {
        return LOG_SENSOR;
    }

    return LOG_UNKNOWN;
}

const char *log_level_to_string(LogLevel level) {
    switch (level) {
        case LOG_INFO:
            return "INFO";
        case LOG_WARN:
            return "WARN";
        case LOG_ERROR:
            return "ERROR";
        case LOG_SENSOR:
            return "SENSOR";
        default:
            return "UNKNOWN";
    }
}

int parse_uart_log_line(const char *line, UartLogEntry *entry) {
    if (line == NULL || entry == NULL) {
        return 0;
    }

    entry->level = detect_log_level(line);
    entry->voltage = -1.0;
    entry->temperature = -1.0;
    entry->signal = -1.0;

    if (entry->level == LOG_SENSOR) {
        sscanf(
            line,
            "[SENSOR] voltage=%lf temperature=%lf signal=%lf",
            &entry->voltage,
            &entry->temperature,
            &entry->signal
        );

        strcpy(entry->event, "SENSOR_SAMPLE");
        return 1;
    }

    if (entry->level == LOG_INFO) {
        strcpy(entry->event, "INFO_EVENT");
        return 1;
    }

    if (entry->level == LOG_WARN) {
        strcpy(entry->event, "WARN_EVENT");
        return 1;
    }

    if (entry->level == LOG_ERROR) {
        strcpy(entry->event, "ERROR_EVENT");
        return 1;
    }

    strcpy(entry->event, "UNKNOWN_EVENT");
    return 0;
}