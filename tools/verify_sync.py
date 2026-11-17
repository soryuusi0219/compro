"""Explicit receipt synchronization using an independent Git branch."""
from __future__ import annotations

import os
from pathlib import Path
import subprocess
from tempfile import TemporaryDirectory

from verify_cache import ROOT, CacheError, Store


def synchronize(store, *, remote='origin', branch='verify-results', push=False):
    if subprocess.run(['git', 'check-ref-format', '--branch', branch], capture_output=True).returncode:
        raise CacheError('invalid results branch')
    if branch in {'main', 'master'}:
        raise CacheError('use a dedicated results branch')
    source = subprocess.run(['git', 'remote', 'get-url', '--', remote], cwd=ROOT, text=True, capture_output=True)
    if source.returncode:
        raise CacheError(f'unknown remote: {remote}')
    url = source.stdout.strip()
    # Reuse checkout's auth configuration without writing credentials to the
    # temporary repository or exposing them in command-line/log output.
    env = os.environ.copy()
    config = subprocess.run(['git', 'config', '--get-regexp', r'^(http\..*\.extraheader|credential\..*|credential.helper)$'], cwd=ROOT, capture_output=True, text=True)
    count = int(env.get('GIT_CONFIG_COUNT', '0'))
    for line in config.stdout.splitlines():
        key, _, value = line.partition(' ')
        env[f'GIT_CONFIG_KEY_{count}'] = key
        env[f'GIT_CONFIG_VALUE_{count}'] = value
        count += 1
    env['GIT_CONFIG_COUNT'] = str(count)
    with TemporaryDirectory(prefix='compro-verify-sync-') as directory:
        temporary = Path(directory)
        def git(*arguments, check=True):
            result = subprocess.run(['git', *arguments], cwd=temporary, env=env, text=True, capture_output=True)
            if check and result.returncode:
                # Remote errors may contain credentials, so report the operation only.
                raise CacheError(f'result synchronization failed: git {arguments[0]} (exit {result.returncode})')
            return result
        git('init', '-q')
        git('config', 'user.name', 'Verification results')
        git('config', 'user.email', 'verify@users.noreply.github.com')
        for attempt in range(4):
            exists = git('ls-remote', '--exit-code', '--heads', url, f'refs/heads/{branch}', check=False)
            if exists.returncode not in (0, 2):
                raise CacheError('cannot query shared verification results')
            if exists.returncode == 0:
                git('fetch', '--depth=1', url, f'refs/heads/{branch}')
                files = git('ls-tree', '-r', '--name-only', 'FETCH_HEAD').stdout.splitlines()
                if any(not name.startswith('records/') or not name.endswith('.json') for name in files):
                    raise CacheError('results branch contains source files; refusing to update it')
                modes = git('ls-tree', '-r', 'FETCH_HEAD').stdout.splitlines()
                if any(not entry.startswith('100644 blob ') for entry in modes):
                    raise CacheError('results branch contains non-regular files')
                git('checkout', '--detach', '-f', 'FETCH_HEAD')
                # Only regular, content-validated JSON receipts are imported.
                shared = Store(temporary)
                for record in shared.records():
                    store.merge(record)
            elif not push:
                print('No shared verification results yet.')
                return
            shared = Store(temporary)
            if not push:
                print('Fetched shared verification results.')
                return
            for record in store.records():
                shared.merge(record)
            if not any(shared.records()):
                print('No successful receipts to publish.')
                return
            git('add', '--', 'records')
            changed = git('diff', '--cached', '--quiet', check=False).returncode
            if not changed:
                print('Shared verification results are up to date.')
                return
            git('commit', '-q', '-m', 'Update successful verification receipts')
            published = git('push', url, f'HEAD:refs/heads/{branch}', check=False)
            if published.returncode == 0:
                print('Published successful verification receipts.')
                return
            # Another publisher may have advanced the branch; fetch its results
            # and merge again. Never force-push or replace another publisher.
        raise CacheError('could not publish receipts after four attempts')
