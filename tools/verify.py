#!/usr/bin/env python3
"""Run local verification with yosupo06/library-checker-problems."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time
from typing import Any, Optional


ROOT = Path(__file__).resolve().parents[1]
VERIFY_DIR = ROOT / "verify/library_checker"
CONFIG_PATH = VERIFY_DIR / "config.json"
MANIFEST_DIR = VERIFY_DIR / "manifests"
DEFAULT_PROBLEMS_DIR = ROOT / "build/library-checker-problems"
BUILD_DIR = ROOT / "build/verify"
EXPANDED_DIR = ROOT / "build/expanded"


class VerificationError(RuntimeError):
    pass


def load_json(path: Path) -> Any:
    with path.open(encoding="utf-8") as file:
        return json.load(file)


def load_targets() -> list[dict[str, Any]]:
    return [load_json(path) for path in sorted(MANIFEST_DIR.rglob("*.json"))]


def run(command: list[str], *, cwd: Optional[Path] = None) -> None:
    print("+", " ".join(command), flush=True)
    subprocess.run(command, cwd=cwd, check=True)


def raise_stack_limit() -> None:
    if os.name == "nt":
        return
    try:
        import resource

        _, hard = resource.getrlimit(resource.RLIMIT_STACK)
        resource.setrlimit(resource.RLIMIT_STACK, (hard, hard))
    except (ImportError, OSError, ValueError):
        pass


def prepare_problems(config: dict[str, Any], problems_dir: Path, offline: bool) -> None:
    repository = str(config["repository"])
    revision = str(config["revision"])

    if not (problems_dir / ".git").is_dir():
        if offline:
            raise VerificationError(f"Library Checker Problems is not available: {problems_dir}")
        problems_dir.parent.mkdir(parents=True, exist_ok=True)
        run(["git", "clone", "--filter=blob:none", repository, str(problems_dir)])

    current = subprocess.run(
        ["git", "rev-parse", "HEAD"],
        cwd=problems_dir,
        check=True,
        capture_output=True,
        text=True,
    ).stdout.strip()
    if current != revision:
        if offline:
            raise VerificationError(
                f"Expected Library Checker Problems {revision}, but found {current}"
            )
        run(["git", "fetch", "origin", revision], cwd=problems_dir)
        run(["git", "checkout", "--detach", revision], cwd=problems_dir)

    if not (problems_dir / "generate.py").is_file():
        raise VerificationError(f"generate.py was not found in {problems_dir}")


def find_problem(problems_dir: Path, problem: str) -> Path:
    direct = problems_dir / problem
    if (direct / "info.toml").is_file():
        return direct

    matches = list(problems_dir.glob(f"**/{problem}/info.toml"))
    if len(matches) == 1:
        return matches[0].parent
    if not matches:
        raise VerificationError(f"Unknown Library Checker problem: {problem}")
    raise VerificationError(f"Ambiguous Library Checker problem name: {problem}")


def read_problem_setting(info_path: Path, key: str, default: str) -> str:
    pattern = re.compile(rf'^\s*{re.escape(key)}\s*=\s*[\"\']?([^\"\'\s#]+)')
    for line in info_path.read_text(encoding="utf-8").splitlines():
        match = pattern.match(line)
        if match:
            return match.group(1)
    return default


def generate_tests(problems_dir: Path, problem_dir: Path) -> None:
    run(
        [sys.executable, str(problems_dir / "generate.py"), str(problem_dir / "info.toml")],
        cwd=problems_dir,
    )


def compile_solution(target: dict[str, Any], expanded: bool) -> Path:
    name = str(target["name"])
    source = (EXPANDED_DIR if expanded else ROOT) / str(target["source"])
    if not source.is_file():
        raise VerificationError(f"Verification source was not found: {source}")

    binary_dir = BUILD_DIR / "bin"
    binary_dir.mkdir(parents=True, exist_ok=True)
    binary = binary_dir / name
    flags = [str(flag) for flag in target.get("cxxflags", [])]
    command = [
        os.environ.get("CXX", "g++"),
        "-std=gnu++20",
        "-O2",
        "-pipe",
    ]
    if not expanded:
        command.extend(["-I", str(ROOT)])
    command.extend(["-I", str(ROOT / "third_party/ac-library")])
    command.extend([*flags, str(source), "-o", str(binary)])
    run(command, cwd=ROOT)
    return binary


def save_failure(name: str, input_path: Path, actual_path: Path) -> Path:
    failure_dir = BUILD_DIR / "failures" / name
    if failure_dir.exists():
        shutil.rmtree(failure_dir)
    failure_dir.mkdir(parents=True)
    shutil.copy2(input_path, failure_dir / input_path.name)
    if actual_path.exists():
        shutil.copy2(actual_path, failure_dir / actual_path.name)
    return failure_dir


def judge_target(
    target: dict[str, Any], problem_dir: Path, binary: Path, timeout_scale: float
) -> None:
    name = str(target["name"])
    info_path = problem_dir / "info.toml"
    time_limit = float(read_problem_setting(info_path, "timelimit", "5.0")) * timeout_scale
    checker_source = problem_dir / read_problem_setting(info_path, "checker", "checker.cpp")
    checker = checker_source.with_suffix("")
    inputs = sorted((problem_dir / "in").glob("*.in"))

    if not checker.is_file() or not inputs:
        raise VerificationError(
            f"Generated checker or test cases are missing for {target['problem']}"
        )

    print(f"\n==> {name}: {target['problem']} ({len(inputs)} cases)", flush=True)
    with tempfile.TemporaryDirectory(prefix=f"{name}-", dir=BUILD_DIR) as temporary:
        temporary_dir = Path(temporary)
        for index, input_path in enumerate(inputs, start=1):
            expected_path = problem_dir / "out" / f"{input_path.stem}.out"
            actual_path = temporary_dir / f"{input_path.stem}.out"
            started = time.monotonic()
            try:
                with input_path.open("rb") as input_file, actual_path.open("wb") as output_file:
                    result = subprocess.run(
                        [str(binary)],
                        stdin=input_file,
                        stdout=output_file,
                        stderr=subprocess.PIPE,
                        timeout=time_limit,
                    )
            except subprocess.TimeoutExpired as error:
                failure = save_failure(name, input_path, actual_path)
                raise VerificationError(
                    f"TLE: {input_path.name} exceeded {time_limit:.2f}s; saved to {failure}"
                ) from error

            elapsed_ms = int((time.monotonic() - started) * 1000)
            if result.returncode != 0:
                failure = save_failure(name, input_path, actual_path)
                stderr = result.stderr.decode(errors="replace").strip()
                raise VerificationError(
                    f"RE: {input_path.name} exited with {result.returncode}; "
                    f"saved to {failure}\n{stderr}"
                )

            checked = subprocess.run(
                [str(checker), str(input_path), str(actual_path), str(expected_path)],
                capture_output=True,
                text=True,
            )
            if checked.returncode != 0:
                failure = save_failure(name, input_path, actual_path)
                message = (checked.stdout + checked.stderr).strip()
                raise VerificationError(
                    f"WA: {input_path.name}; saved to {failure}\n{message}"
                )
            print(f"  [{index:02d}/{len(inputs):02d}] AC {input_path.name} ({elapsed_ms} ms)")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Verify this library with yosupo06/library-checker-problems"
    )
    parser.add_argument("targets", nargs="*", help="target names from manifests/")
    parser.add_argument("--list", action="store_true", help="list verification targets")
    parser.add_argument("--offline", action="store_true", help="disable clone and fetch")
    parser.add_argument("--no-generate", action="store_true", help="reuse generated cases")
    parser.add_argument(
        "--prepare-only",
        action="store_true",
        help="prepare Library Checker Problems without running targets",
    )
    parser.add_argument(
        "--expanded",
        action="store_true",
        help="verify standalone sources generated under build/expanded",
    )
    parser.add_argument(
        "--problems-dir",
        type=Path,
        default=DEFAULT_PROBLEMS_DIR,
        help="path to a library-checker-problems checkout",
    )
    parser.add_argument(
        "--timeout-scale",
        type=float,
        default=1.0,
        help="multiply each official time limit by this value",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    config = load_json(CONFIG_PATH)
    targets = load_targets()

    if args.list:
        for target in targets:
            print(f"{target['name']}: {target['problem']} <- {target['source']}")
        return 0
    if args.timeout_scale <= 0:
        raise VerificationError("--timeout-scale must be positive")

    selected_names = set(args.targets)
    selected = [target for target in targets if not selected_names or target["name"] in selected_names]
    unknown = selected_names - {target["name"] for target in targets}
    if unknown:
        raise VerificationError(f"Unknown verification target(s): {', '.join(sorted(unknown))}")
    if not selected:
        raise VerificationError("No verification targets are configured")

    raise_stack_limit()
    problems_dir = args.problems_dir.resolve()
    prepare_problems(config, problems_dir, args.offline)
    if args.prepare_only:
        print(f"Library Checker Problems is ready at {problems_dir}")
        return 0

    for target in selected:
        problem_dir = find_problem(problems_dir, str(target["problem"]))
        if not args.no_generate:
            generate_tests(problems_dir, problem_dir)
        binary = compile_solution(target, args.expanded)
        judge_target(target, problem_dir, binary, args.timeout_scale)

    print(f"\nAll {len(selected)} verification target(s) passed.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (VerificationError, subprocess.CalledProcessError) as error:
        print(f"verify: {error}", file=sys.stderr)
        raise SystemExit(1)
