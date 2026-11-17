"""Validate UART C diagnostic history and current standalone output."""
from pathlib import Path
import hashlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
BASE = ROOT / "examples" / "uart-diagnostic"
ORIGINAL = BASE / "original"


def git_sha1(raw: bytes) -> str:
    return hashlib.sha1(f"blob {len(raw)}\0".encode("ascii") + raw).hexdigest()


class UartPreservationTests(unittest.TestCase):
    def test_source_sha_identity(self):
        rows = (BASE / "SOURCE_SHA1_MANIFEST.txt").read_text(encoding="utf-8").splitlines()
        self.assertEqual(len(rows), 12)
        for row in rows:
            sha, path = row.split("  ", 1)
            with self.subTest(file=path):
                f = ORIGINAL / path
                self.assertTrue(f.is_file())
                self.assertEqual(git_sha1(f.read_bytes()), sha)

    @unittest.skipUnless(shutil.which("cc"), "C compiler unavailable")
    def test_original_cli_behavior(self):
        with tempfile.TemporaryDirectory() as td:
            exe = Path(td) / "uart_cli"
            src = sorted((ORIGINAL / "src").glob("*.c"))
            self.assertEqual(len(src), 4)
            subprocess.run(
                ["cc", "-std=c11", "-Wall", "-Wextra", "-Wpedantic", "-Werror",
                 "-I" + str(ORIGINAL / "include"),
                 *map(str, src), "-o", str(exe)],
                check=True, text=True, capture_output=True,
            )
            result = subprocess.run(
                [str(exe)], cwd=ORIGINAL, text=True, capture_output=True, check=True
            )
            for line in ("INFO events: 3", "WARN events: 1", "ERROR events: 1",
                         "Sensor samples: 5", "Low voltage events: 1",
                         "High temperature events: 1", "Low signal events: 1",
                         "Device status: NEEDS ATTENTION"):
                with self.subTest(expected=line):
                    self.assertIn(line, result.stdout)


if __name__ == "__main__":
    unittest.main()
