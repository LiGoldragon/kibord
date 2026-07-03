#include QMK_KEYBOARD_H

enum layers {
    BASE,
    SYMBOLS,
    NUMBERS,
    FUNCTIONS,
    QWERTY_RECOVERED,
};

#define SYMBOLS_BSPC LT(SYMBOLS, KC_BSPC)
#define NUMBERS_SPC LT(NUMBERS, KC_SPC)
#define NUMBERS_ESC LT(NUMBERS, KC_ESC)
#define NUMBERS_ENT LT(NUMBERS, KC_ENT)

// These old SAFE_RANGE keycodes were present in the left binary, but their
// stripped process_record_user behavior was not recovered.
#define RECOVERED_USER_0 QK_USER_0
#define RECOVERED_USER_1 QK_USER_1
#define RECOVERED_USER_3 QK_USER_3

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * Recovered firmware-Colemak base from the old binary tables. The top row
     * is high confidence; home/bottom physical rows retain old-macro
     * uncertainty and are kept in recovered matrix order.
     */
    [BASE] = LAYOUT_split_3x5_3(
        KC_Q,           KC_W,           KC_F,           KC_P,    KC_G,             KC_J,     KC_L,           KC_U,           KC_Y,           KC_SCLN,
        KC_Z,           LCTL_T(KC_R),   LALT_T(KC_S),   KC_T,    KC_D,             KC_H,     KC_N,           RALT_T(KC_E),   RCTL_T(KC_I),   KC_SLSH,
        LSFT_T(KC_A),   KC_X,           KC_C,           KC_V,    LGUI_T(KC_B),     RGUI_T(KC_K), KC_M,       KC_COMM,        KC_DOT,         RSFT_T(KC_O),
                                                SYMBOLS_BSPC, QK_LEAD, NUMBERS_SPC, NUMBERS_ENT, LALT(KC_SPC), NUMBERS_ESC
    ),

    [SYMBOLS] = LAYOUT_split_3x5_3(
        KC_EXLM,        KC_AT,          KC_HASH,        KC_DLR,  KC_PERC,          KC_CIRC,  KC_AMPR,        KC_ASTR,        KC_LPRN,        KC_RPRN,
        KC_CAPS,        _______,        KC_UP,          _______, KC_HOME,          KC_PGUP,  KC_PLUS,        KC_LCBR,        KC_RCBR,        KC_UNDS,
        KC_TILD,        KC_LEFT,        KC_DOWN,        KC_RGHT, KC_END,           KC_PGDN,  KC_PERC,        KC_DQUO,        KC_COLN,        KC_SLSH,
                                                _______, KC_DEL, KC_ESC,           KC_TAB,   KC_PIPE,        _______
    ),

    [NUMBERS] = LAYOUT_split_3x5_3(
        KC_GRV,         _______,        _______,        KC_PGDN, KC_HOME,          KC_END,   KC_PGUP,        _______,        _______,        KC_BSLS,
        KC_CAPS,        KC_8,           KC_7,           KC_6,    KC_MINS,          KC_EQL,   KC_1,           KC_2,           KC_3,           KC_QUOT,
        LSFT_T(KC_9),   KC_LEFT,        KC_DOWN,        KC_LBRC, LGUI_T(KC_0),     RGUI_T(KC_5), KC_RBRC,    KC_UP,          KC_RGHT,        RSFT_T(KC_4),
                                                _______, _______, KC_TAB,          KC_ENT,   _______,        _______
    ),

    [FUNCTIONS] = LAYOUT_split_3x5_3(
        KC_F1,          KC_F2,          KC_F3,          KC_F4,   KC_F5,            KC_F6,    KC_F7,          KC_F8,          KC_F9,          KC_F10,
        KC_F11,         QK_BOOT,        _______,        _______, _______,          _______,  KC_PSCR,        KC_SCRL,        KC_PAUS,        KC_F12,
        _______,        RECOVERED_USER_0, RECOVERED_USER_1, _______, _______,      QK_BOOT,  _______,        _______,        _______,        _______,
                                                _______, _______, _______,         _______,  _______,        _______
    ),

    [QWERTY_RECOVERED] = LAYOUT_split_3x5_3(
        KC_Q,           KC_W,           KC_E,           KC_R,    KC_T,             KC_Y,     KC_U,           KC_I,           KC_O,           KC_P,
        KC_Z,           LCTL_T(KC_S),   LALT_T(KC_D),   KC_F,    KC_G,             KC_H,     KC_J,           RALT_T(KC_K),   RCTL_T(KC_L),   KC_SLSH,
        LSFT_T(KC_A),   KC_X,           KC_C,           KC_V,    LGUI_T(KC_B),     RGUI_T(KC_N), KC_M,       KC_COMM,        KC_DOT,         RSFT_T(KC_SCLN),
                                                SYMBOLS_BSPC, RECOVERED_USER_3, NUMBERS_SPC, NUMBERS_ENT, LALT(KC_SPC), NUMBERS_ESC
    ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, SYMBOLS, NUMBERS, FUNCTIONS);
}

void leader_end_user(void) {
    if (leader_sequence_five_keys(KC_R, KC_E, KC_S, KC_E, KC_T)) {
        reset_keyboard();
    }
}
