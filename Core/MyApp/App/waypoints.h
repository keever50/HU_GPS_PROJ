/*
 * waypoints.h
 *
 *  Created on: Oct 17, 2024
 *      Author: kevin
 */

#ifndef MYAPP_APP_WAYPOINTS_H_
#define MYAPP_APP_WAYPOINTS_H_

#include "main.h"
#include "cmsis_os.h"
#include <student.h>


#define WAYPOINT_MAX_WPS 20

int waypoint_get(int index, vector2d_t *waypoint);
int waypointcreate();
void waypointread();
void waypoint_test();

#endif /* MYAPP_APP_WAYPOINTS_H_ */
