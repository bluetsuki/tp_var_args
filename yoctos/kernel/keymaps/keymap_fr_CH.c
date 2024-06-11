#include "common/types.h"
#include "common/keycodes.h"
#include "keymap.h"

keymap_t keymap_fr_CH = {
    // Normal mapping
    {
        KEY_IGNORE,     // 0
        KEY_ESC,        // 1    ESC
        '1',            // 2
        '2',            // 3
        '3',            // 4
        '4',            // 5
        '5',            // 6
        '6',            // 7
        '7',            // 8
        '8',            // 9
        '9',            // 10
        '0',            // 11
        '\'',           // 12
        '^',            // 13
        '\b',           // 14   BACKSPACE
        '\t',           // 15   TAB
        'q',            // 16
        'w',            // 17
        'e',            // 18
        'r',            // 19
        't',            // 20
        'z',            // 21
        'u',            // 22
        'i',            // 23
        'o',            // 24
        'p',            // 25
        KEY_E_GRAVE,    // 26
        '"',            // 27
        '\n',           // 28  ENTER
        KEY_IGNORE,     // 29  LEFT/RIGHT CTRL
        'a',            // 30
        's',            // 31
        'd',            // 32
        'f',            // 33
        'g',            // 34
        'h',            // 35
        'j',            // 36
        'k',            // 37
        'l',            // 38
        KEY_E_ACUTE,    // 39
        KEY_A_GRAVE,    // 40
        KEY_PARAGRAPH,  // 41
        KEY_IGNORE,     // 42  LEFT SHIFT
        '$',            // 43
        'y',            // 44
        'x',            // 45
        'c',            // 46
        'v',            // 47
        'b',            // 48
        'n',            // 49
        'm',            // 50
        ',',            // 51
        '.',            // 52
        '-',            // 53
        KEY_IGNORE,     // 54   RIGHT SHIFT
        KEY_IGNORE,     // 55   ?
        KEY_IGNORE,     // 56   LEFT/RIGHT ALT
        ' ',            // 57
        KEY_IGNORE,     // 58   ?
        KEY_F1,         // 59   F1
        KEY_F2,         // 60   F2
        KEY_F3,         // 61   F3
        KEY_F4,         // 62   F4
        KEY_F5,         // 63   F5
        KEY_F6,         // 64   F6
        KEY_F7,         // 65   F7
        KEY_F8,         // 66   F8
        KEY_F9,         // 67   F9
        KEY_F10,        // 68   F10
        KEY_IGNORE,     // 69   ?
        KEY_IGNORE,     // 70   ?
        KEY_HOME,       // 71   HOME
        KEY_UP,         // 72   UP ARROW
        KEY_PGUP,       // 73   PAGE UP
        KEY_IGNORE,     // 74   ?
        KEY_LEFT,       // 75   LEFT ARROW
        KEY_IGNORE,     // 76   ?
        KEY_RIGHT,      // 77   RIGHT ARROW
        KEY_IGNORE,     // 78   ?
        KEY_END,        // 79   END
        KEY_DOWN,       // 80   DOWN ARROW
        KEY_PGDOWN,     // 81   PAGE DOWN
        KEY_INSERT,     // 82   INSERT
        KEY_DEL,        // 83   DELETE
        KEY_IGNORE,     // 84   ?
        KEY_IGNORE,     // 85   ?
        '<',            // 86
        KEY_F11,        // 87   F11
        KEY_F12,        // 88   F12
    },
    // SHIFT mapping
    {
        KEY_IGNORE,     // 0    ?
        KEY_ESC,        // 1    ESC
        '+',            // 2
        '"',            // 3
        '*',            // 4
        KEY_C_CEDILLA,  // 5
        '%',            // 6
        '&',            // 7
        '/',            // 8
        '(',            // 9
        ')',            // 10
        '=',            // 11
        '?',            // 12
        '`',            // 13
        '\b',           // 14   BACKSPACE
        '\t',           // 15   TAB
        'Q',            // 16
        'W',            // 17
        'E',            // 18
        'R',            // 19
        'T',            // 20
        'Z',            // 21
        'U',            // 22
        'I',            // 23
        'O',            // 24
        'P',            // 25
        KEY_U_DIAERESIS,// 26
        '!',            // 27
        '\n',           // 28  ENTER
        KEY_IGNORE,     // 29  LEFT/RIGHT CTRL
        'A',            // 30
        'S',            // 31
        'D',            // 32
        'F',            // 33
        'G',            // 34
        'H',            // 35
        'J',            // 36
        'K',            // 37
        'L',            // 38
        KEY_O_DIAERESIS,// 39
        KEY_A_DIAERESIS,// 40
        KEY_DEGREE,     // 41
        KEY_IGNORE,     // 42  LEFT SHIFT
        KEY_POUND,      // 43
        'Y',            // 44
        'X',            // 45
        'C',            // 46
        'V',            // 47
        'B',            // 48
        'N',            // 49
        'M',            // 50
        ';',            // 51
        ':',            // 52
        '_',            // 53
        KEY_IGNORE,     // 54   RIGHT SHIFT
        KEY_IGNORE,     // 55   ?
        KEY_IGNORE,     // 56   LEFT/RIGHT ALT
        ' ',            // 57
        KEY_IGNORE,     // 58   ?
        KEY_F1,         // 59   F1
        KEY_F2,         // 60   F2
        KEY_F3,         // 61   F3
        KEY_F4,         // 62   F4
        KEY_F5,         // 63   F5
        KEY_F6,         // 64   F6
        KEY_F7,         // 65   F7
        KEY_F8,         // 66   F8
        KEY_F9,         // 67   F9
        KEY_F10,        // 68   F10
        KEY_IGNORE,     // 69   ?
        KEY_IGNORE,     // 70   ?
        KEY_HOME,       // 71   HOME
        KEY_UP,         // 72   UP ARROW
        KEY_PGUP,       // 73   PAGE UP
        KEY_IGNORE,     // 74   ?
        KEY_LEFT,       // 75   LEFT ARROW
        KEY_IGNORE,     // 76   ?
        KEY_RIGHT,      // 77   RIGHT ARROW
        KEY_IGNORE,     // 78   ?
        KEY_END,        // 79   END
        KEY_DOWN,       // 80   DOWN ARROW
        KEY_PGDOWN,     // 81   PAGE DOWN
        KEY_INSERT,     // 82   INSERT
        KEY_DEL,        // 83   DELETE
        KEY_IGNORE,     // 84   ?
        KEY_IGNORE,     // 85   ?
        '>',            // 86
        KEY_F11,        // 87   F11
        KEY_F12,        // 88   F12
    },
    // CTRL mapping
    {},
    // ALT mapping
    {}
};
