CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = telemetry_parser
BIN_WRITER = binary_writer

SRC = src/main.c src/telemetry.c
WRITER_SRC = src/binary_writer.c src/telemetry.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

writer:
	$(CC) $(CFLAGS) $(WRITER_SRC) -o $(BIN_WRITER)

generate-bin: writer
	./$(BIN_WRITER)

run: all
	./$(TARGET)

run-bin: all generate-bin
	./$(TARGET) data/sample_packets.bin

clean:
	rm -f $(TARGET) $(BIN_WRITER)
	rm -f data/sample_packets.bin
	rm -f logs/*.txt
