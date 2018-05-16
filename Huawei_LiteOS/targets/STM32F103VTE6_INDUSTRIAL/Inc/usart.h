/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"
#include "stm32f1xx_hal_rcc_ex.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

extern void _Error_Handler(char *, int);

void Debug_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */


/*---debug uart hardware configuration---*/
#define DEBUG_USARTx                           UART4
#define DEBUG_USARTx_CLK_ENABLE()              __HAL_RCC_UART4_CLK_ENABLE();
#define DEBUG_USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define DEBUG_USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

#define DEBUG_USARTx_FORCE_RESET()             __HAL_RCC_UART4_FORCE_RESET()
#define DEBUG_USARTx_RELEASE_RESET()           __HAL_RCC_UART4_RELEASE_RESET()

/* Definition for USARTx Pins */
#define DEBUG_USARTx_TX_PIN                    GPIO_PIN_10
#define DEBUG_USARTx_TX_GPIO_PORT              GPIOC
#define DEBUG_USARTx_RX_PIN                    GPIO_PIN_11
#define DEBUG_USARTx_RX_GPIO_PORT              GPIOC


/*---sim or wifi usart hardware configuration---*/
//#define SIM_WIFI_USARTx                           USART2
//#define SIM_WIFI_USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()
//#define SIM_WIFI_USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
//#define SIM_WIFI_USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

//#define SIM_WIFI_USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
//#define SIM_WIFI_USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

///* Definition for USARTx Pins */
//#define SIM_WIFI_USARTx_TX_PIN                    GPIO_PIN_2
//#define SIM_WIFI_USARTx_TX_GPIO_PORT              GPIOA
//#define SIM_WIFI_USARTx_RX_PIN                    GPIO_PIN_3
//#define SIM_WIFI_USARTx_RX_GPIO_PORT              GPIOA


/*---sim or wifi usart hardware configuration---*/
//#define SIM_WIFI_USARTx                           USART3
//#define SIM_WIFI_USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()
//#define SIM_WIFI_USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
//#define SIM_WIFI_USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

//#define SIM_WIFI_USARTx_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
//#define SIM_WIFI_USARTx_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

///* Definition for USARTx Pins */
//#define SIM_WIFI_USARTx_TX_PIN                    GPIO_PIN_10
//#define SIM_WIFI_USARTx_TX_GPIO_PORT              GPIOB
//#define SIM_WIFI_USARTx_RX_PIN                    GPIO_PIN_11
//#define SIM_WIFI_USARTx_RX_GPIO_PORT              GPIOB




/*---sim or wifi usart hardware configuration---*/
#define SIM_WIFI_USARTx                           USART2
#define SIM_WIFI_USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define SIM_WIFI_USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define SIM_WIFI_USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define SIM_WIFI_USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define SIM_WIFI_USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
//#define SIM_WIFI_USARTx_TX_PIN                    GPIO_PIN_12
//#define SIM_WIFI_USARTx_TX_GPIO_PORT              GPIOC
//#define SIM_WIFI_USARTx_RX_PIN                    GPIO_PIN_2
//#define SIM_WIFI_USARTx_RX_GPIO_PORT              GPIOD




#define SIM_WIFI_USARTx_TX_PIN                    GPIO_PIN_2
#define SIM_WIFI_USARTx_TX_GPIO_PORT              GPIOA
#define SIM_WIFI_USARTx_RX_PIN                    GPIO_PIN_3
#define SIM_WIFI_USARTx_RX_GPIO_PORT              GPIOA


#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
