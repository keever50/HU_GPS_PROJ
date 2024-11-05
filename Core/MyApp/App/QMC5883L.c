/**
 * @file QMC5883L.c
 * @brief Compass sensor driver
 *  Created on: Nov 4, 2024
 *      Author: kevin
 */

#include <QMC5883L.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <semphr.h>

#define QMC58831_SM_TAKE  xSemaphoreTake(_qmc5883l_semaphore, portMAX_DELAY)
#define QMC58831_SM_GIVE	xSemaphoreGive(_qmc5883l_semaphore)

/* PRIVATE GLOBALS */

xSemaphoreHandle _qmc5883l_semaphore;
I2C_HandleTypeDef *_qmc5883l_selected_i2c;
uint16_t _qmc5883l_selected_address=0b0001101;
char _qmc5883l_initialized=0;

/* PRIVATE FUNC */

int _qmc58831_write_reg(uint8_t address, uint8_t byte)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(_qmc5883l_selected_i2c, _qmc5883l_selected_address, address, 1, &byte, 1, 1000);
	if(status!=HAL_OK)
		return -1;
	return 0;

//	uint8_t buff[]={address,byte};
//	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(_qmc5883l_selected_i2c, _qmc5883l_selected_address, buff, sizeof(buff), 100);
//	if(status!=HAL_OK)
//		return -1;
//	return 0;
}

int _qmc58831_read_reg(uint8_t address, uint8_t *byte)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(_qmc5883l_selected_i2c, _qmc5883l_selected_address, address, 1, byte, 1, 1000);
	if(status!=HAL_OK)
		return -1;
	return 0;
}

/* PUBLIC FUNC */

int qmc58831_init(I2C_HandleTypeDef *i2c)
{
	if(_qmc5883l_initialized) return -1;
	_qmc5883l_initialized=1;
	_qmc5883l_semaphore = xSemaphoreCreateMutex();
	_qmc5883l_selected_i2c=i2c;

	return qmc58831_mode(QMC58831_CTRL_REG_MODE_CONTINUOUS);
}

int qmc58831_mode(uint8_t mode)
{
	uint8_t data;
	mode = mode & QMC58831_CTRL_REG_MODE_MASK;
	data = mode << QMC58831_CTRL_REG_MODE_SHIFT;
	return _qmc58831_write_reg(QMC58831_CTRL_REG, data);
}

int qmc5883l_get_direction(int16_t *x, int16_t *y, int16_t *z)
{
	return 0;
}
