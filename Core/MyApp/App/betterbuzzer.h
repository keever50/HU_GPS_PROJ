/*
 * betterbuzzer.h
 *
 *  Created on: Oct 15, 2024
 *      Author: kevin
 */
#include <main.h>
#include "cmsis_os.h"
#ifndef MYAPP_APP_BETTERBUZZER_H_
#define MYAPP_APP_BETTERBUZZER_H_

#define BUZZER_MAX_FREQUENCY_RANGE_HZ 20000
/* After prescaler */
#define BUZZER_TIMER_FREQUENCY_HZ 20000
#define BUZZER_AMPLITUDE_DIV	10
void buzzer_buzz(uint32_t duration_ms, uint32_t frequency_hz);

#endif /* MYAPP_APP_BETTERBUZZER_H_ */
