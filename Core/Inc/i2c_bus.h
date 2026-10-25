
#ifndef I2C_BUS_H
#define I2C_BUS_H

#include "main.h"
#define I2C_BUS_USE_SOFT 1

HAL_StatusTypeDef I2C_Bus_Init(void);

HAL_StatusTypeDef I2C_Bus_Write(uint8_t dev_addr,   /* 从机 7 位设备地址(左移 1 位后的写地址) */
                                uint8_t mem_addr,   /* 从机内部寄存器/存储地址 */
                                uint8_t *data,      /* 待写入数据缓冲区首地址 */
                                uint16_t len);      /* 写入字节数 */

HAL_StatusTypeDef I2C_Bus_Read(uint8_t dev_addr,    /* 从机 7 位设备地址(左移 1 位后的读写地址) */
                               uint8_t mem_addr,    /* 从机内部寄存器/存储地址 */
                               uint8_t *data,       /* 存放读取数据的缓冲区首地址 */
                               uint16_t len);       /* 读取字节数 */

HAL_StatusTypeDef I2C_Bus_WaitReady(uint8_t dev_addr);


HAL_StatusTypeDef I2C_Bus_Recover(void);//I2C 总线恢复函数

HAL_StatusTypeDef I2C_Bus_Reinit(void);
#endif