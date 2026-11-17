#!/usr/bin/env python3
"""Run the registered online judge and unit verification targets."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path, PurePosixPath
import signal
import subprocess
import sys
from typing import Any

from verify_cache import CacheError, Fingerprints, Store, atomic_json, compiler, configurations, flags, problem_url


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
        target["manifest"] = path.relative_to(ROOT).as_posix()
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
    cxx = os.environ.get("CXX", "g++")
    options = flags('online')
    CONFIG_PATH.parent.mkdir(parents=True, exist_ok=True)
    values = ", ".join(toml_string(flag) for flag in options)
    CONFIG_PATH.write_text(
        "[[languages.cpp.environments]]\n"
        f"CXX = {toml_string(cxx)}\n"
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


def run_unit_target(target: dict[str, Any], expanded: bool, timeout=None) -> int:
    name = str(target["name"])
    source = Path(str(target["source"]))
    if expanded:
        source = EXPANDED_DIR.relative_to(ROOT) / source
    if not (ROOT / source).is_file():
        raise VerificationError(f"Unit source was not found: {source}")

    binary_dir = BUILD_DIR / "unit"
    binary_dir.mkdir(parents=True, exist_ok=True)
    for configuration in configurations(target):
        suffix = f"-{configuration}" if configuration is not None else ""
        binary = binary_dir / f"{PurePosixPath(name).name}{suffix}"
        command = [*compiler(), *flags('unit')]
        if configuration is not None:
            command.append(f"-DTEMPLATE_KIND={configuration}")
        command.extend([str(source), "-o", str(binary.relative_to(ROOT))])
        print("+", " ".join(command), flush=True)
        compiled = subprocess.run(command, cwd=ROOT, check=False, timeout=timeout)
        if compiled.returncode != 0:
            return compiled.returncode
        run_command = [str(binary.relative_to(ROOT))]
        print("+", " ".join(run_command), flush=True)
        result = subprocess.run(run_command, cwd=ROOT, check=False, timeout=timeout)
        if result.returncode != 0:
            return result.returncode
    return 0


def run_online_target(target, args, fingerprint):
    write_config()
    source = write_test_source(target, args.expanded)
    request = BUILD_DIR / 'requests' / (fingerprint['key'] + '.json')
    evidence = request.with_suffix('.evidence.json')
    evidence.unlink(missing_ok=True)
    atomic_json(request, {'source': source.as_posix(), 'url': problem_url(target),
                         'suite': fingerprint['payload']['suite'], 'refresh': args.refresh_cases,
                         'tle': args.tle, 'jobs': args.jobs})
    command = [sys.executable, str(ROOT / 'tools/verify_online.py'), '--request', str(request), '--evidence', str(evidence)]
    process = subprocess.Popen(command, cwd=ROOT, start_new_session=os.name != 'nt')
    try:
        code = process.wait(timeout=args.timeout)
    except (subprocess.TimeoutExpired, KeyboardInterrupt):
        if os.name != 'nt':
            os.killpg(process.pid, signal.SIGKILL)
        else:
            process.kill()
        process.wait()
        raise
    return code, json.loads(evidence.read_text()) if code == 0 and evidence.exists() else None


def parse_args():
    parser = argparse.ArgumentParser(description='Run verification targets, reusing identical successful receipts')
    parser.add_argument('targets', nargs='*')
    parser.add_argument('--list', action='store_true')
    parser.add_argument('--expanded', action='store_true')
    parser.add_argument('--force', action='store_true', help='ignore successful receipts')
    parser.add_argument('--refresh-cases', action='store_true', help='download cases again and force verification')
    parser.add_argument('--plan', action='store_true', help='show targets without running tests')
    parser.add_argument('--format', choices=['text', 'github'], default='text')
    parser.add_argument('--results-dir', type=Path)
    parser.add_argument('--sync', choices=['pull', 'push'], help='synchronize receipts without running tests')
    parser.add_argument('--remote', default='origin')
    parser.add_argument('--results-branch', default='verify-results')
    parser.add_argument('--import-results', type=Path, help='merge receipt artifacts before synchronization')
    parser.add_argument('-j', '--jobs', type=int, default=1)
    parser.add_argument('--timeout', type=float, help='online target wall time / unit subprocess timeout in seconds')
    parser.add_argument('--tle', type=float, default=60.0)
    return parser.parse_args()


def main():
    args = parse_args()
    store = Store(args.results_dir)
    if args.import_results:
        for path in args.import_results.rglob('*.json'):
            try:
                store.merge(json.loads(path.read_text()))
            except (OSError, ValueError):
                continue
    if args.sync:
        from verify_sync import synchronize
        synchronize(store, remote=args.remote, branch=args.results_branch, push=args.sync == 'push')
        return 0
    targets = load_targets()
    if args.list:
        for target in targets:
            print(f"{target['name']}: {target['problem']} <- {target['source']}")
        return 0
    if args.jobs <= 0 or args.tle <= 0 or (args.timeout is not None and args.timeout <= 0):
        raise VerificationError('jobs, tle, and timeout must be positive')
    selected = select_targets(targets, args.targets)
    if not selected:
        raise VerificationError('No verification targets are configured')
    if args.expanded:
        # Always regenerate: stale generated sources must never receive a new receipt.
        import library_manager
        graph = library_manager.dependency_graph()
        library_manager.validate_graph(graph)
        library_manager.write_expanded_verify(graph, library_manager.load_verify_targets(graph), EXPANDED_DIR, [t['name'] for t in selected])
    fingerprints = Fingerprints(expanded=args.expanded, tle=args.tle, timeout=args.timeout, jobs=args.jobs)
    pending = []
    for target in selected:
        fingerprint = fingerprints.fingerprint(target)
        receipt = store.lookup(fingerprint)
        forced = args.force or args.refresh_cases
        reason = 'forced' if forced else 'successful receipt ' + fingerprint['key'][:12] if receipt else store.reason(fingerprint)
        skip = receipt is not None and not forced
        print(f"{'SKIP' if skip else 'RUN '} {target['name']}: {reason}", file=sys.stderr, flush=True)
        if not skip:
            pending.append((target, fingerprint))
    if args.plan:
        names = [target['name'] for target, _ in pending]
        if args.format == 'github':
            print('matrix=' + json.dumps({'target': names or ['']}, separators=(',', ':')))
            print(f'count={len(names)}')
        else:
            print('\n'.join(names))
        return 0
    failed = False
    for target, before in pending:
        # Revoke an older pass before starting a forced retry. An interruption
        # must not resurrect that pass on the next local run or synchronization.
        store.save(before, status='failed', evidence={'state': 'incomplete'})
        try:
            if target['kind'] == 'unit':
                result, evidence = run_unit_target(target, args.expanded, args.timeout), {'configurations': configurations(target)}
            else:
                result, evidence = run_online_target(target, args, before)
                if result == 0 and evidence is None:
                    raise VerificationError('helper did not confirm completed system tests')
            if result:
                store.save(before, status='failed', evidence={'exit_code': result})
                failed = True
                continue
            after = fingerprints.fingerprint(target)
            if before['key'] != after['key']:
                raise VerificationError(f"{target['name']}: inputs changed during verification; no receipt saved")
            store.save(after, evidence=evidence)
            print(f"PASS {target['name']}: saved {after['key'][:12]}", flush=True)
        except (CacheError, VerificationError, subprocess.TimeoutExpired, OSError) as error:
            store.save(before, status='failed', evidence={'error': str(error)})
            failed = True
            print(f"FAIL {target['name']}: {error}", file=sys.stderr, flush=True)
    return int(failed)


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (CacheError, VerificationError, OSError, subprocess.CalledProcessError) as error:
        print(f'verify: {error}', file=sys.stderr)
        raise SystemExit(1)
