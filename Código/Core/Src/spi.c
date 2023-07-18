/**
  ******************************************************************************
  * File Name          : SPI.c
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */

uint8_t my_SPI1_RX_buf[my_RX_BUF_LEN];
uint8_t my_SPI1_TX_buf[my_TX_BUF_LEN];

my_SPI_handler_t my_spi1 = {my_SPI1_RX_buf, 0, 0,
														my_SPI1_TX_buf, 0, 0,
													 };

												
													 
													 
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    /* SPI1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)	
	{
		if(want_transmit)
		
	}
	
	
}


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)	
	{
		
		
	}
	
	
}


//---------------------------AQUI INICIA-SE A IMPLEMENTAÇAO DA GESTAO DOS BUFFERS--------------------------

//insert an element into Rx_Buffer
int spi_Rx_Buffer_push(my_SPI_handler_t* spi, char c)
{
	//is Rx_Buffer full?
	if((spi->Rx_Buf_write - spi->Rx_Buf_read)>= my_RX_BUF_LEN)
		return -ENOBUFS;
	
	spi->Rx_Buff_ptr[spi->Rx_Buf_write] = c;	//escreve no buffer e incremnta o index
	INC_BUF_IDX(spi->Rx_Buf_write, my_RX_BUF_LEN, 1);
	
	return EXIT_SUCESS;
}


//pop an element from Rx_Buffer
int spi_Rx_Buffer_pop(my_SPI_handler_t* spi)
{
	int c;
	
	//if Rx_Buffer empty?
	if(spi->Rx_Buf_write==spi->Rx_Buf_read)
		return -ENODATA;
	
	c = spi->Rx_Buff_ptr[spi->Rx_Buf_read];
	INC_BUF_IDX(spi->Rx_Buf_read, my_RX_BUF_LEN, 1);
	
	return c;
}


//insert an element into Tx_Buffer
int spi_Tx_Buffer_push(my_SPI_handler_t* spi, char c)
{
	//is Tx_Buffer full?
	if((spi->Tx_Buf_write - spi->Tx_Buf_read)>=my_TX_BUF_LEN)
		return -ENOBUFS;
	
	spi->Tx_Buff_ptr[spi->Tx_Buf_write & (my_TX_BUF_LEN)-1] = c;
	spi->Tx_Buf_write++;
	
	return EXIT_SUCESS;
}


//remove an element from Tx_Buffer
int spi_Tx_Buffer_pop(my_SPI_handler_t* spi)
{
	int c;
	
	//if Tx_Buffer empty?
	if(spi->Tx_Buf_write==spi->Tx_Buf_read)
		return -ENODATA;
	
	c = spi->Tx_Buff_ptr[spi->Tx_Buf_read & (my_TX_BUF_LEN)-1];
	spi->Tx_Buf_read++;
	
	return c;
}
//--------------------------------------------------------------------------------------

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
