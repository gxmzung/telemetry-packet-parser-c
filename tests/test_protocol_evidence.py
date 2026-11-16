import importlib.util
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[1] / "tools" / "protocol_evidence.py"
SPEC = importlib.util.spec_from_file_location("protocol_evidence", SCRIPT)
assert SPEC and SPEC.loader
mod = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(mod)


class EvidenceTests(unittest.TestCase):
    def run_fixture(self, mode, contents):
        with tempfile.TemporaryDirectory() as td:
            path = Path(td) / "capture.csv"
            path.write_text(contents, encoding="utf8")
            return mod.assess_file(path, mode)

    def test_vectors(self):
        self.assertEqual(mod.crc_modbus(b"123456789"), 0x4B37)
        csv_data = "capture_id,source,payload_hex,crc_hex\nsynthetic-1,synthetic,313233343536373839,4B37\n"
        r = self.run_fixture("decoded_payload", csv_data)
        self.assertEqual(r["status"], "SYNTHETIC_CONTRACT_MATCH")
        self.assertEqual(r["counts"]["matches"], 1)
        self.assertFalse(r["device_compatibility_verified"])

    def test_device_claim_never_verified(self):
        r = self.run_fixture("decoded_payload", "capture_id,source,payload_hex,crc_hex\nd-1,device_capture,313233343536373839,4B37\n")
        self.assertEqual(r["status"], "CAPTURE_MATCHES_ASSUMED_CRC_ONLY")
        self.assertFalse(r["device_compatibility_verified"])

    def test_crc_mode_difference(self):
        r = self.run_fixture("ascii_hex_payload", "capture_id,source,payload_hex,crc_hex\ns-1,synthetic,313233343536373839,4B37\n")
        self.assertEqual(r["status"], "CRC_MISMATCH")

    def test_zero_length(self):
        r = self.run_fixture("decoded_payload", "capture_id,source,payload_hex,crc_hex\ns-0,synthetic,,FFFF\n")
        self.assertEqual(r["status"], "SYNTHETIC_CONTRACT_MATCH")

    def test_reject_invalid(self):
        hdr = "capture_id,source,payload_hex,crc_hex\n"
        for line in ("x,synthetic,ABC,FFFF", "x,synthetic,ZZ,FFFF",
                     "x,synthetic,00,XYZ1", "x,device,00,FFFF"):
            with self.subTest(line=line):
                with self.assertRaises(ValueError):
                    self.run_fixture("decoded_payload", hdr + line + "\n")

    def test_reject_too_long(self):
        with self.assertRaises(ValueError):
            self.run_fixture("decoded_payload", "capture_id,source,payload_hex,crc_hex\nx,synthetic," + "AA"*129 + ",0000\n")


if __name__ == "__main__":
    unittest.main()
