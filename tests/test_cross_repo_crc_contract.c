#include "wire_protocol.h"
#include "crc16.h"
#include "packet_validator.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Both implementations must agree on the explicitly selected byte-decoded
   CRC-16/MODBUS contract. Legacy ASCII-hex CRC remains intentionally different. */
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
    assert(wire_parse_record(record, &wire));
    assert(wire.length == size && wire.type == type && wire.crc == inspector_crc);
    assert(memcmp(wire.payload, data, size) == 0);

    if (size > 0) {
        /* The legacy inspector has a limited 127-character payload buffer.
           Exercise its opt-in validator on the overlapping supported range. */
        if (strlen(hex) < 128) {
            BinaryPacket legacy = {0};
            strcpy(legacy.sync, "AA55");
            legacy.type = type;
            legacy.length = (int)size;
            strcpy(legacy.payload, hex);
            legacy.crc = inspector_crc;
            assert(validate_packet_crc_decoded(&legacy));
            legacy.crc ^= 1u;
            assert(!validate_packet_crc_decoded(&legacy));
        }
    }

    record[written - 1] = (record[written - 1] == '0') ? '1' : '0';
    assert(!wire_parse_record(record, &wire));
}

int main(void) {
    verify_sample("313233343536373839", 1); /* MODBUS check vector 4B37 */
    verify_sample("120A33FF", 2);
    verify_sample("00010203AABBCCDD", 4);
    verify_sample("deadbeef", 5);
    verify_sample("00", 0);
    verify_sample("FF", 0xFF);

    unsigned int old_crc = calculate_crc16_hex_payload("120A33FF");
    unsigned int new_crc = 0;
    assert(calculate_crc16_decoded_payload("120A33FF", &new_crc));
    assert(old_crc != new_crc);

    /* A failed decoded-hex parse must not overwrite the caller's CRC. */
    unsigned int untouched = 0x1234u;
    assert(!calculate_crc16_decoded_payload("GG", &untouched));
    assert(untouched == 0x1234u);

    puts("two-repository decoded CRC interoperability: passed");
    return 0;
}
