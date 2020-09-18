/*
 * uiBase.h
 *
 *  Created on: Jul 13, 2014
 *      Author: dmolnar
 *
 *      Helper routines for the 3x7 segment diplay block
 */

#ifndef UIBASE_H_
#define UIBASE_H_

#include "Base.h"

/*
 * INPUT related definitions
 */

// define the number of input lines
#define NR_OF_INPUTS 1

// Event buffer STRUCT
struct input_events
{
  unsigned char   is_handled;
  unsigned char   counter;
};

// define input line pins
#define PLUS_INPUT_PIN P1_3
#define MINUS_INPUT_PIN P1_4
#define MASK_START_VALUE 0x08

// define input events
typedef enum {
    NO_INPUT_EVENT=0,
    PLUS_INPUT_PRESSED=1,
    MINUS_INPUT_PRESSED=2 } input_event_type;

// input event counter threshold
#define INPUT_EVENT_COUNT_THRESHOLD 10

/*
 * DISPLAY related definitions
 */

// The timer ISR routine prototype
ISR(TIMER1,0);

/*
 * segment buffer specific declarations and defines.
 * For segment buffer 74HC595, 8-bit parallell or serial out shift registers are used.
 */

/* Breadboard layout:
#define DS_PIN P3_0
#define SHCP_PIN P3_1
#define NMR_PIN P3_2
#define STCP_PIN P3_3
#define NOE_PIN P3_4
*/

// PCB layout:
#define DS_PIN P3_0 // Pin 14 - 2
#define NOE_PIN P3_1 // Pin 13 - 3
#define STCP_PIN P3_2 // Pin 12 - 6
#define SHCP_PIN P3_3 // Pin 11 - 7
#define NMR_PIN P3_4  // Pin 10 - 8


// digit encoder - encodes digits to segment bits for the buffer
extern const unsigned char digit_encoder[];
extern unsigned char segment_buffer[];
extern unsigned char display_index;
extern bool blink, is_blinking, display_off;
extern volatile bool blink_timeout_signal_flag;

// Display specific definitions
#define NR_OF_DIGITS 3
#define BLINK_PERIOD_MS 600

#ifdef  CRYSTAL_SPEED_LO
  #define DISPLAY_TIMER_RELOAD_HI_BYTE 0x00
  #define DISPLAY_TIMER_RELOAD_LO_BYTE 0xf0
#elif defined CRYSTAL_SPEED_HI
  #define DISPLAY_TIMER_RELOAD_HI_BYTE 0x00
  #define DISPLAY_TIMER_RELOAD_LO_BYTE 0xe0
#else
  #error "No or incorrect crystal speed defined."
#endif



typedef enum {
  FIRST_DIGIT=0,
  SECOND_DIGIT=1,
  THIRD_DIGIT=2
} display_index_type;

// Digit selector pins
#define DIGIT_1_POWER_LINE P3_5
#define DIGIT_2_POWER_LINE P3_7
#define DIGIT_3_POWER_LINE P1_5
#define ON 1
#define OFF 0

// additional segment display values
#define CHAR_L 0x3d
#define CHAR_H 0xa1
#define CHAR_MINUS 0xf7
#define CHAR_SPACE 0xff
#define DOT_MASK 0xdf
#define REVERSE_BUFFER_OUTPUT

// UI initializer
void init_ui(void);

// The UI event generator and ui operator function
input_event_type do_ui(void);
// Sets the temperature value to be displayed into the display buffer. Takes 100 times the value to be displayed
void set_display_temp(signed int value);
void set_display_blink(bool blink_request);

// Handle segments
static void reset_segment_output(void);
static void write_segment_output(unsigned char index) __reentrant;



#endif /* UIBASE_H_ */
