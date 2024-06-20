/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart8;
DMA_HandleTypeDef hdma_uart8_tx;

/* UART8 init function */
void MX_UART8_Init(void)
{

	/* USER CODE BEGIN UART8_Init 0 */

	/* USER CODE END UART8_Init 0 */

	/* USER CODE BEGIN UART8_Init 1 */

	/* USER CODE END UART8_Init 1 */
	huart8.Instance = UART8;
	huart8.Init.BaudRate = 115200;
	huart8.Init.WordLength = UART_WORDLENGTH_8B;
	huart8.Init.StopBits = UART_STOPBITS_1;
	huart8.Init.Parity = UART_PARITY_NONE;
	huart8.Init.Mode = UART_MODE_TX_RX;
	huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart8.Init.OverSampling = UART_OVERSAMPLING_16;
	huart8.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart8.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart8.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart8, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart8, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart8) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART8_Init 2 */

	/* USER CODE END UART8_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	LL_USART_InitTypeDef USART_InitStruct = {0};

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

	LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);
	/**USART3 GPIO Configuration
	   PD8   ------> USART3_TX
	   PD9   ------> USART3_RX
	 */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* USART3 DMA Init */

	/* USART3_TX Init */
	LL_DMA_SetPeriphRequest(DMA1, LL_DMA_STREAM_7, LL_DMAMUX1_REQ_USART3_TX);

	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_7, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_7, LL_DMA_PRIORITY_LOW);

	LL_DMA_SetMode(DMA1, LL_DMA_STREAM_7, LL_DMA_MODE_NORMAL);

	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_7, LL_DMA_PERIPH_NOINCREMENT);

	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_7, LL_DMA_MEMORY_INCREMENT);

	LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_7, LL_DMA_PDATAALIGN_BYTE);

	LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_7, LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_7);

	/* USART3 interrupt Init */
	NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
	NVIC_EnableIRQ(USART3_IRQn);

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART3, &USART_InitStruct);
	LL_USART_SetTXFIFOThreshold(USART3, LL_USART_FIFOTHRESHOLD_8_8);
	LL_USART_SetRXFIFOThreshold(USART3, LL_USART_FIFOTHRESHOLD_8_8);
	LL_USART_EnableFIFO(USART3);
	LL_USART_ConfigAsyncMode(USART3);

	/* USER CODE BEGIN WKUPType USART3 */

	/* USER CODE END WKUPType USART3 */

	LL_USART_Enable(USART3);

	/* Polling USART3 initialisation */
	while((!(LL_USART_IsActiveFlag_TEACK(USART3))) || (!(LL_USART_IsActiveFlag_REACK(USART3))))
	{
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(uartHandle->Instance==UART8)
	{
		/* USER CODE BEGIN UART8_MspInit 0 */

		/* USER CODE END UART8_MspInit 0 */
		/* UART8 clock enable */
		__HAL_RCC_UART8_CLK_ENABLE();

		__HAL_RCC_GPIOE_CLK_ENABLE();
		/**UART8 GPIO Configuration
		   PE0     ------> UART8_RX
		   PE1     ------> UART8_TX
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_0|LD2_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF8_UART8;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		/* UART8 DMA Init */
		/* UART8_TX Init */
		hdma_uart8_tx.Instance = DMA1_Stream6;
		hdma_uart8_tx.Init.Request = DMA_REQUEST_UART8_TX;
		hdma_uart8_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_uart8_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_uart8_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_uart8_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_uart8_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_uart8_tx.Init.Mode = DMA_NORMAL;
		hdma_uart8_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_uart8_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_uart8_tx) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(uartHandle,hdmatx,hdma_uart8_tx);

		/* USER CODE BEGIN UART8_MspInit 1 */

		/* USER CODE END UART8_MspInit 1 */
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

	if(uartHandle->Instance==UART8)
	{
		/* USER CODE BEGIN UART8_MspDeInit 0 */

		/* USER CODE END UART8_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_UART8_CLK_DISABLE();

		/**UART8 GPIO Configuration
		   PE0     ------> UART8_RX
		   PE1     ------> UART8_TX
		 */
		HAL_GPIO_DeInit(GPIOE, GPIO_PIN_0|LD2_Pin);

		/* UART8 DMA DeInit */
		HAL_DMA_DeInit(uartHandle->hdmatx);
		/* USER CODE BEGIN UART8_MspDeInit 1 */

		/* USER CODE END UART8_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
