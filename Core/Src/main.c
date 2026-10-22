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
HAL_StatusTypeDef AT24C02_WriteByte(uint8_t addr , uint8_t *data,uint16_t len)
{
  if(data == NULL || len == 0 || len>256-addr)
    return HAL_ERROR;
  HAL_StatusTypeDef status;
  while(len>0)
  {
     // 计算当前页还剩多少字节
    uint16_t page_left = 8-(addr%8);
     // 本次最多写多少字节
    uint16_t count=len>page_left?page_left:len;
    status= HAL_I2C_Mem_Write(&hi2c1,0x50<<1,addr,I2C_MEMADD_SIZE_8BIT,data,count,100);
    if(status != HAL_OK)
      return status;
    status = HAL_I2C_IsDeviceReady(&hi2c1,0x50<<1,20,100);
    if(status != HAL_OK)
      return status;
    addr+=count;
    data+=count;
    len-=count;
  }
  return HAL_OK;
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
  //测试跨页写入及连续读取
  uint8_t write_buf[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
  uint8_t read_buf[5] = {0};

  HAL_StatusTypeDef status;

  // 从 0x06 开始写入 5 字节
  status = AT24C02_WriteByte(0x06, write_buf, 5);

  if (status==HAL_OK)
  {
    status = HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, 0x06, I2C_MEMADD_SIZE_8BIT, read_buf, 5, 100);
    if (status==HAL_OK)
    {
      uint8_t match=1;
      for(int i=0; i<5 ;i++)
      {
        printf("0x%02X: Write=%02X Read=%02X\r\n",0x06+i,write_buf[i],read_buf[i]);
        
        if (write_buf[i] != read_buf[i])
          match=0;

      }
      
      if (match)
          printf("Page Test PASS\r\n");
      else
          printf("Page Test FAIL\r\n");
    }
  }
  else
  {
    printf("Write Error\r\n");
  }
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
