#ifndef UART_LOG_PARSER_H
#define UART_LOG_PARSER_H

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_SENSOR,
    LOG_UNKNOWN
} LogLevel;

typedef struct {
    LogLevel level;
    char event[64];
    double voltage;
    double temperature;
    double signal;
} UartLogEntry;

LogLevel detect_log_level(const char *line);
int parse_uart_log_line(const char *line, UartLogEntry *entry);
const char *log_level_to_string(LogLevel level);

#endif