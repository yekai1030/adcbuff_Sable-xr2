/*
 * ultrasond.h
 *
 *  Created on: Apr 23, 2018
 *      Author: tang
 */

#ifndef APPLICATION_ULTRASOND_H_
#define APPLICATION_ULTRASOND_H_
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <stdint.h>
#include <stddef.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/ADCBuf.h>
#include <ti/sysbios/knl/Task.h>
//#include <driverlib/aux_adc.h>
//lib local
#include "./driverlib/aux_adc.h"

/* Example/Board Header files */
#include "Board.h"
#include <ti/drivers/PIN.h>

//event queue msg
#include <util.h>

#define SAMPLE_NB 1000

void ultrasond_createTask(void);



#endif /* APPLICATION_ULTRASOND_H_ */
