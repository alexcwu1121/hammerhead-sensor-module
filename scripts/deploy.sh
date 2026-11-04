#!/usr/bin/env bash
#
# Deploy an application binary to the Hammerhead Sensor Module over scp.
#
# Usage: deploy.sh <local_binary> [remote_dir]
#
# Environment:
#   HAMMERHEAD_HOST  target address (default: 192.168.4.1)
#   HAMMERHEAD_USER  target user    (default: root)

set -euo pipefail

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <local_binary> [remote_dir]" >&2
    exit 1
fi

BINARY="$1"
REMOTE_DIR="${2:-/opt/diagnostics}"
NAME="$(basename "${BINARY}")"

# shellcheck source=target-env.sh
source "$(dirname "${BASH_SOURCE[0]}")/target-env.sh"

if [[ ! -f "${BINARY}" ]]; then
    echo "Binary not found: ${BINARY}" >&2
    exit 1
fi

echo "Deploying ${BINARY} -> ${TARGET}:${REMOTE_DIR}/${NAME}"

# Stop any running instance or debug session so the binary isn't busy
ssh "${SSH_OPTS[@]}" "${TARGET}" \
    "mkdir -p '${REMOTE_DIR}' && (killall -q gdbserver '${NAME}' || true)"
scp "${SSH_OPTS[@]}" "${BINARY}" "${TARGET}:${REMOTE_DIR}/${NAME}"

echo "Deployed ${NAME}"
