# UART Diagnostic CLI — 원본 통합 보존

원본 저장소: https://github.com/gxmzung/uart-diagnostic-cli-c

- `original/`에는 **원본 main의 Git 추적 파일 12개 전체**를 원형 보존했으며, `SOURCE_SHA1_MANIFEST.txt`에서 SHA-1 Git blob 식별자를 확인할 수 있다.
- 독립적인 C11 빌드·CLI 샘플 실행이 기존 기능을 보존하는지 자동 테스트한다.
- 원본 센서 임계값(전압 <10.5, 온도 >75, 신호 <35) 및 이벤트·장비 상태 요약 코드는 메인 텔레메트리 파서와 별개로 유지한다. 두 프로토콜이 같다고 가정하지 않는다.
- 기존 파서는 텍스트 기반 UART 스타일 로그의 `[INFO]`, `[WARN]`, `[ERROR]`, `[SENSOR]` 패턴을 검색한다. 실제 UART 드라이버, 물리적인 장비 검증 또는 임의 제조사 통신 규격 대응이 아니다.
- 코드상 `strstr` 기반 로그 타입 식별과 `sscanf` 파싱 결과를 확인하지 않는 부분은 실무용 안정성 개선이 필요하다. **원본 보존 경로에서 임의 수정하지 않는다.**
- 원본 CLI는 파일 상대 경로 `data/sample_uart.log`에 의존한다. 원본 하위 디렉토리에서 실행할 것.
- 본 단계에서는 `telemetry-packet-parser-c`의 주력 파서·프로토콜 정의·빌드 경로를 변경하지 않았다.

## 검증
```bash
python3 -m unittest discover -s tests -p 'test_uart_original_preservation.py' -v
```

## Archive 판단
이 PR과 기존 핵심 CI가 모두 통과하고 `main`에 병합되면 원본 저장소는 Archive 권장. 원본 GitHub URL과 이력은 보존한다. **영구 삭제 금지.**
