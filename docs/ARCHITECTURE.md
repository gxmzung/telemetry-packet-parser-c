# Architecture

## Overview

Telemetry Packet Parser C는 드론/위성/관제 시스템에서 들어오는 텔레메트리 데이터를 C 언어로 파싱하고 진단하는 CLI 기반 학습형 프로젝트입니다.

이 프로젝트는 단순 C 문법 연습이 아니라, 실제 운영 시스템에서 중요한 데이터 흐름을 이해하는 데 목적이 있습니다.

## Data Flow

```txt
CSV Telemetry Packet
→ Line Reader
→ Packet Parser
→ Risk Score Calculator
→ Diagnostic Engine
→ Mission Summary
→ Diagnostic Report
Core Modules
1. main.c

역할:

입력 파일 열기
한 줄씩 데이터 읽기
패킷 파싱 호출
진단 출력
전체 요약 생성
리포트 저장
2. telemetry.c

역할:

CSV 라인 파싱
패킷 출력
위험도 계산
경고 상태 판단
전체 통계 계산
리포트 파일 생성
3. telemetry.h

역할:

TelemetryPacket 구조체 정의
TelemetrySummary 구조체 정의
주요 함수 선언
Diagnostic Rules

현재 경고 조건은 다음과 같습니다.

Condition	Rule
Low battery	battery < 25%
Weak signal	signal_strength < -85 dBm
High altitude	altitude > 1000m
High speed	velocity > 80m/s
Why C?

C는 임베디드 시스템, 저수준 데이터 처리, 센서 데이터 파싱, 드론/위성 시스템의 기반을 이해하는 데 적합한 언어입니다.

이 프로젝트는 C 언어를 이용해 텔레메트리 데이터의 기본 흐름을 직접 구현해보는 데 의미가 있습니다.

Future Architecture

향후 확장 방향은 다음과 같습니다.

Binary Packet
→ C Parser
→ Socket Stream
→ Local Log
→ FastAPI Bridge
→ Web Dashboard

장기적으로는 ROS2/PX4 텔레메트리 브릿지, 실시간 스트리밍, 대시보드 연동까지 확장할 수 있습니다.

## v0.9 JSON Report Export

v0.9부터는 UDP receiver가 종료될 때 세션 요약을 TXT뿐만 아니라 JSON 파일로도 저장합니다.

Data Flow:

UDP Packet
→ Telemetry Parser
→ Risk Score
→ Warning Event Logging
→ Sequence Anomaly Detection
→ Session Summary
→ TXT Report
→ JSON Report

JSON Report Path:

logs/udp_session_report.json

Purpose:

JSON 리포트는 향후 FastAPI 서버, 웹 대시보드, 관제 UI와 연결하기 위한 machine-readable output입니다.
