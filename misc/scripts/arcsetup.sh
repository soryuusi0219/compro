#!/bin/bash

set -e

if (( $# < 1 || $# > 2 )); then
  echo "Usage: $0 CONTEST_NUMBER [PROBLEM_COUNT]" >&2
  exit 1
fi

if ! [[ "$1" =~ ^[1-9][0-9]{,2}$ ]]; then
  echo "Usage: $0 CONTEST_NUMBER [PROBLEM_COUNT]" >&2
  exit 1
fi

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=setup_common.sh
source "$SCRIPT_DIR/setup_common.sh"
setup_contest ARC "$1" "${2:-6}"
