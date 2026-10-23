#ifndef AT24C02_H
#define AT24C02_H

#include "main.h"

// 连续写入
HAL_StatusTypeDef AT24C02_WriteBytes(uint8_t addr,
                                     uint8_t *data,
                                     uint16_t len);

// 连续读取
HAL_StatusTypeDef AT24C02_ReadBytes(uint8_t addr,
                                    uint8_t *data,
                                    uint16_t len);

#endif
