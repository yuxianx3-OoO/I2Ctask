/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "at24c02.h"
#include "i2c_bus.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
    uint8_t data = (uint8_t)ch;

    HAL_UART_Transmit(&huart1, &data, 1, 100);

    return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  I2C_Bus_Init();
volatile uint32_t idle_count = 0;

uint8_t read_buf[256];

uint32_t start = HAL_GetTick();

HAL_StatusTypeDef status =
    AT24C02_ReadBytes(0x00, read_buf, 256);

if (status == HAL_OK)
{
    while (HAL_GetTick() - start < 100)
    {
        idle_count++;
    }

    printf("Idle Count = %lu\r\n",
           (unsigned long)idle_count);
}
    /*软件通信验证*/
//   if (I2C_Bus_Init() != HAL_OK)
// {
//     printf("Soft I2C Init Failed\r\n");
// }
// else
// {
//     uint8_t data = 0;

//     HAL_StatusTypeDef status =
//         AT24C02_ReadBytes(0x10, &data, 1);

//     if (status == HAL_OK)
//     {
//         printf("Soft I2C Read OK\r\n");
//         printf("Data = 0x%02X\r\n", data);
//     }
//     else
//     {
//         printf("Soft I2C Read Failed: %d\r\n",
//                (int)status);
//     }
// }
  //硬件初步容错处理策略
  // uint8_t data = 0;

  // HAL_StatusTypeDef status;

  // status = AT24C02_ReadBytes(0x10, &data, 1);

  // if (status == HAL_OK)
  // {
  //     printf("Read OK: 0x%02X\r\n", data);
  // }
  // else
  // {
  //     // 保存本次错误信息
  //     uint32_t error = HAL_I2C_GetError(&hi2c1);

  //     // 读取物理总线电平
  //     uint8_t scl =
  //         HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_SET;

  //     uint8_t sda =
  //         HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET;

  //     // 读取外设 BUSY 标志
  //     uint8_t busy =
  //         __HAL_I2C_GET_FLAG(&hi2c1, I2C_FLAG_BUSY) == SET;

  //     printf("Status=%d Error=0x%08lX\r\n",
  //           (int)status, (unsigned long)error);

  //     printf("SCL=%u SDA=%u BUSY=%u\r\n",
  //           scl, sda, busy);

  //     if ((error & HAL_I2C_ERROR_AF) && scl && sda)
  //     {
  //         printf("NACK: Check device address\r\n");
  //     }
  //     else if (scl == 0)
  //     {
  //         printf("SCL held LOW: Recovery aborted\r\n");
  //     }
  //     else
  //     {
  //         HAL_StatusTypeDef result;

  //         if (sda == 0)
  //         {
  //             printf("SDA held LOW: Bus recovery\r\n");
  //             result = I2C_Bus_Recover();
  //         }
  //         else
  //         {
  //             printf("Lines HIGH: Reinit I2C\r\n");
  //             result = I2C_Bus_Reinit();
  //         }

  //         if (result == HAL_OK)
  //         {
  //             status = AT24C02_ReadBytes(0x10, &data, 1);

  //             if (status == HAL_OK)
  //                 printf("Recovery PASS: 0x%02X\r\n", data);
  //             else
  //                 printf("Recovery read failed\r\n");
  //         }
  //         else
  //         {
  //             printf("Recovery procedure failed\r\n");
  //         }
  //     }
  // }
  //SDA 断线测试
  // uint8_t data = 0;

  // HAL_StatusTypeDef status =
  //     AT24C02_ReadBytes(0x10, &data, 1);

  // // 立即记录本次通信的错误信息
  // uint32_t error = HAL_I2C_GetError(&hi2c1);

  // printf("Status = %d\r\n", (int)status);
  // printf("Error = 0x%08lX\r\n", (unsigned long)error);

  // if (status == HAL_OK)
  // {
  //     printf("Read OK: 0x%02X\r\n", data);
  // }
  // else
  // {
  //     printf("Read Failed\r\n");

  //     if (error & HAL_I2C_ERROR_AF)
  //         printf("NACK detected\r\n");

  //     if (status == HAL_TIMEOUT ||
  //         (error & HAL_I2C_ERROR_TIMEOUT))
  //         printf("Timeout detected\r\n");
  // }
  // // 第一步：主动调用总线恢复函数
  // status = I2C_Bus_Recover();
  // if (status == HAL_OK)
  // {
  //     printf("I2C Recover OK\r\n");
  // }
  // else
  // {
  //     printf("I2C Recover Failed\r\n");
  // }

  // // 第二步：再次从AT24C02读取数据
  // status = AT24C02_ReadBytes(0x10, &data, 1);

  // // 第三步：检查读取是否成功
  // if (status == HAL_OK)
  // {
  //     printf("Read OK: 0x%02X\r\n", data);
  // }
  // else
  // {
  //     printf("Read Failed\r\n");
  // }    
  //I2C 总线挂死与自动恢复
  // uint8_t data = 0;

  // // 1. 故意读取错误设备地址
  // HAL_StatusTypeDef status;

  // status = AT24C02_ReadBytes(0x10, &data, 1);

  // // 立即保存错误信息
  // uint32_t error = HAL_I2C_GetError(&hi2c1);

  // printf("Status = %d\r\n", (int)status);
  // printf("Error = 0x%08lX\r\n", (unsigned long)error);

  // // 2. 判断故障类型
  // if (error & HAL_I2C_ERROR_AF)
  // {
  //     printf("NACK detected\r\n");
  // }

  // if (error & HAL_I2C_ERROR_BERR)
  // {
  //     printf("Bus error detected\r\n");
  // }

  // if (error & HAL_I2C_ERROR_ARLO)
  // {
  //     printf("Arbitration lost\r\n");
  // }

  // if (status == HAL_TIMEOUT ||
  //     (error & HAL_I2C_ERROR_TIMEOUT))
  // {
  //     printf("Timeout detected\r\n");
  // }

  // // 3. 读取真实设备
  // status = AT24C02_ReadBytes(0x10, &data, 1);

  // if (status == HAL_OK)
  // {
  //     printf("Communication restored\r\n");
  //     printf("Data = 0x%02X\r\n", data);
  // }
  // else
  // {
  //     printf("Communication still failed\r\n");
  // }
   //模拟一次 I2C 通信异常
