#include "embedded_profile.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

int main(void) {
    EmbeddedSample s = {0};
    assert(embedded_parse_csv("1,12.1,34.2,88.0,1234\r\n", &s));
    assert(s.packet_id == 1 && s.recorded_crc == 1234);
    assert(embedded_anomaly_flags(&s) == 0);
    EmbeddedSummary summary;
    embedded_summary_init(&summary);
    assert(embedded_average_voltage(&summary) == 0.0);
    embedded_summary_add(&summary, &s);

    assert(embedded_parse_csv("3,9.7,41.0,82.5,1190", &s));
    assert(embedded_anomaly_flags(&s) == EMBEDDED_VOLTAGE_ANOMALY);
    embedded_summary_add(&summary, &s);

    assert(embedded_parse_csv("4,12.4,78.2,64.0,1299", &s));
    assert(embedded_anomaly_flags(&s) == EMBEDDED_TEMPERATURE_ANOMALY);
    embedded_summary_add(&summary, &s);

    assert(embedded_parse_csv("5,12.0,38.1,28.5,1301", &s));
    assert(embedded_anomaly_flags(&s) == EMBEDDED_SIGNAL_ANOMALY);
    embedded_summary_add(&summary, &s);

    assert(embedded_parse_csv("8,9.5,80.0,20.0,1333", &s));
    assert(embedded_anomaly_flags(&s) == 7u);
    embedded_summary_add(&summary, &s);
    assert(summary.total == 5 && summary.anomaly_packets == 4);
    assert(fabs(embedded_average_voltage(&summary) - 11.14) < 0.00001);
    assert(fabs(embedded_average_temperature(&summary) - 54.3) < 0.00001);

    s.packet_id = 123456;
    const char *invalid[] = {
       "", "5,12,34,70", "5,12,34,70,1,99", "5,12,nan,70,1",
       "5,12,inf,70,1", "5,12x,34,70,1", "5,12,34,70,1junk",
       "9999999999999,12,34,70,1", "5,12,34,70,1,",
       "5,12,34,70,2147483648"
    };
    for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
        assert(!embedded_parse_csv(invalid[i], &s));
        assert(s.packet_id == 123456);
    }
    puts("embedded telemetry research profile: passed");
    return 0;
}
