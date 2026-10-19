#!/usr/bin/env bash
# Prove the current build's keymap matches the original. Compares against the on-board dump when one
# exists (reference/on-board-keymap.json), otherwise against the 2021 GitHub keymap.
set -euo pipefail
source "$(dirname "$0")/common.sh"
[ -f "$HEX" ] || "$ROOT/tools/build.sh"
ref="$ROOT/reference/on-board-keymap.json"
[ -f "$ref" ] || ref="$ROOT/reference/github-2021-keymap.json"
python3 "$ROOT/tools/kmtool.py" diff "$ref" "$HEX"
