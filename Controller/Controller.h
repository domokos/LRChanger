/*
 * Controller.h
 *
 *  Created on: Sep 18, 2020
 *      Author: dmolnar
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_


#include "Base.h"
#include "uiBase.h"

// Coil pulse state type
typedef enum {PWM_OFF, PWM_ON} pwm_states;

// PWM pin values
#define PWM_OUTPUT_OFF 0
#define PWM_OUTPUT_ON 1

// Define controller states
typedef enum {
  STARTUP,
  CHANGER,
  BLUETOOTH} controller_state_type;


#endif /* CONTROLLER_H_ */
