#include "i2c_bus.h"    
#include "i2c.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_i2c.h"
#include "soft_i2c.h"

HAL_StatusTypeDef I2C_Bus_Init(void)
{
#if I2C_BUS_USE_SOFT

    // 停止硬件I2C，使PB6/PB7由GPIO控制
    if (HAL_I2C_DeInit(&hi2c1) != HAL_OK)
        return HAL_ERROR;

    return SoftI2C_Init();

#else

    return HAL_OK;

#endif
}

HAL_StatusTypeDef I2C_Bus_Write(uint8_t dev_addr,  /* 设备7位地址 */
                                uint8_t mem_addr,  /* 寄存器/存储地址 */
                                uint8_t *data,     /* 待写入数据缓冲区 */
                                uint16_t len)      /* 写入字节数 */
{

#if I2C_BUS_USE_SOFT

    return SoftI2C_Write(dev_addr, mem_addr, data, len);

#else

    return HAL_I2C_Mem_Write(&hi2c1,
                             dev_addr << 1,
                             mem_addr,
                             I2C_MEMADD_SIZE_8BIT,
                             data, len, 100);

#endif

}
HAL_StatusTypeDef I2C_Bus_Read(uint8_t dev_addr,
                               uint8_t mem_addr,
                               uint8_t *data,
                               uint16_t len)
{
#if I2C_BUS_USE_SOFT

    return SoftI2C_Read(dev_addr, mem_addr, data, len);

#else

    return HAL_I2C_Mem_Read(&hi2c1,
                            dev_addr << 1,
                            mem_addr,
                            I2C_MEMADD_SIZE_8BIT,
                            data, len, 100);

#endif
}
HAL_StatusTypeDef I2C_Bus_WaitReady(uint8_t dev_addr)
{
#if I2C_BUS_USE_SOFT

    return SoftI2C_WaitReady(dev_addr);

#else

    return HAL_I2C_IsDeviceReady(&hi2c1,
                                 dev_addr << 1,
                                 20, 10);

#endif
}

HAL_StatusTypeDef I2C_Bus_Recover(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    uint8_t bus_ok = 0;

    /* 强制关闭（复位）硬件 I2C 外设 hi2c1。
    如果复位失败，则直接向上层返回错误。*/ 
    if (HAL_I2C_DeInit(&hi2c1) != HAL_OK)
        return HAL_ERROR;

    
    __HAL_RCC_GPIOB_CLK_ENABLE();//开启 GPIOB 端口的硬件时钟

    // 先释放两根线
    HAL_GPIO_WritePin(GPIOB,
                      GPIO_PIN_6 | GPIO_PIN_7,
                      GPIO_PIN_SET);

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;//将 PB6、PB7 改成 GPIO 开漏输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_Delay(1);

    // 3. 首先检查 SCL 能否变高
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_SET)
    {
        // 4. 如果 SDA 被拉低，尝试最多9个时钟
        for (uint8_t i = 0; i < 9; i++)
        {
            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)
                == GPIO_PIN_SET)
            {
                break;
            }

            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6,
                              GPIO_PIN_RESET);
            HAL_Delay(1);

            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6,
                              GPIO_PIN_SET);
            HAL_Delay(1);

            // SCL 无法恢复高电平，终止尝试
            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)
                == GPIO_PIN_RESET)
            {
                break;
            }
        }

        // 5. 两根线都能释放，生成 STOP
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_SET &&
            HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET)
        {
            // SCL 先拉低，SDA 再拉低
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6,
                              GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,
                              GPIO_PIN_RESET);
            HAL_Delay(1);

            // 释放 SCL
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6,
                              GPIO_PIN_SET);
            HAL_Delay(1);

            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)
                == GPIO_PIN_SET)
            {
                // SDA 低到高，形成 STOP
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,
                                  GPIO_PIN_SET);
                HAL_Delay(1);

                if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)
                    == GPIO_PIN_SET)
                {
                    bus_ok = 1;
                }
            }
        }
    }

    // 6. 重新初始化硬件 I2C
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
        return HAL_ERROR;

    return bus_ok ? HAL_OK : HAL_ERROR;
}
HAL_StatusTypeDef I2C_Bus_Reinit(void)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_DeInit(&hi2c1);

    if (status != HAL_OK)
        return status;

    status = HAL_I2C_Init(&hi2c1);

    return status;
}