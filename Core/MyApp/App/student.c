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
#include "direction.h"
#include <betterbuzzer.h>
#include <compass_calib.h>
#include <math.h>
#include <waypoints.h>
//#define M_1_PI      0.318309886183790671537767526745028724

/**
* @brief Oefentask voor studenten
* @param argument, kan evt vanuit tasks gebruikt worden
* @return void
*/

vector2d_t globalVec;

SemaphoreHandle_t student_SemaphoreWaypoints;
char student_initialized=0;
QueueHandle_t student_keyQ;

double lonDMtoM(GNRMC *gnrmc);
double latDMtoM(GNRMC *gnrmc);


int test_orient()
{
	struct lsm303_mag_vector vect;
	int ret = lsm303_mag_get_vector(&vect);
	if(ret) return ret;

	struct compass_vector comp_vect;;
	compass_apply_calibration(vect.x, vect.y, &comp_vect);
	lcdout_printf("X: %f\nY: %f\n", comp_vect.x, comp_vect.y);
	osDelay(1000);
	double deg = (atan2(comp_vect.x,comp_vect.y)/3.14)*180;
	lcdout_printf("%f DEG",deg);
	//osDelay(1000);

	return 0;
}

int test_gps_coords()
{
	static char last_status='\0';
	GNRMC latestgnrmc;
	gps_get_GNRMC(&latestgnrmc);

	if(latestgnrmc.status!='A')
	{
		//if(latestgnrmc.status!=last_status)
		lcdout_printf("NO GPS LOCK %d", 12);
		osDelay(1000);
		//last_status=latestgnrmc.status;
		return -1;
	}
	last_status=latestgnrmc.status;

	double x, y;
	x = lonDMtoM(&latestgnrmc)/60.0;
	y = latDMtoM(&latestgnrmc)/60.0;
	lcdout_printf("X: %f\nY: %f\n", x, y);
	return 0;
}

int student_list_waypoints()
{
	int i=0;
	for(;;)
	{
		vector2d_t waypoint;
		int ret = waypoint_get(i, &waypoint);
		if(ret<0) break;
		lcdout_printf("Index: %d\nX%.4f Y%.4f", i, waypoint.x, waypoint.y);
		i++;
		buzzer_buzz(50, 400);
		osDelay(500);
	}
	return 0;
}

int student_send_keys(char key)
{
	if(!student_initialized) return -1;
	BaseType_t ret = xQueueSend(student_keyQ, &key, portMAX_DELAY);
	if(ret!=pdTRUE) return -2;

	return 0;
}

char student_get_key()
{
	char k=0;
	xQueueReceive(student_keyQ, &k, 10);
	return k;

}

int student_start_program(int ID)
{
	switch(ID)
	{
	case 5:
	{
		return waypointcreate();
	}
	case 4:
	{
		return student_list_waypoints();
	}
	case 3:
	{
		return compass_calibration_start();
	}
	case 2:
	{
		return test_orient();
	}
	case 1:
	{
		return test_gps_coords();
	}
	}
	return -0xFF;
}

int student_init()
{
	student_SemaphoreWaypoints = xSemaphoreCreateMutex();
	student_keyQ = xQueueCreate(STUDENT_KEY_Q_LENGTH, 1);
	//lcdout_init();



	osDelay(1000);


	student_initialized=1;
	return 0;
}


void Student_task1 (void *argument)
{
	globalVec.x=0;
	globalVec.y=0;


	char buf[80];
	unsigned int i = 0;

	student_init();
	buzzer_buzz(100, 400);
//	buzzer_buzz(100, 500);
//	buzzer_buzz(100, 400);
//	buzzer_buzz(100, 500);
//	buzzer_buzz(300, 600);
//	buzzer_buzz(100, 400);
//	buzzer_buzz(100, 500);
//	buzzer_buzz(100, 400);

	UART_puts((char *)__func__); UART_puts(" started\r\n");

	while(TRUE)
	{
       	osDelay(500);

		if (Uart_debug_out & STUDENT_DEBUG_OUT)
		{
	       	sprintf(buf, "\r\n%s: %u", __func__, i++);
			UART_puts(buf);
    	}
		//test_gps_coords();

		char key = student_get_key();
		if(key)
		{
			int err = student_start_program(key);
			if(err)
			{
				lcdout_printf("Program %d\nERR 0x%X", key, err);
				buzzer_buzz(100, 1000);
			}
		}



		Setglobalvector();
	}
}

//double distance(vector2d_t pos, vector2d_t waypoint)
//{
//	double angle,dX,dY;
//
//	dX=waypoint.x-pos.x;
//	dY=waypoint.y-pos.y;
//
//	angle=(atan2(x,y))*(180*M_1_PI);	//hoek vanaf y-as in rad naar graden
//
//	if(angle<0)							//maakt negatieve hoek (dX<0) positief
//		angle+=360;
//	return angle;
//}

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



