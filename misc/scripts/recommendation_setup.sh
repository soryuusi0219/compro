#!/bin/bash

set -e

if [ $# -ne 1 ]; then
  echo "Usage: $0 NAME" >&2
  exit 1
fi

if [[ -z "$1" || "$1" == */* || "$1" == "." || "$1" == ".." ]]; then
  echo "Error: name must not contain a directory path." >&2
  exit 1
fi

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
TEMPLATE="$SCRIPT_DIR/git/compro/templates/main.cpp"
DESTINATION="$SCRIPT_DIR/Recommended/$1.cpp"

if [ ! -d "$SCRIPT_DIR/Recommended" ]; then
  echo "Error: directory 'Recommended' does not exist." >&2
  exit 1
fi

if [ ! -f "$TEMPLATE" ]; then
  echo "Error: template not found: $TEMPLATE" >&2
  exit 1
fi

if [ -e "$DESTINATION" ]; then
  echo "Error: file 'Recommended/$1.cpp' already exists." >&2
  exit 1
fi

cp -p -- "$TEMPLATE" "$DESTINATION"
echo "Successfully made 'Recommended/$1.cpp'." >&2

exit 0
