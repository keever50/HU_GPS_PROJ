/**
 * @file betterbuzzer.h
 * @brief An improved timer and RTOS based buzzer library to replace ports/buzzer.
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

/**
 * @brief play a tone based on duration in milliseconds and hertz. This is a blocking function.
 */
void buzzer_buzz(uint32_t duration_ms, uint32_t frequency_hz);

#endif /* MYAPP_APP_BETTERBUZZER_H_ */
