CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = telemetry_parser
BIN_WRITER = binary_writer
UDP_RECEIVER = udp_receiver
UDP_SENDER = udp_sender
UDP_SENDER_ANOMALY = udp_sender_anomaly

SRC = src/main.c src/telemetry.c
WRITER_SRC = src/binary_writer.c src/telemetry.c
RECEIVER_SRC = src/udp_receiver.c src/telemetry.c
SENDER_SRC = src/udp_sender.c
SENDER_ANOMALY_SRC = src/udp_sender_anomaly.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

writer:
	$(CC) $(CFLAGS) $(WRITER_SRC) -o $(BIN_WRITER)

udp-receiver:
	$(CC) $(CFLAGS) $(RECEIVER_SRC) -o $(UDP_RECEIVER)

udp-sender:
	$(CC) $(CFLAGS) $(SENDER_SRC) -o $(UDP_SENDER)

udp-sender-anomaly:
	$(CC) $(CFLAGS) $(SENDER_ANOMALY_SRC) -o $(UDP_SENDER_ANOMALY)

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

run-sender-anomaly: udp-sender-anomaly
	./$(UDP_SENDER_ANOMALY)

test: clean run run-bin

clean:
	rm -f $(TARGET) $(BIN_WRITER) $(UDP_RECEIVER) $(UDP_SENDER) $(UDP_SENDER_ANOMALY)
	rm -f data/sample_packets.bin
	rm -f logs/*.txt
	rm -f logs/*.csv
	rm -f logs/*.json
