#!/usr/bin/env python3
"""Run one verification-helper target with a fresh, explicit completion marker."""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import logging
import os
from pathlib import Path
import shutil
import subprocess

from verify_cache import ROOT, CacheError, atomic_json, case_policy, digest, file_digest, now


def increase_stack_limit():
    # Generation also runs recursive reference solutions and generators. Match
    # verification-helper's stack policy before launching any of those children.
    try:
        import resource
        _, hard = resource.getrlimit(resource.RLIMIT_STACK)
        resource.setrlimit(resource.RLIMIT_STACK, (hard, hard))
    except (ImportError, OSError, ValueError) as error:
        logging.warning('Could not increase stack limit before case generation: %s', error)
        return False
    return True


def checker_repository():
    revision = case_policy()['library_checker_revision']
    override = os.environ.get('VERIFY_LIBRARY_CHECKER_REPOSITORY')
    path = Path(override).resolve() if override else ROOT / 'build/verify/library-checker' / revision
    if not path.exists():
        path.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(['git', 'init', '-q', str(path)], check=True)
    actual = subprocess.run(['git', '-C', str(path), 'rev-parse', 'HEAD'], capture_output=True, text=True)
    if actual.stdout.strip() != revision:
        if override:
            raise CacheError('VERIFY_LIBRARY_CHECKER_REPOSITORY must be at the pinned revision')
        subprocess.run(['git', '-C', str(path), 'fetch', '--depth=1', 'https://github.com/yosupo06/library-checker-problems.git', revision], check=True)
        subprocess.run(['git', '-C', str(path), 'checkout', '--detach', 'FETCH_HEAD'], check=True)
    # A supplied repository must also have clean tracked generator/checker files.
    clean = subprocess.run(['git', '-C', str(path), 'diff', '--quiet', 'HEAD'], check=False)
    if clean.returncode:
        raise CacheError(f'Library Checker sources are modified: {path}')
    return path


def cases_digest(directory):
    paths = sorted(directory.glob('*'))
    if not paths or not any(p.suffix == '.in' for p in paths):
        return None
    inputs = {p.stem for p in paths if p.suffix == '.in'}
    outputs = {p.stem for p in paths if p.suffix == '.out'}
    if inputs != outputs:
        return None
    return digest({p.name: file_digest(p) for p in paths if p.is_file()})


def prepare_cases(url, suite, *, refresh=False):
    import onlinejudge.dispatch
    from onlinejudge.service.library_checker import LibraryCheckerService
    if suite['kind'] == 'library_checker':
        repository = checker_repository()
        LibraryCheckerService._get_cloned_repository_path = classmethod(lambda cls: repository)
        LibraryCheckerService.is_repository_updated = True
    problem = onlinejudge.dispatch.problem_from_url(url)
    if problem is None:
        raise CacheError(f'unsupported problem: {url}')
    directory = ROOT / '.verify-helper/cache' / hashlib.md5(url.encode()).hexdigest()
    stamp_path = directory / 'suite.json'
    stamp = {}
    try:
        stamp = json.loads(stamp_path.read_text())
    except (OSError, ValueError):
        pass
    actual = cases_digest(directory / 'test')
    try:
        age = (datetime.now(timezone.utc) - datetime.fromisoformat(stamp['downloaded_at'])).total_seconds()
    except (KeyError, ValueError, TypeError):
        age = float('inf')
    if refresh or stamp.get('suite') != suite or not actual or actual != stamp.get('cases') or not 0 <= age < case_policy()['max_age_days'] * 86400:
        cases = problem.download_system_cases()
        if not cases:
            raise CacheError(f'no system cases available: {url}')
        shutil.rmtree(directory / 'test', ignore_errors=True)
        (directory / 'test').mkdir(parents=True)
        for i, case in enumerate(cases):
            if case.input_data is None or case.output_data is None:
                raise CacheError(f'incomplete system case: {url}')
            (directory / 'test' / f'case-{i:05}.in').write_bytes(case.input_data)
            (directory / 'test' / f'case-{i:05}.out').write_bytes(case.output_data)
        actual = cases_digest(directory / 'test')
        stamp = {'suite': suite, 'cases': actual, 'downloaded_at': now()}
        atomic_json(stamp_path, stamp)
    return directory, stamp


class CompletionMarker:
    """Never skip on verification-helper timestamps; require a completed pass."""
    def __init__(self):
        self.passed = set()

    def is_verified(self, path):
        return False

    def mark_verified(self, path):
        self.passed.add(path.resolve())

    def mark_failed(self, path):
        self.passed.discard(path.resolve())


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--request', type=Path, required=True)
    parser.add_argument('--evidence', type=Path, required=True)
    args = parser.parse_args()
    request = json.loads(args.request.read_text())
    import onlinejudge_verify.config
    import onlinejudge_verify.verify
    onlinejudge_verify.config.set_config_path(ROOT / 'build/verify/oj/config.toml')
    increase_stack_limit()
    directory, evidence = prepare_cases(request['url'], request['suite'], refresh=request['refresh'])
    marker = CompletionMarker()
    source = ROOT / request['source']
    before = cases_digest(directory / 'test')
    summary = onlinejudge_verify.verify.main([source], marker=marker, tle=request['tle'], jobs=request['jobs'])
    if not summary.succeeded() or source.resolve() not in marker.passed:
        return 1
    if before != cases_digest(directory / 'test'):
        raise CacheError('test cases changed during verification')
    atomic_json(args.evidence, evidence)
    return 0


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    try:
        raise SystemExit(main())
    except (CacheError, subprocess.CalledProcessError) as error:
        raise SystemExit(str(error))
