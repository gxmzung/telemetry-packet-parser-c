#!/usr/bin/env python3
"""Offline, assumption-scoped CRC evidence report. No hardware I/O.

CSV columns: payload_hex, crc_hex, source, capture_id
source: synthetic | device_capture | unknown
Input is never uploaded; only a summary/hash is printed.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
from pathlib import Path

MAX_BYTES = 128
ALLOWED_SOURCES = {"synthetic", "device_capture", "unknown"}
ALLOWED_MODES = {"decoded_payload", "ascii_hex_payload"}


def crc_modbus(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = ((crc >> 1) ^ 0xA001) if (crc & 1) else (crc >> 1)
    return crc & 0xFFFF


def assess_row(row: dict[str, str], mode: str) -> dict[str, str | int | bool]:
    source = row.get("source", "")
    payload_text = row.get("payload_hex", "")
    expected_text = row.get("crc_hex", "")
    capture_id = row.get("capture_id", "")
    if source not in ALLOWED_SOURCES:
        raise ValueError("source must be synthetic, device_capture, or unknown")
    if not capture_id or len(capture_id) > 128:
        raise ValueError("capture_id is required (1-128 chars)")
    if len(payload_text) > 2 * MAX_BYTES or len(payload_text) % 2:
        raise ValueError("payload must be even-length HEX of at most 128 bytes")
    try:
        payload = bytes.fromhex(payload_text)
    except ValueError as exc:
        raise ValueError("invalid payload HEX") from exc
    if len(payload) > MAX_BYTES or payload.hex().upper() != payload_text.upper():
        raise ValueError("payload must be plain HEX without separators")
    if len(expected_text) != 4:
        raise ValueError("CRC must be exactly four HEX digits")
    try:
        expected = int(expected_text, 16)
    except ValueError as exc:
        raise ValueError("invalid CRC HEX") from exc
    content = payload if mode == "decoded_payload" else payload_text.encode("ascii")
    computed = crc_modbus(content)
    return {
        "capture_id": capture_id,
        "source": source,
        "payload_bytes": len(payload),
        "expected_crc": f"{expected:04X}",
        "computed_crc": f"{computed:04X}",
        "matches": expected == computed,
    }


def assess_file(path: Path, mode: str) -> dict:
    if mode not in ALLOWED_MODES:
        raise ValueError("unsupported CRC calculation mode")
    raw = path.read_bytes()
    if len(raw) > 1_000_000:
        raise ValueError("capture CSV exceeds 1 MB limit")
    text = raw.decode("utf-8-sig")
    reader = csv.DictReader(text.splitlines())
    required = {"payload_hex", "crc_hex", "source", "capture_id"}
    if reader.fieldnames is None or set(reader.fieldnames) != required:
        raise ValueError("CSV header must contain exactly: " + ", ".join(sorted(required)))
    results = []
    for index, row in enumerate(reader, 2):
        try:
            if len(results) >= 1000:
                raise ValueError("too many records (max 1000)")
            results.append(assess_row(row, mode))
        except ValueError as exc:
            raise ValueError(f"CSV line {index}: {exc}") from exc
    if not results:
        raise ValueError("capture CSV is empty")
    sources = sorted(set(str(r["source"]) for r in results))
    all_match = all(bool(r["matches"]) for r in results)
    is_synthetic = sources == ["synthetic"]
    status = (
        "SYNTHETIC_CONTRACT_MATCH" if all_match and is_synthetic else
        "CAPTURE_MATCHES_ASSUMED_CRC_ONLY" if all_match else
        "CRC_MISMATCH"
    )
    return {
        "schema": "protocol_evidence.v1",
        "input_sha256": hashlib.sha256(raw).hexdigest(),
        "assumed_crc_profile": {
            "algorithm": "CRC-16/MODBUS", "initial": "FFFF",
            "polynomial_reflected": "A001", "range": mode
        },
        "counts": {"records": len(results), "matches": sum(bool(r["matches"]) for r in results)},
        "sources": sources,
        "status": status,
        "device_compatibility_verified": False,
        "limitations": [
            "A matching CRC alone cannot establish device protocol compatibility.",
            "Header coverage, framing, endianness and field semantics require device specifications.",
            "source is operator-provided, not independently authenticated.",
        ],
        "results": results,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True, help="local CSV capture/evidence file")
    parser.add_argument("--mode", required=True, choices=sorted(ALLOWED_MODES))
    parser.add_argument("--output", type=Path, help="optional local JSON report (avoid uploading sensitive captures)")
    args = parser.parse_args()
    try:
        result = assess_file(args.input, args.mode)
    except (OSError, UnicodeError, ValueError) as exc:
        parser.error(str(exc))
    report = json.dumps(result, indent=2, ensure_ascii=False) + "\n"
    if args.output:
        args.output.write_text(report, encoding="utf-8")
    else:
        print(report, end="")
    return 0 if result["status"] != "CRC_MISMATCH" else 1


if __name__ == "__main__":
    raise SystemExit(main())
