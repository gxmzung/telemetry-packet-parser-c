#include "wire_protocol.h"
#include <assert.h>
#include <stdio.h>

/* The original binary inspector hashes ASCII hex characters.
   This independent function captures its algorithm from pinned original source.
   It is intentionally NOT equivalent to decoding hex then hashing raw bytes. */
static unsigned legacy_ascii_hex_crc(const char *text) {
    unsigned crc = 0xFFFFu;
    while (*text) {
        crc ^= (unsigned char)*text++;
        for (int bit=0; bit<8; ++bit)
            crc = (crc & 1u) ? ((crc >> 1) ^ 0xA001u) : (crc >> 1);
    }
    return crc & 0xFFFFu;
}

int main(void) {
    const char *hex = "120A33FF";
    uint8_t bytes[4];
    size_t count = 0;
    assert(wire_hex_decode(hex, bytes, sizeof bytes, &count));
    assert(count == 4);
    unsigned old_crc = legacy_ascii_hex_crc(hex);
    unsigned new_crc = wire_crc16(bytes, count, 0xFFFF);
    assert(old_crc != new_crc);

    char frame[80];
    WirePacket output = {0};
    snprintf(frame, sizeof(frame), "AA55,01,4,%s,%04X", hex, new_crc);
    assert(wire_parse_record(frame, &output));
    snprintf(frame, sizeof(frame), "AA55,01,4,%s,%04X", hex, old_crc);
    assert(!wire_parse_record(frame, &output));
    assert(output.length == 4); /* failed validation must preserve output */
    printf("legacy ASCII CRC=0x%04X, decoded-byte CRC=0x%04X (intentionally incompatible)\n", old_crc, new_crc);
    return 0;
}
