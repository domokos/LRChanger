/*
 * Base.c
 *
 *  Created on: Oct 28, 2012
 *      Author: doma
 *      Basic common routines for the RS485 device
 */
#include "Base.h"

// Module variables
static bool timer_initialized = FALSE;
static volatile unsigned int  ms_time_counter, sec_time_counter, sec_time_counter_helper;

static unsigned int timer_start_times[NR_OF_TIMERS];

/*
 * Internal utility functions
 */


// The timer ISR - set up to occur every 1 ms for a 11.0592 MHz Crystal
ISR(TIMER0,0)
{
  // Stop the timer
  TR0  = 0;

  // Increase timers
  ms_time_counter++;
  // Use a helper to avoid calling moduint from the ISR. ms_time_counter % 1000 == 0 would seem more straightforward
  // but this would be a call to a library function from within the ISR, which we want to avoid
  sec_time_counter_helper++;

  // increment sec counter in every 1000th cycle
  if(sec_time_counter_helper == 1000)
    {
      sec_time_counter_helper = 0;
      sec_time_counter++;
    } else { // Ensure that time spent in the ISR takes the same amount of time on both execution paths
      __asm
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      __endasm;
    }

// use 34 machine cycles less to compensate for time
// spent executing the ISR itself

#ifdef  CRYSTAL_SPEED_LO
  TL0  = 0x75;   // Restart from 0xfc66 (1 ms) - decimal 34 = 0xfc44
  TH0  = 0xfc;
#elif defined CRYSTAL_SPEED_HI
  TL0  = 0xaa;   // Restart from 0xf8cc (1 ms) - decimal 34 = 0xf8aa
  TH0  = 0xf8;
#else
#error "No or incorrect crystal speed defined."
#endif
  // Start the timer
  TR0  = 1;
}


/*
 * Public functions
 */

// Initiaize the timer
void init_timer(void)
{
  TR0  = 0;
  PT0  = 0;
#ifdef  CRYSTAL_SPEED_LO
  TL0  = 0x75;    // Start from 0xfc44
  TH0  = 0xfc;
#elif defined CRYSTAL_SPEED_HI
  TL0  = 0xaa;    // Start from 0xf8aa
  TH0  = 0xf8;
#else
#error "No or incorrect crystal speed defined."
#endif
  TMOD = (TMOD&0xF0)|0x01;    // Set Timer 0 16-bit mode
  sec_time_counter_helper = 0; // Initialize time counters
  ms_time_counter = 0;
  sec_time_counter = 0;
  TR0  = 1;       // Start Timer 0
  ET0  = 1;      // Enable Timer0 interrupt
  timer_initialized = TRUE;
}


// Wait for seconds
void delay_sec(unsigned int sec)
{
  if ( sec == 0 ) return;   // Return if delaytime is zero

  reset_timeout(DELAY_TIMEOUT, TIMER_SEC);

  while ( !timeout_occured(DELAY_TIMEOUT, TIMER_SEC, sec))
    {
      __asm nop __endasm;
    }             // Wait delaytime
  return;
}

// Wait for miliseconds
void delay_msec(unsigned int msec)
{
  if ( msec == 0 ) return;   // Return if delaytime is zero

/*      Crystal: 11.0592 MHz
 *      Freq. of 1 machine cycle (1 count) = 12 crystal pulse: 11059200/12 = 921600
 *      Length of 1 machine cycle: 1/921600 sec
 *      Clock ticks for 0.000100 = 100 usec (): 0.0001/(1/921600) = 92.16 ticks
 *      Clock ticks for 0.001 = 1 ms : 921.6 ticks
 *      We will use 922 ticks - to reload timer registers: FC66
 */

  reset_timeout(DELAY_TIMEOUT, TIMER_MS);

  while ( !timeout_occured(DELAY_TIMEOUT, TIMER_MS, msec))
    {
      __asm nop __endasm;
    }             // Wait delaytime
  return;
}

// Reset and start timeout counter
void reset_timeout(timer_id_type id, timer_type type)
{
  // Initialize timer if it is not initialized
  if(!timer_initialized) init_timer();
  if(type == TIMER_MS)
    {
      ET0 = 0;
      timer_start_times[id] = ms_time_counter;
      ET0 = 1;
    } else {
      ET0 = 0;
      timer_start_times[id] = sec_time_counter;
      ET0 = 1;
    }
}

// Get the time elapsed since reset in the unit indicated
unsigned int get_time_elapsed(timer_id_type id, timer_type type)
{
  unsigned int counter;

  // Get the time counter value
  if(type == TIMER_MS)
    {
      ET0  = 0;
      counter = ms_time_counter;
      ET0  = 1;
    } else {
      ET0  = 0;
      counter = sec_time_counter;
      ET0  = 1;
    }

  // If there is no owerflow in the interrupt ticks
  // (equality is regarded as no timeout - just started)
  if (counter >= timer_start_times[id])
    {
      return counter - timer_start_times[id];
  // There was an overflow - no multiple overflow is expected - timer must firs be reset and then queried regularily
    } else {
      return ((unsigned int)((unsigned int) 0xffff - timer_start_times[id])) + counter;
    }
// Prevent Eclipse mockering about function not returning a value
#ifdef __CDT_PARSER__
  return 0;
#endif
}

// Return if there was a timeout
// The calling parameter holds the timeout limit in miliseconds
bool timeout_occured(timer_id_type id, timer_type type, unsigned int timeout_limit)
{
 return get_time_elapsed(id, type) >= timeout_limit;
}

