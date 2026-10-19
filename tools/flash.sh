#!/usr/bin/env bash
# Build, then flash one half. Run it twice: once with the left half plugged in, once with the right.
# Handedness lives in each half's EEPROM (EE_HANDS) and survives reflashing.
set -euo pipefail
source "$(dirname "$0")/common.sh"
"$ROOT/tools/build.sh"
wait_for_bootloader "${1:-120}"
hex="/work/$(basename "$HEX")"
if [ "$BOOTLOADER" = dfu ]; then
    usb_docker sh -c "dfu-programmer atmega32u4 erase --force && dfu-programmer atmega32u4 flash $hex && dfu-programmer atmega32u4 launch"
else
    usb_docker avrdude -p atmega32u4 -c avr109 -P "$SERIAL_PORT" -U "flash:w:$hex:i"
fi
echo "Flashed. Now do the other half."
