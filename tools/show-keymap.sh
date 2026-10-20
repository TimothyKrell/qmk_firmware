#!/usr/bin/env bash
# Toggle the keymap overlay. Bound to Mod+/ in ~/.config/niri/config.kdl.
set -euo pipefail
dir="$(dirname "$(readlink -f "$0")")"
pidfile="${XDG_RUNTIME_DIR:-/tmp}/keymap-overlay.pid"
if pid=$(cat "$pidfile" 2>/dev/null) && grep -qs keymap-viewer.py "/proc/$pid/cmdline"; then
    kill "$pid"
    exit 0
fi
echo $$ > "$pidfile"
# The layer-shell library has to load before libwayland, hence the preload.
LD_PRELOAD=/usr/lib/libgtk4-layer-shell.so exec python3 "$dir/keymap-viewer.py"
