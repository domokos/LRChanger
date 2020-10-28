/*
 * uiBase.h
 *
 *  Created on: Sep 19, 2020
 *      Author: dmolnar
 *
*/

#ifndef UIBASE_H_
#define UIBASE_H_

#include "Base.h"

// Define controller states
typedef enum {
  CHANGER,
  BLUETOOTH} controller_state_type;

/*
 * INPUT related definitions
*/

// Event buffer STRUCT
struct input_event
{
  unsigned char is_handled;
  unsigned char counter;
};

// define UI pins
#define INPUT_PIN P3_3
#define UI_CHANGER_PIN P3_4
#define UI_BT_PIN P3_5

// define input events
typedef enum {
    NO_INPUT_EVENT=0,
    INPUT_PRESSED=1 } input_event_type;

// input event counter threshold
#define INPUT_EVENT_COUNT_THRESHOLD 10

// Init the UI
void init_ui(void);

// The UI event generator and ui operator function
input_event_type do_ui(void);

// Display the state
void display_state(controller_state_type state);

#endif /* UIBASE_H_ */