//   uint8_t data = 0;
//   HAL_StatusTypeDef status;

//   status = AT24C02_ReadBytes(0x10, &data, 1);
//   if (status == HAL_OK)
// {
//     printf("Read OK: 0x%02X\r\n", data);
// }
// else
// {
//     uint32_t error = HAL_I2C_GetError(&hi2c1);

//     printf("Read Failed!\r\n");
//     printf("HAL Status: %d\r\n", (int)status);
//     printf("I2C Error: 0x%08lX\r\n",
//            (unsigned long)error);

//     if (error & HAL_I2C_ERROR_AF)/*判断error 里是否包含 AF（应答失败）这个错误位*/
//     {
//         printf("No ACK received!\r\n");
//     }

//     if (error & HAL_I2C_ERROR_BERR)
//     {
//         printf("Bus Error!\r\n");
//     }

//     if (error & HAL_I2C_ERROR_ARLO)
//     {
//         printf("Arbitration Lost!\r\n");
//     }
// }
  //测试跨页写入及连续读取
  // static uint8_t write_buf[256];
  // static uint8_t read_buf[256];
  // //这里使用 static，让数组使用静态存储空间，避免两个较大的数组占用函数调用栈
  // HAL_StatusTypeDef status;
  // uint16_t errors = 0;

  // for (int i = 0; i < 256; i++)
  // {
  //   write_buf[i] = i;
  //   read_buf[i] = 0;
  // }

  
  // status = AT24C02_WriteBytes(0x00, write_buf, 256);

  // if (status==HAL_OK)
  // {
  //   status = AT24C02_ReadBytes(0x00, read_buf, 256);//现在 main.c 不再直接调用 HAL 的 EEPROM 读写函数
  //   if (status==HAL_OK)
  //   {
  //     printf("Read 256 bytes OK\r\n");
  //     for(int i=0; i<256 ;i++)
  //     {
        
  //       if (write_buf[i] != read_buf[i])
  //       {
  //         errors++;
  //         printf("Error at 0x%02X\r\n", i);
  //       }

  //     }
      
  //     if (errors == 0)
  //         printf("Full EEPROM Test PASS!\r\n");
  //     else
  //         printf("Full EEPROM Test FAIL: %u errors\r\n",  (unsigned int)errors);
                 
  //   }
  // }
  // else
  // {
  //   printf("Write Error\r\n");
  // }
  
  // uint8_t write_data = 0x55;单字节上传测试

  // uint8_t read_data = 0;

  // HAL_StatusTypeDef status;

  // status = HAL_I2C_Mem_Write(
  //   &hi2c1,                     // 1. I2C 句柄
  //   0x50 << 1,                  // 2. 设备地址 (DevAddress)
  //   0x10,                       // 3. 内部存储地址 (MemAddress)
  //   I2C_MEMADD_SIZE_8BIT,       // 4. 内部地址大小 (MemAddSize)
  //   &write_data,                // 5. 数据源指针 (pData)
  //   1,                          // 6. 数据长度 (Size)
  //   100                         // 7. 超时时间 (Timeout)
  // );

  // if (status == HAL_OK)
  // {
  //   HAL_Delay(10); // Wait for the write operation to complete
  //   status = HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, 0x10, I2C_MEMADD_SIZE_8BIT, &read_data, 1, 100);
  //   if (status == HAL_OK)
  //   {
  //     char text[50];
  //     sprintf(text, "Write: 0x%02X\r\n", write_data);
  //     HAL_UART_Transmit(&huart1, (uint8_t *)text, strlen(text), 100);
  //     sprintf(text, "Read: 0x%02X\r\n", read_data);
  //     HAL_UART_Transmit(&huart1, (uint8_t *)text, strlen(text), 100);
  //     if (read_data == write_data)
  //     {
  //       HAL_UART_Transmit(&huart1, (uint8_t *)"Data matches!\r\n", 15, 100);
  //     }
  //     else
  //     {
  //       HAL_UART_Transmit(&huart1, (uint8_t *)"Data does not match!\r\n", 23, 100);
  //     }
  //   }
  //   else
  //   {
  //     HAL_UART_Transmit(&huart1, (uint8_t *)"Read operation failed\r\n", 24, 100);
  //   }
  // }
  // else
  // {
  //   HAL_UART_Transmit(&huart1, (uint8_t *)"Write operation failed\r\n", 25, 100);
  // }    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    // status = HAL_I2C_IsDeviceReady(&hi2c1, 0x50<<1, 3, 100);
    // if (status == HAL_OK)
    // {
    //   HAL_UART_Transmit(&huart1, (uint8_t *)"Device is ready\r\n", 17, 100);
    // }
    // else
    // {
    //   HAL_UART_Transmit(&huart1, (uint8_t *)"Device is not ready\r\n", 21, 100);
    // }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
