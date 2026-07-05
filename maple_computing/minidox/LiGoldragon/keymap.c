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
#define HIDDEN_BOOT QK_BOOT

static uint8_t leader_saved_layer = BASE;

static void move_to_layer(uint8_t target_layer) {
    reset_oneshot_layer();

    if (get_highest_layer(layer_state) == target_layer) {
        return;
    }

    if (target_layer == BASE) {
        layer_clear();
    } else {
        layer_move(target_layer);
    }
}

static void restore_leader_layer(void) {
    if (leader_saved_layer == BASE) {
        layer_clear();
    } else {
        layer_move(leader_saved_layer);
    }
}

static void set_leader_oneshot_layer(uint8_t layer) {
    set_oneshot_layer(layer, ONESHOT_START);
    clear_oneshot_layer_state(ONESHOT_PRESSED);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base: recovered firmware-Colemak.
     *
     * Thumb order is left outside/middle/inside, then right inside/middle/outside.
     * QK_LEAD stays on the outside thumb key on both halves.
     *
     * ,----------------------------------.           ,----------------------------------.
     * |   Q  |   W  |   F  |   P  |   G  |           |   J  |   L  |   U  |   Y  |   ;  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * |   Z  | Ctl/R| Alt/S|   T  |   D  |           |   H  |   N  | Alt/E| Ctl/I|   /  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * | Sft/A|   X  |   C  |   V  | Gui/B|           | Gui/K|   M  |   ,  |   .  | Sft/O|
     * `----------------------------------'           `----------------------------------'
     *                  ,--------------------.    ,------,-------------.
     *                  | Lead | Bspc | Num  |    | Num  | AltSp| Lead |
     *                  `-------------| Space|    | Enter|------+------.
     *                                |      |    |      |
     *                                `------'    `------'
     */
    [BASE] = LAYOUT_split_3x5_3(
        KC_Q,           KC_W,           KC_F,           KC_P,    KC_G,             KC_J,     KC_L,           KC_U,           KC_Y,           KC_SCLN,
        KC_Z,           LCTL_T(KC_R),   LALT_T(KC_S),   KC_T,    KC_D,             KC_H,     KC_N,           RALT_T(KC_E),   RCTL_T(KC_I),   KC_SLSH,
        LSFT_T(KC_A),   KC_X,           KC_C,           KC_V,    LGUI_T(KC_B),     RGUI_T(KC_K), KC_M,       KC_COMM,        KC_DOT,         RSFT_T(KC_O),
                                                QK_LEAD, KC_BSPC, NUMBERS_SPC, NUMBERS_ENT, LALT(KC_SPC),  QK_LEAD
    ),

    /* Symbols: retained recovered left-artifact layer.
     *
     * ,----------------------------------.           ,----------------------------------.
     * |   !  |   @  |   #  |   $  |   %  |           |   ^  |   &  |   *  |   (  |   )  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * | Caps |      |  Up  |      | Home |           | PgUp |   +  |   {  |   }  |   _  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * |   ~  | Left | Down |Right | End  |           | PgDn |   %  |   "  |   :  |   /  |
     * `----------------------------------'           `----------------------------------'
     *                  ,--------------------.    ,------,-------------.
     *                  | Lead |  Del | Esc  |    | Tab  |   |  | Lead |
     *                  `-------------|      |    |      |------+------.
     *                                |      |    |      |
     *                                `------'    `------'
     */
    [SYMBOLS] = LAYOUT_split_3x5_3(
        KC_EXLM,        KC_AT,          KC_HASH,        KC_DLR,  KC_PERC,          KC_CIRC,  KC_AMPR,        KC_ASTR,        KC_LPRN,        KC_RPRN,
        KC_CAPS,        _______,        KC_UP,          _______, KC_HOME,          KC_PGUP,  KC_PLUS,        KC_LCBR,        KC_RCBR,        KC_UNDS,
        KC_TILD,        KC_LEFT,        KC_DOWN,        KC_RGHT, KC_END,           KC_PGDN,  KC_PERC,        KC_DQUO,        KC_COLN,        KC_SLSH,
                                                QK_LEAD, KC_DEL,  KC_ESC,          KC_TAB,   KC_PIPE,        QK_LEAD
    ),

    /* Numbers: recovered navigation shape, with number taps reordered by
     * physical left-to-right position as 1 2 3 4 5 / 6 7 8 9 0.
     *
     * ,----------------------------------.           ,----------------------------------.
     * |   `  |      |      | PgDn | Home |           | End  | PgUp |      |      |   \  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * | Caps |   2  |   3  |   4  |   -  |           |   =  |   7  |   8  |   9  |   '  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * | Sft/1| Left | Down |   [  | Gui/5|           | Gui/6|   ]  |  Up  |Right | Sft/0|
     * `----------------------------------'           `----------------------------------'
     *                  ,--------------------.    ,------,-------------.
     *                  | Lead |      | Tab  |    | Enter|      | Lead |
     *                  `-------------|      |    |      |------+------.
     *                                |      |    |      |
     *                                `------'    `------'
     */
    [NUMBERS] = LAYOUT_split_3x5_3(
        KC_GRV,         _______,        _______,        KC_PGDN, KC_HOME,          KC_END,   KC_PGUP,        _______,        _______,        KC_BSLS,
        KC_CAPS,        KC_2,           KC_3,           KC_4,    KC_MINS,          KC_EQL,   KC_7,           KC_8,           KC_9,           KC_QUOT,
        LSFT_T(KC_1),   KC_LEFT,        KC_DOWN,        KC_LBRC, LGUI_T(KC_5),     RGUI_T(KC_6), KC_RBRC,    KC_UP,          KC_RGHT,        RSFT_T(KC_0),
                                                QK_LEAD, _______, KC_TAB,          KC_ENT,   _______,        QK_LEAD
    ),

    /* Functions: direct layer. QK_BOOT remains tucked away here as a
     * last-resort fallback; leader r e s e t is the documented reset path.
     *
     * ,----------------------------------.           ,----------------------------------.
     * |  F1  |  F2  |  F3  |  F4  |  F5  |           |  F6  |  F7  |  F8  |  F9  | F10  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * |  F11 | Boot |      |      |      |           |      | PrSc |Scroll|Pause | F12  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * |      |  No  |  No  |      |      |           | Boot |      |      |      |      |
     * `----------------------------------'           `----------------------------------'
     *                  ,--------------------.    ,------,-------------.
     *                  | Lead |      |      |    |      |      | Lead |
     *                  `-------------|      |    |      |------+------.
     *                                |      |    |      |
     *                                `------'    `------'
     */
    [FUNCTIONS] = LAYOUT_split_3x5_3(
        KC_F1,          KC_F2,          KC_F3,          KC_F4,   KC_F5,            KC_F6,    KC_F7,          KC_F8,          KC_F9,          KC_F10,
        KC_F11,         HIDDEN_BOOT,    _______,        _______, _______,          _______,  KC_PSCR,        KC_SCRL,        KC_PAUS,        KC_F12,
        _______,        KC_NO,          KC_NO,          _______, _______,          HIDDEN_BOOT, _______,     _______,        _______,        _______,
                                                QK_LEAD, _______, _______,         _______,  _______,        QK_LEAD
    ),

    /* Qwerty Recovered: retained historical layer from the left artifact.
     *
     * ,----------------------------------.           ,----------------------------------.
     * |   Q  |   W  |   E  |   R  |   T  |           |   Y  |   U  |   I  |   O  |   P  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * |   Z  | Ctl/S| Alt/D|   F  |   G  |           |   H  |   J  | Alt/K| Ctl/L|   /  |
     * |------+------+------+------+------|           |------+------+------+------+------|
     * | Sft/A|   X  |   C  |   V  | Gui/B|           | Gui/N|   M  |   ,  |   .  |Sft/; |
     * `----------------------------------'           `----------------------------------'
     *                  ,--------------------.    ,------,-------------.
     *                  | Lead |  No  | Num  |    | Num  | AltSp| Lead |
     *                  `-------------| Space|    | Enter|------+------.
     *                                |      |    |      |
     *                                `------'    `------'
     */
    [QWERTY_RECOVERED] = LAYOUT_split_3x5_3(
        KC_Q,           KC_W,           KC_E,           KC_R,    KC_T,             KC_Y,     KC_U,           KC_I,           KC_O,           KC_P,
        KC_Z,           LCTL_T(KC_S),   LALT_T(KC_D),   KC_F,    KC_G,             KC_H,     KC_J,           RALT_T(KC_K),   RCTL_T(KC_L),   KC_SLSH,
        LSFT_T(KC_A),   KC_X,           KC_C,           KC_V,    LGUI_T(KC_B),     RGUI_T(KC_N), KC_M,       KC_COMM,        KC_DOT,         RSFT_T(KC_SCLN),
                                                QK_LEAD, KC_NO, NUMBERS_SPC,       NUMBERS_ENT, LALT(KC_SPC), QK_LEAD
    ),
};

