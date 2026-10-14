#!/usr/bin/env python3
"""Inspect library dependencies, expand headers, and report verify coverage."""

from __future__ import annotations

import argparse
from collections import defaultdict
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys
from typing import Iterable


ROOT = Path(__file__).resolve().parents[1]
LIBRARY_DIR = ROOT / "library"
ATCODER_DIR = ROOT / "third_party/ac-library/atcoder"
MANIFEST_DIR = ROOT / "verify/library_checker/manifests"
DEFAULT_EXPANDED_DIR = ROOT / "build/expanded"
DEFAULT_REPORT_PREFIX = ROOT / "build/library_status"
GITHUB_LIBRARY_BASE = "https://github.com/soryuusi0219/compro/blob/main/"

LIBRARY_INCLUDE = re.compile(
    r'^\s*#\s*include\s+"(library/[^"\n]+)"\s*(?://.*)?$'
)
ATCODER_INCLUDE = re.compile(
    r'^\s*#\s*include\s*[<"](atcoder/[^>"\n]+)[>"]\s*(?://.*)?$'
)


class LibraryError(RuntimeError):
    pass


def relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def library_headers() -> list[Path]:
    return sorted(path for path in LIBRARY_DIR.rglob("*.hpp") if path.is_file())


def atcoder_headers() -> list[Path]:
    return sorted(
        path for path in ATCODER_DIR.rglob("*")
        if path.is_file() and path.name != "LICENSE"
    )


def dependency_includes(path: Path) -> list[str]:
    dependencies: list[str] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        match = LIBRARY_INCLUDE.match(line) or ATCODER_INCLUDE.match(line)
        if match:
            dependencies.append(match.group(1))
    return dependencies


def dependency_graph() -> dict[str, list[str]]:
    graph: dict[str, list[str]] = {}
    for header in library_headers():
        graph[relative(header)] = dependency_includes(header)
    for header in atcoder_headers():
        name = "atcoder/" + header.relative_to(ATCODER_DIR).as_posix()
        graph[name] = dependency_includes(header)
    return graph


def dependency_path(name: str) -> Path:
    if name.startswith("library/"):
        return ROOT / name
    if name.startswith("atcoder/"):
        return ATCODER_DIR / name[len("atcoder/"):]
    return ROOT / name


def library_names(graph: dict[str, list[str]]) -> list[str]:
    return sorted(name for name in graph if name.startswith("library/"))


def atcoder_names(graph: dict[str, list[str]]) -> list[str]:
    return sorted(name for name in graph if name.startswith("atcoder/"))


def validate_graph(graph: dict[str, list[str]]) -> None:
    errors: list[str] = []
    for header, dependencies in graph.items():
        if header.startswith("library/"):
            expected_url = f"// {GITHUB_LIBRARY_BASE}{header}"
            lines = dependency_path(header).read_text(encoding="utf-8").splitlines()
            first_line = lines[0] if lines else ""
            if first_line != expected_url:
                errors.append(
                    f"{header}: first line must be {expected_url!r}"
                )
        for dependency in dependencies:
            if dependency not in graph:
                errors.append(f"{header}: missing dependency {dependency}")

    state: dict[str, int] = {}
    stack: list[str] = []

    def visit(header: str) -> None:
        status = state.get(header, 0)
        if status == 2:
            return
        if status == 1:
            first = stack.index(header)
            errors.append("dependency cycle: " + " -> ".join(stack[first:] + [header]))
            return
        state[header] = 1
        stack.append(header)
        for dependency in graph.get(header, []):
            if dependency in graph:
                visit(dependency)
        stack.pop()
        state[header] = 2

    for header in graph:
        visit(header)
    if errors:
        raise LibraryError("\n".join(errors))


def transitive_dependencies(
    header: str, graph: dict[str, list[str]], *, include_self: bool = False
) -> list[str]:
    found: set[str] = set()

    def visit(current: str) -> None:
        for dependency in graph.get(current, []):
            if dependency not in found:
                found.add(dependency)
                visit(dependency)

    visit(header)
    if include_self:
        found.add(header)
    return sorted(found)


