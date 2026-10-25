#include "at24c02.h"
#include "i2c_bus.h"
#include "stm32f1xx_hal_def.h"

#define AT24C02_ADDR  0x50
#define AT24C02_SIZE     256
#define AT24C02_PAGE_SIZE     8

HAL_StatusTypeDef AT24C02_WriteBytes(uint8_t addr, uint8_t *data, uint16_t len)
{
  if (data == NULL || len == 0 ||
      addr >= AT24C02_SIZE ||
      len > AT24C02_SIZE - addr)
    return HAL_ERROR;
  HAL_StatusTypeDef status;
  while(len>0)
  {
     // 计算当前页还剩多少字节
    uint16_t page_left = AT24C02_PAGE_SIZE-(addr%AT24C02_PAGE_SIZE);
     // 本次最多写多少字节
    uint16_t count=len>page_left?page_left:len;
    status= I2C_Bus_Write( AT24C02_ADDR,
                                   addr,
                                   data,
                                   count);
    if(status != HAL_OK)
      return status;
    status = I2C_Bus_WaitReady(AT24C02_ADDR);
    if(status != HAL_OK)
      return status;
    addr+=count;
    data+=count;
    len-=count;
  }
  return HAL_OK;
}

HAL_StatusTypeDef AT24C02_ReadBytes(uint8_t addr,
                                    uint8_t *data,
                                    uint16_t len)
{
    if (data == NULL || len == 0 ||
        addr >= AT24C02_SIZE ||
        len > AT24C02_SIZE - addr)
    {
        return HAL_ERROR;
    }

    return I2C_Bus_Read(AT24C02_ADDR,
                        addr,
                        data,
                        len);
                            
}