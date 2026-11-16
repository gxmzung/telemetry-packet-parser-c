# 패킷 파서 결정적 스트레스 검증 (2026-11-16)

## 검사 대상
- `telemetry-packet-parser-c`: `wire_parse_record` + `wire_crc16`
- `binary-packet-inspector-c`: 수정 PR #1의 `parse_packet_record` + `validate_packet_crc_decoded` + `calculate_crc16_decoded_payload`
- 고정된 참조 SHA: `e533184d5b52355e763fe6ebfff4a349e0ac65a6`

## 구현 및 실행
- GitHub Actions `Cross-repository CRC contract`의 결정적 10,000개 의사난수 테스트.
- Xorshift32 고정 시드 `0x6F3A91C5`: 재현 가능한 패킷 길이 0~128바이트, TYPE 0~255, 페이로드 바이트.
- 두 C 구현을 함께 링크해 CRC 계산, 파싱, 페이로드 디코딩, 크기 및 타입 동등성 대조.
- CRC 변조, 필수 필드 절단, 잘못된 구분자/HEX/초과 길이 처리 및 거부 시 출력 불변성 검사.
- `-std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=undefined`를 적용한 Ubuntu 자동 실행.

## 검증 범위 주의
- 기본 `validate_packet_crc`은 레거시 문자열 CRC이므로 비교 대상이 아니다.
- 결정적 의사난수 샘플 검사는 완전탐색, 커버리지 계측, libFuzzer 연속 퍼징, 실물 장비 상호운용성 검증을 대신하지 않는다.
- 누적 실행 건수는 테스트가 통과한 GitHub Actions 실행에 의해서만 확정한다.
- 실제 장비 프로토콜의 CRC 범위, 엔디언 및 종단 표시는 별도로 검증해야 한다.