def load_verify_targets(graph: dict[str, list[str]]) -> list[dict[str, object]]:
    targets: list[dict[str, object]] = []
    names: set[str] = set()
    manifest_paths = sorted(MANIFEST_DIR.rglob("*.json"))
    for manifest_path in manifest_paths:
        target = json.loads(manifest_path.read_text(encoding="utf-8"))
        name = str(target["name"])
        if not re.fullmatch(r"[a-z0-9_]+", name):
            raise LibraryError(
                f"invalid verify target name {name!r}; use lowercase snake_case"
            )
        if name in names:
            raise LibraryError(f"duplicate verify target: {name}")
        if manifest_path.stem != name:
            raise LibraryError(
                f"{relative(manifest_path)}: filename must match target name {name!r}"
            )
        names.add(name)
        source_name = str(target["source"])
        source_path = Path(source_name)
        try:
            source_category = source_path.relative_to("verify/library_checker").parent
        except ValueError as error:
            raise LibraryError(
                f"{name}: invalid verify source path {source_name}"
            ) from error
        manifest_category = manifest_path.relative_to(MANIFEST_DIR).parent
        if manifest_category != source_category:
            raise LibraryError(
                f"{relative(manifest_path)}: category must match {source_category.as_posix()}"
            )
        source = ROOT / source_name
        if not source.is_file():
            raise LibraryError(f"{name}: missing verify source {source_name}")
        direct = dependency_includes(source)
        missing = [dependency for dependency in direct if dependency not in graph]
        if missing:
            raise LibraryError(f"{name}: missing dependency {', '.join(missing)}")
        all_dependencies: set[str] = set(direct)
        for dependency in direct:
            all_dependencies.update(transitive_dependencies(dependency, graph))
        direct_libraries = [item for item in direct if item.startswith("library/")]
        direct_atcoder = [item for item in direct if item.startswith("atcoder/")]
        targets.append(
            {
                "name": name,
                "problem": str(target["problem"]),
                "source": source_name,
                "manifest": relative(manifest_path),
                "direct_libraries": direct_libraries,
                "all_libraries": sorted(
                    item for item in all_dependencies if item.startswith("library/")
                ),
                "direct_atcoder_dependencies": direct_atcoder,
                "all_atcoder_dependencies": sorted(
                    item for item in all_dependencies if item.startswith("atcoder/")
                ),
            }
        )
    return targets


def expand_source(source_name: str, graph: dict[str, list[str]], *, header: bool) -> str:
    visited: set[str] = set()

    def expand(current: str) -> list[str]:
        if current in visited:
            return [f"// dependency already expanded: {current}"]
        visited.add(current)
        output: list[str] = [f"// begin: {current}"]
        source = dependency_path(current).read_text(encoding="utf-8")
        for line in source.splitlines():
            if line.strip() == "#pragma once":
                continue
            library_dependency = LIBRARY_INCLUDE.match(line)
            if library_dependency:
                dependency_name = library_dependency.group(1)
                if dependency_name not in graph:
                    raise LibraryError(
                        f"{current}: missing dependency {dependency_name}"
                    )
                output.extend(expand(dependency_name))
                continue
            atcoder_dependency = ATCODER_INCLUDE.match(line)
            if atcoder_dependency:
                dependency_name = atcoder_dependency.group(1)
                if dependency_name not in graph:
                    raise LibraryError(
                        f"{current}: missing dependency {dependency_name}"
                    )
                output.append(f"#include <{dependency_name}>")
                continue
            output.append(line)
        output.append(f"// end: {current}")
        return output

    body = expand(source_name)
    preamble = [
        "// Generated by tools/library_manager.py. Do not edit.",
        f"// Source: {source_name}",
    ]
    if header:
        preamble.extend(["#pragma once", ""])
    else:
        preamble.append("")
    return "\n".join([*preamble, *body, ""])


def normalize_requested_headers(
    requested: Iterable[str], graph: dict[str, list[str]]
) -> list[str]:
    result: list[str] = []
    for item in requested:
        candidate = item.replace("\\", "/")
        if not candidate.startswith("library/"):
            candidate = "library/" + candidate
        if not candidate.endswith(".hpp"):
            candidate += ".hpp"
        if candidate not in graph:
            raise LibraryError(f"unknown library header: {item}")
        result.append(candidate)
    return result


