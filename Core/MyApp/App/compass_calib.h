/**
 * @file compass_calib.h
 * @brief Compass calibration library for LSM303.h. and HU_GPS program
 *
 *  Created on: Oct 15, 2024
 *      Author: kevin
 */

#ifndef MYAPP_APP_COMPASS_CALIB_H_
#define MYAPP_APP_COMPASS_CALIB_H_
#include <main.h>

/**
 * @brief Calibration information. Filled in after running the calibration program.
 */
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

/**
 * @brief Compass magnetic direction vector from the LSM303.
 */
struct compass_vector
{
	double x;
	double y;
};

/**
 * @brief Starts the calibration program
 * @return 0 when OK
 */
int compass_calibration_start();

/**
 * @brief Apply the calibration results to x,y and output to compass_vector vect.
 * @return Returns 0 when OK
 */
int compass_apply_calibration(int16_t x, int16_t y, struct compass_vector *vect);

#endif /* MYAPP_APP_COMPASS_CALIB_H_ */
