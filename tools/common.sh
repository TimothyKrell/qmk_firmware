# Shared helpers for the tools/*.sh scripts. Everything runs inside the official QMK container,
# so the only host requirements are docker and lsusb.
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE="${QMK_IMAGE:-ghcr.io/qmk/qmk_cli:latest}"
KB=splitkb/kyria/rev1
KM=timothykrell
HEX="$ROOT/splitkb_kyria_rev1_base_${KM}.hex"

qmk_docker() {
    docker run --rm -u "$(id -u):$(id -g)" -e HOME=/tmp \
        -e GIT_CONFIG_COUNT=1 -e GIT_CONFIG_KEY_0=safe.directory -e GIT_CONFIG_VALUE_0='*' \
        -v "$ROOT:/qmk_userspace" -w /qmk_userspace \
        -e QMK_HOME=/qmk_userspace/qmk_firmware -e QMK_USERSPACE=/qmk_userspace \
        "$IMAGE" "$@"
}

usb_docker() {
    docker run --rm --privileged -v /dev:/dev -v "$ROOT:/work" -w /work "$IMAGE" "$@"
}

# Waits for an AVR bootloader to enumerate. Sets BOOTLOADER=dfu|caterina (and SERIAL_PORT for caterina).
wait_for_bootloader() {
    local timeout="${1:-120}"
    echo "Waiting up to ${timeout}s for the bootloader. Press the RESET button on the half that is plugged into USB."
    for ((i = 0; i < timeout * 5; i++)); do
        if lsusb -d 03eb:2ff4 >/dev/null 2>&1; then
            BOOTLOADER=dfu
            return 0
        fi
        for id in 2341:0036 2341:0037 1b4f:9203 1b4f:9205 2a03:0036 239a:000c; do
            if lsusb -d "$id" >/dev/null 2>&1; then
                BOOTLOADER=caterina
                sleep 0.5
                SERIAL_PORT=$(ls -t /dev/ttyACM* 2>/dev/null | head -1)
                return 0
            fi
        done
        sleep 0.2
    done
    echo "No bootloader showed up." >&2
    return 1
}
