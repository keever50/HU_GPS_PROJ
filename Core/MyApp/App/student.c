/**
* @file student.c
* @brief Hier kunnen studenten hun eigen tasks aanmaken
*
* @author MSC

* @date 5/5/2022
*/
#include <admin.h>
#include "main.h"
#include "cmsis_os.h"
#include "gps.h"
#include "lcd.h"
#include "buzzer.h"
#include "lcdout.h"
/**
* @brief Oefentask voor studenten
* @param argument, kan evt vanuit tasks gebruikt worden
* @return void
*/

double lonDMtoM(GNRMC *gnrmc);
double latDMtoM(GNRMC *gnrmc);

struct vector2d_s
{
    double x,y;
};

typedef struct vector2d_s vector2d_t;

vector2d_t waypoints[20];

vector2d_t globalVec;

SemaphoreHandle_t student_SemaphoreWaypoints;

void waypointsReset()
{
    for(int i=0;i<20;i++)
    {
        vector2d_t vec;
        vec.x=0;
        vec.y=0;
        waypoints[i]=vec;
    }
}


void test_gps_coords()
{
	static char last_status='\0';
	GNRMC latestgnrmc;
	gps_get_GNRMC(&latestgnrmc);

	if(latestgnrmc.status!='A')
	{
		if(latestgnrmc.status!=last_status)
			lcdout_printf("NO GPS LOCK %d", 12);
		last_status=latestgnrmc.status;
		return;
	}
	last_status=latestgnrmc.status;

	BUZZER_put(1000);
	double x, y;
	x = lonDMtoM(&latestgnrmc)/60.0;
	y = latDMtoM(&latestgnrmc)/60.0;
	lcdout_printf("X: %f\nY: %f\n", x, y);
}

void Student_task1 (void *argument)
{
	student_SemaphoreWaypoints = xSemaphoreCreateMutex();
	lcdout_init();
	waypointsReset();
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	globalVec.x=0;
	globalVec.y=0;
	char buf[80];
	unsigned int i = 0;

	while(TRUE)
	{
       	osDelay(100);

		if (Uart_debug_out & STUDENT_DEBUG_OUT)
		{
	       	sprintf(buf, "\r\n%s: %u", __func__, i++);
			UART_puts(buf);
    	}
		test_gps_coords();

	}
}

double lonDMtoM(GNRMC *gnrmc)
{
	char *lon=gnrmc->longitude;
	char lonEW=gnrmc->EW_ind;
	double minX=0;
    char* pEnd;
    minX=(*lon-'0')*6000;          //10^2*60
    minX+=(*(lon+1)-'0')*600;      //10^1*60
    minX+=(*(lon+2)-'0')*60;       //10^0*60
    minX+=strtod(lon+3,&pEnd);
    if(lonEW=='W')
    	minX=minX*-1;
    return minX;
}

double latDMtoM(GNRMC *gnrmc)
{
	char *lat=gnrmc->latitude;
	char latNS=gnrmc->NS_ind;
	double minY=0;
	char* pEnd;
    minY=(*lat-'0')*600;
    minY+=(*(lat+1)-'0')*60;
    minY+=strtod(lat+2,&pEnd);
    if(latNS=='S')
    	minY=minY*-1;
    return minY;
}
void Setglobalvector()
{
	xSemaphoreTake(student_SemaphoreWaypoints, portMAX_DELAY);
	GNRMC latestgnrmc;
	gps_get_GNRMC(&latestgnrmc);
	double x, y;
	x = lonDMtoM(&latestgnrmc)/60.0;
	y = latDMtoM(&latestgnrmc)/60.0;
	globalVec.x= x;
	globalVec.y= y;

	xSemaphoreGive(student_SemaphoreWaypoints);
}

void Getglobalvector(vector2d_t* vec)
{
	xSemaphoreTake(student_SemaphoreWaypoints, portMAX_DELAY);
	double x = globalVec.x;
	double y = globalVec.y;
	vec->x=x;
	vec->y=y;
	xSemaphoreGive(student_SemaphoreWaypoints);
}



