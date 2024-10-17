/*
 * direction.c
 *
 *  Created on: Oct 15, 2024
 *      Author: milan
 */



double dirNext()
{
	double angle;
	vector2d_t wp1,wp2;
	wp1=waypoints[currentWP];
	wp2=waypoints[currentWP+1];
	angle=direction(wp1, wp2);
	currentWP++;
	return angle;
}

double direction(vector2d_t pos, vector2d_t waypoint)
{
	double angle,dX,dY;

	dX=waypoint.x-pos.x;
	dY=waypoint.y-pos.y;

	angle=(atan2(dX,dY))*(180*M_1_PI);	//hoek vanaf y-as in rad naar graden

	if(angle<0)							//maakt negatieve hoek (dX<0) positief
		angle+=360;
	return angle;
}

double calcDiff(double currentDir, double waypointDir)
{
    double diff;

    diff=currentDir-waypointDir;
    if(diff>180)
    {
        diff-180*-1;
    }
    else if(diff<-180)
    {
        diff+360;
    }
    return diff;
}
