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

void set_servo_angle(double angle)
{
	if(angle<-45)
	{
		angle=-45;
	}else if(angle>45)
	{
		angle=45;
	}
	double pwm=(angle/90)+0.5;
	double servo=0.05+(pwm*0.05);
	set_servo(servo);
}

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
	char last_status='\0';
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

int next_waypoint()
{
	char WP = dir_get_waypoint();
	double dir = dir_next();

	lcdout_printf("WP %d\nDIR %f", WP, dir);
	return 0;
}

int start_route()
{
	lcdout_printf("Starting\nroute");
	buzzer_buzz(200, 440/2);
	buzzer_buzz(200, 880/2);
	buzzer_buzz(200, 1200/2);
	buzzer_buzz(200, 440);
	buzzer_buzz(200, 880);
	buzzer_buzz(200, 1200);

	osDelay(1000);

	uint16_t current_wp_index=0;
	for(;;)
	{
		/* Get a waypoint. This returns -1 when the route is finished */
		vector2d_t WP;
		int ret = waypoint_get(current_wp_index, &WP);
		if(ret<0)
		{
			lcdout_printf("Route\nFinished!");
			buzzer_buzz(200, 440);
			buzzer_buzz(200, 880);
			buzzer_buzz(200, 1200);
			buzzer_buzz(200, 440/2);
			buzzer_buzz(200, 880/2);
			buzzer_buzz(200, 1200/2);
			break;
		}

		/* Keep updating our position */

		for(;;)
		{
			/* Bumper */
			while(!HAL_GPIO_ReadPin(GPIOD, bumper_Pin))
			{
				buzzer_buzz(200,1000);
				buzzer_buzz(200,900);
				lcdout_printf("BUMPED");
			}

			vector2d_t our_pos;
			Setglobalvector(); /* Get the current GPS coord */
			Getglobalvector(&our_pos);

			/* Check distance to next wp */
			double dist;
			double dx = our_pos.x-WP.x;
			double dy = our_pos.y-WP.y;
			dist = sqrt(pow(dx,2)+pow(dy,2));
			if(dist<WP_RADIUS)
			{
				lcdout_printf("Waypoint\nreached");
				buzzer_buzz(400, 440);
				buzzer_buzz(400, 880);
				buzzer_buzz(400, 1200);
				osDelay(1000);
				break;
			}

			double dir = -dir_direction(&our_pos, &WP);

//			// Turn around
			if(dir>90)
			{
				dir = dir - 180;
				buzzer_buzz(200, 1000);
				lcdout_printf("Reversed");
			}
			if(dir<-90)
			{
				dir = dir + 180;
				buzzer_buzz(200, 1000);
				lcdout_printf("Reversed");
			}
			osDelay(50);
			if(dist>0.01) dist=0.01;
			buzzer_buzz(200, (WP_RADIUS/dist)*1000);
			lcdout_printf("POS%.4f|%.4f\nDIR%.1f", our_pos.x, our_pos.y, dir);
			set_servo_angle(dir);
			osDelay(1000);

		}




		current_wp_index++;
	}
}

int student_start_program(int ID)
{
	switch(ID)
	{
	case 8:
	{
		return start_route();
	}
	case 7:
	{
		waypoint_test();
		lcdout_printf("Waypoints placed");
		return 0;
	}
	case 6:
	{
		return next_waypoint();
	}
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



