/**
 * @file LSM303.c
 * @brief Magnetic sensor library
 *  Created on: Oct 11, 2024
 *      Author: kevin
 */

#include <LSM303.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <semphr.h>
/* MACROS */

#define LSM303_SM_TAKE  xSemaphoreTake(_lsm303_semaphore, portMAX_DELAY)
#define LSM303_SM_GIVE	xSemaphoreGive(_lsm303_semaphore)


/* PRIVATE GLOBALS */

xSemaphoreHandle _lsm303_semaphore;
I2C_HandleTypeDef *_lsm303_selected_i2c;
uint16_t _lsm303_selected_address=0b00111100;
char _lsm303_initialized=0;

/* PRIVATE FUNC */

int _lsm303_write_register(uint8_t addr, uint8_t value)
{
	UART_puts("lsm303 write reg\n");
	uint8_t buff[]={addr,value};
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(_lsm303_selected_i2c, _lsm303_selected_address, buff, sizeof(buff), 100);
	if(status!=HAL_OK)
		return -1;
	return 0;
}

int _lsm303_read_register(uint8_t addr, uint8_t *value)
{
	//HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(_lsm303_selected_i2c, _lsm303_selected_address, &addr, 1, 1000);
	UART_puts("lsm303 read reg\n");
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(_lsm303_selected_i2c, _lsm303_selected_address, addr, 1, value, 1, 100);


	if(status!=HAL_OK)
		return -1;
	return 0;
}

int _lsm303_read_register_seq(uint8_t *value)
{
	//HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(_lsm303_selected_i2c, _lsm303_selected_address, &addr, 1, 1000);
	UART_puts("lsm303 SEQ read reg\n");
	HAL_StatusTypeDef status = HAL_I2C_Master_Receive(_lsm303_selected_i2c, _lsm303_selected_address, value, 1, 100);


	if(status!=HAL_OK)
		return -1;
	return 0;
}

/* PUBLIC FUNC */

int lsm303_mag_datarate(uint8_t rate)
{
	if(!_lsm303_initialized) return -1;
	uint8_t value=0;
	value=rate&LSM303_CRA_REG_M_DO_MASK;
	value=value<<LSM303_CRA_REG_M_DO_OFFS;
	return _lsm303_write_register(LSM303_CRA_REG_M, value);
}

int lsm303_mag_gain(uint8_t gain)
{
	if(!_lsm303_initialized) return -1;
	uint8_t value;
	value=gain&LSM303_CRB_REG_M_MASK;
	value=value<<LSM303_CRB_REG_M_OFFS;
	return _lsm303_write_register(LSM303_CRB_REG_M, value);
}

int lsm303_mag_mode(enum lsm303_mag_mode mode)
{
	if(!_lsm303_initialized) return -1;
	UART_puts("lsm303 mode\n");
	switch(mode)
	{
	case eLSM303_MAG_MODE_CONTINUOUS:
	{
		return _lsm303_write_register(LSM303_MR_REG_M, LSM303_MR_REG_M_CONTINUOUS);
	}
	case eLSM303_MAG_MODE_SINGLE:
	{
		return _lsm303_write_register(LSM303_MR_REG_M, LSM303_MR_REG_M_SINGLE);
	}
	case eLSM303_MAG_MODE_SLEEP:
	{
		return _lsm303_write_register(LSM303_MR_REG_M, LSM303_MR_REG_M_SLEEP);
	}
	default:
		return -1;
	}
}

int lsm303_mag_get_vector(struct lsm303_mag_vector *vect)
{
	if(!_lsm303_initialized) return -1;
	LSM303_SM_TAKE;


	uint8_t L, H;
	if(_lsm303_read_register(LSM303_OUT_X_H_M, &H))
	{
		LSM303_SM_GIVE;
		return -1;
	}

	if(_lsm303_read_register_seq(&L))
	{
		LSM303_SM_GIVE;
		return -1;
	}
	vect->x = H;
	vect->x = vect->x<<8;
	vect->x = vect->x | L;

	if(_lsm303_read_register_seq(&H))
	{
		LSM303_SM_GIVE;
		return -1;
	}
	if(_lsm303_read_register_seq(&L))
	{
		LSM303_SM_GIVE;
		return -1;
	}
	vect->z = H;
	vect->z = vect->z<<8;
	vect->z = vect->z | L;

	if(_lsm303_read_register_seq(&H))
	{
		LSM303_SM_GIVE;
		return -1;
	}
	if(_lsm303_read_register_seq(&L))
	{
		LSM303_SM_GIVE;
		return -1;
	}
	vect->y = H;
	vect->y = vect->y<<8;
	vect->y = vect->y | L;

	LSM303_SM_GIVE;
	return 0;
}

int lsm303_mag_init(I2C_HandleTypeDef *i2c, uint16_t address)
{

	_lsm303_initialized=1;
	UART_puts("lsm303 init\n");
	_lsm303_semaphore = xSemaphoreCreateMutex();
	/* Settings */
	_lsm303_selected_i2c=i2c;
	if(address!=0)
		_lsm303_selected_address=address;

	/* Set default mode and confirm mode */
	int ret = lsm303_mag_mode(eLSM303_MAG_MODE_CONTINUOUS);
	if(ret)
	{
		_lsm303_initialized=0;
		return ret;
	}
	uint8_t mode;
	_lsm303_read_register(LSM303_MR_REG_M, &mode);
	if(mode!=LSM303_MR_REG_M_CONTINUOUS)
	{
		_lsm303_initialized=0;
		return -1;
	}
	lsm303_mag_datarate(0b111);
	lsm303_mag_gain(0);



	return 0;
}
