#!/usr/bin/env bash
# READ-ONLY: copy the firmware + EEPROM currently on the keyboard into dumps/, then decode the
# keymap and diff it against reference/github-2021-keymap.json. Nothing on the keyboard is changed;
# the bootloader is told to start the existing firmware again as soon as the read finishes.
set -euo pipefail
source "$(dirname "$0")/common.sh"
mkdir -p "$ROOT/dumps"
stamp=$(date +%Y%m%d-%H%M%S)
wait_for_bootloader "${1:-300}"
if [ "$BOOTLOADER" = dfu ]; then
    usb_docker sh -c "dfu-programmer atmega32u4 read --force --bin > dumps/flash-$stamp.bin; \
        dfu-programmer atmega32u4 read --force --bin --eeprom > dumps/eeprom-$stamp.bin; \
        dfu-programmer atmega32u4 launch"
else
    usb_docker avrdude -p atmega32u4 -c avr109 -P "$SERIAL_PORT" \
        -U "flash:r:dumps/flash-$stamp.bin:r" -U "eeprom:r:dumps/eeprom-$stamp.bin:r"
fi
fix_owner() { docker run --rm -v "$ROOT/dumps:/d" "$IMAGE" chown -R "$(id -u):$(id -g)" /d; }
fix_owner
dump="$ROOT/dumps/flash-$stamp.bin"
echo "Saved $dump ($(stat -c %s "$dump") bytes)"
python3 "$ROOT/tools/kmtool.py" json "$dump:old" > "$ROOT/reference/on-board-keymap.json"
python3 "$ROOT/tools/kmtool.py" show "$ROOT/reference/on-board-keymap.json" || true
python3 "$ROOT/tools/kmtool.py" diff "$ROOT/reference/github-2021-keymap.json" "$ROOT/reference/on-board-keymap.json"
