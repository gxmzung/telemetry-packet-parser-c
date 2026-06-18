# Telemetry Packet Parser C

C 기반 드론/위성 텔레메트리 패킷 파서입니다.

이 프로젝트는 위성, 드론, 관제 시스템에서 들어오는 텔레메트리 데이터를 C 언어로 파싱하고, 배터리·신호·고도·속도 상태를 진단하는 CLI 도구입니다.

## Purpose

단순 C 문법 연습이 아니라, 실제 관제 시스템에서 중요한 데이터 흐름을 이해하기 위해 만들었습니다.

```txt
Telemetry Data
→ Packet Parsing
→ State Diagnosis
→ Warning Detection
→ Operator-readable Output
Features
CSV 기반 텔레메트리 패킷 파싱
바이너리 텔레메트리 패킷 파싱
Magic number 기반 패킷 검증
위험도 점수 계산
배터리 부족 감지
약한 신호 감지
고고도/고속 상태 감지
미션 요약 통계 출력
진단 리포트 저장
Tech Stack
C
GCC
Makefile
CLI
Binary file I/O
Telemetry data parsing
Build
make
Run CSV Parser
make run
Run Binary Parser
make run-bin
Direct Run
./telemetry_parser data/sample_packets.csv
./telemetry_parser data/sample_packets.bin
Binary Packet Format
typedef struct {
    unsigned int magic;
    int packet_id;
    double latitude;
    double longitude;
    double altitude;
    double velocity;
    double battery;
    int signal_strength;
} BinaryTelemetryPacket;
Magic Number
0x54504B54

매직 넘버는 바이너리 패킷이 올바른 텔레메트리 패킷인지 확인하기 위한 값입니다.

Report Output

실행 후 진단 리포트가 생성됩니다.

logs/diagnostic_report.txt
Why this project matters

위성데이터, 드론 데이터, 센서 데이터는 단순히 수집하는 것에서 끝나지 않습니다.

중요한 것은 데이터를 사람이 판단할 수 있는 구조로 바꾸는 것입니다.

이 프로젝트는 다음 방향을 보여줍니다.

데이터 흐름
관제 시스템
텔레메트리
상태 진단
운영자 중심 출력
C 기반 시스템 프로그래밍
Current Limitations

현재는 학습형 CLI 프로젝트입니다.

실제 드론 또는 위성 데이터와 직접 연동되어 있지는 않으며, 실무 수준의 안정성이나 실시간 스트리밍 구조는 포함되어 있지 않습니다.

Future Work
UDP telemetry receiver
Real-time telemetry stream
Socket communication
Log file export
Dashboard API integration
ROS2/PX4 telemetry bridge

## v0.8 Update

이번 버전에서는 UDP 수신 중 패킷 번호의 순서 이상을 감지하는 기능을 추가했습니다.

### Added

- Packet sequence tracking
- Missing packet detection
- Out-of-order packet detection
- Sequence anomaly CSV log
- Anomaly sender for testing

### Run Normal UDP Test

터미널 1에서 receiver를 실행합니다.

make run-receiver

터미널 2에서 정상 sender를 실행합니다.

make run-sender

### Run Anomaly UDP Test

터미널 1에서 receiver를 실행합니다.

make run-receiver

터미널 2에서 이상 상황 sender를 실행합니다.

make run-sender-anomaly

### Sequence Anomaly Log

패킷 번호가 순서대로 들어오지 않으면 아래 파일에 기록됩니다.

logs/sequence_anomalies.csv

### Why sequence anomaly detection matters

실시간 관제 시스템에서는 데이터 값 자체뿐만 아니라, 데이터가 올바른 순서로 도착했는지도 중요합니다.

패킷 손실, 지연, 순서 뒤바뀜은 드론·센서·위성 데이터 처리에서 운영자가 확인해야 하는 중요한 이상 징후입니다.

## v0.9 Update

이번 버전에서는 UDP 수신 세션 요약을 JSON 파일로 저장하는 기능을 추가했습니다.

### Added

- UDP session JSON report export
- Machine-readable mission summary
- Future dashboard/API bridge format

### JSON Report Path

logs/udp_session_report.json

### JSON Report Fields

- total_packets
- warning_packets
- normal_packets
- average_altitude
- average_velocity
- average_battery
- max_altitude
- max_velocity
- min_battery

### Why JSON export matters

TXT와 CSV는 사람이 확인하기 좋지만, JSON은 웹 대시보드나 API 서버가 읽기 좋습니다.

이 업데이트는 C 기반 텔레메트리 수신기가 향후 FastAPI, 웹 대시보드, 관제 UI와 연결될 수 있도록 machine-readable report를 생성하는 단계입니다.
