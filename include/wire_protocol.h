#ifndef WIRE_PROTOCOL_H
#define WIRE_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

/* Independent protocol module: no dependency on legacy TelemetryPacket. */
#define WIRE_MAX_PAYLOAD 128

typedef struct {
    uint8_t type;
    size_t length;
    uint8_t payload[WIRE_MAX_PAYLOAD];
    uint16_t crc;
} WirePacket;

/* CRC-16/ARC: init=0x0000, poly reflected=0xA001, xorout=0x0000.
   CRC-16/MODBUS: init=0xFFFF, poly reflected=0xA001, xorout=0x0000.
   Test check: ASCII "123456789" -> ARC 0xBB3D, MODBUS 0x4B37. */
uint16_t wire_crc16(const uint8_t *bytes, size_t length, uint16_t initial);

/* Decode hex into raw bytes; 1 success, 0 invalid or out of capacity. */
int wire_hex_decode(const char *hex, uint8_t *out, size_t capacity, size_t *out_length);

/* Parse ASCII record AA55,TYPE_HEX,LENGTH_DEC,PAYLOAD_HEX,CRC_HEX.
   CRC is computed over the decoded payload bytes only, using MODBUS init.
   Returns 1 on success; 0 on any malformed frame, bounds or CRC error.
   Empty payload is permitted only for declared length 0. */
int wire_parse_record(const char *line, WirePacket *out);

#endif
