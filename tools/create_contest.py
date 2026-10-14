#!/usr/bin/env python3
"""Create a configured contest workspace from C++ templates."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import shutil
import sys
from typing import Any


def parse_bool(value: str) -> bool:
    normalized = value.lower()
    if normalized in {"1", "true", "yes", "on"}:
        return True
    if normalized in {"0", "false", "no", "off"}:
        return False
    raise argparse.ArgumentTypeError(
        f"expected true or false for --acl, got {value!r}"
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", help="contest directory to create")
    parser.add_argument("--root", type=Path, default=Path("."))
    parser.add_argument("--config", type=Path)
    parser.add_argument("--template")
    parser.add_argument("--tester-template")
    parser.add_argument("--problems")
    parser.add_argument("--problem-count", type=int)
    parser.add_argument("--acl", type=parse_bool)
    return parser.parse_args()


def load_config(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except OSError as error:
        raise ValueError(f"cannot read contest config {path}: {error}") from error
    except json.JSONDecodeError as error:
        raise ValueError(f"invalid contest config {path}: {error}") from error
    if not isinstance(data, dict):
        raise ValueError(f"contest config must be a JSON object: {path}")
    return data


def problem_names(count: int) -> list[str]:
    if count <= 0:
        raise ValueError("problem_count must be positive")
    names: list[str] = []
    for number in range(count):
        label = ""
        current = number
        while True:
            label = chr(ord("A") + current % 26) + label
            current = current // 26 - 1
            if current < 0:
                break
        names.append(label)
    return names


def resolve_from_root(root: Path, value: str) -> Path:
    path = Path(value)
    return (path if path.is_absolute() else root / path).resolve()


def relative_to_root(path: Path, root: Path) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return path.as_posix()


def write_if_missing(path: Path, content: str) -> bool:
    if path.exists():
        return False
    path.write_text(content, encoding="utf-8")
    return True


def main() -> int:
    args = parse_args()
    root = args.root.resolve()
    config_path = args.config or root / "contest.json"
    if not config_path.is_absolute():
        config_path = root / config_path
    config = load_config(config_path.resolve())

    contest_dir = Path(args.directory)
    if not contest_dir.is_absolute():
        contest_dir = root / contest_dir
    contest_dir = contest_dir.resolve()

    acl_available = (
        args.acl
        if args.acl is not None
        else bool(config.get("acl_available", True))
    )
    if args.template:
        template_name = args.template
    elif acl_available:
        template_name = str(config.get("template", "templates/main.cpp"))
    else:
        template_name = str(
            config.get("template_no_acl", config.get("template", ""))
        )
    tester_name = args.tester_template or str(
        config.get("tester_template", "templates/tester.cpp")
    )
    if not template_name:
        raise ValueError("no template configured for the selected ACL mode")

    template = resolve_from_root(root, template_name)
    tester_template = resolve_from_root(root, tester_name)
    if not template.is_file():
        raise FileNotFoundError(f"template not found: {template}")
    if not tester_template.is_file():
        raise FileNotFoundError(f"tester template not found: {tester_template}")

    if args.problems is not None and args.problem_count is not None:
        raise ValueError("use either --problems or --problem-count, not both")
    if args.problems is not None:
        problems = [item for item in args.problems if not item.isspace()]
        if not problems:
            raise ValueError("--problems must contain at least one problem name")
    else:
        count = (
            args.problem_count
            if args.problem_count is not None
            else int(config.get("problem_count", 12))
        )
        problems = problem_names(count)

    contest_dir.mkdir(parents=True, exist_ok=True)
    created: list[str] = []
    skipped: list[str] = []

    root_makefile = root / "Makefile"
    include_path = os.path.relpath(root_makefile, contest_dir)
    local_makefile = contest_dir / "Makefile"
    if write_if_missing(local_makefile, f"include {include_path}\n"):
        created.append(local_makefile.name)
    else:
        skipped.append(local_makefile.name)

    settings = {
        "acl_available": acl_available,
        "template": relative_to_root(template, root),
        "tester_template": relative_to_root(tester_template, root),
        "problems": problems,
    }
    settings_path = contest_dir / ".contest.json"
    settings_text = json.dumps(settings, indent=2, ensure_ascii=False) + "\n"
    if write_if_missing(settings_path, settings_text):
        created.append(settings_path.name)
    else:
        skipped.append(settings_path.name)

    tester = contest_dir / "tester.cpp"
    if tester.exists():
        skipped.append(tester.name)
    else:
        shutil.copyfile(tester_template, tester)
        created.append(tester.name)

    for problem in problems:
        source = contest_dir / f"{problem}.cpp"
        if source.exists():
            skipped.append(source.name)
            continue
        shutil.copyfile(template, source)
        created.append(source.name)

    print(f"contest directory: {contest_dir}")
    print(f"ACL available: {'yes' if acl_available else 'no'}")
    print(f"template: {relative_to_root(template, root)}")
    if created:
        print("created: " + " ".join(created))
    if skipped:
        print("skipped existing: " + " ".join(skipped))
    print(f"next: cd {contest_dir} && make run {problems[0]}.cpp")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError) as error:
        print(f"create_contest.py: {error}", file=sys.stderr)
        raise SystemExit(1)
