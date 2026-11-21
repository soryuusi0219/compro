#!/bin/bash

set -e

if (( $# < 1 || $# > 2 )); then
  echo "Usage: $0 CONTEST_NAME [PROBLEM_COUNT]" >&2
  exit 1
fi

if [[ -z "$1" || "$1" == */* || "$1" == "." || "$1" == ".." ]]; then
  echo "Error: contest name must be a single directory name." >&2
  exit 1
fi

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=setup_common.sh
source "$SCRIPT_DIR/setup_common.sh"
setup_contest CF "$1" "${2:-14}" main_noACL.cpp false
