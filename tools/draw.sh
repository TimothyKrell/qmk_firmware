#!/usr/bin/env bash
# Render keymap.svg from keymap.c with keymap-drawer (https://github.com/caksoylar/keymap-drawer).
set -euo pipefail
source "$(dirname "$0")/common.sh"
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
# Drawing order, read down each column of the 3-column screen grid (and top to bottom in keymap.svg).
# Independent of the enum order in keymap.c, which sets layer priority and must keep the default layers lowest.
ORDER=(QWERTY RAISE LOWER  GAMING GRAISE GFLIP  GAMING_NORM MACRO ADJUST)
SCREEN_COLS=3
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
# Combos aren't in c2json output; add them by key position (thumb row starts at 40).
cat >> "$tmp/keymap.yaml" <<'YAML'
combos:
  - {p: [44, 45], k: Caps Word, l: [QWERTY]}
YAML
# keymap-drawer draws layers in file order (its -s flag filters but doesn't reorder), so reorder the YAML.
reorder() {
    uvx --from keymap-drawer python - "$tmp/keymap.yaml" "$@" <<'PY'
import sys, yaml
km, order = yaml.safe_load(open(sys.argv[1])), sys.argv[2:]
assert sorted(order) == sorted(km["layers"]), f"ORDER in draw.sh {order} doesn't match the layers {list(km['layers'])}"
km["layers"] = {name: km["layers"][name] for name in order}
yaml.safe_dump(km, sys.stdout, allow_unicode=True, sort_keys=False)
PY
}
reorder "${ORDER[@]}" > "$tmp/tall.yaml"
uvx --from keymap-drawer keymap -c "$ROOT/tools/keymap-drawer.yaml" draw "$tmp/tall.yaml" > "$ROOT/keymap.svg"
echo "Wrote $ROOT/keymap.svg"
# Landscape copy for the full-screen viewer: same config with keymap-drawer-screen.yaml's draw_config on top.
uvx --from keymap-drawer python - "$ROOT/tools/keymap-drawer.yaml" "$ROOT/tools/keymap-drawer-screen.yaml" > "$tmp/screen.yaml" <<'PY'
import sys, yaml
base, over = (yaml.safe_load(open(p)) for p in sys.argv[1:])
base["draw_config"].update(over["draw_config"])
yaml.safe_dump(base, sys.stdout, allow_unicode=True)
PY
# keymap-drawer fills the grid row by row, so transpose ORDER to run it down the columns.
rows=$(( (${#ORDER[@]} + SCREEN_COLS - 1) / SCREEN_COLS ))
screen_order=()
for ((r = 0; r < rows; r++)); do
    for ((c = 0; c < SCREEN_COLS; c++)); do
        i=$(( c * rows + r ))
        if (( i < ${#ORDER[@]} )); then screen_order+=("${ORDER[i]}"); fi
    done
done
reorder "${screen_order[@]}" > "$tmp/screen-layers.yaml"
uvx --from keymap-drawer keymap -c "$tmp/screen.yaml" draw "$tmp/screen-layers.yaml" > "$ROOT/keymap-screen.svg"
echo "Wrote $ROOT/keymap-screen.svg"
