CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
TARGET = telemetry_parser

SRC = src/main.c src/telemetry.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
