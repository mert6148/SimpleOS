#!/usr/bin/env sh
set -eu
here="$(CDPATH="" cd "$(dirname "$0")" && pwd)"
cd "$here/.."

target="${1:-all}"
exec make "${target}"
