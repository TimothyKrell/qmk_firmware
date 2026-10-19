# Regenerate keycode_names.py:  uv run --with hjson tools/gen_keycode_names.py <path-to-qmk_firmware>
import hjson, glob, os, re, sys, json
d = os.path.join(sys.argv[1] if len(sys.argv) > 1 else os.path.expanduser('~/qmk_firmware'), 'data/constants/keycodes')
def ver(f):
    m = re.match(r'keycodes_(\d+)\.(\d+)\.(\d+)(?:_(\w+))?\.hjson', os.path.basename(f))
    return tuple(map(int, m.groups()[:3]))
table = {}
for f in sorted(glob.glob(d + '/keycodes_*.hjson'), key=ver):
    kc = hjson.load(open(f)).get('keycodes', {})
    grp = (re.match(r'keycodes_[\d.]+?(?:_(\w+))?\.hjson', os.path.basename(f)).group(1)) or ''
    if '!reset!' in kc:
        table = {k2: e for k2, e in table.items() if e.get('grp') != grp}
    for k, v in kc.items():
        if k == '!reset!': continue
        val = int(k, 16)
        if v == '!delete!':
            table.pop(val, None); continue
        aliases = v.get('aliases', [])
        if '!reset!' in aliases:
            aliases = [a for a in aliases if a != '!reset!']
        elif val in table and table[val]['key'] == v['key']:
            aliases = table[val]['aliases'] + aliases
        table[val] = {'key': v['key'], 'aliases': aliases, 'grp': grp}
def short(e):
    names = [e['key']] + e['aliases']
    return min(names, key=len)
modern = {v: short(e) for v, e in table.items() if v < 0x0100 or 0x7000 <= v < 0x8000}
allnames = {}
for v, e in table.items():
    for n in [e['key']] + e['aliases']:
        allnames[n] = v
# Feb-2022 era (QMK before 0.19 renumbering): 0xA5..0xBF system/media, 0xED.. mouse (non-VIA builds)
old_special = ['KC_SYSTEM_POWER','KC_SYSTEM_SLEEP','KC_SYSTEM_WAKE','KC_AUDIO_MUTE','KC_AUDIO_VOL_UP','KC_AUDIO_VOL_DOWN',
 'KC_MEDIA_NEXT_TRACK','KC_MEDIA_PREV_TRACK','KC_MEDIA_STOP','KC_MEDIA_PLAY_PAUSE','KC_MEDIA_SELECT','KC_MEDIA_EJECT',
 'KC_MAIL','KC_CALCULATOR','KC_MY_COMPUTER','KC_WWW_SEARCH','KC_WWW_HOME','KC_WWW_BACK','KC_WWW_FORWARD','KC_WWW_STOP',
 'KC_WWW_REFRESH','KC_WWW_FAVORITES','KC_MEDIA_FAST_FORWARD','KC_MEDIA_REWIND','KC_BRIGHTNESS_UP','KC_BRIGHTNESS_DOWN']
old_mouse = ['MS_UP','MS_DOWN','MS_LEFT','MS_RGHT','MS_BTN1','MS_BTN2','MS_BTN3','MS_BTN4','MS_BTN5','MS_BTN6','MS_BTN7','MS_BTN8',
 'MS_WHLU','MS_WHLD','MS_WHLL','MS_WHLR','MS_ACL0','MS_ACL1','MS_ACL2']
old = {}
for i, n in enumerate(old_special):
    old[0xA5 + i] = modern[allnames[n]]
for i, n in enumerate(old_mouse):
    old[0xED + i] = modern[allnames[n]]
for v, n in modern.items():
    if 0x04 <= v <= 0xA4 or 0xE0 <= v <= 0xE7 or v < 2:
        old[v] = n
out = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'keycode_names.py')
with open(out, 'w') as fh:
    fh.write('# Generated from qmk_firmware/data/constants/keycodes. Do not edit by hand.\n')
    fh.write('# MODERN: current QMK numbering. OLD: QMK before the 0.19 (Nov 2022) renumbering.\n\n')
    fh.write('MODERN = {\n' + ''.join(f'    0x{v:04X}: {n!r},\n' for v, n in sorted(modern.items())) + '}\n\n')
    fh.write('OLD_BASIC = {\n' + ''.join(f'    0x{v:04X}: {n!r},\n' for v, n in sorted(old.items())) + '}\n')
print(len(modern), len(old), modern.get(0xA8), old.get(0xA8), old.get(0xEF), [modern[allnames[x]] for x in ['UG_TOGG','UG_NEXT','QK_BOOT']])
