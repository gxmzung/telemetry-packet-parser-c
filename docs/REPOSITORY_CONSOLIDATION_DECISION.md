# 중복 저장소 정리 결정 — 2026-11-16

## 핵심 저장소
`gxmzung/telemetry-packet-parser-c`를 C 텔레메트리 연구용 중심 저장소로 유지한다.

## 이관 내용
- `embedded-telemetry-lab-c`의 **고유 스키마** (전압, 온도, 신호 품질, 기록된 CRC 값)를 `EmbeddedSample`로 별도 보존.
- 전압 <10.5 또는 >13.0, 온도 <-10 또는 >75, 신호 <35 임계치 및 세션 평균/이상 패킷 수 보존.
- 기존 타입 `TelemetryPacket`과 이름이 겹치지 않게 독립 `embedded_profile` 모듈을 추가.
- 원본 저장소에 있는 `crc` 열은 **실제 CRC 계산 대상과 인코딩이 명시되어 있지 않음**. 값만 보관하며 검증 결과로 사용하지 않음.
- `binary-packet-inspector-c`의 CRC 프로토콜 파서·교차 테스트는 중심 저장소의 `wire_protocol`/테스트와 중첩되지만, 별도 CLI의 패킷 종류 분류와 보고서 기능은 아직 보존 여부를 검토해야 함.

## Archive 기준
- 두 원본의 README, 소스코드, 데이터/테스트 및 커밋 이력은 기존 GitHub 저장소에 계속 남긴다.
- `embedded-telemetry-lab-c`: 이관 CI 및 코드 리뷰 통과 후 Archive **후보**.
- `binary-packet-inspector-c`: 독립 CLI 보고서·분류 등 기능 이관/유지 결정을 내리기 전에는 Archive 금지.
- Archive는 삭제가 아니며 언제든 되돌릴 수 있다.
- 원본의 외부 참조/배포 의존성이 있는 경우 이를 먼저 정리한다.

## 별도 검증
실제 하드웨어 프로토콜 적합성, 레거시 CSV의 원시 CRC 진위 및 장비 측정 성능은 검증하지 않음.
