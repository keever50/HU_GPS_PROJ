/*
 * waypoints.c
 *
 *  Created on: Oct 17, 2024
 *      Author: kevin
 */


#include <waypoints.h>
#include <lcdout.h>
#include <student.h>

vector2d_t waypoints[20];
int waypointteller=0;

//xSemaphoreHandle waypoint_sema;

int waypoint_get(int index, vector2d_t *waypoint)
{
	if(index>=waypointteller) return -1;
	*waypoint=waypoints[index];
	return 0;
}

int waypointcreate()
{
    BUZZER_put (500);//check of functie wordt aangeroepen
	osDelay(500);
	if(waypointteller>=sizeof(waypoints))
		return -1;
	vector2d_t vec;
	Getglobalvector(&vec);
	waypoints[waypointteller] = vec;
	waypointteller++;
	lcdout_printf("Waypoint set");
	return 0;
}

void waypointread()
{
	for(int i=0;i<waypointteller;i++)
	{
		vector2d_t vec = waypoints[i];

		char msg[256];
		snprintf(msg,sizeof(msg),"waypoint %d. x %f. y %f\n",i, vec.x, vec.y);
		UART_puts(msg);
	}
	UART_puts("\n");
}
