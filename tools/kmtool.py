#!/usr/bin/env python3
"""Decode and compare the compiled keymap table inside Kyria rev1 firmware images.

The keymap lives in flash as uint16_t keymaps[layers][8][8]. This tool finds that table in a
.hex/.bin (a fresh build or a dump read off the keyboard), decodes every keycode to a canonical
name, and diffs two images by meaning rather than by raw value. That matters because QMK
renumbered most keycodes in late 2022, so old and new builds of the same keymap differ byte-wise.

  kmtool.py show  FIRMWARE[:old]
  kmtool.py json  FIRMWARE[:old]       > layers.json
  kmtool.py diff  A[:old] B[:old]      (A/B may also be a .json produced above)

Append :old for firmware built before QMK 0.19 (Nov 2022), e.g. a dump of the board's
original firmware. Without it, current keycode numbering is assumed.
"""
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from keycode_names import MODERN, OLD_BASIC  # noqa: E402

ROWS, COLS = 8, 8
LAYER_BYTES = ROWS * COLS * 2
# LAYOUT_split_3x6_5 key order -> (row, col), from keyboards/splitkb/kyria/rev1/info.json
LAYOUT = [
    (0, 7), (0, 6), (0, 5), (0, 4), (0, 3), (0, 2), (4, 2), (4, 3), (4, 4), (4, 5), (4, 6), (4, 7),
    (1, 7), (1, 6), (1, 5), (1, 4), (1, 3), (1, 2), (5, 2), (5, 3), (5, 4), (5, 5), (5, 6), (5, 7),
    (2, 7), (2, 6), (2, 5), (2, 4), (2, 3), (2, 2), (2, 1), (2, 0), (6, 0), (6, 1), (6, 2), (6, 3), (6, 4), (6, 5), (6, 6), (6, 7),
    (3, 4), (3, 3), (3, 2), (3, 1), (3, 0), (7, 0), (7, 1), (7, 2), (7, 3), (7, 4),
]
UNUSED = [(r, c) for r in range(ROWS) for c in range(COLS) if (r, c) not in LAYOUT]
ROW_SPLITS = [12, 12, 16, 10]

MOD_BITS = [(0x01, 'CTL'), (0x02, 'SFT'), (0x04, 'ALT'), (0x08, 'GUI')]
SHIFTED = {
    'KC_1': 'KC_EXLM', 'KC_2': 'KC_AT', 'KC_3': 'KC_HASH', 'KC_4': 'KC_DLR', 'KC_5': 'KC_PERC',
    'KC_6': 'KC_CIRC', 'KC_7': 'KC_AMPR', 'KC_8': 'KC_ASTR', 'KC_9': 'KC_LPRN', 'KC_0': 'KC_RPRN',
    'KC_MINS': 'KC_UNDS', 'KC_EQL': 'KC_PLUS', 'KC_LBRC': 'KC_LCBR', 'KC_RBRC': 'KC_RCBR',
    'KC_BSLS': 'KC_PIPE', 'KC_SCLN': 'KC_COLN', 'KC_QUOT': 'KC_DQT', 'KC_GRV': 'KC_TILD',
    'KC_COMM': 'KC_LT', 'KC_DOT': 'KC_GT', 'KC_SLSH': 'KC_QUES',
}
# Custom quantum keycodes in the 2021/early-2022 numbering (QK 0x5C00 block). Values taken from a
# build of QMK 9bf25d1cd5 (2022-02-20); builds from a few months earlier may be offset slightly.
OLD_QUANTUM = {
    0x5C00: 'QK_BOOT',
    0x5CC3: 'UG_TOGG', 0x5CC4: 'UG_NEXT', 0x5CC5: 'UG_PREV', 0x5CC6: 'UG_HUEU', 0x5CC7: 'UG_HUED',
    0x5CC8: 'UG_SATU', 0x5CC9: 'UG_SATD', 0x5CCA: 'UG_VALU', 0x5CCB: 'UG_VALD',
}


def mods_name(m, style):
    side = 'R' if m & 0x10 else 'L'
    names = [side + n for bit, n in MOD_BITS if m & bit]
    if style == 'bits':
        return '|'.join('MOD_' + n for n in names) or '0'
    return names


def basic(v, scheme):
    table = OLD_BASIC if scheme == 'old' else MODERN
    if v == 0:
        return 'KC_NO'
    if v == 1:
        return '_______'
    return table.get(v, f'0x{v:02X}')


def wrap_mods(m, inner):
    names = mods_name(m, 'list')
    if names == ['LSFT'] and inner in SHIFTED:
        return SHIFTED[inner]
    out = inner
    for n in reversed(names):
        out = f'{n}({out})'
    return out


def tap_mod(m, kc):
    names = mods_name(m, 'list')
    if len(names) == 1:
        return f'{names[0]}_T({kc})'
    return f'MT({mods_name(m, "bits")},{kc})'


