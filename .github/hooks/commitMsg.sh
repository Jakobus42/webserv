#!/usr/bin/env bash

prefixes=("feat" "fix" "chore" "docs" "test" "refactor" "style" "ci" "build" "merge")

commit_message=$(cat "$1")

valid_prefix=false

for prefix in "${prefixes[@]}"; do
  if [[ $commit_message == "$prefix:"* ]]; then
    valid_prefix=true
    break
  fi
done

if [ "$valid_prefix" == false ]; then
  echo "Invalid commit message format."
  echo "Commit messages must start with one of the following prefixes: ${prefixes[*]}"
  echo "Example: 'feat: add new logging feature'"
  exit 1
fi
