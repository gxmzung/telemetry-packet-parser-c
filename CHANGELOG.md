# Changelog

## v1.0.0

### Summary

Telemetry Packet Parser C의 MVP 버전입니다.

CSV, Binary, UDP 텔레메트리 데이터를 처리하고, 위험도 분석, 위험 등급 분류, 이벤트 로그 저장, 패킷 순서 이상 감지, 세션 요약 TXT/JSON 리포트를 제공합니다.

### Added

- CSV telemetry parser
- Binary telemetry packet parser
- Magic number validation
- UDP telemetry receiver
- UDP telemetry sender
- UDP anomaly sender
- Realtime risk scoring
- Risk level classification
- Warning event CSV log
- Sequence anomaly CSV log
- UDP session TXT report
- UDP session JSON report
- Architecture documentation
- Clean Makefile commands

## v0.9

- UDP session summary JSON export

## v0.8

- Packet sequence anomaly detection
- Missing packet detection
- Out-of-order packet detection
- Anomaly sender

## v0.7

- Risk level classification
- NORMAL, LOW, MEDIUM, HIGH, CRITICAL levels

## v0.6

- UDP session summary report
- Receiver shutdown summary

## v0.5

- Realtime warning event logging

## v0.4

- UDP telemetry sender and receiver

## v0.3

- Binary packet parsing
- Binary packet generator

## v0.2

- Risk scoring
- Mission summary
- Diagnostic report

## v0.1

- CSV telemetry packet parser
