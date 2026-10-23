#include "i2c_bus.h"    
#include "i2c.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_i2c.h"

HAL_StatusTypeDef I2C_Bus_Write(uint8_t dev_addr,  /* 设备7位地址 */
                                uint8_t mem_addr,  /* 寄存器/存储地址 */
                                uint8_t *data,     /* 待写入数据缓冲区 */
                                uint16_t len)      /* 写入字节数 */
{

    return HAL_I2C_Mem_Write(&hi2c1,
                             dev_addr << 1,
                             mem_addr,
                             I2C_MEMADD_SIZE_8BIT,
                             data,
                             len,
                             100);
}
HAL_StatusTypeDef I2C_Bus_Read(uint8_t dev_addr,  /* 设备7位地址 */
                               uint8_t mem_addr,  /* 寄存器/存储地址 */
                               uint8_t *data,     /* 读取数据缓冲区 */
                               uint16_t len)      /* 读取字节数 */
{
    HAL_StatusTypeDef status = HAL_ERROR;
    for(uint8_t i=0 ; i<3 ;i++) /*读取失败时最多尝试 3 次，仍然失败就返回错误*/
    {
        status= HAL_I2C_Mem_Read(&hi2c1,
                                 dev_addr << 1,
                                 mem_addr,
                                 I2C_MEMADD_SIZE_8BIT,
                                 data,
                                 len,
                                 100);
        if(status==HAL_OK)
        {
            return HAL_OK;
        }
        if(i<2) 
            HAL_Delay(2);                        

    }

    return status;
}
HAL_StatusTypeDef I2C_Bus_WaitReady(uint8_t dev_addr)  /* 设备7位地址 */
{
    return HAL_I2C_IsDeviceReady(&hi2c1,
                                 dev_addr << 1,
                                 20,   /* 尝试次数 */
                                 10);  /* 每次尝试超时(ms) */
}