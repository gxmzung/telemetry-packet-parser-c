#include "packet_parser.h"
#include "anomaly_detector.h"
#include "crc16.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    TelemetryPacket item = {0};
    assert(parse_packet_line("7,12.0,25.0,70.0,123", &item));
    assert(item.packet_id == 7);
    assert(item.voltage == 12.0 && item.temperature == 25.0);
    assert(count_packet_anomalies(&item) == 0);
    assert(parse_packet_line("8,9.5,80.0,20.0,123", &item));
    assert(count_packet_anomalies(&item) == 3);
    assert(!parse_packet_line(NULL, &item));

    /* The original CRC implementation uses init FFFF and ASCII bytes. */
    assert(calculate_crc16("123456789") == 0x4B37);
    assert(validate_crc("123456789", 0x4B37));
    assert(!validate_crc("123456789", 0x0000));
    puts("legacy embedded telemetry: regression passed");
    return 0;
}
