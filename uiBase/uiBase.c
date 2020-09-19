/*
 * uiBase.c
 *
 *  Created on: Jul 13, 2014
 *      Author: dmolnar
 *
 *      Helper routines for the 3x7 segment diplay block
 */

#include "uiBase.h"

// The state of input
static struct input_event input_line_event_buffer;

/*
 * INPUT handler
 */

void
init_ui(void)
{
  input_line_event_buffer.counter = 0;
  input_line_event_buffer.is_handled = FALSE;

  // Set Input high so key press events can be read
  INPUT_PIN = 1;

  // Display nothing
  UI_CHANGER_PIN = 0;
  UI_BT_PIN = 0;
}

input_event_type
do_ui(void)
{
  input_event_type return_event;

  return_event = NO_INPUT_EVENT;

  if (INPUT_PIN)
    {
      if (input_line_event_buffer.counter < INPUT_EVENT_COUNT_THRESHOLD
          && !input_line_event_buffer.is_handled)
        {
          input_line_event_buffer.counter++;
          if (input_line_event_buffer.counter == INPUT_EVENT_COUNT_THRESHOLD)
            {
              // If the threshold is reached then return the press event of the corresponding input and set the handled flag to true
              input_line_event_buffer.is_handled = TRUE;
              return_event = INPUT_PRESSED;
            }
        }
    }
  else
    {
      if (input_line_event_buffer.counter > 0)
        {
          input_line_event_buffer.counter--;
          // If enough time has elapsed since the release of the input then clear the handled flag
          if (input_line_event_buffer.counter == 0)
            {
              input_line_event_buffer.is_handled = FALSE;
            }
        }
    }
  return return_event;
}

void
display_state(controller_state_type state)
{
  if (state == CHANGER)
    {
      UI_CHANGER_PIN = 1;
      UI_BT_PIN = 0;
    }
  else
    {
      UI_CHANGER_PIN = 0;
      UI_BT_PIN = 1;
    }
}
