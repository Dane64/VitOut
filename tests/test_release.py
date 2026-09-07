import os
from pathlib import Path
import subprocess
import unittest


BUILD_SCRIPT = Path(__file__).resolve().parents[1] / "scripts" / "build.sh"


class ReleaseMetadataTests(unittest.TestCase):
    def metadata(self, tag):
        return subprocess.run(
            ["bash", str(BUILD_SCRIPT), "--metadata"],
            env={**os.environ, "RELEASE_TAG": tag},
            capture_output=True,
            text=True,
            check=False,
        )

    def assert_metadata(self, tag, filename, version, prerelease):
        result = self.metadata(tag)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(
            result.stdout,
            f"filename={filename}\nvita-version={version}\nprerelease={prerelease}\n",
        )

    def test_branch_build_keeps_repository_version_and_unversioned_filename(self):
        self.assert_metadata("", "VitOut.vpk", "", "false")

    def test_release_tags(self):
        for tag, version in [
            ("v2.1", "02.01"),
            ("2.0", "02.00"),
            ("1.3", "01.03"),
            ("v0.0", "00.00"),
            ("v9.8", "09.08"),
            ("99.99", "99.99"),
        ]:
            with self.subTest(tag=tag):
                self.assert_metadata(
                    tag, f"VitOut-{tag.removeprefix('v')}.vpk", version, "false"
                )

    def test_prerelease_tags(self):
        for tag, version in [
            ("v0.9-alpha", "00.09"),
            ("v1.0-beta", "01.00"),
            ("2.0-beta.2", "02.00"),
            ("v2.1-rc.1", "02.01"),
        ]:
            with self.subTest(tag=tag):
                self.assert_metadata(
                    tag, f"VitOut-{tag.removeprefix('v')}.vpk", version, "true"
                )

    def test_invalid_tags_fail_without_metadata(self):
        for tag in [
            "main",
            "v2",
            "v2.1.0",
            "v100.1",
            "v1.100",
            "v02.1",
            "v2.01",
            "v-1.0",
            "v2.1-",
            "v2.1-beta..1",
            "v2.1+build",
            "feature/v2.1",
            "v2.1/../../file",
            "v2.1\nprerelease=false",
            "$(touch unwanted)",
        ]:
            with self.subTest(tag=tag):
                result = self.metadata(tag)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("Invalid release tag:", result.stderr)
                self.assertEqual(result.stdout, "")

    def test_unknown_arguments_fail(self):
        result = subprocess.run(
            ["bash", str(BUILD_SCRIPT), "--unknown"],
            capture_output=True,
            text=True,
            check=False,
        )
        self.assertEqual(result.returncode, 2)
        self.assertIn("Usage:", result.stderr)


if __name__ == "__main__":
    unittest.main()
