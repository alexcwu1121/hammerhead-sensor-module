#!/usr/bin/env bash
#
# Start gdbserver on the Hammerhead Sensor Module for a deployed application.
# Runs in the foreground; the application's stdout/stderr stream back here.
#
# Usage: gdbserver.sh <remote_binary> [port] [args...]
#
# Environment:
#   HAMMERHEAD_HOST  target address (default: 192.168.4.1)
#   HAMMERHEAD_USER  target user    (default: root)

set -euo pipefail

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <remote_binary> [port] [args...]" >&2
    exit 1
fi

REMOTE_BINARY="$1"
PORT="${2:-2323}"
shift $(( $# >= 2 ? 2 : 1 ))

# shellcheck source=target-env.sh
source "$(dirname "${BASH_SOURCE[0]}")/target-env.sh"

# Kill any stale gdbserver still holding the port, then serve the binary
exec ssh "${SSH_OPTS[@]}" "${TARGET}" \
    "(killall -q gdbserver || true); exec gdbserver :${PORT} '${REMOTE_BINARY}' $*"
