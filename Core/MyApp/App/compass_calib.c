/*
 * compass_calib.c
 *
 *  Created on: Oct 15, 2024
 *      Author: kevin
 */


#include <compass_calib.h>
#include <LSM303.h>
#include "cmsis_os.h"


int compass_calibration_start()
{
	UART_puts("CALIB\n");
	return 0;
}