def write_expanded(
    graph: dict[str, list[str]], output_dir: Path, requested: list[str]
) -> int:
    headers = (
        normalize_requested_headers(requested, graph)
        if requested
        else library_names(graph)
    )
    if not requested:
        shutil.rmtree(output_dir / "library", ignore_errors=True)
    for header in headers:
        destination = output_dir / header
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(
            expand_source(header, graph, header=True), encoding="utf-8"
        )
    return len(headers)


def normalize_requested_targets(
    requested: Iterable[str], targets: list[dict[str, object]]
) -> list[dict[str, object]]:
    by_name = {str(target["name"]): target for target in targets}
    result: list[dict[str, object]] = []
    for name in requested:
        if name not in by_name:
            raise LibraryError(f"unknown verify target: {name}")
        result.append(by_name[name])
    return result


def write_expanded_verify(
    graph: dict[str, list[str]],
    targets: list[dict[str, object]],
    output_dir: Path,
    requested: list[str],
) -> int:
    selected = normalize_requested_targets(requested, targets) if requested else targets
    if not requested:
        shutil.rmtree(output_dir / "verify", ignore_errors=True)
    for target in selected:
        source_name = str(target["source"])
        destination = output_dir / source_name
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(
            expand_source(source_name, graph, header=False), encoding="utf-8"
        )
    return len(selected)


def coverage_data(
    graph: dict[str, list[str]], targets: list[dict[str, object]]
) -> list[dict[str, object]]:
    direct_targets: dict[str, list[str]] = defaultdict(list)
    all_targets: dict[str, list[str]] = defaultdict(list)
    for target in targets:
        name = str(target["name"])
        for header in target["direct_libraries"]:
            direct_targets[str(header)].append(name)
        for header in target["all_libraries"]:
            all_targets[str(header)].append(name)

    libraries: list[dict[str, object]] = []
    for header in library_names(graph):
        direct = sorted(direct_targets[header])
        indirect = sorted(set(all_targets[header]) - set(direct))
        status = "direct" if direct else "indirect" if indirect else "unverified"
        all_dependencies = transitive_dependencies(header, graph)
        libraries.append(
            {
                "header": header,
                "direct_dependencies": [
                    item for item in graph[header] if item.startswith("library/")
                ],
                "all_dependencies": [
                    item for item in all_dependencies if item.startswith("library/")
                ],
                "direct_atcoder_dependencies": [
                    item for item in graph[header] if item.startswith("atcoder/")
                ],
                "all_atcoder_dependencies": [
                    item for item in all_dependencies if item.startswith("atcoder/")
                ],
                "verify_status": status,
                "direct_verify_targets": direct,
                "indirect_verify_targets": indirect,
            }
        )
    return libraries


def markdown_report(data: dict[str, object]) -> str:
    summary = data["summary"]
    lines = [
        "# Library status",
        "",
        "このファイルは `python3 tools/library_manager.py report` で生成されます。",
        "verify 状態は `verify/library_checker/manifests/` への登録状況を表します。",
        "",
        f"- Libraries: {summary['libraries']}",
        f"- Directly verified: {summary['directly_verified']}",
        f"- Indirectly verified: {summary['indirectly_verified']}",
        f"- Unverified: {summary['unverified']}",
        f"- Verify targets: {summary['verify_targets']}",
        f"- AtCoder Library headers referenced: {summary['atcoder_headers_referenced']}",
        "",
        "## Libraries",
        "",
        "| Library | Status | Direct libraries | AtCoder Library | Verify targets |",
        "|---|---|---|---|---|",
    ]
    for library in data["libraries"]:
        targets = library["direct_verify_targets"] or library["indirect_verify_targets"]
        lines.append(
            "| `{}` | {} | {} | {} | {} |".format(
                library["header"],
                library["verify_status"],
                ", ".join(f"`{item}`" for item in library["direct_dependencies"]) or "-",
                ", ".join(
                    f"`{item}`" for item in library["all_atcoder_dependencies"]
                ) or "-",
                ", ".join(f"`{item}`" for item in targets) or "-",
            )
        )
    lines.extend(
        [
            "",
            "## Verify dependencies",
            "",
            "| Target | Problem | Direct libraries | All libraries | AtCoder Library |",
            "|---|---|---|---|---|",
        ]
    )
    for target in data["verify_targets"]:
        lines.append(
            "| `{}` | `{}` | {} | {} | {} |".format(
                target["name"],
                target["problem"],
                ", ".join(f"`{item}`" for item in target["direct_libraries"]) or "-",
                ", ".join(f"`{item}`" for item in target["all_libraries"]) or "-",
                ", ".join(
                    f"`{item}`" for item in target["all_atcoder_dependencies"]
                ) or "-",
            )
        )
    return "\n".join(lines) + "\n"


