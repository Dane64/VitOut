import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


RESOLVER = Path(__file__).resolve().parents[1] / "scripts" / "resolve-sdk.sh"


class DependencyResolutionTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.channels = self.root / "channels"
        self.channels.mkdir()
        self.bootstrap = self.root / "bootstrap.sh"
        self.bootstrap.write_text("#!/usr/bin/env bash\nexit 0\n")
        self.output = self.root / "output"

    def resolve(self, channels):
        (self.channels / "index.json").write_text(json.dumps({"channels": channels}))
        return subprocess.run(
            ["bash", str(RESOLVER), str(self.output)],
            env={
                **os.environ,
                "VITASDK_CHANNEL_BASE_URL": self.channels.as_uri(),
                "VITASDK_BOOTSTRAP_SCRIPT_URL": self.bootstrap.as_uri(),
            },
            capture_output=True,
            text=True,
            check=False,
        )

    def publish(self, series, manifest):
        (self.channels / f"{series}.json").write_text(manifest)
        (self.channels / f"{series}.json.sig").write_text("signature fixture\n")

    def test_selects_newest_supported_series_for_existing_abi(self):
        self.publish("2026.08", "older compiler and libraries\n")
        self.publish("2026.10", "newest supported compiler and libraries\n")
        result = self.resolve({
            "2026.08": {"status": "supported", "world": "vita"},
            "2026.10": {"status": "supported", "world": "vita"},
            "2026.11-softfp": {"status": "supported", "world": "vita_softfp"},
            "2026.12": {"status": "unsupported", "world": "vita"},
            "nightly": {"status": "development", "world": "vita"},
        })
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(
            (self.output / "channel.json").read_text(),
            "newest supported compiler and libraries\n",
        )
        self.assertEqual(
            (self.output / "bootstrap-vitasdk.sh").read_bytes(),
            self.bootstrap.read_bytes(),
        )
        self.assertTrue((self.output / "channel.json.sig").is_file())

    def test_channel_updates_change_cache_inputs(self):
        channels = {"2026.08": {"status": "supported", "world": "vita"}}
        self.publish("2026.08", "first package release\n")
        self.assertEqual(self.resolve(channels).returncode, 0)
        before = (self.output / "channel.json").read_bytes()
        self.publish("2026.08", "updated package release\n")
        self.assertEqual(self.resolve(channels).returncode, 0)
        self.assertNotEqual(before, (self.output / "channel.json").read_bytes())

    def test_missing_world_uses_upstream_default_abi(self):
        self.publish("2026.08", "default ABI\n")
        result = self.resolve({"2026.08": {"status": "supported"}})
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_bootstrap_updates_change_cache_inputs(self):
        channels = {"2026.08": {"status": "supported", "world": "vita"}}
        self.publish("2026.08", "same compiler and libraries\n")
        self.assertEqual(self.resolve(channels).returncode, 0)
        self.bootstrap.write_text("#!/usr/bin/env bash\nprintf 'updated bootstrap\\n'\n")
        self.assertEqual(self.resolve(channels).returncode, 0)
        self.assertEqual(
            (self.output / "bootstrap-vitasdk.sh").read_bytes(),
            self.bootstrap.read_bytes(),
        )

    def test_missing_or_invalid_supported_series_fails(self):
        for channels in [
            {},
            {"nightly": {"status": "development", "world": "vita"}},
            {"../../invalid": {"status": "supported", "world": "vita"}},
        ]:
            with self.subTest(channels=channels):
                result = self.resolve(channels)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("VitaSDK series", result.stderr)

    def test_missing_manifest_fails(self):
        result = self.resolve({"2026.08": {"status": "supported", "world": "vita"}})
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("curl:", result.stderr)

    def test_missing_signature_fails(self):
        (self.channels / "2026.08.json").write_text("unsigned channel fixture\n")
        result = self.resolve({"2026.08": {"status": "supported", "world": "vita"}})
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("curl:", result.stderr)


if __name__ == "__main__":
    unittest.main()
