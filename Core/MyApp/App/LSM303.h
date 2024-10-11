/*
 * LSM303.h
 *
 *  Created on: Oct 11, 2024
 *      Author: kevin
 */

#ifndef MYAPP_APP_LSM303_H_
#define MYAPP_APP_LSM303_H_

#include "cmsis_os.h"
#include "main.h"

#define LSM303_MR_REG_M	0x02
#define LSM303_MR_REG_M_SLEEP 0b11
#define LSM303_MR_REG_M_CONTINUOUS 0b00
#define LSM303_MR_REG_M_SINGLE	0b01

#define LSM303_OUT_X_H_M 0x03
#define LSM303_OUT_X_L_M 0x04
#define LSM303_OUT_Z_H_M 0x05
#define LSM303_OUT_Z_L_M 0x06
#define LSM303_OUT_Y_H_M 0x07
#define LSM303_OUT_Y_L_M 0x08

#define LSM303_SR_REG_M 0x09
#define LSM303_SR_REG_M_DRDY_BIT 0b01
#define LSM303_SR_REG_M_LOCK_BIT 0b10

#define LSM303_CRB_REG_M 0x01
#define LSM303_CRB_REG_M_MASK 0x111
#define LSM303_CRB_REG_M_OFFS 5

#define LSM303_CRA_REG_M 0x00
#define LSM303_CRA_REG_M_DO_MASK 0x111
#define LSM303_CRA_REG_M_DO_OFFS 2


enum lsm303_mag_mode
{
	eLSM303_MAG_MODE_CONTINUOUS,
	eLSM303_MAG_MODE_SINGLE,
	eLSM303_MAG_MODE_SLEEP
};

struct lsm303_mag_vector
{
	int16_t x, y, z;
};

int lsm303_mag_datarate(uint8_t rate);
int lsm303_mag_gain(uint8_t gain);
int lsm303_mag_init(I2C_HandleTypeDef *i2c, uint16_t address);
int lsm303_mag_mode(enum lsm303_mag_mode mode);
int lsm303_mag_get_vector(struct lsm303_mag_vector *vect);

#endif /* MYAPP_APP_LSM303_H_ */
