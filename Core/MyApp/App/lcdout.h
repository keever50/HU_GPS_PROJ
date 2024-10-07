/**
* @file lcdout.h
* @brief Thread safe printf like printing to LCD.
*
* @author Kevin Witteveen

* @date 07/10/2024
*/

#ifndef MYAPP_APP_LCDOUT_H_
#define MYAPP_APP_LCDOUT_H_

/**
* @brief Init
* @return void
*/
void lcdout_init();

/**
* @brief Thread safe printf like printing to LCD.
* @return void
*/
void lcdout_printf(char* format, ...);

#endif /* MYAPP_APP_LCDOUT_H_ */
