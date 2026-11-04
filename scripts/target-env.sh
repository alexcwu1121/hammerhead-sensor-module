#!/usr/bin/env bash
#
# Shared target connection settings, sourced by the deploy/debug scripts.
#
# Environment:
#   HAMMERHEAD_HOST  target address (default: 192.168.4.1)
#   HAMMERHEAD_USER  target user    (default: root)

HOST="${HAMMERHEAD_HOST:-192.168.4.1}"
TARGET_USER="${HAMMERHEAD_USER:-root}"
TARGET="${TARGET_USER}@${HOST}"

# Target host keys change on every reflash, so don't pin them.
# The first connection prompts for the password and leaves a master
# connection running in the background; later ssh/scp calls reuse it without
# prompting. The socket lives in the container's /tmp, so it is dropped when
# the devcontainer stops, or when the board goes away (keepalive timeout).
SSH_OPTS=(
    -o StrictHostKeyChecking=no
    -o UserKnownHostsFile=/dev/null
    -o LogLevel=ERROR
    -o ConnectTimeout=5
    -o ControlMaster=auto
    -o "ControlPath=/tmp/hammerhead-ssh-%C"
    -o ControlPersist=yes
    -o ServerAliveInterval=5
    -o ServerAliveCountMax=3
)
