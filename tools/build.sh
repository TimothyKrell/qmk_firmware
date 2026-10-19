#!/usr/bin/env bash
# Compile the firmware with the pinned qmk_firmware submodule. Output: ./splitkb_kyria_rev1_base_timothykrell.hex
set -euo pipefail
source "$(dirname "$0")/common.sh"
git -C "$ROOT" submodule update --init --depth 1 qmk_firmware
qmk_docker qmk compile -kb "$KB" -km "$KM"
echo "Built $HEX"
