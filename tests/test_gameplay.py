import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]


class GameplayTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temporary = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.temporary.cleanup)
        cls.output = Path(cls.temporary.name)
        flags = [
            "-std=c11", "-O2", "-g", "-Wall", "-Wextra", "-Wpedantic", "-Werror",
            "-Wstrict-prototypes", "-Wmissing-prototypes", "-fno-common",
        ]
        if os.environ.get("VITOUT_SANITIZERS") == "1":
            flags += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
        targets = {
            "gameplay": [
                "tests/gameplay_test.c", "src/game.c", "src/menu.c", "src/level.c",
            ],
            "input": ["tests/input_test.c", "src/input.c"],
            "app": [
                "tests/app_test.c", "src/main.c", "src/game.c", "src/menu.c",
                "src/input.c", "src/level.c", "src/scores.c",
            ],
        }
        for name, sources in targets.items():
            result = subprocess.run(
                [
                    os.environ.get("HOST_CC", "cc"), *flags,
                    "-I", str(ROOT / "src"), "-I", str(ROOT / "tests" / "stubs"),
                    *(str(ROOT / source) for source in sources),
                    "-lm", "-o", str(cls.output / name),
                ],
                capture_output=True, text=True, check=False,
            )
            if result.returncode != 0:
                raise AssertionError(result.stdout + result.stderr)

    def run_case(self, name, *arguments):
        result = subprocess.run(
            [str(self.output / name), *map(str, arguments)],
            capture_output=True, text=True, check=False, timeout=120,
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_engine_menus_and_bundled_level_simulation(self):
        self.run_case("gameplay", ROOT / "levels", self.output / "missing-custom")

    def test_vita_controller_edges_and_deadzone(self):
        self.run_case("input")

    def test_app_hot_reload_pause_results_and_score_persistence(self):
        root = self.output / "app-data"
        bundled = root / "app0:" / "levels"
        custom = root / "ux0:" / "data" / "VitOut" / "levels"
        bundled.mkdir(parents=True)
        custom.mkdir(parents=True)
        rows = ["." * 30 for _ in range(20)]
        rows[3] = "." * 17 + "1" + "." * 12
        for directory, filename, identifier in [
            (bundled, "01-first.lvl", "fixture-first"),
            (custom, "10-second.lvl", "fixture-second"),
        ]:
            (directory / filename).write_text(
                f"VITOUT_LEVEL 1\nid={identifier}\nname=App Fixture\n"
                "times=10,20,30\n[bricks]\n" + "\n".join(rows) + "\n"
            )
        result = subprocess.run(
            [str(self.output / "app")], cwd=root,
            capture_output=True, text=True, check=False, timeout=30,
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