def build_report(
    graph: dict[str, list[str]], targets: list[dict[str, object]]
) -> dict[str, object]:
    libraries = coverage_data(graph, targets)
    counts = {status: 0 for status in ("direct", "indirect", "unverified")}
    for library in libraries:
        counts[str(library["verify_status"])] += 1
    referenced_atcoder = {
        str(dependency)
        for library in libraries
        for dependency in library["all_atcoder_dependencies"]
    }
    referenced_atcoder.update(
        str(dependency)
        for target in targets
        for dependency in target["all_atcoder_dependencies"]
    )
    return {
        "summary": {
            "libraries": len(libraries),
            "directly_verified": counts["direct"],
            "indirectly_verified": counts["indirect"],
            "unverified": counts["unverified"],
            "verify_targets": len(targets),
            "atcoder_headers_referenced": len(referenced_atcoder),
        },
        "libraries": libraries,
        "verify_targets": targets,
    }


def write_report(data: dict[str, object], prefix: Path) -> tuple[Path, Path]:
    json_path = prefix.with_suffix(".json")
    markdown_path = prefix.with_suffix(".md")
    json_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
    markdown_path.write_text(markdown_report(data), encoding="utf-8")
    return json_path, markdown_path


def changed_paths(base: str, head: str) -> list[tuple[str, str]]:
    if not base or set(base) == {"0"}:
        return [("F", "")]
    result = subprocess.run(
        ["git", "diff", "--name-status", "--find-renames", base, head],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    )
    changes: list[tuple[str, str]] = []
    for line in result.stdout.splitlines():
        fields = line.split("\t")
        status = fields[0][0]
        paths = fields[1:]
        if status in {"R", "C"}:
            changes.append(("D" if status == "R" else status, paths[0]))
            changes.append(("A", paths[-1]))
        elif paths:
            changes.append((status, paths[0]))
    return changes


