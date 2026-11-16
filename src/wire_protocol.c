#include "wire_protocol.h"

#include <limits.h>
#include <string.h>

static int hex_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

uint16_t wire_crc16(const uint8_t *bytes, size_t length, uint16_t initial) {
    uint16_t crc = initial;
    if (bytes == NULL && length != 0) return crc; /* caller validates buffer */
    for (size_t i = 0; i < length; ++i) {
        crc ^= bytes[i];
        for (unsigned bit = 0; bit < 8; ++bit) {
            crc = (crc & 1u) ? (uint16_t)((crc >> 1) ^ 0xA001u)
                             : (uint16_t)(crc >> 1);
        }
    }
    return crc;
}

int wire_hex_decode(const char *hex, uint8_t *out, size_t capacity, size_t *out_length) {
    if (!hex || !out_length) return 0;
    size_t n = strlen(hex);
    if ((n & 1u) || n / 2 > capacity || (n != 0 && !out)) return 0;
    for (size_t i = 0; i < n / 2; ++i) {
        int hi = hex_digit(hex[2 * i]), lo = hex_digit(hex[2 * i + 1]);
        if (hi < 0 || lo < 0) return 0;
        out[i] = (uint8_t)((hi << 4) | lo);
    }
    *out_length = n / 2;
    return 1;
}

static int parse_hex_u16(const char *s, size_t n, uint16_t *value) {
    if (!s || !value || n == 0 || n > 4) return 0;
    unsigned result = 0;
    for (size_t i = 0; i < n; ++i) {
        int d = hex_digit(s[i]);
        if (d < 0) return 0;
        result = result * 16u + (unsigned)d;
    }
    *value = (uint16_t)result;
    return 1;
}

int wire_parse_record(const char *line, WirePacket *out) {
    if (!line || !out) return 0;
    /* Fixed-size input cap prevents scanning attacker-controlled unbounded text. */
    size_t n = 0;
    while (n < 2 * WIRE_MAX_PAYLOAD + 40 && line[n]) ++n;
    if (n == 2 * WIRE_MAX_PAYLOAD + 40) return 0;
    if (n && line[n - 1] == '\n') --n;
    if (n && line[n - 1] == '\r') --n;

    const char *fields[5];
    size_t lens[5];
    size_t start = 0, part = 0;
    for (size_t i = 0; i <= n; ++i) {
        if (i != n && line[i] != ',') continue;
        if (part >= 5) return 0;
        fields[part] = line + start;
        lens[part] = i - start;
        ++part;
        start = i + 1;
    }
    if (part != 5 || lens[0] != 4 || strncmp(fields[0], "AA55", 4) != 0 ||
        lens[1] != 2 || lens[2] == 0 || lens[2] > 3 || lens[4] != 4) return 0;

    uint16_t type = 0, crc = 0;
    if (!parse_hex_u16(fields[1], lens[1], &type) ||
        !parse_hex_u16(fields[4], lens[4], &crc)) return 0;

    size_t declared = 0;
    for (size_t i = 0; i < lens[2]; ++i) {
        char d = fields[2][i];
        if (d < '0' || d > '9') return 0;
        declared = declared * 10 + (size_t)(d - '0');
    }
    if (declared > WIRE_MAX_PAYLOAD || lens[3] != declared * 2) return 0;
    uint8_t payload[WIRE_MAX_PAYLOAD];
    for (size_t i = 0; i < declared; ++i) {
        int hi = hex_digit(fields[3][2 * i]);
        int lo = hex_digit(fields[3][2 * i + 1]);
        if (hi < 0 || lo < 0) return 0;
        payload[i] = (uint8_t)((hi << 4) | lo);
    }
    if (wire_crc16(payload, declared, 0xFFFFu) != crc) return 0;

    WirePacket verified = {0};
    verified.type = (uint8_t)type;
    verified.length = declared;
    verified.crc = crc;
    memcpy(verified.payload, payload, declared);
    *out = verified;
    return 1;
}
