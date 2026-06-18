# Telemetry Packet Parser C

C 기반 드론/위성 텔레메트리 패킷 파서입니다.

이 프로젝트는 위성, 드론, 관제 시스템에서 들어오는 텔레메트리 데이터를 C 언어로 파싱하고, 배터리·신호·고도·속도 상태를 진단하는 CLI 도구입니다.

## Purpose

단순 C 문법 연습이 아니라, 실제 관제 시스템에서 중요한 데이터 흐름을 이해하기 위해 만들었습니다.

핵심 흐름은 다음과 같습니다.

```txt
Telemetry Data
→ Packet Parsing
→ State Diagnosis
→ Warning Detection
→ Operator-readable Output
Features
CSV 기반 텔레메트리 패킷 파싱
위도, 경도, 고도, 속도, 배터리, 신호 세기 처리
배터리 부족 감지
약한 신호 감지
고고도/고속 상태 감지
CLI 기반 진단 결과 출력
Tech Stack
C
GCC
Makefile
CLI
Telemetry Data Parsing
Build
make
Run
make run

또는 직접 파일 지정:

./telemetry_parser data/sample_packets.csv
Sample Output
Packet #4
  Position : 36.353450, 127.387800
  Altitude : 450.20 m
  Velocity : 35.10 m/s
  Battery  : 24.50 %
  Signal   : -79 dBm
  Diagnostic: WARNING - LOW_BATTERY
Why this project matters

위성데이터, 드론 데이터, 센서 데이터는 단순히 수집하는 것에서 끝나지 않습니다.

중요한 것은 데이터를 사람이 판단할 수 있는 구조로 바꾸는 것입니다.

이 프로젝트는 제가 관심 있는 다음 방향을 보여줍니다.

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
Binary packet parsing
Real-time telemetry stream
Socket communication
Log file export
Dashboard API integration
ROS2/PX4 telemetry bridge
