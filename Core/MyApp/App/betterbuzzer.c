/*
 * betterbuzzer.c
 *
 *  Created on: Oct 15, 2024
 *      Author: kevin
 */
#include <betterbuzzer.h>
#include <main.h>
#include "cmsis_os.h"
extern TIM_HandleTypeDef htim3;


void buzzer_buzz(uint32_t duration_ms, uint32_t frequency_hz)
{
	uint32_t wavelength=BUZZER_TIMER_FREQUENCY_HZ/frequency_hz;

	__HAL_TIM_SET_AUTORELOAD(&htim3, wavelength);
	__HAL_TIM_SET_COUNTER(&htim3, 0);

	TIM_OC_InitTypeDef sConfigPWM;
	sConfigPWM.Pulse=wavelength/BUZZER_AMPLITUDE_DIV;
	sConfigPWM.OCMode=TIM_OCMODE_PWM1;
	HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigPWM, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	osDelay(duration_ms);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
}
