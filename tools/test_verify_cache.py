"""End-to-end cache invalidation, execution, and Git synchronization tests."""
from contextlib import redirect_stdout, redirect_stderr
from datetime import datetime, timedelta, timezone
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
from tempfile import TemporaryDirectory
import unittest
from unittest.mock import patch

import library_manager
import verify
import verify_cache as cache
import verify_online
import verify_sync


class VerificationCacheTest(unittest.TestCase):
    def setUp(self):
        self.directory = TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name).resolve()
        values = [(cache, 'ROOT', self.root), (verify, 'ROOT', self.root),
                  (verify_sync, 'ROOT', self.root), (library_manager, 'ROOT', self.root),
                  (verify, 'UNIT_DIR', self.root / 'verify/unit'),
                  (verify, 'MANIFEST_DIRS', ()),
                  (verify, 'BUILD_DIR', self.root / 'build/verify/oj'),
                  (verify, 'EXPANDED_DIR', self.root / 'build/expanded')]
        for module, name, value in values:
            p = patch.object(module, name, value)
            p.start()
            self.addCleanup(p.stop)
        environment = patch.dict(os.environ, {'CXX': 'g++', 'VERIFY_CXXFLAGS': ''})
        environment.start()
        self.addCleanup(environment.stop)
        self.write('headers/value.hpp', '#define VALUE 0\n')
        self.write('verify/unit/a.cpp', '#include "headers/value.hpp"\nint main(){return VALUE;}\n')
        self.write('verify/unit/b.cpp', 'int main(){return 0;}\n')
        for name in ('verify.py', 'verify_cache.py', 'verify_online.py'):
            self.write('tools/' + name, '// verifier revision\n')
        self.write('requirements-verify.txt', '')
        cache.atomic_json(self.root / 'verify/cases.json', {'max_age_days': 30, 'aoj_epoch': 1, 'library_checker_revision': 'a' * 40})
        self.target = {'name': 'unit/a', 'kind': 'unit', 'source': 'verify/unit/a.cpp', 'problem': 'local/unit'}
        self.fingerprints = cache.Fingerprints()
        self.store = cache.Store()

    def write(self, name, text):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text)
        return path

    def fingerprint(self):
        return self.fingerprints.fingerprint(self.target)

    def cli(self, *arguments):
        out, err = io.StringIO(), io.StringIO()
        with patch('sys.argv', ['verify.py', *arguments]), redirect_stdout(out), redirect_stderr(err):
            code = verify.main()
        return code, out.getvalue(), err.getvalue()

    def git(self, *args, cwd=None):
        return subprocess.check_output(['git', *args], cwd=cwd or self.root, text=True, stderr=subprocess.DEVNULL).strip()

    def test_real_execution_then_skip(self):
        code, _, _ = self.cli('unit/a')
        self.assertEqual(code, 0)
        with patch.object(verify, 'run_unit_target', side_effect=AssertionError('must skip')):
            code, _, err = self.cli('unit/a')
        self.assertEqual(code, 0)
        self.assertIn('SKIP unit/a', err)

    def test_dependency_edit_invalidates_and_reverting_reuses(self):
        before = self.fingerprint()
        self.store.save(before)
        self.write('headers/value.hpp', '#define VALUE 1\n')
        self.assertIsNone(self.store.lookup(self.fingerprint()))
        self.write('headers/value.hpp', '#define VALUE 0\n')
        self.assertIsNotNone(self.store.lookup(self.fingerprint()))

    def test_timestamps_and_unrelated_document_do_not_invalidate(self):
        before = self.fingerprint()
        self.store.save(before)
        os.utime(self.root / self.target['source'], (10000, 10000))
        self.write('README.md', 'documentation update')
        self.assertEqual(before['key'], self.fingerprint()['key'])
        self.assertIsNotNone(self.store.lookup(self.fingerprint()))

    def test_receipt_key_survives_different_checkout_directory(self):
        before = self.fingerprint()
        with TemporaryDirectory() as directory:
            clone = Path(directory) / 'clone'
            shutil.copytree(self.root, clone)
            with patch.object(cache, 'ROOT', clone):
                after = cache.Fingerprints().fingerprint(self.target)
            self.assertEqual(before['key'], after['key'])

    def test_flags_environment_timeout_and_mode_are_separate(self):
        before = self.fingerprint()
        with patch.dict(os.environ, {'VERIFY_CXXFLAGS': '-DNDEBUG'}):
            self.assertNotEqual(before['key'], self.fingerprint()['key'])
        with patch.dict(os.environ, {'VERIFY_ENVIRONMENT': 'another-platform'}):
            self.assertNotEqual(before['key'], cache.Fingerprints().fingerprint(self.target)['key'])
        self.assertNotEqual(before['key'], cache.Fingerprints(timeout=1).fingerprint(self.target)['key'])
        self.write('build/expanded/verify/unit/a.cpp', 'int main(){return 0;}')
        self.assertNotEqual(before['key'], cache.Fingerprints(expanded=True).fingerprint(self.target)['key'])

    def test_verifier_changes_invalidate_but_report_changes_do_not(self):
        before = self.fingerprint()
        self.write('tools/library_manager.py', '# report update\n')
        self.assertEqual(before['key'], self.fingerprint()['key'])
        self.write('tools/verify.py', '# different execution\n')
        self.assertNotEqual(before['key'], self.fingerprint()['key'])

    def test_all_template_configurations_are_dependencies(self):
        self.target.update(name='unit/templates')
        self.write('verify/unit/a.cpp', '#if TEMPLATE_KIND == 0\n#include "templates/zero.cpp"\n#elif TEMPLATE_KIND == 1\n#include "templates/one.cpp"\n#else\n#include "templates/two.cpp"\n#endif\n')
        for name in ('zero', 'one', 'two'):
            self.write('templates/' + name + '.cpp', 'int main(){return 0;}')
        before = self.fingerprint()
        self.assertEqual(before['payload']['configurations'], [0, 1, 2])
        self.write('templates/two.cpp', 'int main(){return 1;}')
        self.assertNotEqual(before['key'], self.fingerprint()['key'])

    def test_macro_include_and_runtime_dependency(self):
        self.write('verify/unit/a.cpp', '#define HEADER "headers/value.hpp"\n#include HEADER\nint main(){return VALUE;}')
        self.target['dependencies'] = ['test-data.txt']
        self.write('test-data.txt', 'first')
        before = self.fingerprint()
        self.assertIn('headers/value.hpp', before['payload']['inputs'])
        self.write('test-data.txt', 'second')
        self.assertNotEqual(before['key'], self.fingerprint()['key'])

    def test_corrupt_receipts_are_never_hits(self):
        fingerprint = self.fingerprint()
        path = self.store.directory / 'records' / (fingerprint['key'] + '.json')
        cache.atomic_json(path, {'status': 'passed', 'key': fingerprint['key']})
        self.assertIsNone(self.store.lookup(fingerprint))
        record = self.store.save(fingerprint)
        record['payload']['inputs'] = {}
        cache.atomic_json(path, record)
        self.assertIsNone(self.store.lookup(fingerprint))

    def test_expired_online_receipt_is_not_reused(self):
        fingerprint = self.fingerprint()
        fingerprint['payload']['suite'] = {'kind': 'aoj'}
        fingerprint['key'] = cache.digest(fingerprint['payload'])
        record = self.store.save(fingerprint)
        record['verified_at'] = (datetime.now(timezone.utc) - timedelta(days=31)).isoformat()
        cache.atomic_json(self.store.directory / 'records' / (record['key'] + '.json'), record)
        self.assertIsNone(self.store.lookup(fingerprint))

    def test_old_cases_expire_even_after_recent_success(self):
        fingerprint = self.fingerprint()
        fingerprint['payload']['suite'] = {'kind': 'aoj'}
        fingerprint['key'] = cache.digest(fingerprint['payload'])
        evidence = {'suite': {'kind': 'aoj'}, 'cases': 'a' * 64, 'downloaded_at': cache.now()}
        self.store.save(fingerprint, evidence=evidence)
        self.assertIsNotNone(self.store.lookup(fingerprint))
        evidence['downloaded_at'] = (datetime.now(timezone.utc) - timedelta(days=31)).isoformat()
        self.store.save(fingerprint, evidence=evidence)
        self.assertIsNone(self.store.lookup(fingerprint))

    def test_force_runs_even_with_success(self):
        self.store.save(self.fingerprint())
        with patch.object(verify, 'run_unit_target', return_value=0) as run:
            self.assertEqual(self.cli('unit/a', '--force')[0], 0)
            run.assert_called_once()

    def test_failed_target_has_no_receipt_and_other_success_is_preserved(self):
        self.write('verify/unit/a.cpp', 'int main(){return 1;}')
        self.assertEqual(self.cli('unit/a', 'unit/b')[0], 1)
        self.assertIsNone(self.store.lookup(self.fingerprint()))
        self.assertEqual([r['payload']['target'] for r in self.store.records() if r['status'] == 'passed'], ['unit/b'])

    def test_changed_inputs_during_run_do_not_receive_receipt(self):
        def run(*args):
            self.write('headers/value.hpp', '#define VALUE 1\n')
            return 0
        with patch.object(verify, 'run_unit_target', side_effect=run):
            code, _, err = self.cli('unit/a')
        self.assertEqual(code, 1)
        self.assertIn('inputs changed', err)
        self.assertFalse(any(r['status'] == 'passed' for r in self.store.records()))

    def test_failed_force_invalidates_older_success_even_after_merge(self):
        before = self.fingerprint()
        previous = self.store.save(before)
        with patch.object(verify, 'run_unit_target', return_value=1):
            self.assertEqual(self.cli('unit/a', '--force')[0], 1)
        self.assertIsNone(self.store.lookup(before))
        self.assertFalse(self.store.merge(previous))
        self.assertIsNone(self.store.lookup(before))

    def test_plan_omits_successful_targets_and_emits_zero_matrix(self):
        self.store.save(self.fingerprint())
        code, out, err = self.cli('unit/a', '--plan', '--format', 'github')
        self.assertEqual(code, 0)
        self.assertIn('count=0', out)
        self.assertEqual(json.loads(out.splitlines()[0].split('=', 1)[1]), {'target': ['']})
        self.assertIn('SKIP', err)
        self.assertFalse((self.root / 'build/verify/oj/unit/a').exists())

    def test_completion_marker_never_trusts_helper_timestamps(self):
        marker = verify_online.CompletionMarker()
        source = self.root / self.target['source']
        self.assertFalse(marker.is_verified(source))
        marker.mark_verified(source)
        self.assertIn(source, marker.passed)
        self.assertFalse(marker.is_verified(source))
        marker.mark_failed(source)
        self.assertEqual(marker.passed, set())

    def test_git_sync_creates_dedicated_branch_and_merges_records(self):
        remote = self.root / 'remote.git'
        self.git('init', '--bare', '-q', str(remote))
        self.git('init', '-q')
        self.git('config', 'user.name', 'Test')
        self.git('config', 'user.email', 'test@example.com')
        self.git('add', '.')
        self.git('commit', '-qm', 'source')
        self.git('remote', 'add', 'origin', str(remote))
        source_head = self.git('rev-parse', 'HEAD')
        self.store.save(self.fingerprint())
        verify_sync.synchronize(self.store, push=True)
        other = cache.Store(self.root / 'other-results')
        verify_sync.synchronize(other)
        self.assertEqual(len(list(other.records())), 1)
        target = dict(self.target, name='unit/b', source='verify/unit/b.cpp')
        other.save(self.fingerprints.fingerprint(target))
        verify_sync.synchronize(other, push=True)
        verify_sync.synchronize(self.store)
        self.assertEqual(len(list(self.store.records())), 2)
        self.assertEqual(self.git('rev-parse', 'HEAD'), source_head)
        files = self.git('--git-dir', str(remote), 'ls-tree', '--name-only', 'verify-results')
        self.assertEqual(files, 'records')

    def test_sync_reports_missing_remote(self):
        self.git('init', '-q')
        with self.assertRaisesRegex(cache.CacheError, 'unknown remote: origin'):
            verify_sync.synchronize(self.store)

    def test_sync_reports_ownership_error_instead_of_missing_remote(self):
        self.git('init', '-q')
        self.git('remote', 'add', 'origin', str(self.root / 'remote.git'))
        with patch.dict(os.environ, {
            'GIT_TEST_ASSUME_DIFFERENT_OWNER': '1',
            'GIT_CONFIG_GLOBAL': str(self.root / 'isolated.gitconfig'),
            'GIT_CONFIG_SYSTEM': '/dev/null', 'GIT_CONFIG_COUNT': '0',
            'LC_ALL': 'C',
        }):
            with self.assertRaisesRegex(cache.CacheError, 'ownership.*safe.directory'):
                verify_sync.synchronize(self.store)

    def test_workflow_safe_directory_restores_access_after_checkout(self):
        self.git('init', '-q')
        url = str(self.root / 'remote.git')
        self.git('remote', 'add', 'origin', url)
        with patch.dict(os.environ, {
            'GIT_TEST_ASSUME_DIFFERENT_OWNER': '1',
            'GIT_CONFIG_GLOBAL': str(self.root / 'isolated.gitconfig'),
            'GIT_CONFIG_SYSTEM': '/dev/null', 'GIT_CONFIG_COUNT': '0',
            'GITHUB_WORKSPACE': str(self.root), 'LC_ALL': 'C',
        }):
            before = subprocess.run(['git', 'remote', 'get-url', 'origin'],
                                    cwd=self.root, capture_output=True, text=True)
            self.assertIn('detected dubious ownership', before.stderr)
            subprocess.run(['sh', '-ec', 'git config --global --add safe.directory "$GITHUB_WORKSPACE"'],
                           cwd=self.root, check=True)
            self.assertEqual(self.git('remote', 'get-url', 'origin'), url)
            self.assertEqual(self.git('config', '--global', '--get-all', 'safe.directory'), str(self.root))

    def test_concurrent_publish_refetches_and_preserves_both_writers(self):
        remote = self.root / 'remote.git'
        self.git('init', '--bare', '-q', str(remote))
        self.git('init', '-q')
        self.git('remote', 'add', 'origin', str(remote))
        self.store.save(self.fingerprint())
        verify_sync.synchronize(self.store, push=True)
        competing = cache.Store(self.root / 'competing')
        verify_sync.synchronize(competing)
        self.store.save(self.fingerprints.fingerprint(dict(self.target, name='unit/b')))
        competing.save(self.fingerprints.fingerprint(dict(self.target, name='unit/c')))
        real_run = subprocess.run
        injected = False
        def race(command, **kwargs):
            nonlocal injected
            if command[:2] == ['git', 'push'] and not injected:
                injected = True
                with patch.object(subprocess, 'run', real_run):
                    verify_sync.synchronize(competing, push=True)
            return real_run(command, **kwargs)
        with patch.object(subprocess, 'run', side_effect=race):
            verify_sync.synchronize(self.store, push=True)
        verify_sync.synchronize(self.store)
        self.assertTrue(injected)
        self.assertEqual(len(list(self.store.records())), 3)

    def test_timeout_does_not_save_a_pass(self):
        with patch.object(verify, 'run_unit_target', side_effect=subprocess.TimeoutExpired('unit/a', 1)):
            self.assertEqual(self.cli('unit/a', '--timeout', '1')[0], 1)
        self.assertFalse(any(r['status'] == 'passed' for r in self.store.records()))

    def test_case_policy_revision_and_aoj_epoch_invalidate_suite(self):
        target = dict(self.target, kind='online', problem='number_theory/primality_test')
        before = cache.suite(target)
        self.assertEqual(before['kind'], 'library_checker')
        policy = cache.case_policy()
        policy['library_checker_revision'] = 'b' * 40
        cache.atomic_json(self.root / 'verify/cases.json', policy)
        self.assertNotEqual(before, cache.suite(target))
        target['url'] = 'https://onlinejudge.u-aizu.ac.jp/problems/DSL_2_A'
        before = cache.suite(target)
        policy['aoj_epoch'] += 1
        cache.atomic_json(self.root / 'verify/cases.json', policy)
        self.assertNotEqual(before, cache.suite(target))

    def test_case_digest_detects_changed_and_incomplete_cases(self):
        directory = self.root / 'test'
        self.write('test/a.in', '1')
        self.assertIsNone(verify_online.cases_digest(directory))
        self.write('test/a.out', '2')
        before = verify_online.cases_digest(directory)
        self.assertIsNotNone(before)
        self.write('test/a.out', '3')
        self.assertNotEqual(before, verify_online.cases_digest(directory))

    def test_new_branch_and_missing_diff_base_use_merge_base(self):
        self.git('init', '-qb', 'main')
        self.git('config', 'user.name', 'Test')
        self.git('config', 'user.email', 'test@example.com')
        self.git('add', '.')
        self.git('commit', '-qm', 'initial')
        self.git('switch', '-qc', 'feature')
        self.write('README.md', 'feature')
        self.git('add', 'README.md')
        self.git('commit', '-qm', 'feature')
        self.assertEqual(library_manager.changed_paths('0' * 40, 'HEAD'), [('A', 'README.md')])
        self.assertEqual(library_manager.changed_paths('missing-sha', 'HEAD'), [('A', 'README.md')])


if __name__ == '__main__':
    unittest.main()
