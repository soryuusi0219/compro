#!/usr/bin/env python3
"""Check one Markdown reference per library header and local documentation links."""

from pathlib import Path
import re
import sys
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parent.parent
REFERENCE = ROOT / "docs/reference"
SECTIONS = ("機能・前提条件", "インターフェース", "計算量", "依存関係", "Verify・利用例")


def check() -> list[str]:
    errors = []
    expected = {
        REFERENCE / path.relative_to(ROOT / "library").with_suffix(".md"): path
        for path in (ROOT / "library").rglob("*.hpp")
    }
    index = REFERENCE / "README.md"
    index_text = index.read_text() if index.exists() else ""
    if not index.exists():
        errors.append("missing docs/reference/README.md")
    for page, header in sorted(expected.items()):
        if not page.exists():
            errors.append(f"missing {page.relative_to(ROOT)}")
            continue
        content = page.read_text()
        for section in SECTIONS:
            if f"## {section}\n" not in content:
                errors.append(f"{page.relative_to(ROOT)}: missing section {section}")
        if str(header.relative_to(ROOT)) not in content:
            errors.append(f"{page.relative_to(ROOT)}: missing include path")
        if f"]({page.relative_to(REFERENCE)})" not in index_text:
            errors.append(f"{page.relative_to(ROOT)}: missing index entry")
    for page in sorted(REFERENCE.rglob("*.md")):
        if page != index and page not in expected:
            errors.append(f"orphan reference {page.relative_to(ROOT)}")
    documents = list(REFERENCE.rglob("*.md")) + [ROOT / "README.md", ROOT / "AGENTS.md", ROOT / "library/readme.md", ROOT / "tools/README.md"]
    for page in documents:
        if not page.exists():
            continue
        # Remove fenced examples so code containing Markdown-like text is ignored.
        content = re.sub(r"```.*?```", "", page.read_text(), flags=re.S)
        for target in re.findall(r"\]\(([^)]+)\)", content):
            target = target.strip("<>")
            parsed = urlsplit(target)
            if parsed.scheme or not parsed.path:
                continue
            destination = page.parent / unquote(parsed.path)
            if not destination.exists():
                errors.append(f"{page.relative_to(ROOT)}: broken link {target}")
        if "_document.txt" in content and page not in (ROOT / "AGENTS.md", index):
            errors.append(f"{page.relative_to(ROOT)}: stale txt reference")
    return errors


if __name__ == "__main__":
    problems = check()
    if problems:
        print("\n".join(problems), file=sys.stderr)
        sys.exit(1)
    count = len(list((ROOT / "library").rglob("*.hpp")))
    print(f"OK: {count} library references, required sections and local links")
