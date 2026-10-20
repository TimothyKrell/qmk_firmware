#!/usr/bin/env bash
# Toggle the full-screen keymap viewer. Bound to Mod+/ in ~/.config/niri/config.kdl.
set -euo pipefail
dir="$(dirname "$(readlink -f "$0")")"
id=$(niri msg -j windows | python3 -c '
import json, sys
print(next((w["id"] for w in json.load(sys.stdin) if w["app_id"] == "dev.timothykrell.KeymapViewer"), ""))')
if [ -n "$id" ]; then
    niri msg action close-window --id "$id"
else
    exec python3 "$dir/keymap-viewer.py"
fi
