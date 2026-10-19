// Copyright 2026 Timothy Krell
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Ported from the 2021 keymap in TimothyKrell/qmk_firmware (keyboards/kyria/keymaps/timothykrell).
// Every keycode below is what the old LAYOUT_base/LAYOUT_base2 wrapper macros expanded to, so
// the compiled keymap is identical to the old firmware. Run `tools/verify.sh` to prove it.

#include QMK_KEYBOARD_H

enum layers {
    _QWERTY = 0,
    _GAMING,
    _GAMING_NORM,
    _GFLIP,
    _LOWER,
    _RAISE,
    _GRAISE,
    _ADJUST,
    _MACRO,
};

// Home-row mods, mirrored on each hand. Row 2 is Shift/Alt/Ctrl/GUI from the outside in,
// row 3 is GUI/Ctrl/Alt/Shift. They apply on every layer built from the old LAYOUT_base
// (QWERTY, RAISE, G-RAISE).
#define HM_A LSFT_T(KC_A)
#define HM_S LALT_T(KC_S)
#define HM_D LCTL_T(KC_D)
#define HM_F LGUI_T(KC_F)
#define HM_J RGUI_T(KC_J)
#define HM_K RCTL_T(KC_K)
#define HM_L RALT_T(KC_L)
#define HM_SCLN RSFT_T(KC_SCLN)
#define HM_Z LGUI_T(KC_Z)
#define HM_X LCTL_T(KC_X)
#define HM_C LALT_T(KC_C)
#define HM_V LSFT_T(KC_V)
#define HM_M RSFT_T(KC_M)
#define HM_COMM RALT_T(KC_COMM)
#define HM_DOT RCTL_T(KC_DOT)
#define HM_SLSH RGUI_T(KC_SLSH)

// Thumb keys
#define RAI_ESC LT(_RAISE, KC_ESC)
#define CTL_TAB LCTL_T(KC_TAB)
#define MAC_DEL LT(_MACRO, KC_DEL)
#define RAI_ENT LT(_RAISE, KC_ENT)
#define LOW_SPC LT(_LOWER, KC_SPC)
#define SFT_ESC LSFT_T(KC_ESC)
#define SFT_ENT RSFT_T(KC_ENT)
#define RAI_SPC LT(_RAISE, KC_SPC)
#define LOW_BSP LT(_LOWER, KC_BSPC)
#define MAC_TAB LT(_MACRO, KC_TAB)
#define LOW_ENT LT(_LOWER, KC_ENT)
#define SFT_BSP RSFT_T(KC_BSPC)
// The old source wrote LM(_GRAISE, KC_LSFT). LM() takes a MOD_* bitmask, and KC_LSFT (0xE1)
// masks down to MOD_LCTL, so this key has always been G-RAISE + Left Ctrl.
#define GRAI_CT LM(_GRAISE, MOD_LCTL)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * QWERTY
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |RAIS/Esc|   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |   |    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Tab|A/Sft |S/Alt |D/Ctl |F/GUI |   G  |                              |   H  |J/GUI |K/Ctl |L/Alt |;/Sft |   '    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |Z/GUI |X/Ctl |C/Alt |V/Sft |   B  |Space |LShift|  |LShift|LShift|   N  |M/Sft |,/Alt |./Ctl |//GUI |   -    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | GUI  | Del  |Enter |Space | Esc  |  |Enter |Space | Bksp | Tab  |AltGr |
 *                        |      |Macro |Raise |Lower |Shift |  |Shift |Raise |Lower |Macro |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_QWERTY] = LAYOUT_split_3x6_5(
      RAI_ESC,          KC_Q,          KC_W,          KC_E,          KC_R,             KC_T,                                                KC_Y,    KC_U,            KC_I,           KC_O,            KC_P,            KC_PIPE,
      CTL_TAB,          HM_A,          HM_S,          HM_D,          HM_F,             KC_G,                                                KC_H,    HM_J,            HM_K,           HM_L,            HM_SCLN,         KC_QUOT,
      KC_LSFT,          HM_Z,          HM_X,          HM_C,          HM_V,             KC_B,        KC_SPC,  KC_LSFT,     KC_LSFT, KC_LSFT, KC_N,    HM_M,            HM_COMM,        HM_DOT,          HM_SLSH,         KC_MINS,
                                                      KC_LGUI,       MAC_DEL,          RAI_ENT,     LOW_SPC, SFT_ESC,     SFT_ENT, RAI_SPC, LOW_BSP, MAC_TAB,         KC_RALT
    ),
