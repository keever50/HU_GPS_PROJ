/*
 * comprass_calib.h
 *
 *  Created on: Oct 15, 2024
 *      Author: kevin
 */

#ifndef MYAPP_APP_COMPASS_CALIB_H_
#define MYAPP_APP_COMPASS_CALIB_H_
#include <main.h>
struct compass_calibration_info
{
	double x_center;
	double x_range;
	double x_hi;
	double x_lo;
	double y_center;
	double y_range;
	double y_hi;
	double y_lo;
};

struct compass_vector
{
	double x;
	double y;
};

int compass_calibration_start();
int compass_apply_calibration(int16_t x, int16_t y, struct compass_vector *vect);

#endif /* MYAPP_APP_COMPASS_CALIB_H_ */
