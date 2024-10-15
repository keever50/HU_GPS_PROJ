/*
 * student.h
 *
 *  Created on: Oct 8, 2024
 *      Author: tomfv
 */

#ifndef MYAPP_APP_STUDENT_H_
#define MYAPP_APP_STUDENT_H_

#define STUDENT_KEY_Q_LENGTH 5

struct vector2d_s
{
	double x,y;
};

typedef struct vector2d_s vector2d_t;





void Setglobalvector();
void Getglobalvector(vector2d_t* vec);
int student_send_keys(char key);

#endif /* MYAPP_APP_STUDENT_H_ */
