/**
  ******************************************************************************
  * File Name          : USART.c
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

//UART_HandleTypeDef huart1;
UART_HandleTypeDef debug_uart_handle;
extern UART_HandleTypeDef SIM_UsartHandle;

/* USART1 init function */
void Debug_UART_Init(void)
{
	debug_uart_handle.Instance = UART4;
	debug_uart_handle.Init.BaudRate = 115200;
	debug_uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	debug_uart_handle.Init.StopBits = UART_STOPBITS_1;
	debug_uart_handle.Init.Parity = UART_PARITY_NONE;
	debug_uart_handle.Init.Mode = UART_MODE_TX_RX;
	debug_uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	debug_uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&debug_uart_handle) != HAL_OK)
	{
			_Error_Handler(__FILE__, __LINE__);
	}
}


void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(huart->Instance==UART4)
	{
		/* USER CODE BEGIN USART1_MspInit 0 */
		
		GPIO_InitTypeDef  GPIO_InitStruct;

		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable GPIO TX/RX clock */
		DEBUG_USARTx_TX_GPIO_CLK_ENABLE();
		DEBUG_USARTx_RX_GPIO_CLK_ENABLE();

		/* Enable USARTx clock */
		DEBUG_USARTx_CLK_ENABLE();

		/*##-2- Configure peripheral GPIO ##########################################*/
		//debug uart tx pin -->PC10
		//debug uart rx pin -->PC11
		
		/* UART TX GPIO pin configuration  */
		GPIO_InitStruct.Pin       = DEBUG_USARTx_TX_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

		HAL_GPIO_Init(DEBUG_USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

		/* UART RX GPIO pin configuration  */
		GPIO_InitStruct.Pin = DEBUG_USARTx_RX_PIN;

		HAL_GPIO_Init(DEBUG_USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

		/* USER CODE END USART1_MspInit 0 */
	}
	else if(huart->Instance == SIM_WIFI_USARTx)
	{
		/* USER CODE BEGIN USART1_MspInit 1 */
		GPIO_InitTypeDef  GPIO_InitStruct;

		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable GPIO TX/RX clock */
		SIM_WIFI_USARTx_TX_GPIO_CLK_ENABLE();
		SIM_WIFI_USARTx_RX_GPIO_CLK_ENABLE();

		/* Enable USARTx clock */
		SIM_WIFI_USARTx_CLK_ENABLE();

		/*##-2- Configure peripheral GPIO ##########################################*/
		/* UART TX GPIO pin configuration  */
		GPIO_InitStruct.Pin       = SIM_WIFI_USARTx_TX_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	   // GPIO_InitStruct.

		HAL_GPIO_Init(SIM_WIFI_USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

		/* UART RX GPIO pin configuration  */
		GPIO_InitStruct.Pin = SIM_WIFI_USARTx_RX_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;

		HAL_GPIO_Init(SIM_WIFI_USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
		
		/* USER CODE END USART1_MspInit 1 */
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if(huart->Instance==UART4)
	{
		/* USER CODE BEGIN USART1_MspDeInit 0 */
		
		/*##-1- Reset peripherals ##################################################*/
		DEBUG_USARTx_FORCE_RESET();
		DEBUG_USARTx_RELEASE_RESET();

		/*##-2- Disable peripherals and GPIO Clocks #################################*/
		/* Configure UART Tx as alternate function  */
		HAL_GPIO_DeInit(DEBUG_USARTx_TX_GPIO_PORT, DEBUG_USARTx_TX_PIN);
		/* Configure UART Rx as alternate function  */
		HAL_GPIO_DeInit(DEBUG_USARTx_RX_GPIO_PORT, DEBUG_USARTx_RX_PIN);
		
		/* USER CODE END USART1_MspDeInit 0 */
	}
	else if(huart->Instance == SIM_WIFI_USARTx)
	{
		/* USER CODE BEGIN USART1_MspDeInit 1 */
		
		/*##-1- Reset peripherals ##################################################*/
		SIM_WIFI_USARTx_FORCE_RESET();
		SIM_WIFI_USARTx_RELEASE_RESET();
		
		/*##-2- Disable peripherals and GPIO Clocks #################################*/
		/* Configure UART Tx as alternate function  */
		HAL_GPIO_DeInit(SIM_WIFI_USARTx_TX_GPIO_PORT, SIM_WIFI_USARTx_TX_PIN);
		/* Configure UART Rx as alternate function  */
		HAL_GPIO_DeInit(SIM_WIFI_USARTx_RX_GPIO_PORT, SIM_WIFI_USARTx_RX_PIN);
	
		/* USER CODE END USART1_MspDeInit 1 */
	}
}


/* define fputc */
#if defined ( __CC_ARM ) || defined ( __ICCARM__ )  /* KEIL and IAR: printf will call fputc to print */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&debug_uart_handle, (uint8_t *)&ch, 1, 0xFFFF);
	//HAL_UART_Transmit(&SIM_UsartHandle, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
#elif defined ( __GNUC__ )  /* GCC: printf will call _write to print */
__attribute__((used)) int _write(int fd, char *ptr, int len)
{
    HAL_UART_Transmit(&debug_uart_handle, (uint8_t *)ptr, len, 0xFFFF);
    return len;
}
#endif

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/