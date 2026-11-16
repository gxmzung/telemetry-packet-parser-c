#include "packet_reader.h"
#include "packet_validator.h"
#include "packet_classifier.h"
#include "hex_utils.h"
#include "crc16.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    BinaryPacket frame = {0};
    unsigned int ascii_crc = calculate_crc16_hex_payload("120A33FF");
    char record[100];
    snprintf(record, sizeof(record), "AA55,01,4,120A33FF,%04X", ascii_crc);

    assert(parse_packet_record(record, &frame));
    assert(validate_sync_word(&frame));
    assert(validate_payload_length(&frame));
    assert(validate_packet_crc(&frame));
    assert(count_packet_errors(&frame) == 0);
    assert(hex_payload_byte_length("120A33FF") == 4);
    assert(!is_valid_hex_string("120Z"));
    assert(!validate_payload_length(&(BinaryPacket){.length=3, .payload="120A33FF"}));
    assert(classify_packet_type(0x01)[0] == 'H');
    assert(is_critical_packet_type(0x04));
    assert(!is_critical_packet_type(0x01));
    puts("legacy binary packet inspector: regression passed");
    return 0;
}
