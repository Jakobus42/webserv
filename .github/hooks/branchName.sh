#!/usr/bin/env bash

prefixes=("feat" "bugfix" "hotfix" "chore" "sandbox")

branch_name=$(git rev-parse --abbrev-ref HEAD)

valid_prefix=false

for prefix in "${prefixes[@]}"; do
  if [[ $branch_name == "$prefix/"* ]]; then
    valid_prefix=true
    break
  fi
done

if [ "$valid_prefix" == false ]; then
  echo "Invalid branch name."
  echo "Branch name should start with one of the following prefixes: ${prefixes[*]}"
  echo "Example: 'feat/hooks'"
  exit 1
fi
