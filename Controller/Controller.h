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
#define CHANGER_COIL P3_2
#define BLUETOOTH_COIL P3_3
#define STATE_PIN P1_2
#define POWER_ENABLE_PIN P1_7

#define POWER_ENABLED 0
#define POWER_DISABLED 1

// Coil pulsing times in msec
#define COIL_PULSE_TIME_MSEC 200
#define STATE_CHANGE_HOLD_TIME_SEC 2
#define POWER_HOLDOFF_TIME_SEC 30

#endif /* CONTROLLER_H_ */
