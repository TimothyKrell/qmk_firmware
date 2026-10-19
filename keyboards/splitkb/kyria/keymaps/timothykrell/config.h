// Copyright 2026 Timothy Krell
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Handedness is stored in each half's EEPROM (flash with :dfu-split-left / :dfu-split-right once).
#define EE_HANDS

#define TAPPING_TERM 200
#define PERMISSIVE_HOLD
// Replaces the old IGNORE_MOD_TAP_INTERRUPT_PER_KEY; see get_hold_on_other_key_press().
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
// Replaces the old TAPPING_FORCE_HOLD_PER_KEY; see get_quick_tap_term().
#define QUICK_TAP_TERM_PER_KEY
// While typing quickly, letter/number mod-taps always type; see get_flow_tap_term().
#define FLOW_TAP_TERM 150

// Holding both Shifts also turns on Caps Word (the thumb combo in keymap.c is the quick way).
#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD

#ifdef RGBLIGHT_ENABLE
// RGBLIGHT_ANIMATIONS (all effects) was removed upstream; enable them individually.
#    define RGBLIGHT_EFFECT_ALTERNATING
#    define RGBLIGHT_EFFECT_BREATHING
#    define RGBLIGHT_EFFECT_CHRISTMAS
#    define RGBLIGHT_EFFECT_KNIGHT
#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    define RGBLIGHT_EFFECT_RGB_TEST
#    define RGBLIGHT_EFFECT_SNAKE
#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
#    define RGBLIGHT_EFFECT_TWINKLE
#    define RGBLIGHT_HUE_STEP 8
#    define RGBLIGHT_SAT_STEP 8
#    define RGBLIGHT_VAL_STEP 8
#endif

// Space savers: no one-shot keys in this keymap, and 9 layers fit in 16 bits.
#define NO_ACTION_ONESHOT
#define LAYER_STATE_16BIT
