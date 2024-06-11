#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

extern void keyb_init();

// Returns the key that was pressed or 0 if no key is present in the internal buffer.
// This function never blocks.
extern int keyb_get_key();

#endif
