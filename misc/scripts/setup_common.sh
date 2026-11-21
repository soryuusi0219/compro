#!/bin/bash

# Shared implementation for the contest setup scripts.
setup_contest() {
  local category=$1
  local contest_id=$2
  local problem_count=$3
  local template_name=${4:-main.cpp}
  local acl_available=${5:-true}
  local root_dir template tester_template contest_dir local_makefile contest_config i
  local -a problem_names

  root_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
  template="$root_dir/git/compro/templates/$template_name"
  tester_template="$root_dir/git/compro/templates/tester.cpp"
  contest_dir="$root_dir/$category/$contest_id"
  problem_names=({a..z})

  if ! [[ "$problem_count" =~ ^[1-9][0-9]*$ ]] || ((problem_count > 26)); then
    echo "Error: problem count must be an integer from 1 to 26." >&2
    return 1
  fi
  if [[ ! -d "$root_dir/$category" ]]; then
    echo "Error: directory '$category' does not exist." >&2
    return 1
  fi
  if [[ ! -f "$template" ]]; then
    echo "Error: template not found: $template" >&2
    return 1
  fi
  if [[ ! -f "$tester_template" ]]; then
    echo "Error: tester template not found: $tester_template" >&2
    return 1
  fi

  mkdir -p -- "$contest_dir"
  local_makefile="$contest_dir/Makefile"
  if [[ ! -e "$local_makefile" ]]; then
    printf 'include ../../Makefile\n' > "$local_makefile"
  fi
  contest_config="$contest_dir/.contest.json"
  if [[ ! -e "$contest_config" ]]; then
    printf '{\n  "acl_available": %s\n}\n' "$acl_available" > "$contest_config"
  fi
  for ((i = 0; i < problem_count; ++i)); do
    cp -n -p -- "$template" "$contest_dir/${problem_names[i]}.cpp"
  done
  cp -n -p -- "$tester_template" "$contest_dir/tester.cpp"

  echo "Set up $category/$contest_id with $problem_count problems."
}
