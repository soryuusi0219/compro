"""Regression tests for the verification workflow's bounded matrix."""

import json
from pathlib import Path
import subprocess
import sys
import unittest

from verify_shards import shard_matrix


class VerifyShardsTest(unittest.TestCase):
    def test_larger_than_github_matrix_limit(self):
        targets = [{'name': f'unit/test_{i}', 'kind': 'unit'} for i in range(296)]
        shards = shard_matrix(targets)['include']
        self.assertLessEqual(len(shards), 32)
        self.assertTrue(all(shard['targets'] for shard in shards))
        names = [name for shard in shards for name in shard['targets']]
        self.assertCountEqual(names, [target['name'] for target in targets])

    def test_variants_share_a_stable_shard(self):
        target = {'name': 'library_checker/example', 'kind': 'online', 'problem': 'other/example'}
        variant = {'name': 'library_checker/example_alternative', 'kind': 'online',
                   'problem': 'other/example', 'url': 'https://judge.yosupo.jp/problem/example'}
        other = {'name': 'unit/example', 'kind': 'unit'}
        shard = shard_matrix([variant])['include'][0]['shard']
        groups = {entry['shard']: entry['targets'] for entry in shard_matrix([target, variant, other])['include']}
        self.assertIn(target['name'], groups[shard])
        self.assertIn(variant['name'], groups[shard])

    def test_empty_plan_emits_no_jobs(self):
        script = Path(__file__).with_name('verify_shards.py')
        output = subprocess.check_output([sys.executable, script], input='\n', text=True)
        matrix, count = output.splitlines()
        self.assertEqual(count, 'count=0')
        self.assertEqual(json.loads(matrix.removeprefix('matrix=')), {'include': []})


if __name__ == '__main__':
    unittest.main()
