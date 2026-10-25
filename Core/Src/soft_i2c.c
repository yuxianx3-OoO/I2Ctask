#include "soft_i2c.h"

// PB6 = SCL, PB7 = SDA
#define SCL_LOW()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define SCL_HIGH() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)

#define SDA_LOW()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
#define SDA_HIGH() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)

#define READ_SCL() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)
#define READ_SDA() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)

// DWT 微秒延时
static void I2C_Delay(void)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = SystemCoreClock / 1000000U * 5U;

    while ((uint32_t)(DWT->CYCCNT - start) < cycles);
}

// 释放 SCL，并等待其实际变高
static HAL_StatusTypeDef SCL_Release(void)
{
    SCL_HIGH();

    uint32_t start = DWT->CYCCNT;

    while (READ_SCL() == GPIO_PIN_RESET)
    {
        if ((uint32_t)(DWT->CYCCNT - start)
            >= SystemCoreClock / 1000U)//1 毫秒超时（防死锁）
        {
            return HAL_TIMEOUT;
        }
    }

    return HAL_OK;
}

// 初始化软件 I2C
HAL_StatusTypeDef SoftI2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 开启 DWT 计数器
    //  |=按位或：只要有1就是1
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // 1. 打开内核调试外设的总电源（否则 DWT 没电）
    DWT->CYCCNT = 0;
    // 3. 按下开始键（硬件开始每个时钟周期自动 +1）
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    SDA_HIGH();
    SCL_HIGH();

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    I2C_Delay();

    return HAL_OK;
}

// START，也可用于重复起始
static HAL_StatusTypeDef I2C_Start(void)
{
    SDA_HIGH();
    I2C_Delay();

    HAL_StatusTypeDef status = SCL_Release();
    if (status != HAL_OK)
        return status;

    if (READ_SDA() == GPIO_PIN_RESET)
        return HAL_BUSY;

    I2C_Delay();

    SDA_LOW();
    I2C_Delay();

    SCL_LOW();
    I2C_Delay();

    return HAL_OK;
}

// STOP
static HAL_StatusTypeDef I2C_Stop(void)
{
    SCL_LOW();
    SDA_LOW();
    I2C_Delay();

    HAL_StatusTypeDef status = SCL_Release();
    if (status != HAL_OK)
        return status;

    I2C_Delay();

    SDA_HIGH();
    I2C_Delay();

    return READ_SDA() == GPIO_PIN_SET
           ? HAL_OK : HAL_BUSY;
}

// 发送8位数据，并检查第9位ACK
static HAL_StatusTypeDef I2C_SendByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        SCL_LOW();

        if (data & 0x80)
            SDA_HIGH();
        else
            SDA_LOW();

        I2C_Delay();

        HAL_StatusTypeDef status = SCL_Release();
        if (status != HAL_OK)
            return status;

        I2C_Delay();

        SCL_LOW();
        I2C_Delay();

        data <<= 1;
    }

    // 第9个时钟，释放SDA，让接收方应答
    SDA_HIGH();
    I2C_Delay();

    HAL_StatusTypeDef status = SCL_Release();
    if (status != HAL_OK)
        return status;

    I2C_Delay();

    uint8_t ack = (READ_SDA() == GPIO_PIN_RESET);

    SCL_LOW();
    I2C_Delay();

    return ack ? HAL_OK : HAL_ERROR;
}

