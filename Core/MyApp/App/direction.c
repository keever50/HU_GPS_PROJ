/*
 * direction.c
 *
 *  Created on: Oct 15, 2024
 *      Author: milan
 */

#include <student.h>
#include <waypoints.h>
#include <math.h>
#include <direction.h>
char _currentWP;

double dir_direction(vector2d_t *pos, vector2d_t *waypoint)
{
	double angle,dX,dY;

	dX=waypoint->x-pos->x;
	dY=waypoint->y-pos->y;

	angle=atan2(dX,dY)*(180*M_1_PI);	//hoek vanaf y-as in rad naar graden

	if(angle<0)							//maakt negatieve hoek (dX<0) positief
		angle+=360;
	return angle;
}

double dir_next()						//hoek tussen waypoints
{
	double angle;
	vector2d_t wp1,wp2;

	waypoint_get(_currentWP,&wp1);
	waypoint_get(_currentWP+1,&wp2);
	angle=dir_direction(&wp1, &wp2);
	_currentWP++;
	return angle;
}


double dir_calcDiff(double currentDir, double waypointDir)	//verschil in hoek tussen kompas en waypoint
{
    double diff;

    diff=currentDir-waypointDir;
    if(diff>180)
    {
        diff=diff-180*-1;
    }
    else if(diff<-180)
    {
        diff=diff+360;
    }
    return diff;
}
