#!/usr/bin/env bash
# Render keymap.svg from keymap.c with keymap-drawer (https://github.com/caksoylar/keymap-drawer).
set -euo pipefail
source "$(dirname "$0")/common.sh"
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
qmk_docker qmk c2json -kb "$KB" -km "$KM" "keyboards/splitkb/kyria/keymaps/$KM/keymap.c" > "$tmp/keymap.json"
layers=$(sed -n '/^enum layers {/,/^};/p' "$ROOT/keyboards/splitkb/kyria/keymaps/$KM/keymap.c" | grep -oE '^\s+_[A-Z_]+' | tr -d ' ')
# keymap-drawer resolves layer references by index; swap the enum names for their values.
python3 - "$tmp/keymap.json" $layers <<'PY'
import json, re, sys
path, names = sys.argv[1], sys.argv[2:]
km = json.load(open(path))
sub = lambda k: re.sub(r'\b(' + '|'.join(names) + r')\b', lambda m: str(names.index(m.group(1))), k)
km['layers'] = [[sub(k) for k in layer] for layer in km['layers']]
json.dump(km, open(path, 'w'))
PY
names=$(echo $layers | sed 's/^_//; s/ _/ /g')
uvx --from keymap-drawer keymap -c "$ROOT/tools/keymap-drawer.yaml" parse -q "$tmp/keymap.json" -l $names > "$tmp/keymap.yaml"
uvx --from keymap-drawer keymap -c "$ROOT/tools/keymap-drawer.yaml" draw "$tmp/keymap.yaml" > "$ROOT/keymap.svg"
echo "Wrote $ROOT/keymap.svg"
