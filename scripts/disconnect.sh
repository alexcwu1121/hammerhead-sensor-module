#!/usr/bin/env bash
#
# Close the shared master connection to the target, so the next deploy or
# debug session prompts for the password again.

set -euo pipefail

# shellcheck source=target-env.sh
source "$(dirname "${BASH_SOURCE[0]}")/target-env.sh"

ssh "${SSH_OPTS[@]}" -O exit "${TARGET}" 2>/dev/null || echo "No open connection to ${TARGET}"
