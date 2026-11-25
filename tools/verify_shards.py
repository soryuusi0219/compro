"""Group a verify.py --plan target list into a bounded GitHub Actions matrix."""

import hashlib
import json
import sys

from verify import load_targets, select_targets
from verify_cache import problem_url


SHARD_COUNT = 32


def shard_matrix(targets):
    shards = {}
    for target in targets:
        name = target['name']
        # Variants of one problem can reuse the generated cases in one job.
        key = problem_url(target) if target['kind'] == 'online' else name
        shard = int(hashlib.sha256(key.encode()).hexdigest(), 16) % SHARD_COUNT
        shards.setdefault(shard, []).append(name)
    return {'include': [dict(shard=shard, targets=names) for shard, names in sorted(shards.items())]}


def main():
    names = [name for line in sys.stdin if (name := line.strip())]
    targets = select_targets(load_targets(), names) if names else []
    print('matrix=' + json.dumps(shard_matrix(targets), separators=(',', ':')))
    print(f'count={len(targets)}')


if __name__ == '__main__':
    main()
