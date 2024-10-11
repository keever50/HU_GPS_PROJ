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
#include "student.h"
#include "LSM303.h"
/**
* @brief Oefentask voor studenten
* @param argument, kan evt vanuit tasks gebruikt worden
* @return void
*/

vector2d_t globalVec;

SemaphoreHandle_t student_SemaphoreWaypoints;

double lonDMtoM(GNRMC *gnrmc);
double latDMtoM(GNRMC *gnrmc);


void test_orient()
{
	struct lsm303_mag_vector vect;
	lsm303_mag_get_vector(&vect);
	lcdout_printf("X: %d\nY: %d\n", vect.x, vect.y);
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
	//lcdout_init();
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	globalVec.x=0;
	globalVec.y=0;
	char buf[80];
	unsigned int i = 0;

	osDelay(1000);
	lsm303_mag_datarate(0b000);
	lsm303_mag_gain(0);
	while(TRUE)
	{
       	osDelay(500);

		if (Uart_debug_out & STUDENT_DEBUG_OUT)
		{
	       	sprintf(buf, "\r\n%s: %u", __func__, i++);
			UART_puts(buf);
    	}
		//test_gps_coords();
		test_orient();
		Setglobalvector();

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



