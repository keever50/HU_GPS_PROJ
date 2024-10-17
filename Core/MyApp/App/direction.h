/*
 * direction.h
 *
 *  Created on: Oct 15, 2024
 *      Author: milan
 */

#ifndef MYAPP_APP_DIRECTION_H_
#define MYAPP_APP_DIRECTION_H_
#include "student.h"
#include <math.h>
#define M_1_PI      0.318309886183790671537767526745028724

double distance(vector2d_t pos, vector2d_t waypoint);
double calcDiff(double currentDir, double waypointDir);

#endif /* MYAPP_APP_DIRECTION_H_ */
