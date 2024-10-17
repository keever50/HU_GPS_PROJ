/*
 * student.h
 *
 *  Created on: Oct 8, 2024
 *      Author: tomfv
 */

// 1: get gps coords
// 2: get orientation
// 3: calibrate mag
// 4: list waypoints
// 5: set waypoint

#ifndef MYAPP_APP_STUDENT_H_
#define MYAPP_APP_STUDENT_H_

#define STUDENT_KEY_Q_LENGTH 5

#define M_1_PI      0.318309886183790671537767526745028724

struct vector2d_s
{
	double x,y;
};

typedef struct vector2d_s vector2d_t;





void Setglobalvector();
void Getglobalvector(vector2d_t* vec);
int student_send_keys(char key);
char student_get_key();

#endif /* MYAPP_APP_STUDENT_H_ */
