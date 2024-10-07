/**
* @file lcdout.c
* @brief Thread safe printf like printing to LCD.
*
* @author Kevin Witteveen

* @date 07/10/2024
*/

/* INCLUDES */

#include <lcdout.h>
#include "cmsis_os.h"
#include <lcd.h>
#include <stdarg.h>
#include <stdio.h>
#include <semphr.h>

/* MACROS */

#define LCDOUT_BUFFER_SIZE	128
#define LCDOUT_SM_GIVE	xSemaphoreGive(_lcdout_semaphore)
#define LCDOUT_SM_TAKE  xSemaphoreTake(_lcdout_semaphore, portMAX_DELAY)

/* GLOBALS */

xSemaphoreHandle _lcdout_semaphore;
char lcdout_buffer[LCDOUT_BUFFER_SIZE];

/* PUBLIC FUNCTIONS */

void lcdout_init()
{
	_lcdout_semaphore=xSemaphoreCreateMutex();
}

void lcdout_printf(char* format, ...)
{
	LCDOUT_SM_TAKE;
	va_list args;
	va_start(args, format);

	/* Convert ... arguments to printf like formatting */
	vsnprintf(lcdout_buffer,LCDOUT_BUFFER_SIZE,format, args);

	/* Clear LCD */
	LCD_clear();
	uint8_t line=0;

	/* Print out per character */
	for(size_t i=0;i<LCDOUT_BUFFER_SIZE;i++)
	{
		char c = lcdout_buffer[i];
		if(c=='\0') break;
		if(c=='\n')
		{
			LCD_XY(0, ++line);
			continue;
		}
		LCD_putchar(c);
	}

	va_end(args);
	LCDOUT_SM_GIVE;
}
