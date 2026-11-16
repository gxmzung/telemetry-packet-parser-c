#include "wire_protocol.h"
#include "packet_reader.h"
#include "packet_validator.h"
#include "crc16.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Fixed-seed, deterministic stress tests; not a substitute for libFuzzer
   or hardware tests. Both projects' *actual* C parsers are linked. */
static uint32_t rng = 0x6F3A91C5u;
static uint32_t next_random(void) {
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}
static const char hex_digits[] = "0123456789ABCDEF";

static void assert_equivalent(unsigned number) {
    const size_t bytes = (size_t)(next_random() % 129u);
    uint8_t payload[WIRE_MAX_PAYLOAD];
    char hex[WIRE_MAX_PAYLOAD * 2 + 1];
    for (size_t i = 0; i < bytes; ++i) {
        payload[i] = (uint8_t)next_random();
        hex[2*i] = hex_digits[(payload[i] >> 4) & 15u];
        hex[2*i+1] = hex_digits[payload[i] & 15u];
    }
    hex[2*bytes] = '\0';
    const uint8_t type = (uint8_t)next_random();
    unsigned int crc_other = 0;
    assert(calculate_crc16_decoded_payload(hex, &crc_other));
    assert(wire_crc16(payload, bytes, 0xFFFFu) == crc_other);

    char line[2 * WIRE_MAX_PAYLOAD + 40];
    int n = snprintf(line, sizeof line, "AA55,%02X,%zu,%s,%04X",
                     (unsigned)type, bytes, hex, crc_other);
    assert(n > 0 && (size_t)n < sizeof line);
    WirePacket left = {0};
    BinaryPacket right = {0};
    assert(wire_parse_record(line, &left));
    assert(parse_packet_record(line, &right));
    assert(left.length == bytes && right.length == (int)bytes);
    assert(left.type == type && right.type == (int)type);
    assert(memcmp(left.payload, payload, bytes) == 0);
    assert(validate_packet_crc_decoded(&right));
    assert(validate_payload_length(&right));

    /* Bit-flipped CRC must be detected by both parsers / validators. */
    right.crc ^= 1u;
    assert(!validate_packet_crc_decoded(&right));
    line[n-1] = line[n-1] == '0' ? '1' : '0';
    assert(!wire_parse_record(line, &left));
    assert(left.length == bytes); /* rejected input does not mutate output */

    /* Truncation at a valid field boundary fails on both. */
    char *comma = strrchr(line, ',');
    assert(comma != NULL);
    *comma = '\0';
    assert(!wire_parse_record(line, &left));
    assert(!parse_packet_record(line, &right));
    (void)number;
}

static void check_malformed_inputs(void) {
    const char *cases[] = {
        "", "AA55", "AA55,01", "AA55,01,4",
        "AA55,01,4,AB", "AA55,01,4,ABCDEFGH",
        "AA55,01,4,ABCDEFGH,0000,EXTRA",
        "AA55,01,-1,,FFFF", "AA55,01,129,,FFFF",
        "AA55,100,1,00,FFFF", "AA55,GG,1,00,FFFF",
        "AA55,01,0000,,FFFF", "AA55,01,1,GG,FFFF"
    };
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i) {
        WirePacket a = {.length = 77};
        BinaryPacket b = {.length = 77};
        assert(!wire_parse_record(cases[i], &a));
        /* Older inspector distinguishes parsing from semantic validation;
           either rejects input or reports failure in full decoded validation. */
        if (parse_packet_record(cases[i], &b)) {
            assert(!validate_packet_crc_decoded(&b));
        } else {
            assert(b.length == 77);
        }
        assert(a.length == 77);
    }
}

int main(void) {
    check_malformed_inputs();
    for (unsigned i = 0; i < 10000u; ++i) assert_equivalent(i);
    puts("deterministic cross-repo stress: 10000 generated packets passed");
    return 0;
}
