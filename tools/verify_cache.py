"""Content-addressed verification receipts shared by local runs and CI."""
from __future__ import annotations

from datetime import datetime, timezone
import hashlib
from importlib import metadata
import json
import os
from pathlib import Path
import platform
import re
import shlex
import subprocess
import sys
import tempfile
from urllib.parse import urlsplit

ROOT = Path(__file__).resolve().parents[1]
SCHEMA = 1
UNIT_FLAGS = ['-std=c++20', '-O2', '-Wall', '-Wextra', '-Werror', '-I.', '-Ithird_party/ac-library']
ONLINE_FLAGS = ['-std=gnu++20', '-O2', '-pipe', '-I.', '-Ithird_party/ac-library']


class CacheError(RuntimeError):
    pass


def digest(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def file_digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def atomic_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(mode='w', dir=path.parent, delete=False) as stream:
        temporary = Path(stream.name)
        json.dump(value, stream, sort_keys=True, indent=2)
        stream.write('\n')
    os.replace(temporary, path)


def now():
    return datetime.now(timezone.utc).isoformat()


def compiler():
    return shlex.split(os.environ.get('CXX', 'g++'))


def flags(kind):
    return (UNIT_FLAGS if kind == 'unit' else ONLINE_FLAGS) + shlex.split(os.environ.get('VERIFY_CXXFLAGS', ''))


def configurations(target):
    return [0, 1, 2] if target['name'] == 'unit/templates' else [None]


def case_policy():
    return json.loads((ROOT / 'verify/cases.json').read_text())


def problem_url(target):
    return target.get('url', 'https://judge.yosupo.jp/problem/' + target['problem'].split('/')[-1])


def suite(target):
    if target['kind'] == 'unit':
        return {'kind': 'unit'}
    policy = case_policy()
    if urlsplit(problem_url(target)).hostname == 'judge.yosupo.jp':
        return {'kind': 'library_checker', 'revision': policy['library_checker_revision'], 'url': problem_url(target)}
    return {'kind': 'aoj', 'epoch': policy['aoj_epoch'], 'url': problem_url(target)}


class Fingerprints:
    def __init__(self, *, expanded=False, tle=60.0, timeout=None, jobs=1):
        self.expanded = expanded
        self.conditions = {'tle': float(tle), 'timeout': timeout, 'jobs': jobs}
        self._environment = None
        self._hashes = {}

    def hash_file(self, path):
        stat = path.stat()
        stamp = (stat.st_size, stat.st_mtime_ns, stat.st_ctime_ns)
        if path not in self._hashes or self._hashes[path][0] != stamp:
            self._hashes[path] = (stamp, file_digest(path))
        return self._hashes[path][1]

    def environment(self, kind):
        if self._environment is None:
            def output(arguments, **kwargs):
                return subprocess.check_output([*compiler(), *arguments], cwd=ROOT, text=True, **kwargs).strip()
            macros = output(['-std=c++20', '-dM', '-E', '-x', 'c++', '-'], input='#include <version>\n')
            self._environment = {
                'system': platform.system(), 'machine': platform.machine(), 'libc': list(platform.libc_ver()),
                'os_release': Path('/etc/os-release').read_text() if Path('/etc/os-release').exists() else '',
                'command': compiler(), 'compiler': output(['--version']), 'target': output(['-dumpmachine']),
                'stdlib': [line for line in macros.splitlines() if re.match(r'#define (_GLIBCXX|__GLIBCXX|_LIBCPP|__GLIBC__|__GLIBC_MINOR__)', line)],
                'python': platform.python_version(),
                'include_environment': {name: os.environ.get(name, '') for name in ('CPATH', 'CPLUS_INCLUDE_PATH', 'C_INCLUDE_PATH', 'LIBRARY_PATH', 'LD_LIBRARY_PATH', 'SOURCE_DATE_EPOCH')},
                'profile': os.environ.get('VERIFY_ENVIRONMENT', ''),
            }
        result = dict(self._environment)
        if kind == 'online':
            result['packages'] = {}
            for package in ('online-judge-verify-helper', 'online-judge-tools', 'online-judge-api-client'):
                try:
                    result['packages'][package] = metadata.version(package)
                except metadata.PackageNotFoundError:
                    raise CacheError(f'{package} is required; install requirements-verify.txt')
        return result

    def fingerprint(self, target):
        original = ROOT / target['source']
        source = ROOT / 'build/expanded' / target['source'] if self.expanded else original
        dependencies = {original, source}
        # Compiler dependency analysis also covers templates, macro includes, and
        # conditional includes under each actual compilation configuration.
        for configuration in configurations(target):
            command = [*compiler(), *flags(target['kind'])]
            if configuration is not None:
                command.append(f'-DTEMPLATE_KIND={configuration}')
            command.extend(['-MM', '-MT', 'verify', str(source)])
            process = subprocess.run(command, cwd=ROOT, text=True, capture_output=True)
            if process.returncode:
                raise CacheError(f'{target["name"]}: dependency analysis failed\n{process.stderr}')
            words = shlex.split(process.stdout.replace('\\\n', ' '))
            dependencies.update((ROOT / item).resolve() for item in words[1:])
        if target.get('manifest'):
            dependencies.add(ROOT / target['manifest'])
        for name in target.get('dependencies', []):
            path = (ROOT / name).resolve()
            if not path.is_relative_to(ROOT):
                raise CacheError(f'{target["name"]}: dependency outside repository: {name}')
            dependencies.add(path)
        for name in ('tools/verify.py', 'tools/verify_cache.py'):
            dependencies.add(ROOT / name)
        if target['kind'] == 'online':
            dependencies.update(ROOT / name for name in ('tools/verify_online.py', 'requirements-verify.txt'))
        inputs = {}
        for path in sorted(dependencies):
            # Outside project includes still participate, without absolute paths
            # tying a receipt to a particular checkout directory.
            name = path.relative_to(ROOT).as_posix() if path.is_relative_to(ROOT) else 'external/' + path.as_posix().lstrip('/')
            inputs[name] = self.hash_file(path)
        payload = {
            'schema': SCHEMA, 'target': target['name'], 'inputs': inputs,
            'environment': self.environment(target['kind']), 'flags': flags(target['kind']),
            'configurations': configurations(target), 'expanded': self.expanded,
            'suite': suite(target), 'conditions': self.conditions if target['kind'] == 'online' else {'timeout': self.conditions['timeout']},
        }
        return {'key': digest(payload), 'payload': payload}


def valid_record(record):
    try:
        return (record['status'] in ('passed', 'failed') and record['payload']['schema'] == SCHEMA
                and re.fullmatch('[0-9a-f]{64}', record['key']) is not None
                and digest(record['payload']) == record['key']
                and datetime.fromisoformat(record['verified_at']).utcoffset() is not None
                and bool(record['payload']['target']))
    except (KeyError, TypeError, ValueError):
        return False


class Store:
    def __init__(self, directory=None):
        self.directory = Path(directory) if directory else ROOT / 'build/verify/results'

    def records(self):
        if (self.directory / 'records').is_symlink():
            return
        for path in self.directory.glob('records/*.json'):
            if path.is_symlink():
                continue
            try:
                record = json.loads(path.read_text())
            except (OSError, ValueError):
                continue
            if valid_record(record) and path.stem == record['key']:
                yield record

    def lookup(self, fingerprint):
        path = self.directory / 'records' / (fingerprint['key'] + '.json')
        try:
            record = json.loads(path.read_text())
        except (OSError, ValueError):
            return None
        if not valid_record(record) or record['status'] != 'passed' or record['payload'] != fingerprint['payload']:
            return None
        if record['payload']['suite']['kind'] != 'unit':
            evidence = record.get('evidence', {})
            try:
                if evidence['suite'] != record['payload']['suite'] or not re.fullmatch('[0-9a-f]{64}', evidence['cases']):
                    return None
                timestamps = [record['verified_at'], evidence['downloaded_at']]
                ages = [(datetime.now(timezone.utc) - datetime.fromisoformat(t)).total_seconds() for t in timestamps]
                if any(age < 0 or age > case_policy()['max_age_days'] * 86400 for age in ages):
                    return None
            except (KeyError, TypeError, ValueError):
                return None
        return record

    def merge(self, record):
        if not valid_record(record):
            return False
        path = self.directory / 'records' / (record['key'] + '.json')
        try:
            old = json.loads(path.read_text())
            if valid_record(old) and datetime.fromisoformat(old['verified_at']) >= datetime.fromisoformat(record['verified_at']):
                return False
        except (OSError, ValueError, KeyError):
            pass
        atomic_json(path, record)
        return True

    def save(self, fingerprint, *, evidence=None, status='passed'):
        commit = subprocess.run(['git', 'rev-parse', 'HEAD'], cwd=ROOT, capture_output=True, text=True)
        record = {**fingerprint, 'status': status, 'verified_at': now(),
                  'commit': commit.stdout.strip(), 'evidence': evidence or {},
                  'origin': os.environ.get('GITHUB_SERVER_URL', 'https://github.com') + '/' + os.environ['GITHUB_REPOSITORY'] + '/actions/runs/' + os.environ['GITHUB_RUN_ID'] if os.environ.get('GITHUB_RUN_ID') and os.environ.get('GITHUB_REPOSITORY') else 'local'}
        self.merge(record)
        return record

    def reason(self, fingerprint):
        previous = [r for r in self.records() if r['payload']['target'] == fingerprint['payload']['target']]
        if not previous:
            return 'no successful receipt'
        latest = max(previous, key=lambda r: r['verified_at'])
        if latest['status'] == 'failed' and latest['key'] == fingerprint['key']:
            return 'previous attempt failed'
        old = latest['payload']
        for field, label in (('inputs', 'source/dependencies changed'), ('environment', 'environment changed'), ('flags', 'compiler flags changed'), ('expanded', 'build mode changed'), ('suite', 'test suite changed'), ('conditions', 'execution conditions changed')):
            if old.get(field) != fingerprint['payload'].get(field):
                return label
        return 'receipt expired or invalid'
