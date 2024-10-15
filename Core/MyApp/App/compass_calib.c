/*
 * compass_calib.c
 *
 *  Created on: Oct 15, 2024
 *      Author: kevin
 */


#include <compass_calib.h>
#include <LSM303.h>
#include "cmsis_os.h"
#include <lcdout.h>
#include <LSM303.h>
#include <betterbuzzer.h>
#include <student.h>

/* MACROS */

#define COMPASS_CHECK_ERR(err) if(err) return _compass_show_err(err);

/* PRIVATE GLOBALS */
struct compass_calibration_info _compass_calib_info;

/* PRIVATE FUNC */

int _compass_show_err(int err)
{
	lcdout_printf("COMPASS ERROR\n0x%X", err);
	buzzer_buzz(500,1000);
	osDelay(1000);
	return err;
}

/* PUBLIC FUNC */

int compass_apply_calibration(int16_t x, int16_t y, struct compass_vector *vect)
{
	vect->x=(x-_compass_calib_info.x_center);///(_compass_calib_info.x_range/2);
	vect->y=(y-_compass_calib_info.y_center);///(_compass_calib_info.y_range/2);

	vect->x=vect->x/(_compass_calib_info.x_range/2.0);
	vect->y=vect->y/(_compass_calib_info.y_range/2.0);

	return 0;
}

int compass_calibration_start()
{
	lcdout_printf("COMPASS\nCALIBRATION");
	osDelay(1000);

	/* Find highs and lows */
	int16_t hi_x=INT16_MIN, lo_x=INT16_MAX, hi_y=INT16_MIN, lo_y=INT16_MAX;
	for(;;)
	{
		struct lsm303_mag_vector vect;
		COMPASS_CHECK_ERR(lsm303_mag_get_vector(&vect));
		int16_t x = vect.x;
		int16_t y = vect.y;
		if(x>hi_x) hi_x=x;
		if(x<lo_x) lo_x=x;
		if(y>hi_y) hi_y=y;
		if(y<lo_y) lo_y=y;

		lcdout_printf("X: H%d L%d\nY: H%d L%d", hi_x, lo_x, hi_y, lo_y);

		char key = student_get_key();
		if(key) break;
		osDelay(100);
	}

	_compass_calib_info.x_hi=hi_x;
	_compass_calib_info.x_lo=lo_x;
	_compass_calib_info.y_hi=hi_y;
	_compass_calib_info.y_lo=lo_y;
	_compass_calib_info.x_range = _compass_calib_info.x_hi-_compass_calib_info.x_lo;
	_compass_calib_info.x_center = _compass_calib_info.x_hi-_compass_calib_info.x_range/2.0;
	_compass_calib_info.y_range = _compass_calib_info.y_hi-_compass_calib_info.y_lo;
	_compass_calib_info.y_center = _compass_calib_info.y_hi-_compass_calib_info.y_range/2.0;

	lcdout_printf("SET");
	osDelay(1000);

	lcdout_printf("");
	return 0;
}
