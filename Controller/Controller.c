/*
 * Chiller controller.c
 *
 *  Created on: Sep 18, 2020
 *      Author: dmolnar
 */

#include "Controller.h"

controller_state_type controller_state;

// Must be called periodically to take care of pulse output
void
pulse_output(void)
{
  if (timeout_occured(PWM_TIMER, TIMER_SEC, pwm_wait_time))
    {
      if(!pwm_active)
        {
          PWM_PIN = inactive_pwm_pin_value;
          pwm_state = PWM_OFF;
          return;
        }
      if (pwm_state == PWM_OFF)
        {
          pwm_wait_time = pwm_on_time;
          PWM_PIN = PWM_OUTPUT_ON;
          pwm_state = PWM_ON;
        } else {  // If pwm_state == PWM_ON
          pwm_wait_time = pwm_off_time;
          PWM_PIN = PWM_OUTPUT_OFF;
          pwm_state = PWM_OFF;
        }
      reset_timeout(PWM_TIMER, TIMER_SEC);
    }
}


void
operate_chilling_logic(void)
{

  int radiator_temp, room_temp;

  if(reevaluate_chill_logic)
    {
      // reset need for evaluation
      reevaluate_chill_logic = FALSE;

      radiator_temp = get_filtered_mean_temp(RADIATOR_SENSOR);
      room_temp = get_filtered_mean_temp(ROOM_SENSOR);

      if (radiator_temp < 0 && icing_condition_counter < 0xffff)
          icing_condition_counter++;
        else if (icing_condition_counter > 0)
          icing_condition_counter--;

      switch (chiller_state)
      {
      case COOLING:
        if (icing_condition_counter > ICING_CONDITION_THRESHOLD)
          {
            chiller_state = DEICING;
            // Turn of cooling
            pwm_on_time = 0;
            pwm_off_time = 0;
            pwm_active = FALSE;
            inactive_pwm_pin_value = PWM_OUTPUT_OFF;
            reset_timeout(DEICING_TIMER, TIMER_SEC);
            break;
          }
        calculate_PWM_times(room_temp);
        break;

      case DEICING:
        if(timeout_occured(PWM_TIMER, TIMER_SEC, DEICING_TIME_SEC))
            {
            // Exiting DEICING state
            chiller_state = COOLING;
            icing_condition_counter = 0;
            calculate_PWM_times(room_temp);
            }
        break;
      }
    }
}

void handle_ui(void)
{
  unsigned char input_event;
  input_event = do_ui();

  switch (input_event)
  {
  case NO_INPUT_EVENT:
    if (ui_state == SETTING_TARGET_TEMP && timeout_occured( UI_STATE_TIMER, TIMER_MS, UI_STATE_RESET_TIME_MS))
      ui_state = ACTUAL_TEMP_DISPLAY;
    break;

  case PLUS_INPUT_PRESSED:
    if (ui_state == ACTUAL_TEMP_DISPLAY)
      ui_state = SETTING_TARGET_TEMP;

      // ui_state == SETTING_TARGET_TEMP
      else if (target_temp < MAX_TARGET_TEMP)
          target_temp++;

    reset_timeout( UI_STATE_TIMER , TIMER_MS);
    break;

  case MINUS_INPUT_PRESSED:
    if (ui_state == ACTUAL_TEMP_DISPLAY)
        ui_state = SETTING_TARGET_TEMP;

      // ui_state == SETTING_TARGET_TEMP
      else if (target_temp > MIN_TARGET_TEMP)
         target_temp--;

    reset_timeout( UI_STATE_TIMER , TIMER_MS);
    break;
  }

  if (ui_state == ACTUAL_TEMP_DISPLAY)
    {
      set_display_temp(get_filtered_mean_temp(ROOM_SENSOR));
      set_display_blink(FALSE);

    // ui_state == SETTING_TARGET_TEMP
    } else {
       set_display_temp(target_temp);
       set_display_blink(TRUE);
    }
}

void
init_device(void)
{
  unsigned char i,j;

  i = NR_OF_TEMP_SENSORS;
  while (i--)
    {
    j = FILTER_BUFFER_LENGTH;
    while (j--)
      temperatures_buffer[i][j] = -199;
    }
  temp_buffer_index[RADIATOR_SENSOR] = 0;
  temp_buffer_index[ROOM_SENSOR] = 0;

  reset_timeout(TEMP_MEASUREMENT_TIMER, TIMER_SEC);

  // Set initial resolutions to 12 bit
  set_temp_resolution(0, TEMP_RESOLUTION_12BIT);
  set_temp_resolution(1, TEMP_RESOLUTION_12BIT);

  // We need to start a new conversion so it is complete on init
  conv_complete = TRUE;

  bus0_conv_initiated = bus1_conv_initiated = FALSE;
  bus_to_address = 0;

  // Reset PWM
  pwm_on_time = 0;
  pwm_off_time = 1;
  pwm_state = PWM_OFF;
  pwm_active = FALSE;
  inactive_pwm_pin_value = PWM_OUTPUT_OFF;

  //Set the initial target temp

  target_temp = INITIAL_TARGET_TEMP;

  init_ui();
  set_display_temp(get_filtered_mean_temp(ROOM_SENSOR));

  // Init chill logic
  reevaluate_chill_logic = FALSE;
  icing_condition_counter = 0;
  chiller_state = COOLING;

  // Reset UI state
  ui_state = ACTUAL_TEMP_DISPLAY;
}

void
main(void)
{
// Enable interrupts and initialize timer
  EA = 1;
  init_timer();
// Initialize the device
  init_device();

// Start the main execution loop
  while (TRUE)
    {
      // Operate main device functions and sets reevaluate_chill_logic flag if change is detected
      operate_onewire_temp_measurement();
      handle_ui();

      operate_chilling_logic();

      operate_PWM();

    }
}
