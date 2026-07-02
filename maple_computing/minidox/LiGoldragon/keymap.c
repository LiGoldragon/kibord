#include QMK_KEYBOARD_H

enum layers {
    BASE,
    LOWER,
    RAISE,
    ADJUST,
};

#define LOWER_KEY MO(LOWER)
#define RAISE_KEY MO(RAISE)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * Base sends QWERTY-position letter keycodes. With a host us(colemak)
     * layout, the letter rows produce Colemak: QWFPG JLUY; / ARSTD HNEIO.
     */
    [BASE] = LAYOUT_split_3x5_3(
        KC_Q,           KC_W,           KC_E,           KC_R,    KC_T,             KC_Y,     KC_U,           KC_I,           KC_O,           KC_P,
        LSFT_T(KC_A),   LCTL_T(KC_S),   LALT_T(KC_D),   KC_F,    LGUI_T(KC_G),     KC_H,     RGUI_T(KC_J),   RALT_T(KC_K),   RCTL_T(KC_L),   RSFT_T(KC_SCLN),
        KC_Z,           KC_X,           KC_C,           KC_V,    KC_B,             KC_N,     KC_M,           KC_COMM,        KC_DOT,         KC_SLSH,
                                                KC_LCTL, LOWER_KEY, KC_SPC,        KC_BSPC,  RAISE_KEY,      KC_ENT
    ),

    [LOWER] = LAYOUT_split_3x5_3(
        KC_EXLM,        KC_AT,          KC_HASH,        KC_DLR,  KC_PERC,          KC_CIRC,  KC_AMPR,        KC_ASTR,        KC_LPRN,        KC_RPRN,
        KC_ESC,         KC_MINS,        KC_EQL,         KC_LBRC, KC_RBRC,          KC_LEFT,  KC_DOWN,        KC_UP,          KC_RGHT,        KC_BSLS,
        KC_TAB,         KC_GRV,         KC_LCBR,        KC_RCBR, KC_PIPE,          KC_HOME,  KC_PGDN,        KC_PGUP,        KC_END,         KC_QUOT,
                                                _______, _______, _______,         KC_DEL,   _______,        _______
    ),

    [RAISE] = LAYOUT_split_3x5_3(
        KC_1,           KC_2,           KC_3,           KC_4,    KC_5,             KC_6,     KC_7,           KC_8,           KC_9,           KC_0,
        KC_F1,          KC_F2,          KC_F3,          KC_F4,   KC_F5,            KC_F6,    KC_F7,          KC_F8,          KC_F9,          KC_F10,
        KC_F11,         KC_F12,         KC_MPRV,        KC_MPLY, KC_MNXT,          KC_MUTE,  KC_VOLD,        KC_VOLU,        KC_BRID,        KC_BRIU,
                                                _______, _______, _______,         _______,  _______,        _______
    ),

    [ADJUST] = LAYOUT_split_3x5_3(
        _______,        _______,        _______,        _______, _______,          _______,  _______,        _______,        _______,        _______,
        _______,        _______,        _______,        _______, _______,          _______,  _______,        _______,        _______,        _______,
        _______,        _______,        _______,        _______, _______,          _______,  _______,        _______,        _______,        _______,
                                                _______, _______, _______,         _______,  _______,        _______
    ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, LOWER, RAISE, ADJUST);
}