def affected_verify_targets(
    targets: list[dict[str, object]], changes: list[tuple[str, str]]
) -> tuple[list[str], bool]:
    all_names = [str(target["name"]) for target in targets]
    by_source = {str(target["source"]): str(target["name"]) for target in targets}
    by_manifest = {str(target["manifest"]): str(target["name"]) for target in targets}
    selected: set[str] = set()
    force_all = False

    for status, path in changes:
        if status == "F":
            force_all = True
            continue
        if path in {
            ".github/workflows/verify.yml",
            "verify/library_checker/config.json",
        } or path.startswith("tools/"):
            force_all = True
            continue
        if status == "D" and (
            path.startswith("library/")
            or path.startswith("verify/library_checker/")
            or path.startswith("third_party/ac-library/atcoder/")
        ):
            force_all = True
            continue
        if path in by_source:
            selected.add(by_source[path])
            continue
        if path in by_manifest:
            selected.add(by_manifest[path])
            continue
        if path.startswith("library/"):
            selected.update(
                str(target["name"])
                for target in targets
                if path in target["all_libraries"]
            )
            continue
        prefix = "third_party/ac-library/"
        if path.startswith(prefix):
            dependency = path[len(prefix):]
            selected.update(
                str(target["name"])
                for target in targets
                if dependency in target["all_atcoder_dependencies"]
            )

    if force_all:
        return all_names, True
    return [name for name in all_names if name in selected], False


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Check dependencies, expand headers, and report verify coverage"
    )
    subparsers = parser.add_subparsers(dest="command")
    subparsers.required = True

    subparsers.add_parser("check", help="validate library and verify dependencies")

    expand = subparsers.add_parser("expand", help="generate self-contained headers")
    expand.add_argument("headers", nargs="*", help="library paths; defaults to all headers")
    expand.add_argument("--output-dir", type=Path, default=DEFAULT_EXPANDED_DIR)

    expand_verify = subparsers.add_parser(
        "expand-verify", help="generate standalone Library Checker submissions"
    )
    expand_verify.add_argument(
        "targets", nargs="*", help="verify target names; defaults to all targets"
    )
    expand_verify.add_argument("--output-dir", type=Path, default=DEFAULT_EXPANDED_DIR)

    report = subparsers.add_parser("report", help="write dependency and verify reports")
    report.add_argument("--output-prefix", type=Path, default=DEFAULT_REPORT_PREFIX)

    affected = subparsers.add_parser(
        "affected", help="list verify targets affected by a git diff"
    )
    affected.add_argument("--base", required=True, help="base git revision")
    affected.add_argument("--head", default="HEAD", help="head git revision")
    affected.add_argument(
        "--format", choices=("names", "github"), default="names"
    )

    all_command = subparsers.add_parser("all", help="check, expand all, and report")
    all_command.add_argument("--output-dir", type=Path, default=DEFAULT_EXPANDED_DIR)
    all_command.add_argument("--output-prefix", type=Path, default=DEFAULT_REPORT_PREFIX)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    graph = dependency_graph()
    validate_graph(graph)
    targets = load_verify_targets(graph)

    if args.command == "check":
        libraries = library_names(graph)
        library_edges = sum(
            dependency.startswith("library/")
            for name in libraries
            for dependency in graph[name]
        )
        library_acl_edges = sum(
            dependency.startswith("atcoder/")
            for name in libraries
            for dependency in graph[name]
        )
        acl = atcoder_names(graph)
        acl_edges = sum(len(graph[name]) for name in acl)
        print(
            f"OK: {len(libraries)} libraries, {library_edges} internal dependency "
            f"edges, {library_acl_edges} direct ACL dependency edges, "
            f"{len(acl)} AtCoder Library headers, {acl_edges} ACL internal edges, "
            f"{len(targets)} verify targets"
        )
        return 0
    if args.command == "expand":
        count = write_expanded(graph, args.output_dir.resolve(), args.headers)
        print(f"Expanded {count} header(s) into {args.output_dir.resolve()}")
        return 0
    if args.command == "expand-verify":
        count = write_expanded_verify(
            graph, targets, args.output_dir.resolve(), args.targets
        )
        print(
            f"Expanded {count} verify source(s) into {args.output_dir.resolve()}"
        )
        return 0
    if args.command == "report":
        paths = write_report(build_report(graph, targets), args.output_prefix.resolve())
        print(f"Wrote {paths[0]} and {paths[1]}")
        return 0
    if args.command == "affected":
        selected, all_selected = affected_verify_targets(
            targets, changed_paths(args.base, args.head)
        )
        if args.format == "github":
            matrix_targets = selected if selected else [""]
            matrix = json.dumps({"target": matrix_targets}, separators=(",", ":"))
            print(f"matrix={matrix}")
            print(f"count={len(selected)}")
            print(f"all={'true' if all_selected else 'false'}")
        else:
            print("\n".join(selected))
        return 0

    count = write_expanded(graph, args.output_dir.resolve(), [])
    verify_count = write_expanded_verify(
        graph, targets, args.output_dir.resolve(), []
    )
    paths = write_report(build_report(graph, targets), args.output_prefix.resolve())
    print(f"OK: expanded {count} headers into {args.output_dir.resolve()}")
    print(
        f"OK: expanded {verify_count} verify sources into "
        f"{args.output_dir.resolve()}"
    )
    print(f"Wrote {paths[0]} and {paths[1]}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (LibraryError, OSError, ValueError, json.JSONDecodeError) as error:
        print(f"library_manager: {error}", file=sys.stderr)
        raise SystemExit(1)
