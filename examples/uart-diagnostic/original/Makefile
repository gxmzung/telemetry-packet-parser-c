CC=gcc
CFLAGS=-Wall -Wextra -Iinclude

SRC=src/main.c src/uart_log_parser.c src/diagnostic_engine.c src/device_report.c
OUT=uart_diagnostic_cli

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)