#!/usr/bin/env bash
# Toggle the keymap overlay. Bound to Mod+/ in ~/.config/niri/config.kdl.
# niri also runs `show-keymap.sh --daemon` at login so the overlay is already loaded (hidden) and shows instantly.
pidfile="${XDG_RUNTIME_DIR:-/tmp}/keymap-overlay.pid"
if pid=$(<"$pidfile") 2>/dev/null && grep -qs keymap-viewer.py "/proc/$pid/cmdline"; then
    [ "${1:-}" = --daemon ] || kill -USR1 "$pid"
    exit 0
fi
dir="$(dirname "$(readlink -f "$0")")"
# The layer-shell library has to load before libwayland, hence the preload.
LD_PRELOAD=/usr/lib/libgtk4-layer-shell.so exec python3 "$dir/keymap-viewer.py" "$@"
