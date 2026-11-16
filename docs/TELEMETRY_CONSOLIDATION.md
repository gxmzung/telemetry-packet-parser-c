# C 텔레메트리 저장소 통합 검토 — 2026-11-16

## 목적과 경계

- 통합 후보: `embedded-telemetry-lab-c`, `binary-packet-inspector-c`.
- 기준 저장소: `telemetry-packet-parser-c`.
- 기존 세 저장소의 소스 및 Git 이력은 수정하거나 삭제하지 않는다.
- 먼저 **독립** `wire_protocol` 모듈을 추가하여 기존 CSV/UDP/원시 C 구조체 파일 형식에 영향을 주지 않는다.

## 확인된 차이점

| 원본 | 패킷 구성 | CRC 처리 |
| --- | --- | --- |
| telemetry-packet-parser-c | packet_id, lat/lon, altitude, velocity, battery, signal_strength; raw C struct binary | legacy raw struct magic 검사만 존재; wire CRC 없음 |
| embedded-telemetry-lab-c | packet_id, voltage, temperature, signal_strength, crc | ASCII payload 문자 순회, CRC-16/MODBUS 초기값 0xFFFF |
| binary-packet-inspector-c | AA55, TYPE, LENGTH, hex PAYLOAD, hex CRC | 기존 코드는 hex 문자열의 **문자 바이트**로 CRC 계산 |

서로 다른 데이터 스키마를 같은 `TelemetryPacket`으로 선언하므로 헤더를 합치면 충돌한다.
특히 바이너리 페이로드에 대한 CRC와 ASCII hex 인코딩 문자에 대한 CRC는 다를 수 있다.

## 신규 독립 규격

`wire_protocol.h` / `wire_protocol.c`:

- 입력: `AA55,01,4,120A33FF,CCCC` (마지막 CRC는 실제 연산값)
- TYPE 1 byte hex, LENGTH decimal 0..128, PAYLOAD hex (정확히 2*LENGTH), CRC 4자리 hex
- CRC: 디코딩한 **실제 payload bytes** 대상 CRC-16/MODBUS (poly 0xA001, init 0xFFFF)
- CRC는 패킷 헤더를 포함하지 않음. 외부 벤더 규격을 대체하는 보편 프로토콜이 아님
- 유효하지 않은 프레임은 거부하고 출력은 이전 값 유지
- 테스트 벡터: ASCII "123456789" → MODBUS 0x4B37, ARC 0xBB3D

## 후속 통합 기준

1. 기존 세 프로그램의 빌드와 실행 결과 보존
2. 출처별 프로토콜 테스트 벡터 확보 (특히 CRC가 ASCII 대상인지 바이너리 대상인지)
3. 버전별 프레임 디코더와 타입 명칭을 분리
4. CI 통과 후 실제 합치기 판단; 원본 Archive 여부는 별도 결정

## 한계

실제 장비 패킷 규격·실물 수신·상호운용성은 검증하지 않았다.
현재 테스트는 독립 프로토콜 및 레거시 빌드만 확인한다.
