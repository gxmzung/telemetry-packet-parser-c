# 텔레메트리 CRC 인터페이스 v1 — 2026-11-16

## 연결하는 연구 저장소

- `gxmzung/telemetry-packet-parser-c` (본 저장소, 독립 패킷 파서)
- `gxmzung/binary-packet-inspector-c` (CRC 구현 수정 PR #1, 고정 커밋 `0fd9c1af307955349dbcc0ee0aff41645a2ce0b8`)
- `gxmzung/embedded-telemetry-lab-c` (원본 CRC-16/MODBUS 문자열 처리 회귀검증만 유지)

## v1 공통 불변조건

1. CRC는 **CRC-16/MODBUS**: reflected polynomial `0xA001`, init `0xFFFF`, xorout 0.
2. `PAYLOAD_HEX`를 먼저 디코딩하고 **실제 바이트에 대해서만** CRC 계산한다.
3. CRC 계산 범위는 payload 바이트뿐이다. SYNC/TYPE/LENGTH/구분자 미포함.
4. CRC를 ASCII 16진수 4자리로 대문자 직렬화한다.
5. TYPE은 0~255의 한 바이트, LENGTH는 실제 payload 바이트 수(0~128).
6. 실제 전송 기록: `AA55,TYPE_HEX,LENGTH_DEC,PAYLOAD_HEX,CRC_HEX`.
7. 잘못된 길이, CRC, HEX는 거부하고 유효성 검증 실패로 기존 출력 상태를 바꾸지 않는다.
8. 원본 ASCII HEX *문자* 기반 CRC는 레거시 호환 API로 유지하되 신규 프로토콜에서 사용하지 않는다.

## 실증 한계

이는 원본 두 프로젝트 간 공통으로 합의한 **연구용 규격**이다. 기존 장비·벤더의 프로토콜 또는 `embedded-telemetry-lab-c`의 패킷 스키마와 1:1 호환된다는 뜻이 아니다.

## 통합 검증

GitHub Actions `Cross-repository CRC contract`는 `binary-packet-inspector-c`의 지정된 SHA를 checkout하여 **양쪽 저장소의 실제 C 소스**를 링크하고 공통 바이트 CRC를 검증한다. 출처 변경 시 SHA를 갱신하고 계약 회귀검증을 다시 진행해야 한다.

`wire_protocol.c`의 독립 와이어 프레임은 `binary-packet-inspector-c`의 기본 `validate_packet_crc`가 아닌, **선택 API `validate_packet_crc_decoded`**에만 맞는다.