/*
 * Gaming (left hand shifted one column right; T/G/B moved to the outer column)
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |   T    | Esc  |   Q  |   W  |   E  |   R  |                              |   Y  |   U  |   I  |   O  |   P  |   |    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |   G    | Ctrl |   A  |   S  |   D  |   F  |                              |   H  |   J  |   K  |   L  |   ;  |   '    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |   B    |LShift|   Z  |   X  |   C  |   V  |GRai+C|Enter |  |LShift|LShift|   N  |   M  |   ,  |   .  |   /  |   -    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |QWERTY|GFlip |GRaise|Space | Tab  |  |Enter |Space | Bksp | Tab  |AltGr |
 *                        |(def) |(hold)|(hold)|      |      |  |Lower |Raise |Shift |Macro |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_GAMING] = LAYOUT_split_3x6_5(
      KC_T,             KC_ESC,        KC_Q,          KC_W,          KC_E,             KC_R,                                                KC_Y,    KC_U,            KC_I,           KC_O,            KC_P,            KC_PIPE,
      KC_G,             KC_LCTL,       KC_A,          KC_S,          KC_D,             KC_F,                                                KC_H,    KC_J,            KC_K,           KC_L,            KC_SCLN,         KC_QUOT,
      KC_B,             KC_LSFT,       KC_Z,          KC_X,          KC_C,             KC_V,        GRAI_CT, KC_ENT,      KC_LSFT, KC_LSFT, KC_N,    KC_M,            KC_COMM,        KC_DOT,          KC_SLSH,         KC_MINS,
                                                      DF(_QWERTY),   MO(_GFLIP),       MO(_GRAISE), KC_SPC,  KC_TAB,      LOW_ENT, RAI_SPC, SFT_BSP, MAC_TAB,         KC_RALT
    ),
/*
 * Gaming (normal QWERTY positions, no home-row mods)
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Esc   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |   |    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Ctrl  |   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  |   ;  |   '    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  |GRai+C|Enter |  |LShift|LShift|   N  |   M  |   ,  |   .  |   /  |   -    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |QWERTY|GFlip |GRaise|Space | Tab  |  |Enter |Space | Bksp | Tab  |AltGr |
 *                        |(def) |(hold)|(hold)|      |      |  |Lower |Raise |Shift |Macro |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_GAMING_NORM] = LAYOUT_split_3x6_5(
      KC_ESC,           KC_Q,          KC_W,          KC_E,          KC_R,             KC_T,                                                KC_Y,    KC_U,            KC_I,           KC_O,            KC_P,            KC_PIPE,
      KC_LCTL,          KC_A,          KC_S,          KC_D,          KC_F,             KC_G,                                                KC_H,    KC_J,            KC_K,           KC_L,            KC_SCLN,         KC_QUOT,
      KC_LSFT,          KC_Z,          KC_X,          KC_C,          KC_V,             KC_B,        GRAI_CT, KC_ENT,      KC_LSFT, KC_LSFT, KC_N,    KC_M,            KC_COMM,        KC_DOT,          KC_SLSH,         KC_MINS,
                                                      DF(_QWERTY),   MO(_GFLIP),       MO(_GRAISE), KC_SPC,  KC_TAB,      LOW_ENT, RAI_SPC, SFT_BSP, MAC_TAB,         KC_RALT
    ),
/*
 * G-Flip: right-hand letters mirrored onto the left hand while gaming
 */
    [_GFLIP] = LAYOUT_split_3x6_5(
      _______,          KC_P,          KC_O,          KC_I,          KC_U,             KC_Y,                                                _______, _______,         _______,        _______,         _______,         _______,
      _______,          KC_SCLN,       KC_L,          KC_K,          KC_J,             KC_H,                                                _______, _______,         _______,        _______,         _______,         _______,
      _______,          KC_SLSH,       KC_DOT,        KC_COMM,       KC_M,             KC_N,        _______, _______,     _______, _______, _______, _______,         _______,        _______,         _______,         _______,
                                                      _______,       _______,          _______,     _______, _______,     _______, _______, _______, _______,         _______
    ),
