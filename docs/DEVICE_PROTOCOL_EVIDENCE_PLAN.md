# 실물 장비 프로토콜 증빙 수집·분석 계획 (M1 준비)

## 현재 확인된 사항
- 소프트웨어 내부 연구용 포맷: `AA55,TYPE_HEX,LENGTH_DEC,PAYLOAD_HEX,CRC_HEX`.
- 연구용 CRC-16/MODBUS 계산은 헤더를 제외한 **디코딩된 페이로드 바이트**에 적용한다.
- 기존 바이너리 검사기의 레거시 기본 CRC는 **ASCII HEX 문자**를 계산한다.
- 현재 **공식 벤더 패킷 명세·승인된 실물 캡처가 제공되지 않았으므로 장비 호환을 증명하지 못한다.**

## 실증 전 확인할 자료
1. 장비 모델·펌웨어 버전·소유 및 테스트 권한(승인된 장비만)
2. 물리 인터페이스 및 연결 규격(UART/UDP/CAN 등), 패킷 종단·바이트 순서
3. SYNC·TYPE·LENGTH·PAYLOAD·CRC 필드 정의(헤더 포함 여부, 이스케이프·시퀀스 등)
4. CRC 정확한 파라미터(폴리노미얼, 초기값, 반사, xorout, 계산 범위, 출력 엔디언)
5. 정상·오류 사례의 공식 테스트 벡터와 버전별 차이
6. 개인정보/위치·보안정보 포함 여부, 저장·전달 금지 항목

## 오프라인 검증 도구
```bash
python3 tools/protocol_evidence.py --input fixtures/synthetic_crc_vectors.csv --mode decoded_payload
python3 -m unittest discover -s tests -p 'test_protocol_evidence.py' -v
```
CSV 헤더: `capture_id,source,payload_hex,crc_hex`; source는 `synthetic`, `device_capture`, `unknown`.
도구는 CSV SHA-256, 선택한 CRC 가정, 레코드별 일치 여부를 JSON으로 출력한다.

- `SYNTHETIC_CONTRACT_MATCH`: 합성 벡터가 계산 가정에 일치함.
- `CAPTURE_MATCHES_ASSUMED_CRC_ONLY`: 제출된 캡처가 해당 CRC 가정에 일치함. **기기 호환 인증 아님**.
- `CRC_MISMATCH`: 하나 이상 불일치. 헤더 포함 CRC·엔디언 등의 다른 규격 가능성 검토.
- `device_compatibility_verified`는 항상 `false`이다.

## 보안 및 연구 품질
- 실물 데이터는 개인 계정/공개 저장소에 업로드하지 않는다. 승인된 로컬 환경에서만 보관.
- 식별 가능한 위치·장비 시리얼·접속 토큰·원본 시험증빙을 GitHub에 커밋하지 않는다.
- 연구용 합성 데이터와 실물 측정은 같은 성과로 표기하지 않는다.
- 본 단계는 **증빙 준비 및 CRC 가정검토**이지 실제 하드웨어 실증이 아니다.
