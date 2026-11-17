# UART Diagnostic CLI C

C-based UART log diagnostic command-line tool for embedded and field operation systems.

This project parses UART-style device logs, detects warning/error events, summarizes sensor states, and generates a simple diagnostic report.

## Goals

- Parse UART-style text logs
- Detect INFO / WARN / ERROR events
- Track sensor values from device messages
- Summarize device health
- Practice C-based embedded diagnostic logic

## Example Log

```text
[INFO] BOOT_OK device=EDGE-01
[SENSOR] voltage=12.1 temperature=36.4 signal=88
[WARN] LOW_SIGNAL signal=31
[ERROR] SENSOR_TIMEOUT channel=imu

Project Structure
src/
  main.c
  uart_log_parser.c
  diagnostic_engine.c
  device_report.c

include/
  uart_log_parser.h
  diagnostic_engine.h
  device_report.h

data/
  sample_uart.log

docs/
  log_format.md
  roadmap.md
Target Position

Embedded systems, UART diagnostics, telemetry monitoring, field operation tools, and mission device debugging.

Tech Stack
C
CLI
UART-style logs
Embedded diagnostic rules