#ifndef _TIMER_H_
#define _TIMER_H_

// Initializes the timer with the specified frequency in Hz.
// Note that the effective frequency might be different than the desired one.
// Use timer_get_freq() to obtain the effective frequency.
extern void timer_init(uint_t freq_hz);

// Returns the number of ticks since boot.
extern uint_t timer_get_ticks();

// Sleeps (actively) the specified time in milliseconds.
extern void timer_sleep(uint_t ms);

// Returns the timer frequency in Hz.
extern uint_t timer_get_freq();

#endif
