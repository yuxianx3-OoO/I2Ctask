#ifndef SOFT_I2C_H
#define SOFT_I2C_H

#include "main.h"

HAL_StatusTypeDef SoftI2C_Init(void);

HAL_StatusTypeDef SoftI2C_Write(uint8_t dev_addr,
                                uint8_t mem_addr,
                                uint8_t *data,
                                uint16_t len);

HAL_StatusTypeDef SoftI2C_Read(uint8_t dev_addr,
                               uint8_t mem_addr,
                               uint8_t *data,
                               uint16_t len);

HAL_StatusTypeDef SoftI2C_WaitReady(uint8_t dev_addr);

#endif