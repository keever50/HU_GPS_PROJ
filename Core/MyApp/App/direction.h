/*
 * direction.h
 *
 *  Created on: Oct 15, 2024
 *      Author: milan
 */

#ifndef MYAPP_APP_DIRECTION_H_
#define MYAPP_APP_DIRECTION_H_
#include "student.h"

//#define M_1_PI      0.318309886183790671537767526745028724


double dir_direction(vector2d_t *pos, vector2d_t *waypoint);
double dir_calcDiff(double currentDir, double waypointDir);
double dir_next();


#endif /* MYAPP_APP_DIRECTION_H_ */
