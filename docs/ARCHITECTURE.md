# Architecture

## Overview

Telemetry Packet Parser C는 드론·위성·관제 시스템에서 들어오는 텔레메트리 데이터를 C 언어로 파싱하고 진단하는 CLI 기반 미니 관제 코어입니다.

이 프로젝트는 단순 문법 예제가 아니라, 실제 운영 시스템에서 중요한 데이터 흐름을 이해하는 데 목적이 있습니다.

## High-Level Architecture

Telemetry Source
→ CSV / Binary / UDP Input
→ Parser
→ Risk Scoring
→ Risk Level Classification
→ Warning Event Logging
→ Sequence Anomaly Detection
→ Session Summary
→ TXT / JSON Report

## CSV Data Flow

CSV Telemetry Packet
→ Line Reader
→ Packet Parser
→ Risk Score Calculator
→ Diagnostic Engine
→ Mission Summary
→ Diagnostic Report

## Binary Data Flow

Binary Packet File
→ fread()
→ Magic Number Validation
→ BinaryTelemetryPacket
→ TelemetryPacket Conversion
→ Risk Score
→ Diagnostic Output
→ Mission Summary

## UDP Data Flow

udp_sender
→ UDP Packet
→ udp_receiver
→ Telemetry Parser
→ Risk Score
→ Risk Level
→ Warning Event Log
→ Session Summary

## Sequence Anomaly Flow

UDP Packet
→ Packet ID Check
→ Expected ID Compare
→ Missing Packet Detection
→ Out-of-order Detection
→ Sequence Anomaly Log

## Report Flow

Session Summary
→ TXT Report
→ JSON Report
→ Future API / Dashboard Bridge

## Core Modules

### main.c

- CSV/Binary 입력 파일 열기
- 파일 확장자 기반 모드 감지
- 패킷 파싱 호출
- 진단 출력
- 전체 요약 생성
- 리포트 저장

### telemetry.c

- CSV 라인 파싱
- 바이너리 패킷 파싱
- 위험도 점수 계산
- 위험 등급 분류
- 패킷 출력
- 경고 상태 판단
- 전체 통계 계산
- TXT/JSON 리포트 생성
- 위험 이벤트 로그 저장

### binary_writer.c

- 테스트용 바이너리 텔레메트리 패킷 생성
- sample_packets.bin 생성

### udp_receiver.c

- UDP 포트 수신
- 실시간 패킷 파싱
- 위험 이벤트 저장
- 패킷 순서 이상 감지
- 세션 요약 누적
- 종료 시 리포트 저장

### udp_sender.c

- 정상 UDP 텔레메트리 패킷 송신

### udp_sender_anomaly.c

- 누락/순서 이상 패킷 송신
- sequence anomaly 테스트용

### telemetry.h

- TelemetryPacket 구조체 정의
- BinaryTelemetryPacket 구조체 정의
- TelemetrySummary 구조체 정의
- 주요 함수 선언

## Diagnostic Rules

| Condition | Rule |
|---|---|
| Low battery | battery < 25% |
| Weak signal | signal_strength < -85 dBm |
| High altitude | altitude > 1000m |
| High speed | velocity > 80m/s |

## Risk Level

| Score | Level |
|---|---|
| 0 | NORMAL |
| 1-39 | LOW |
| 40-59 | MEDIUM |
| 60-79 | HIGH |
| 80-100 | CRITICAL |

## Generated Outputs

- logs/warning_events.csv
- logs/sequence_anomalies.csv
- logs/udp_session_report.txt
- logs/udp_session_report.json

## Why C?

C는 임베디드 시스템, 저수준 데이터 처리, 센서 데이터 파싱, 드론/위성 시스템의 기반을 이해하는 데 적합한 언어입니다.

이 프로젝트는 C 언어를 이용해 텔레메트리 데이터의 기본 흐름을 직접 구현해보는 데 의미가 있습니다.

## v1.0 Definition

v1.0은 파일 기반 파싱, UDP 실시간 수신, 위험도 분석, 이벤트 로그, 패킷 순서 이상 감지, TXT/JSON 리포트까지 포함한 MVP 버전입니다.

## Future Architecture

Real Device / Simulator
→ UDP Telemetry Stream
→ C Socket Receiver
→ Packet Parser
→ Risk Scoring
→ Event Logger
→ FastAPI Bridge
→ Web Dashboard
