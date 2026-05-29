#!/usr/bin/env sh
set -eu
EXEC_DIR="$(CDPATH='' cd "$(dirname "$0")" && pwd)"
exec "$EXEC_DIR/install.sh" x64