// 接收8位数据，并发送ACK/NACK
static HAL_StatusTypeDef I2C_ReadByte(uint8_t *data,
                                      uint8_t ack)
{
    *data = 0;
    SDA_HIGH();

    for (uint8_t i = 0; i < 8; i++)
    {
        SCL_LOW();
        I2C_Delay();

        HAL_StatusTypeDef status = SCL_Release();
        if (status != HAL_OK)
            return status;

        I2C_Delay();

        *data <<= 1;/*取出 *data 指向的值；把它左移 1 位；
                      把结果写回 *data 指向的位置*/

        if (READ_SDA() == GPIO_PIN_SET)
            *data |= 1;

        SCL_LOW();/*处理时钟拉伸*/
        I2C_Delay();
    }

    // ack=1发送ACK，ack=0发送NACK
    if (ack)
        SDA_LOW();
    else
        SDA_HIGH();

    I2C_Delay();

    HAL_StatusTypeDef status = SCL_Release();
    if (status != HAL_OK)
        return status;

    I2C_Delay();

    SCL_LOW();
    SDA_HIGH();
    I2C_Delay();

    return HAL_OK;
}
HAL_StatusTypeDef SoftI2C_Write(uint8_t dev_addr,
                                uint8_t mem_addr,
                                uint8_t *data,
                                uint16_t len)
{
    HAL_StatusTypeDef status;

    // 1. 检查参数
    if (data == NULL || len == 0)
    {
        return HAL_ERROR;
    }

    // 2. 发送起始信号
    status = I2C_Start();

    if (status != HAL_OK)
    {
        return status;
    }

    // 3. 发送设备地址（写方向）
    status = I2C_SendByte(dev_addr << 1);

    if (status != HAL_OK)
    {
        I2C_Stop();
        return status;
    }

    // 4. 发送 EEPROM 内部存储地址
    status = I2C_SendByte(mem_addr);

    if (status != HAL_OK)
    {
        I2C_Stop();
        return status;
    }

    // 5. 依次发送数据
    for (uint16_t i = 0; i < len; i++)
    {
        status = I2C_SendByte(data[i]);

        if (status != HAL_OK)
        {
            I2C_Stop();
            return status;
        }
    }

    // 6. 全部发送成功，产生停止信号
    return I2C_Stop();
}
HAL_StatusTypeDef SoftI2C_Read(uint8_t dev_addr,
                               uint8_t mem_addr,
                               uint8_t *data,
                               uint16_t len)
{
    HAL_StatusTypeDef status;

    if (data == NULL || len == 0)
    {
        return HAL_ERROR;
    }

    // 1. START
    status = I2C_Start();

    if (status != HAL_OK)
    {
        return status;
    }

    // 2. 发送设备地址（写方向）
    status = I2C_SendByte(dev_addr << 1);

    if (status != HAL_OK)
    {
        I2C_Stop();
        return status;
    }

    // 3. 指定 EEPROM 内部存储地址
    status = I2C_SendByte(mem_addr);

    if (status != HAL_OK)
    {
        I2C_Stop();
        return status;
    }

    // 4. 发送重复起始信号 RESTART
    status = I2C_Start();

    if (status != HAL_OK)
    {
        I2C_Stop();
        return status;
    }

    // 5. 发送设备地址（读方向）
    status = I2C_SendByte((dev_addr << 1) | 1);

    if (status != HAL_OK)
    {
        I2C_Stop();
        return status;
    }

    // 6. 连续接收数据
    for (uint16_t i = 0; i < len; i++)
    {
        uint8_t ack = 1;

        // 最后一个字节发送 NACK
        if (i == len - 1)
        {
            ack = 0;
        }

        status = I2C_ReadByte(&data[i], ack);

        if (status != HAL_OK)
        {
            I2C_Stop();
            return status;
        }
    }

    // 7. 读取完成，发送 STOP
    return I2C_Stop();
}

/*I2C 设备就绪轮询（Polling）” 函数，通常专门用来等待 EEPROM 写完数据*/
HAL_StatusTypeDef SoftI2C_WaitReady(uint8_t dev_addr)
{
    uint32_t start = HAL_GetTick();
  /*记录当前时间。如果超过了 20 毫秒还没准备好，
  就退出循环，返回 HAL_TIMEOUT*/
    while ((uint32_t)(HAL_GetTick() - start) < 20U)
    {
        HAL_StatusTypeDef status = I2C_Start();

        if (status != HAL_OK)
            return status;

        status = I2C_SendByte(dev_addr << 1);
        /*如果是写操作，且 EEPROM 还在忙，
        它就不会拉低 SDA，主机就会收到 NACK（返回 HAL_ERROR）*/

        HAL_StatusTypeDef stop_status = I2C_Stop();

        if (stop_status != HAL_OK)
            return stop_status;

        if (status == HAL_OK)
            return HAL_OK;

        
        if (status != HAL_ERROR)
            return status;
        /*如果状态既不是 HAL_OK，也不是 HAL_ERROR
        （比如是超时、总线错误等严重故障）,
        那就没必要重试了，直接把这个错误返回给上层*/

        HAL_Delay(1);
    }

    return HAL_TIMEOUT;
}