/*
 * Lower: symbols
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  !   |  @   |  [   |  ]   |  |   |                              |      |      |      |      |      |   \    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  #   |  $   |  (   |  )   |  `   |                              |  +   |  -   |  /   |  *   |  :   |   "    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |  %   |  ^   |  {   |  }   |  ~   |      |      |  |      |      |  &   |  =   |  <   |  >   |  ?   |   _    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |  ;   |  =   |  |  =   |  ;   |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_LOWER] = LAYOUT_split_3x6_5(
      _______,          KC_EXLM,       KC_AT,         KC_LBRC,       KC_RBRC,          KC_PIPE,                                             _______, _______,         _______,        _______,         _______,         KC_BSLS,
      _______,          KC_HASH,       KC_DLR,        KC_LPRN,       KC_RPRN,          KC_GRV,                                              KC_PLUS, KC_MINS,         KC_SLSH,        KC_ASTR,         KC_COLN,         KC_DQT,
      _______,          KC_PERC,       KC_CIRC,       KC_LCBR,       KC_RCBR,          KC_TILD,     _______, _______,     _______, _______, KC_AMPR, KC_EQL,          KC_LT,          KC_GT,           KC_QUES,         KC_UNDS,
                                                      _______,       _______,          _______,     KC_SCLN, KC_EQL,      KC_EQL,  KC_SCLN, _______, _______,         _______
    ),
/*
 * Raise: numbers, F-keys, navigation (home-row mods still active)
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  1   |  2   |  3   |  4   |  5   |                              |  F6  |  F7  |  F8  |  F9  | F10  |  F11   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |6/Sft |7/Alt |8/Ctl |9/GUI |  0   |                              | Left |Dn/GUI|Up/Ctl|Rt/Alt|PU/Sft|  F12   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |F1/GUI|F2/Ctl|F3/Alt|F4/Sft|  F5  |      |      |  |      |      |      |Hm/Sft|En/Alt| Ctl  |PD/GUI|        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_RAISE] = LAYOUT_split_3x6_5(
      _______,          KC_1,          KC_2,          KC_3,          KC_4,             KC_5,                                                KC_F6,   KC_F7,           KC_F8,          KC_F9,           KC_F10,          KC_F11,
      _______,          LSFT_T(KC_6),  LALT_T(KC_7),  LCTL_T(KC_8),  LGUI_T(KC_9),     KC_0,                                                KC_LEFT, RGUI_T(KC_DOWN), RCTL_T(KC_UP),  RALT_T(KC_RGHT), RSFT_T(KC_PGUP), KC_F12,
      _______,          LGUI_T(KC_F1), LCTL_T(KC_F2), LALT_T(KC_F3), LSFT_T(KC_F4),    KC_F5,       _______, _______,     _______, _______, _______, RSFT_T(KC_HOME), RALT_T(KC_END), RCTL_T(_______), RGUI_T(KC_PGDN), _______,
                                                      _______,       _______,          _______,     _______, _______,     _______, _______, _______, _______,         _______
    ),
/*
 * G-Raise: gaming numbers / F-keys (home-row mods still active)
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |  F1  |  1   |  2   |  3   |  F4  |                              |  F6  |  F7  |  F8  |  F9  | F10  |  F11   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |GameNorm|F2/Sft|4/Alt |5/Ctl |6/GUI |  0   |                              | Left |Dn/GUI|Up/Ctl|Rt/Alt| Sft  |  F12   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Gaming |F3/GUI|7/Ctl |8/Alt |9/Sft |  F5  |      |      |  |      |      |MsLeft|MsDn/S|MsUp/A|MsRt/C| GUI  |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_GRAISE] = LAYOUT_split_3x6_5(
      KC_TAB,           KC_F1,         KC_1,          KC_2,          KC_3,             KC_F4,                                               KC_F6,   KC_F7,           KC_F8,          KC_F9,           KC_F10,          KC_F11,
      DF(_GAMING_NORM), LSFT_T(KC_F2), LALT_T(KC_4),  LCTL_T(KC_5),  LGUI_T(KC_6),     KC_0,                                                KC_LEFT, RGUI_T(KC_DOWN), RCTL_T(KC_UP),  RALT_T(KC_RGHT), RSFT_T(_______), KC_F12,
      DF(_GAMING),      LGUI_T(KC_F3), LCTL_T(KC_7),  LALT_T(KC_8),  LSFT_T(KC_9),     KC_F5,       _______, _______,     _______, _______, MS_LEFT, RSFT_T(MS_DOWN), RALT_T(MS_UP),  RCTL_T(MS_RGHT), RGUI_T(_______), _______,
                                                      _______,       _______,          _______,     _______, _______,     _______, _______, _______, _______,         _______
    ),
/*
 * Adjust (Lower + Raise): F-keys, RGB, mouse
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  F1  |  F2  |  F3  |  F4  |  F5  |                              |  F6  |  F7  |  F8  |  F9  | F10  |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | TOG  | SAI  | HUI  | VAI  | MOD  |                              |      |      |      | F11  | F12  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      | SAD  | HUD  | VAD  | RMOD |      |      |  |      |      |MsLeft|MsDown| MsUp |MsRght|      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_ADJUST] = LAYOUT_split_3x6_5(
      _______,          KC_F1,         KC_F2,         KC_F3,         KC_F4,            KC_F5,                                               KC_F6,   KC_F7,           KC_F8,          KC_F9,           KC_F10,          _______,
      _______,          UG_TOGG,       UG_SATU,       UG_HUEU,       UG_VALU,          UG_NEXT,                                             _______, _______,         _______,        KC_F11,          KC_F12,          _______,
      _______,          _______,       UG_SATD,       UG_HUED,       UG_VALD,          UG_PREV,     _______, _______,     _______, _______, MS_LEFT, MS_DOWN,         MS_UP,          MS_RGHT,         _______,         _______,
                                                      _______,       _______,          _______,     _______, _______,     _______, _______, _______, _______,         _______
    ),
/*
 * Macro: media keys and default-layer switching
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * | QWERTY |      |      |      |GameNm|Gaming|                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | Prev | Play | Next | Vol+ |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      | Mute | Vol- |      |      |      |  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_MACRO] = LAYOUT_split_3x6_5(
      DF(_QWERTY),      _______,       _______,       _______,       DF(_GAMING_NORM), DF(_GAMING),                                         _______, _______,         _______,        _______,         _______,         _______,
      _______,          KC_MPRV,       KC_MPLY,       KC_MNXT,       KC_VOLU,          _______,                                             _______, _______,         _______,        _______,         _______,         _______,
      _______,          _______,       _______,       KC_MUTE,       KC_VOLD,          _______,     _______, _______,     _______, _______, _______, _______,         _______,        _______,         _______,         _______,
                                                      _______,       _______,          _______,     _______, _______,     _______, _______, _______, _______,         _______
    ),
};

// clang-format on

// Tap both Shift thumbs together for Caps Word: the next word is typed in capitals
// (MAX_RETRIES), then it turns itself off.
const uint16_t PROGMEM caps_word_combo[] = {SFT_ESC, SFT_ENT, COMBO_END};
combo_t key_combos[] = {
    COMBO(caps_word_combo, CW_TOGG),
};

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

// Only the Shift/Esc and Shift/Enter thumbs resolve to Shift as soon as another key is pressed,
// which allows rolling shifted sequences. Everything else keeps the default (wait for the tapping
// term or permissive hold).
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SFT_ESC:
        case SFT_ENT:
            return true;
        default:
            return false;
    }
}

// Tap-then-hold on Lower/Space always activates the layer instead of auto-repeating space.
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LOW_SPC:
            return 0;
        default:
            return QUICK_TAP_TERM;
    }
}

static bool is_typing_key(uint16_t keycode) {
    switch (get_tap_keycode(keycode)) {
        case KC_A ... KC_0: // letters and digits
        case KC_DOT:
        case KC_COMM:
        case KC_SCLN:
        case KC_SLSH:
            return true;
        default:
            return false;
    }
}

// Flow Tap: a letter/number mod-tap pressed within FLOW_TAP_TERM of another letter/number is
// always a tap, so fast typing never fires a stray mod. After a short pause, mods work as
// before, including one-handed shortcuts like holding Z (Cmd) and tapping C. Thumb keys are
// left out so Space/Enter layer keys can still be held straight after typing. Once Cmd, Ctrl
// or Alt is held it is also skipped, so Cmd+C then Cmd+V in quick succession still works.
uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    if (get_mods() & (MOD_MASK_CG | MOD_MASK_ALT)) {
        return 0;
    }
    if (is_typing_key(keycode) && is_typing_key(prev_keycode)) {
        return FLOW_TAP_TERM;
    }
    return 0;
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_180;
}

static void render_kyria_logo(void) {
    // clang-format off
    static const char PROGMEM kyria_logo[] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,192,224,240,112,120, 56, 60, 28, 30, 14, 14, 14,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 14, 14, 14, 30, 28, 60, 56,120,112,240,224,192,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,192,224,240,124, 62, 31, 15,  7,  3,  1,128,192,224,240,120, 56, 60, 28, 30, 14, 14,  7,  7,135,231,127, 31,255,255, 31,127,231,135,  7,  7, 14, 14, 30, 28, 60, 56,120,240,224,192,128,  1,  3,  7, 15, 31, 62,124,240,224,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,240,252,255, 31,  7,  1,  0,  0,192,240,252,254,255,247,243,177,176, 48, 48, 48, 48, 48, 48, 48,120,254,135,  1,  0,  0,255,255,  0,  0,  1,135,254,120, 48, 48, 48, 48, 48, 48, 48,176,177,243,247,255,254,252,240,192,  0,  0,  1,  7, 31,255,252,240,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,255,255,255,  0,  0,  0,  0,  0,254,255,255,  1,  1,  7, 30,120,225,129,131,131,134,134,140,140,152,152,177,183,254,248,224,255,255,224,248,254,183,177,152,152,140,140,134,134,131,131,129,225,120, 30,  7,  1,  1,255,255,254,  0,  0,  0,  0,  0,255,255,255,  0,  0,  0,  0,255,255,  0,  0,192,192, 48, 48,  0,  0,240,240,  0,  0,  0,  0,  0,  0,240,240,  0,  0,240,240,192,192, 48, 48, 48, 48,192,192,  0,  0, 48, 48,243,243,  0,  0,  0,  0,  0,  0, 48, 48, 48, 48, 48, 48,192,192,  0,  0,  0,  0,  0,
        0,  0,  0,255,255,255,  0,  0,  0,  0,  0,127,255,255,128,128,224,120, 30,135,129,193,193, 97, 97, 49, 49, 25, 25,141,237,127, 31,  7,255,255,  7, 31,127,237,141, 25, 25, 49, 49, 97, 97,193,193,129,135, 30,120,224,128,128,255,255,127,  0,  0,  0,  0,  0,255,255,255,  0,  0,  0,  0, 63, 63,  3,  3, 12, 12, 48, 48,  0,  0,  0,  0, 51, 51, 51, 51, 51, 51, 15, 15,  0,  0, 63, 63,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 48, 48, 63, 63, 48, 48,  0,  0, 12, 12, 51, 51, 51, 51, 51, 51, 63, 63,  0,  0,  0,  0,  0,
        0,  0,  0,  0, 15, 63,255,248,224,128,  0,  0,  3, 15, 63,127,255,239,207,141, 13, 12, 12, 12, 12, 12, 12, 12, 30,127,225,128,  0,  0,255,255,  0,  0,128,225,127, 30, 12, 12, 12, 12, 12, 12, 12, 13,141,207,239,255,127, 63, 15,  3,  0,  0,128,224,248,255, 63, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  3,  7, 15, 62,124,248,240,224,192,128,  1,  3,  7, 15, 30, 28, 60, 56,120,112,112,224,224,225,231,254,248,255,255,248,254,231,225,224,224,112,112,120, 56, 60, 28, 30, 15,  7,  3,  1,128,192,224,240,248,124, 62, 15,  7,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  3,  7, 15, 14, 30, 28, 60, 56,120,112,112,112,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,112,112,112,120, 56, 60, 28, 30, 14, 15,  7,  3,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    };
    // clang-format on
    oled_write_raw_P(kyria_logo, sizeof(kyria_logo));
}

static void render_qmk_logo(void) {
    // clang-format off
    static const char PROGMEM qmk_logo[] = {
        0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
        0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
        0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0};
    // clang-format on
    oled_write_P(qmk_logo, false);
}

static void render_status(void) {
    render_qmk_logo();
    oled_write_P(PSTR("Tim's Keyboard\n\n"), false);

    oled_write_P(PSTR("Layer: "), false);
    // Include the default layer so a gaming default shows as "Gaming" rather than "Default".
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("Default\n"), false);
            break;
        case _LOWER:
            oled_write_P(PSTR("Lower\n"), false);
            break;
        case _RAISE:
            oled_write_P(PSTR("Raise\n"), false);
            break;
        case _ADJUST:
            oled_write_P(PSTR("Adjust\n"), false);
            break;
        case _MACRO:
            oled_write_P(PSTR("Macro\n"), false);
            break;
        case _GAMING:
            oled_write_P(PSTR("Gaming\n"), false);
            break;
        case _GAMING_NORM:
            oled_write_P(PSTR("Gaming Norm\n"), false);
            break;
        case _GRAISE:
            oled_write_P(PSTR("G-Raise\n"), false);
            break;
        case _GFLIP:
            oled_write_P(PSTR("G-Flip\n"), false);
            break;
        default:
            oled_write_P(PSTR("Undefined\n"), false);
    }

    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR("NUMLCK ") : PSTR("       "), false);
    oled_write_P(led_state.caps_lock ? PSTR("CAPLCK ") : PSTR("       "), false);
    oled_write_P(led_state.scroll_lock ? PSTR("SCRLCK ") : PSTR("       "), false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        render_status();
    } else {
        render_kyria_logo();
    }
    return false;
}
#endif