void leader_start_user(void) {
    leader_saved_layer = get_highest_layer(layer_state);
    layer_clear();
}

void leader_end_user(void) {
    if (leader_sequence_five_keys(KC_R, KC_E, KC_S, KC_E, KC_T)) {
        reset_keyboard();
    } else if (leader_sequence_one_key(KC_A)) {
        move_to_layer(BASE);
    } else if (leader_sequence_one_key(KC_R)) {
        move_to_layer(SYMBOLS);
    } else if (leader_sequence_one_key(KC_S)) {
        move_to_layer(NUMBERS);
    } else if (leader_sequence_one_key(KC_T)) {
        move_to_layer(FUNCTIONS);
    } else if (leader_sequence_one_key(KC_G)) {
        move_to_layer(QWERTY_RECOVERED);
    } else if (leader_sequence_two_keys(KC_L, KC_S)) {
        restore_leader_layer();
        set_leader_oneshot_layer(SYMBOLS);
    } else if (leader_sequence_two_keys(KC_L, KC_N)) {
        restore_leader_layer();
        set_leader_oneshot_layer(NUMBERS);
    } else if (leader_sequence_two_keys(KC_L, KC_F)) {
        restore_leader_layer();
        set_leader_oneshot_layer(FUNCTIONS);
    } else if (leader_sequence_two_keys(KC_L, KC_Q)) {
        restore_leader_layer();
        set_leader_oneshot_layer(QWERTY_RECOVERED);
    } else if (leader_sequence_two_keys(KC_M, KC_S)) {
        restore_leader_layer();
        set_oneshot_mods(MOD_BIT(KC_LSFT));
    } else if (leader_sequence_two_keys(KC_M, KC_C)) {
        restore_leader_layer();
        set_oneshot_mods(MOD_BIT(KC_LCTL));
    } else if (leader_sequence_two_keys(KC_M, KC_A)) {
        restore_leader_layer();
        set_oneshot_mods(MOD_BIT(KC_LALT));
    } else if (leader_sequence_two_keys(KC_M, KC_G)) {
        restore_leader_layer();
        set_oneshot_mods(MOD_BIT(KC_LGUI));
    } else {
        restore_leader_layer();
    }
}
