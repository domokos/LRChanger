/*
 * Chiller controller.c
 *
 *  Created on: Sep 18, 2020
 *      Author: dmolnar
 */

#include "Controller.h"

controller_state_type controller_state;
bool state_change_prohibited;

// Must be called periodically to take care of pulse output
void
pulse_output (controller_state_type output)
{
  // Activate the requested output
  if (output == CHANGER)
    {
      CHANGER_COIL = 1;
    }
  else
    {
      // output == BLUETOOTH
      BLUETOOTH_COIL = 1;
    }

  // Wait for pulse duration
  delay_msec (COIL_PULSE_TIME_MSEC);

  // Deactivate all outputs
  CHANGER_COIL = 0;
  BLUETOOTH_COIL = 0;
}

void
handle_ui (void)
{
  unsigned char input_event;
  // Acquire input evenet
  input_event = do_ui ();

  // Allow state change if forbidden period has elapsed
  if (state_change_prohibited
      && timeout_occured (STATE_CHANGE_HOLD_TIMER, TIMER_SEC,
      STATE_CHANGE_HOLD_TIME_SEC))
    state_change_prohibited = FALSE;

  // Handle the input event
  switch (input_event)
    {
    case NO_INPUT_EVENT:
      break;

    case INPUT_PRESSED:
      // User activity detected maintain on state
      reset_timeout (POWER_HOLDOFF_TIMER, TIMER_SEC);

      if (state_change_prohibited)
	break;

      if (controller_state == CHANGER)
	{
	  controller_state = BLUETOOTH;
	}
      else // controller_state == BLUETOOTH
	{
	  controller_state = CHANGER;
	}
      display_state (controller_state);
      state_change_prohibited = TRUE;
      reset_timeout (STATE_CHANGE_HOLD_TIMER, TIMER_SEC);
      break;
    }
}

void
init_device (void)
{

  // Enable power pin to maintain power to the controller
  // and reset power holdoff timer
  POWER_ENABLE_PIN = POWER_ENABLED;
  reset_timeout (POWER_HOLDOFF_TIMER, TIMER_SEC);

  // Init variables
  state_change_prohibited = FALSE;
  CHANGER_COIL = 0;
  BLUETOOTH_COIL = 0;

  // Init the UI
  init_ui ();

  // Acquire the current state from the feedback line
  if (STATE_PIN)
    {
      controller_state = CHANGER;
    }
  else
    {
      controller_state = BLUETOOTH;
    }

  // Display the state on the UI LED
  display_state (controller_state);
}

void
shutdown (void)
{
  // Cut our own power
  POWER_ENABLE_PIN = POWER_DISABLED;

  // Disable interrupts
  EA = 0;
  // Initiate a terminal inifilite loop
  while (TRUE)
    {
    }
}

void
main (void)
{
  // Enable interrupts and initialize timer
  EA = 1;
  init_timer ();
  // Initialize the device
  init_device ();

  // Start the main execution loop
  while (TRUE)
    {
      // Operate main device functions and sets reevaluate_chill_logic flag if change is detected
      handle_ui ();

      // Check and shutdown if power holdoff time has expired
      if (timeout_occured (POWER_HOLDOFF_TIMER, TIMER_SEC,
      POWER_HOLDOFF_TIME_SEC))
	{
	  shutdown ();
	}
    }
}
