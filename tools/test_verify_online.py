"""Resource limits must apply to generation as well as tested submissions."""
import json
import os
from pathlib import Path
import subprocess
import sys
from tempfile import TemporaryDirectory
from types import ModuleType
import unittest
from unittest.mock import patch, Mock

import verify_online


class StackLimitTest(unittest.TestCase):
    @unittest.skipUnless(os.name == 'posix', 'POSIX stack limits')
    def test_soft_limit_is_raised_to_existing_hard_limit(self):
        import resource
        for hard in (resource.RLIM_INFINITY, 64 * 1024 * 1024):
            with (
                self.subTest(hard=hard),
                patch.object(resource, 'getrlimit', return_value=(8 * 1024 * 1024, hard)),
                patch.object(resource, 'setrlimit') as set_limit,
            ):
                self.assertTrue(verify_online.increase_stack_limit())
                set_limit.assert_called_once_with(resource.RLIMIT_STACK, (hard, hard))

    @unittest.skipUnless(os.name == 'posix', 'POSIX stack limits')
    def test_denied_limit_change_is_reported(self):
        import resource
        with patch.object(resource, 'setrlimit', side_effect=OSError('denied')), self.assertLogs(level='WARNING') as logs:
            self.assertFalse(verify_online.increase_stack_limit())
        self.assertIn('before case generation', logs.output[0])

    @unittest.skipUnless(os.name == 'posix', 'POSIX stack limits')
    def test_new_limit_is_inherited_by_generated_child_processes(self):
        script = '''
import json, resource, subprocess, sys
from verify_online import increase_stack_limit
_, hard = resource.getrlimit(resource.RLIMIT_STACK)
if hard != resource.RLIM_INFINITY and hard <= 1024 * 1024:
    sys.exit(77)
resource.setrlimit(resource.RLIMIT_STACK, (1024 * 1024, hard))
assert increase_stack_limit()
child = subprocess.check_output([sys.executable, '-c',
    'import json, resource; print(json.dumps(resource.getrlimit(resource.RLIMIT_STACK)))'], text=True)
assert json.loads(child) == [hard, hard], child
'''
        process = subprocess.run([sys.executable, '-c', script], cwd=Path(__file__).resolve().parent,
                                 capture_output=True, text=True)
        if process.returncode == 77:
            self.skipTest('hard limit already too small')
        self.assertEqual(process.returncode, 0, process.stderr)

    def test_stack_is_configured_before_case_preparation(self):
        package = ModuleType('onlinejudge_verify')
        package.config = ModuleType('onlinejudge_verify.config')
        package.config.set_config_path = Mock()
        package.verify = ModuleType('onlinejudge_verify.verify')
        events = []
        def prepare(*args, **kwargs):
            events.append('cases')
            raise RuntimeError('stop before generation')
        with TemporaryDirectory() as directory:
            request = Path(directory) / 'request.json'
            request.write_text(json.dumps({'url': 'https://judge.yosupo.jp/problem/jump_on_tree',
                                           'suite': {}, 'refresh': False}))
            with (
                patch.dict(sys.modules, {'onlinejudge_verify': package,
                                         'onlinejudge_verify.config': package.config,
                                         'onlinejudge_verify.verify': package.verify}),
                patch.object(sys, 'argv', ['verify_online.py', '--request', str(request),
                                          '--evidence', str(request.with_suffix('.evidence.json'))]),
                patch.object(verify_online, 'increase_stack_limit', side_effect=lambda: events.append('stack')),
                patch.object(verify_online, 'prepare_cases', side_effect=prepare),
            ):
                with self.assertRaisesRegex(RuntimeError, 'stop before generation'):
                    verify_online.main()
        self.assertEqual(events, ['stack', 'cases'])


if __name__ == '__main__':
    unittest.main()
