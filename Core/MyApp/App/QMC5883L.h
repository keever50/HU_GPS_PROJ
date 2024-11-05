/**
 * @file QMC5883L.h
 * @brief Compass sensor driver
 *  Created on: Nov 4, 2024
 *      Author: kevin
 */

#ifndef MYAPP_APP_QMC5883L_H_
#define MYAPP_APP_QMC5883L_H_

#include "cmsis_os.h"
#include "main.h"

#define QMC58831_XOUT_LSB	0x00
#define QMC58831_XOUT_MSB	0x01
#define QMC58831_YOUT_LSB	0x02
#define QMC58831_YOUT_MSB	0x03
#define QMC58831_ZOUT_LSB	0x04
#define QMC58831_ZOUT_MSB	0x05
#define QMC58831_CTRL_REG	0x09
	#define QMC58831_CTRL_REG_MODE_MASK 0b11
	#define QMC58831_CTRL_REG_MODE_SHIFT 0
	#define QMC58831_CTRL_REG_MODE_STANDBY 0b00
	#define QMC58831_CTRL_REG_MODE_CONTINUOUS 0b01


/**
 * @brief Initializes the driver. This is required to be run once.
 * @return error
 */
int qmc58831_init(I2C_HandleTypeDef *i2c);
/**
 * @brief Changes the sensor mode
 * @return error
 */
int qmc58831_mode(uint8_t mode);
/**
 * @brief Gets the magnetic XYZ directions
 * @return error
 */
int qmc5883l_get_direction(int16_t *x, int16_t *y, int16_t *z);


#endif /* MYAPP_APP_QMC5883L_H_ */
