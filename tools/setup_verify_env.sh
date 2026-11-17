#!/bin/sh
# Used by the CI container and tools/verify.Dockerfile.
set -eu
export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
apt-get install -y --no-install-recommends ca-certificates git make g++ python3 python3-venv
python3 -m venv /opt/verify
/opt/verify/bin/python -m pip install -r "${1:-requirements-verify.txt}"
