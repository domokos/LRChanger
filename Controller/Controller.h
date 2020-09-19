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

// Coil pin values
#define CHANGER_COIL P1_6
#define BLUETOOTH_COIL P1_7
#define STATE_PIN P3_0

// Coil pulsing times in msec
#define COIL_PULSE_TIME 200
#define STATE_CHANGE_HOLD_TIME_SEC 2
#define STARTUP_DELAY_SEC 2

#endif /* CONTROLLER_H_ */
