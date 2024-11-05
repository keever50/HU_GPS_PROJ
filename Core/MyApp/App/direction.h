/*
 *  @file direction.h
 *	@brief functies die de hoek berekenen tussen waypoints of eigen positie
 *  Created on: Oct 15, 2024
 *      Author: milan
 */

#ifndef MYAPP_APP_DIRECTION_H_
#define MYAPP_APP_DIRECTION_H_
#include "student.h"

//#define M_1_PI      0.318309886183790671537767526745028724


/*
 * @brief berekent hoek tussen twee  coordinaten
 * @return hoek tussen twee coordinaten in graden (-180 tot 180)
 */
double dir_direction(vector2d_t *pos, vector2d_t *waypoint);
/*
 * @brief berekent verschil tussen kompas hoek en hoek volgend waypoint
 * @return de hoek tussen twee hoeken (-180 tot 180)
 */
double dir_calcDiff(double currentDir, double waypointDir);
/*
 * @brief berekent hoek tussen het huidige en volgende waypoint
 * @return de hoek tussen twee hoeken (-180 tot 180)
 */
double dir_next();
/*
 * @brief krijg het huidige waypoint
 * @return huidige waypoint
 */
char dir_get_waypoint();

#endif /* MYAPP_APP_DIRECTION_H_ */
