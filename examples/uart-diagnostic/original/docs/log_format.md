# UART Log Format

## Supported Log Types

```text
[INFO] message
[WARN] message
[ERROR] message
[SENSOR] voltage=12.1 temperature=36.4 signal=88

Diagnostic Rules
Rule	Condition
Low voltage	voltage < 10.5
High temperature	temperature > 75.0
Low signal	signal < 35.0
Error event	Any [ERROR] log line
Output

The CLI prints parsed log events and a final device diagnostic report.