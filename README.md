# Telemetry Packet Parser C

C 기반 드론·위성·관제 텔레메트리 미니 코어 프로젝트입니다.

이 프로젝트는 CSV, Binary, UDP 형태의 텔레메트리 데이터를 C 언어로 파싱하고, 위험도 점수 계산, 위험 등급 분류, 이벤트 로그 저장, 패킷 순서 이상 감지, 세션 요약 리포트 생성을 수행합니다.

## Project Purpose

단순 C 문법 연습이 아니라, 실제 관제 시스템에서 중요한 데이터 흐름을 이해하기 위해 만들었습니다.

Telemetry Data
→ Packet Parsing
→ Risk Scoring
→ Risk Level Classification
→ Warning Event Logging
→ Sequence Anomaly Detection
→ Session Summary Report

## Features

- CSV 텔레메트리 패킷 파싱
- Binary 텔레메트리 패킷 파싱
- Magic number 기반 바이너리 패킷 검증
- UDP 실시간 텔레메트리 송신기/수신기
- 위험도 점수 계산
- 위험 등급 분류: NORMAL, LOW, MEDIUM, HIGH, CRITICAL
- 위험 이벤트 CSV 로그 저장
- 패킷 순서 이상 감지
- 세션 요약 TXT 리포트 생성
- 세션 요약 JSON 리포트 생성

## Tech Stack

- C
- GCC
- Makefile
- CLI
- Binary file I/O
- UDP socket programming
- Telemetry data parsing

## Project Structure

telemetry-packet-parser-c/
├── include/
│   └── telemetry.h
├── src/
│   ├── main.c
│   ├── telemetry.c
│   ├── binary_writer.c
│   ├── udp_receiver.c
│   ├── udp_sender.c
│   └── udp_sender_anomaly.c
├── data/
│   └── sample_packets.csv
├── docs/
│   └── ARCHITECTURE.md
├── logs/
│   └── .gitkeep
├── Makefile
├── README.md
├── CHANGELOG.md
└── VERSION

## Build

make

## Run CSV Parser

make run

## Run Binary Parser

make run-bin

## Run UDP Receiver

터미널 1에서 실행합니다.

make run-receiver

## Run UDP Sender

터미널 2에서 실행합니다.

make run-sender

## Run UDP Anomaly Sender

패킷 누락과 순서 이상을 테스트합니다.

make run-sender-anomaly

## Output Logs

실행 중 생성되는 로그는 logs/ 폴더에 저장됩니다.

- logs/warning_events.csv
- logs/sequence_anomalies.csv
- logs/udp_session_report.txt
- logs/udp_session_report.json

생성 로그는 .gitignore에 의해 Git에 포함되지 않습니다.

## Risk Level

| Score | Level |
|---|---|
| 0 | NORMAL |
| 1-39 | LOW |
| 40-59 | MEDIUM |
| 60-79 | HIGH |
| 80-100 | CRITICAL |

## v1.0 Scope

v1.0은 C 기반 텔레메트리 미니 관제 코어의 MVP입니다.

포함 범위:

- 파일 기반 CSV/Binary 파싱
- UDP 기반 실시간 수신
- 위험도 분석
- 위험 등급 분류
- 이벤트 로그 저장
- 패킷 순서 이상 감지
- TXT/JSON 리포트 출력

## Current Limitations

현재는 학습형 CLI 프로젝트입니다.

실제 드론, 위성, 센서 장비와 직접 연동되어 있지는 않으며, 실무 수준의 보안, 장애 복구, 멀티스레딩, 실시간 대시보드는 포함되어 있지 않습니다.

## Future Work

- Real device telemetry integration
- ROS2/PX4 telemetry bridge
- TCP/UDP hybrid communication
- FastAPI bridge
- Web dashboard
- Realtime monitoring UI
- Unit tests
- CI build check

## Why this project matters

위성데이터, 드론 데이터, 센서 데이터는 단순히 수집하는 것에서 끝나지 않습니다.

중요한 것은 데이터를 사람이 판단할 수 있는 구조로 바꾸는 것입니다.

이 프로젝트는 C 언어를 기반으로 텔레메트리 데이터가 수신, 파싱, 진단, 기록, 리포트로 이어지는 관제 시스템의 기본 흐름을 구현한 프로젝트입니다.
