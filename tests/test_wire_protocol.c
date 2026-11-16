#include "wire_protocol.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const uint8_t check[] = "123456789";
    assert(wire_crc16(check, 9, 0xFFFFu) == 0x4B37u);
    assert(wire_crc16(check, 9, 0x0000u) == 0xBB3Du);
    assert(wire_crc16(NULL, 0, 0xFFFFu) == 0xFFFFu);

    uint8_t bytes[4] = {0};
    size_t length = 99;
    assert(wire_hex_decode("120A33FF", bytes, sizeof bytes, &length));
    assert(length == 4 && bytes[0] == 0x12 && bytes[1] == 0x0A &&
           bytes[2] == 0x33 && bytes[3] == 0xFF);
    assert(!wire_hex_decode("123", bytes, sizeof bytes, &length));
    assert(!wire_hex_decode("12ZZ", bytes, sizeof bytes, &length));
    assert(!wire_hex_decode("1234567890", bytes, sizeof bytes, &length));

    /* CRC is calculated on binary payload bytes, not ASCII hex characters. */
    const uint8_t data[] = {0x12, 0x0A, 0x33, 0xFF};
    char record[128];
    snprintf(record, sizeof record, "AA55,01,4,120A33FF,%04X",
             wire_crc16(data, sizeof data, 0xFFFFu));
    WirePacket pkt = {0};
    assert(wire_parse_record(record, &pkt));
    assert(pkt.type == 1 && pkt.length == 4 && pkt.payload[3] == 0xFF);
    assert(!wire_parse_record("AA55,01,4,120A33FF,0000", &pkt));
    assert(!wire_parse_record("AA55,01,3,120A33FF,0000", &pkt));
    assert(!wire_parse_record("AB55,01,4,120A33FF,0000", &pkt));
    assert(!wire_parse_record("AA55,GG,4,120A33FF,0000", &pkt));
    assert(!wire_parse_record("AA55,01,4,120Z33FF,0000", &pkt));
    assert(!wire_parse_record("AA55,01,4,120A33FF,0000,extra", &pkt));
    assert(!wire_parse_record("AA55,01,999,120A33FF,0000", &pkt));
    assert(!wire_parse_record(NULL, &pkt));
    assert(!wire_parse_record(record, NULL));

    /* Rejected records must not overwrite previously verified output. */
    assert(pkt.length == 4);
    puts("wire_protocol: all assertions passed");
    return 0;
}
