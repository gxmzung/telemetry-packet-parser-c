CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = telemetry_parser
BIN_WRITER = binary_writer
UDP_RECEIVER = udp_receiver
UDP_SENDER = udp_sender

SRC = src/main.c src/telemetry.c
WRITER_SRC = src/binary_writer.c src/telemetry.c
RECEIVER_SRC = src/udp_receiver.c src/telemetry.c
SENDER_SRC = src/udp_sender.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

writer:
	$(CC) $(CFLAGS) $(WRITER_SRC) -o $(BIN_WRITER)

udp-receiver:
	$(CC) $(CFLAGS) $(RECEIVER_SRC) -o $(UDP_RECEIVER)

udp-sender:
	$(CC) $(CFLAGS) $(SENDER_SRC) -o $(UDP_SENDER)

generate-bin: writer
	./$(BIN_WRITER)

run: all
	./$(TARGET)

run-bin: all generate-bin
	./$(TARGET) data/sample_packets.bin

run-receiver: udp-receiver
	./$(UDP_RECEIVER)

run-sender: udp-sender
	./$(UDP_SENDER)

clean:
	rm -f $(TARGET) $(BIN_WRITER) $(UDP_RECEIVER) $(UDP_SENDER)
	rm -f data/sample_packets.bin
	rm -f logs/*.txt
