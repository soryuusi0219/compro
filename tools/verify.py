#!/usr/bin/env python3
"""Run the registered online judge and unit verification targets."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import subprocess
import sys
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
ONLINE_SECTIONS = ("library_checker", "aoj")
MANIFEST_DIRS = tuple(
    ROOT / "verify" / section / "manifests" for section in ONLINE_SECTIONS
)
UNIT_DIR = ROOT / "verify/unit"
EXPANDED_DIR = ROOT / "build/expanded"
BUILD_DIR = ROOT / "build/verify/oj"
CONFIG_PATH = BUILD_DIR / "config.toml"
TEST_SOURCE_DIR = BUILD_DIR / "sources"


class VerificationError(RuntimeError):
    pass


def load_targets() -> list[dict[str, Any]]:
    targets: list[dict[str, Any]] = []
    paths = sorted(
        path for directory in MANIFEST_DIRS for path in directory.rglob("*.json")
    )
    for path in paths:
        with path.open(encoding="utf-8") as file:
            target = json.load(file)
        section = path.relative_to(ROOT / "verify").parts[0]
        target["name"] = f"{section}/{target['name']}"
        target["kind"] = "online"
        targets.append(target)
    for path in sorted(UNIT_DIR.glob("*.cpp")):
        targets.append(
            {
                "name": f"unit/{path.stem}",
                "problem": "local/unit",
                "source": path.relative_to(ROOT).as_posix(),
                "kind": "unit",
            }
        )
    return sorted(targets, key=lambda target: str(target["name"]))


def select_targets(
    targets: list[dict[str, Any]], requested: list[str]
) -> list[dict[str, Any]]:
    if not requested:
        return targets
    by_name = {str(target["name"]): target for target in targets}
    unknown = sorted(set(requested) - by_name.keys())
    if unknown:
        names = ", ".join(unknown)
        raise VerificationError(f"Unknown verification target(s): {names}")
    return [by_name[name] for name in requested]


def toml_string(value: str) -> str:
    # A JSON string is also a valid TOML basic string.
    return json.dumps(value)


def write_config() -> None:
    compiler = os.environ.get("CXX", "g++")
    flags = [
        "-std=gnu++20",
        "-O2",
        "-pipe",
        "-Ithird_party/ac-library",
    ]
    CONFIG_PATH.parent.mkdir(parents=True, exist_ok=True)
    values = ", ".join(toml_string(flag) for flag in flags)
    CONFIG_PATH.write_text(
        "[[languages.cpp.environments]]\n"
        f"CXX = {toml_string(compiler)}\n"
        f"CXXFLAGS = [{values}]\n",
        encoding="utf-8",
    )


def write_test_source(target: dict[str, Any], expanded: bool) -> Path:
    name = str(target["name"])
    problem = str(target["problem"])
    problem_name = PurePosixPath(problem).name
    problem_url = str(
        target.get("url", f"https://judge.yosupo.jp/problem/{problem_name}")
    )
    source = Path(str(target["source"]))
    if expanded:
        source = EXPANDED_DIR.relative_to(ROOT) / source
    if not (ROOT / source).is_file():
        raise VerificationError(f"Verification source was not found: {source}")

    test_source = TEST_SOURCE_DIR / f"{name}.test.cpp"
    test_source.parent.mkdir(parents=True, exist_ok=True)
    test_source.write_text(
        f"#define PROBLEM {toml_string(problem_url)}\n"
        f'#include "{source.as_posix()}"\n',
        encoding="utf-8",
    )
    return test_source.relative_to(ROOT)


def run_unit_target(target: dict[str, Any], expanded: bool) -> int:
    name = str(target["name"])
    source = Path(str(target["source"]))
    if expanded:
        source = EXPANDED_DIR.relative_to(ROOT) / source
    if not (ROOT / source).is_file():
        raise VerificationError(f"Unit source was not found: {source}")

    binary_dir = BUILD_DIR / "unit"
    binary_dir.mkdir(parents=True, exist_ok=True)
    configurations = (0, 1, 2) if name == "unit/templates" else (None,)
    for configuration in configurations:
        suffix = f"-{configuration}" if configuration is not None else ""
        binary = binary_dir / f"{PurePosixPath(name).name}{suffix}"
        command = [
            os.environ.get("CXX", "g++"),
            "-std=c++20",
            "-O2",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-I.",
            "-Ithird_party/ac-library",
        ]
        if configuration is not None:
            command.append(f"-DTEMPLATE_KIND={configuration}")
        command.extend([str(source), "-o", str(binary.relative_to(ROOT))])
        print("+", " ".join(command), flush=True)
        compiled = subprocess.run(command, cwd=ROOT, check=False)
        if compiled.returncode != 0:
            return compiled.returncode
        run_command = [str(binary.relative_to(ROOT))]
        print("+", " ".join(run_command), flush=True)
        result = subprocess.run(run_command, cwd=ROOT, check=False)
        if result.returncode != 0:
            return result.returncode
    return 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run registered online judge and unit verification targets"
    )
    parser.add_argument("targets", nargs="*", help="verification target names")
    parser.add_argument("--list", action="store_true", help="list verification targets")
    parser.add_argument(
        "--expanded",
        action="store_true",
        help="verify standalone sources generated under build/expanded",
    )
    parser.add_argument("-j", "--jobs", type=int, default=1, help="parallel oj jobs")
    parser.add_argument("--timeout", type=float, help="overall timeout in seconds")
    parser.add_argument("--tle", type=float, help="per-case time limit in seconds")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    targets = load_targets()
    if args.list:
        for target in targets:
            print(f"{target['name']}: {target['problem']} <- {target['source']}")
        return 0
    if args.jobs <= 0:
        raise VerificationError("--jobs must be positive")
    if args.timeout is not None and args.timeout <= 0:
        raise VerificationError("--timeout must be positive")
    if args.tle is not None and args.tle <= 0:
        raise VerificationError("--tle must be positive")

    selected = select_targets(targets, args.targets)
    if not selected:
        raise VerificationError("No verification targets are configured")
    online_targets = [target for target in selected if target["kind"] == "online"]
    unit_targets = [target for target in selected if target["kind"] == "unit"]

    if online_targets:
        oj_verify = shutil.which("oj-verify")
        if oj_verify is None:
            raise VerificationError(
                "oj-verify was not found; install requirements-verify.txt first"
            )
        write_config()
        shutil.rmtree(TEST_SOURCE_DIR, ignore_errors=True)
        sources = [
            write_test_source(target, args.expanded) for target in online_targets
        ]
        command = [
            oj_verify,
            "--config-file",
            str(CONFIG_PATH.relative_to(ROOT)),
            "run",
            *(str(source) for source in sources),
            "--jobs",
            str(args.jobs),
        ]
        if args.timeout is not None:
            command.extend(["--timeout", str(args.timeout)])
        if args.tle is not None:
            command.extend(["--tle", str(args.tle)])
        print("+", " ".join(command), flush=True)
        result = subprocess.run(command, cwd=ROOT, check=False)
        if result.returncode != 0:
            return result.returncode

    for target in unit_targets:
        result = run_unit_target(target, args.expanded)
        if result != 0:
            return result
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except VerificationError as error:
        print(f"verify: {error}", file=sys.stderr)
        raise SystemExit(1)
