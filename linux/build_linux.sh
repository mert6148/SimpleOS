#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "${SCRIPT_DIR}/x86"
make

cd "${SCRIPT_DIR}/x64"
make

cd "${SCRIPT_DIR}/distribution"
dotnet run --project SimpleOSLinuxDist.csproj -- --root "${SCRIPT_DIR}" --package

echo "Linux distribution build complete."
