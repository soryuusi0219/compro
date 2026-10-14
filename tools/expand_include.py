#!/usr/bin/env python3
"""Expand repository includes into a submission-ready C++ source."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import sys


QUOTE_INCLUDE_RE = re.compile(r'^(\s*)#\s*include\s+"([^"]+)"')
SYSTEM_INCLUDE_RE = re.compile(r"^(\s*)#\s*include\s+<([^>]+)>")
PRAGMA_ONCE_RE = re.compile(r"^\s*#\s*pragma\s+once\s*$")
FALSE_RELEASE_RE = re.compile(
    r"^\s*#\s*(?:ifdef\s+(?:LOCAL|_DEBUG)|ifndef\s+ONLINE_JUDGE)\b"
)
ELSE_RE = re.compile(r"^\s*#\s*else\b")
ENDIF_RE = re.compile(r"^\s*#\s*endif\b")
IF_RE = re.compile(r"^\s*#\s*(?:if|ifdef|ifndef)\b")
LIBRARY_URL_PREFIX = (
    "// https://github.com/soryuusi0219/compro/blob/main/library/"
)


class ExpandError(RuntimeError):
    pass


def relative(path: Path, root: Path) -> str:
    try:
        return path.resolve().relative_to(root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def split_conditional(
    lines: list[str], start: int
) -> tuple[list[str], list[str], int]:
    depth = 0
    else_at = -1
    index = start + 1
    while index < len(lines):
        line = lines[index]
        if IF_RE.match(line):
            depth += 1
        elif ENDIF_RE.match(line):
            if depth == 0:
                true_part = lines[start + 1 : else_at if else_at != -1 else index]
                false_part = lines[else_at + 1 : index] if else_at != -1 else []
                return true_part, false_part, index + 1
            depth -= 1
        elif ELSE_RE.match(line) and depth == 0:
            else_at = index
        index += 1
    raise ExpandError("unterminated preprocessor conditional in release filtering")


def filter_release(lines: list[str]) -> list[str]:
    result: list[str] = []
    index = 0
    while index < len(lines):
        if FALSE_RELEASE_RE.match(lines[index]):
            _, false_part, next_index = split_conditional(lines, index)
            result.extend(filter_release(false_part))
            index = next_index
            continue
        result.append(lines[index])
        index += 1
    return result


def strip_cpp_comments(source: str) -> str:
    """Strip C++ comments while retaining library source URL comments."""
    result: list[str] = []
    index = 0
    while index < len(source):
        character = source[index]
        following = source[index + 1] if index + 1 < len(source) else ""

        if character == "/" and following == "/":
            end = source.find("\n", index)
            if end == -1:
                end = len(source)
            comment = source[index:end]
            if comment.startswith(LIBRARY_URL_PREFIX):
                result.append(comment)
            if end < len(source):
                result.append("\n")
            index = end + (end < len(source))
            continue

        if character == "/" and following == "*":
            result.append(" ")
            index += 2
            while index < len(source):
                if source[index] == "\n":
                    result.append("\n")
                    index += 1
                    continue
                if source[index:index + 2] == "*/":
                    index += 2
                    break
                index += 1
            continue

        if character == "R" and following == '"':
            start = index
            delimiter_end = index + 2
            while (
                delimiter_end < len(source)
                and source[delimiter_end] not in ("(", "\n")
            ):
                delimiter_end += 1
            if delimiter_end >= len(source) or source[delimiter_end] != "(":
                result.append(character)
                index += 1
                continue
            delimiter = source[start + 2:delimiter_end]
            terminator = ")" + delimiter + '"'
            end = source.find(terminator, delimiter_end + 1)
            if end == -1:
                result.append(source[start:])
                break
            end += len(terminator)
            result.append(source[start:end])
            index = end
            continue

        if character in ("'", '"'):
            quote = character
            result.append(character)
            index += 1
            while index < len(source):
                result.append(source[index])
                if source[index] == "\\":
                    index += 1
                    if index < len(source):
                        result.append(source[index])
                elif source[index] == quote:
                    index += 1
                    break
                index += 1
            continue

        result.append(character)
        index += 1
    return "".join(result)


def load_expand_acl(input_path: Path, root: Path) -> bool:
    candidates = [input_path.parent / ".contest.json", root / "contest.json"]
    for path in candidates:
        if not path.is_file():
            continue
        try:
            config = json.loads(path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError) as error:
            raise ExpandError(f"failed to read contest config {path}: {error}") from error
        if "acl_available" in config:
            return not bool(config["acl_available"])
        if "expand_acl" in config:
            return bool(config["expand_acl"])
    return True


class Expander:
    def __init__(
        self,
        root: Path,
        *,
        release: bool,
        comments: bool,
        line_directive: bool,
        expand_acl: bool,
        strip_included_comments: bool,
    ) -> None:
        self.root = root.resolve()
        self.release = release
        self.comments = comments
        self.line_directive = line_directive
        self.expand_acl = expand_acl
        self.strip_included_comments = strip_included_comments
        self.seen: set[Path] = set()

    def resolve_include(self, include: str, current: Path) -> Path:
        candidates = [current.parent / include, self.root / include]
        if include.startswith("atcoder/"):
            candidates.append(self.root / "third_party/ac-library" / include)
        for candidate in candidates:
            if candidate.is_file():
                return candidate.resolve()
        raise ExpandError(
            f'include not found: "{include}" from {relative(current, self.root)}'
        )

    def resolve_atcoder_include(self, include: str) -> Path | None:
        if not include.startswith("atcoder/"):
            return None
        base = self.root / "third_party/ac-library" / include
        for candidate in (base, base.with_suffix(".hpp")):
            if candidate.is_file():
                return candidate.resolve()
        return None

    def expand_file(
        self, path: Path, stack: list[Path], *, is_root: bool = False
    ) -> list[str]:
        path = path.resolve()
        if path in stack:
            cycle = stack[stack.index(path):] + [path]
            raise ExpandError(
                "include cycle: "
                + " -> ".join(relative(item, self.root) for item in cycle)
            )
        if not is_root and path in self.seen:
            return []
        if not is_root:
            self.seen.add(path)

        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except OSError as error:
            raise ExpandError(
                f"failed to read {relative(path, self.root)}: {error}"
            ) from error
        if self.release:
            lines = filter_release(lines)

        output: list[str] = []
        if self.line_directive:
            output.append(f'#line 1 "{relative(path, self.root)}"')

        stack.append(path)
        for line in lines:
            if PRAGMA_ONCE_RE.match(line):
                continue

            quote_match = QUOTE_INCLUDE_RE.match(line)
            system_match = SYSTEM_INCLUDE_RE.match(line)
            include = (
                quote_match.group(2)
                if quote_match
                else system_match.group(2)
                if system_match
                else None
            )
            if include is None:
                output.append(line)
                continue

            if include.startswith("atcoder/") and not self.expand_acl:
                output.append(line)
                continue

            if quote_match:
                included = self.resolve_include(include, path)
            else:
                included = self.resolve_atcoder_include(include)
                if included is None:
                    output.append(line)
                    continue

            if included in self.seen:
                continue
            name = relative(included, self.root)
            if self.comments:
                output.append(f"// ===== begin: {name} =====")
            output.extend(self.expand_file(included, stack))
            if self.comments:
                output.append(f"// ===== end: {name} =====")
        stack.pop()
        if not is_root and self.strip_included_comments:
            stripped = strip_cpp_comments("\n".join(output) + "\n")
            output = stripped.splitlines()
        return output


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input")
    parser.add_argument("-o", "--output", required=True)
    parser.add_argument("--root", type=Path, default=Path("."))
    parser.add_argument("--release", action="store_true")
    parser.add_argument("--line-directive", action="store_true")
    parser.add_argument("--no-comments", action="store_true")
    parser.add_argument("--strip-comments", action="store_true")
    parser.add_argument("--strip-blank-lines", action="store_true")
    acl = parser.add_mutually_exclusive_group()
    acl.add_argument("--expand-acl", action="store_true", dest="expand_acl")
    acl.add_argument("--keep-acl", action="store_false", dest="expand_acl")
    parser.set_defaults(expand_acl=None)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    root = args.root.resolve()
    input_path = Path(args.input)
    if not input_path.is_absolute():
        input_path = root / input_path
    input_path = input_path.resolve()
    output_path = Path(args.output)
    if not output_path.is_absolute():
        output_path = root / output_path

    expand_acl = (
        load_expand_acl(input_path, root)
        if args.expand_acl is None
        else args.expand_acl
    )
    expander = Expander(
        root,
        release=args.release,
        comments=not args.no_comments,
        line_directive=args.line_directive,
        expand_acl=expand_acl,
        strip_included_comments=args.strip_comments,
    )
    lines = expander.expand_file(input_path, [], is_root=True)
    source = "\n".join(lines).rstrip() + "\n"
    if args.strip_blank_lines:
        source = "\n".join(
            line for line in source.splitlines() if line.strip()
        ) + "\n"

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(source, encoding="utf-8")
    action = "expanded" if expand_acl else "kept"
    print(f"Wrote {output_path} (ACL includes: {action})")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (ExpandError, OSError, ValueError) as error:
        print(f"expand_include.py: {error}", file=sys.stderr)
        raise SystemExit(1)
