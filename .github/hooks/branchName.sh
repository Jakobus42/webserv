#!/usr/bin/env bash

prefixes=("feat" "bugfix" "hotfix" "chore" "sandbox")

branch_name=$(git rev-parse --abbrev-ref HEAD)

if [[ "$branch_name" == "docs" ]]; then
  exit 0
fi

valid_prefix=false

for prefix in "${prefixes[@]}"; do
  if [[ $branch_name == "$prefix/"* ]]; then
    valid_prefix=true
    break
  fi
done

if [ "$valid_prefix" == false ]; then
  echo "Invalid branch name."
  echo "Branch name should start with one of the following prefixes: ${prefixes[*]}, or be named 'docs'."
  echo "Example: 'feat/hooks'"
  exit 1
fi