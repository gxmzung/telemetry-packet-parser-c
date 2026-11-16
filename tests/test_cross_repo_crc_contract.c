#include "wire_protocol.h"
#include "packet_reader.h"
#include "crc16.h"
#include "packet_validator.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* CRC-16/MODBUS over decoded payload bytes; both repositories agree over
   the full 0-128-byte range. No legacy ASCII-CRC mode switch is implied. */
static void verify_sample(const char *hex, uint8_t type) {
    uint8_t data[WIRE_MAX_PAYLOAD] = {0};
    size_t size = 0;
    unsigned int inspector_crc = 0;
    assert(wire_hex_decode(hex, data, sizeof data, &size));
    assert(calculate_crc16_decoded_payload(hex, &inspector_crc));
    assert((unsigned int)wire_crc16(data, size, 0xFFFFu) == inspector_crc);

    char record[2 * WIRE_MAX_PAYLOAD + 40];
    int written = snprintf(record, sizeof record, "AA55,%02X,%zu,%s,%04X",
                           (unsigned)type, size, hex, inspector_crc);
    assert(written > 0 && (size_t)written < sizeof record);

    WirePacket wire = {0};
    BinaryPacket inspector = {0};
    assert(wire_parse_record(record, &wire));
    assert(parse_packet_record(record, &inspector));
    assert(wire.length == size && wire.type == type && wire.crc == inspector_crc);
    assert(inspector.length == (int)size && inspector.type == type);
    assert(memcmp(wire.payload, data, size) == 0);
    assert(strcmp(inspector.payload, hex) == 0);
    assert(validate_payload_length(&inspector));
    assert(validate_packet_crc_decoded(&inspector));

    inspector.crc ^= 1u;
    assert(!validate_packet_crc_decoded(&inspector));

    /* Failure must not mutate a previously accepted packet. */
    record[written-1] = (record[written-1] == '0') ? '1' : '0';
    assert(!wire_parse_record(record, &wire));
    assert(wire.length == size);
}

static void verify_generated(size_t n) {
    char hex[2 * WIRE_MAX_PAYLOAD + 1];
    static const char alphabet[] = "0123456789ABCDEF";
    for (size_t i = 0; i < n; ++i) {
        hex[2*i] = alphabet[i % 16u];
        hex[2*i + 1] = alphabet[(i + 7u) % 16u];
    }
    hex[2*n] = '\0';
    verify_sample(hex, 0x05);
}

int main(void) {
    verify_sample("313233343536373839", 1);
    verify_sample("120A33FF", 2);
    verify_sample("00010203AABBCCDD", 4);
    verify_sample("deadbeef", 5);
    verify_generated(0);
    verify_generated(1);
    verify_generated(63);
    verify_generated(64);
    verify_generated(127);
    verify_generated(128);

    /* The original legacy ASCII-hex CRC remains deliberately different. */
    unsigned int old_crc = calculate_crc16_hex_payload("120A33FF");
    unsigned int new_crc = 0;
    assert(calculate_crc16_decoded_payload("120A33FF", &new_crc));
    assert(old_crc != new_crc);

    unsigned int unchanged = 0x1234u;
    assert(!calculate_crc16_decoded_payload("GG", &unchanged));
    assert(unchanged == 0x1234u);
    puts("cross-repository full 0-128-byte decoded CRC interoperability: passed");
    return 0;
}
