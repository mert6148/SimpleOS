#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
if command -v g++ >/dev/null 2>&1; then
  g++ -std=c++17 -O2 -Wall -Wextra -o table_cli TableDB.cpp table_cli.cpp
  g++ -std=c++17 -O2 -Wall -Wextra -o print TableDB.cpp print.cpp
  echo "Built table_cli and print"
else
  echo "g++ not found. Install GCC/MinGW or use MSVC on Windows." >&2
  exit 1
fi
