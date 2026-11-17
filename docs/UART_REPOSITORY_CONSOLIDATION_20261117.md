# C 텔레메트리 저장소 정리 — UART CLI (2026-11-17)

- **유지:** `telemetry-packet-parser-c` — C 기반 CSV·Binary·UDP 텔레메트리, 위험점수, 이벤트 로그 및 기존 실험용 wire CRC 구현.
- **Archive 후보:** `uart-diagnostic-cli-c` — 텍스트 UART 로그의 경고·오류·센서 임계값 분류 실습 코드.
- 중복은 관제 텔레메트리 진단·요약의 개념 수준이다. Binary 패킷과 UART 로그 포맷을 기술적으로 동일하다고 보지 않는다.
- 원본 12개 Git 추적 파일 전체의 원본 blob SHA-1을 기록하고 독립 빌드/실행으로 보존을 검증한다.
- 기존 코드의 `strstr`·`sscanf` 입력 검증 부족 및 실물 UART 미검증 사실을 숨기지 않는다.
- 새로운 보존 패키지는 기본 빌드에 편입하지 않아 기존 파서·기능을 변경하지 않는다.
- `main` 병합과 CI 성공 뒤에 원본 `uart-diagnostic-cli-c`를 보관 처리할 수 있다. Archive는 데이터 삭제가 아니라 읽기 전용 보존이다.
