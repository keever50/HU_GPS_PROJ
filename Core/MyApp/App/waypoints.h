/**
 * @file waypoints.h
 * @brief a library for waypoint related functions
 *  Created on: Oct 17, 2024
 *      Author: Tom Veldkamp
 */


#ifndef MYAPP_APP_WAYPOINTS_H_
#define MYAPP_APP_WAYPOINTS_H_

#include "main.h"
#include "cmsis_os.h"
#include <student.h>


#define WAYPOINT_MAX_WPS 20

/**
 * @brief Get a waypoint from the waypoint list using the index
 * @return Returns -1 when no waypoint was found.
 */
int waypoint_get(int index, vector2d_t *waypoint);
/**
 * @brief Adds a waypoint from the current position to the waypoint list.
 * @return -1 when the waypoint list is full
 */
int waypointcreate();

/**
 * @brief Lists all the waypoints to the LCD
 */
void waypointread();
/**
 * @brief Creates 4 pre-defined waypoints for debug
 */
void waypoint_test();

#endif /* MYAPP_APP_WAYPOINTS_H_ */