def decode(v, scheme, layers):
    L = lambda n: layers[n] if n < len(layers) else str(n)  # noqa: E731
    if v < 0x100:
        return basic(v, scheme)
    if v < 0x2000:
        return wrap_mods((v >> 8) & 0x1F, basic(v & 0xFF, scheme))
    if 0x4000 <= v < 0x5000:
        return f'LT({L((v >> 8) & 0xF)},{basic(v & 0xFF, scheme)})'
    if scheme == 'old':
        hi = v & 0xFF00
        if 0x6000 <= v < 0x8000:
            return tap_mod((v >> 8) & 0x1F, basic(v & 0xFF, scheme))
        if hi == 0x5000:
            return f'TO({L(v & 0xF)})'
        if hi == 0x5100:
            return f'MO({L(v & 0xFF)})'
        if hi == 0x5200:
            return f'DF({L(v & 0xFF)})'
        if hi == 0x5300:
            return f'TG({L(v & 0xFF)})'
        if hi == 0x5400:
            return f'OSL({L(v & 0xFF)})'
        if hi == 0x5500:
            return f'OSM({mods_name(v & 0x1F, "bits")})'
        if hi == 0x5800:
            return f'TT({L(v & 0xFF)})'
        if hi == 0x5900:
            return f'LM({L((v >> 4) & 0xF)},{mods_name(v & 0xF, "bits")})'
        return OLD_QUANTUM.get(v, f'QK_0x{v:04X}')
    if 0x2000 <= v < 0x4000:
        return tap_mod((v >> 8) & 0x1F, basic(v & 0xFF, scheme))
    if 0x5000 <= v < 0x5200:
        return f'LM({L((v >> 5) & 0xF)},{mods_name(v & 0x1F, "bits")})'
    for base, fn in [(0x5200, 'TO'), (0x5220, 'MO'), (0x5240, 'DF'), (0x5260, 'TG'), (0x5280, 'OSL'), (0x52C0, 'TT'), (0x52E0, 'PDF')]:
        if base <= v < base + 0x20:
            return f'{fn}({L(v & 0x1F)})'
    if 0x52A0 <= v < 0x52C0:
        return f'OSM({mods_name(v & 0x1F, "bits")})'
    return MODERN.get(v, f'QK_0x{v:04X}')


def load_image(path):
    data = open(path, 'rb').read()
    if not path.endswith('.hex'):
        return data
    mem = bytearray(b'\xff' * 0x10000)
    top, base = 0, 0
    for line in data.decode().splitlines():
        line = line.strip()
        if not line.startswith(':'):
            continue
        raw = bytes.fromhex(line[1:])
        n, addr, typ = raw[0], (raw[1] << 8) | raw[2], raw[3]
        if typ == 0:
            a = base + addr
            mem[a:a + n] = raw[4:4 + n]
            top = max(top, a + n)
        elif typ == 2:
            base = ((raw[4] << 8) | raw[5]) << 4
    return bytes(mem[:top])


def word(data, off, r, c):
    i = off + (r * COLS + c) * 2
    return data[i] | (data[i + 1] << 8)


def looks_like_layer(data, off):
    if off + LAYER_BYTES > len(data):
        return False
    if any(word(data, off, r, c) for r, c in UNUSED):
        return False
    return sum(1 for r, c in LAYOUT if word(data, off, r, c) not in (0, 0xFFFF)) >= 30


def find_table(data):
    best = (0, None)
    for off in range(0, len(data) - LAYER_BYTES):
        if not looks_like_layer(data, off):
            continue
        n = 1
        while looks_like_layer(data, off + n * LAYER_BYTES):
            n += 1
        if n > best[0]:
            best = (n, off)
    if best[1] is None:
        sys.exit('could not find a keymap table in this image')
    return best[1], best[0]


DEFAULT_LAYERS = ['QWERTY', 'GAMING', 'GAMING_NORM', 'GFLIP', 'LOWER', 'RAISE', 'GRAISE', 'ADJUST', 'MACRO']


def read(spec):
    path, _, scheme = spec.partition(':')
    if path.endswith('.json'):
        return json.load(open(path))
    scheme = scheme or 'new'
    data = load_image(path)
    off, n = find_table(data)
    names = DEFAULT_LAYERS + [str(i) for i in range(len(DEFAULT_LAYERS), n)]
    layers = {}
    for i in range(n):
        base = off + i * LAYER_BYTES
        layers[names[i]] = [decode(word(data, base, r, c), scheme, names) for r, c in LAYOUT]
    return {'source': os.path.basename(path), 'scheme': scheme, 'offset': off, 'layers': layers}


def show(km):
    print(f"# {km['source']}  ({km['scheme']} keycodes, table at 0x{km['offset']:04X}, {len(km['layers'])} layers)")
    for name, keys in km['layers'].items():
        print(f'\n[{name}]')
        i = 0
        for row, n in enumerate(ROW_SPLITS):
            cells = keys[i:i + n]
            i += n
            if row == 3:
                print(' ' * 30 + '  '.join(f'{k:>14}' for k in cells[:5]) + '  |  ' + '  '.join(f'{k:>14}' for k in cells[5:]))
            else:
                h = n // 2
                print('  '.join(f'{k:>14}' for k in cells[:h]) + '  |  ' + '  '.join(f'{k:>14}' for k in cells[h:]))


def diff(a, b):
    diffs = 0
    for name in sorted(set(a['layers']) | set(b['layers']), key=lambda n: (DEFAULT_LAYERS + [n]).index(n)):
        ka, kb = a['layers'].get(name), b['layers'].get(name)
        if ka is None or kb is None:
            print(f'[{name}] only in {"A" if kb is None else "B"}')
            diffs += 1
            continue
        for i, (x, y) in enumerate(zip(ka, kb)):
            if x != y:
                r, c = LAYOUT[i]
                print(f'[{name}] key #{i:2d} (matrix {r},{c}): {x}  ->  {y}')
                diffs += 1
    print(f"\n{a['source']} vs {b['source']}: " + ('IDENTICAL' if not diffs else f'{diffs} difference(s)'))
    return diffs


if __name__ == '__main__':
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    cmd = sys.argv[1]
    if cmd == 'show':
        show(read(sys.argv[2]))
    elif cmd == 'json':
        print(json.dumps(read(sys.argv[2]), indent=1))
    elif cmd == 'diff':
        sys.exit(1 if diff(read(sys.argv[2]), read(sys.argv[3])) else 0)
    else:
        sys.exit(__doc__)
