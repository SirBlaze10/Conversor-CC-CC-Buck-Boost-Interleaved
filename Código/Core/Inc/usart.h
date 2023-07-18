/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

//my defines
#define BUF_LEN	256	//tamanho dos buffers de Rx e Tx
#define BUF_LEN_T	256
#define NRCHAR   		70	//no received char
#define	EXIT_SUCESS 0		//all is ok
#define	ENODATA			61	//no data available in BUFFER = 
#define	ENOBUFS			65	//no BUFFER space available
#define	EPERM				1		//operation nor permited



//macro like function	 
#define INC_BUF_IDX(index, buf_len, n)  	(index = ((buf_len-1) & (index+n))) 	//incrementa e mantem o buffer dentro dos limites
#define DEC_BUF_IDX(index, buf_len, n)		(index = ((buf_len-1) & (index-n)))		//decrementa e mantem o buffer dentro dos limites
#define INIT_TRANSMITION			(HAL_UART_Transmit_IT(&huart3, &UART3_Tx_char, 1)) //macro para começar a transmiçao
#define INIT_TRANSMITION_U_3		SET_BIT(huart3.Instance->CR1, USART_CR1_TXEIE)


/*	atalhos
Rx_BufferM[BUF_LEN];
Rx_Bf_read=0;
Rx_Bf_write=0;
*/


typedef struct struct_control_char
{
  char c;						//caracter espercial
	void (*funcPtr)(void);

}struct_control_char_t;


/* USER CODE END Private defines */

void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
//extern variables
	extern	volatile uint8_t	Rx_BufferM[BUF_LEN];
	extern	volatile uint8_t  Rx_Bf_read;


	extern uint8_t UART3_Tx_char;

void init_UART3(void);

//funçao responsavel pelo parsing do carater recebido
int Rx_char_parsing(void);

//funçao de parssing do control char
int process_control(void);


/*
*		FUNÇOES DE BUFFERS
*/
//------------------------------------RX--------------------------------
int Rx_Buffer_push(char c);
extern int Rx_Buffer_pop(void);
	
//-------------------TX----------
extern int Tx_Buffer_push(char c);
extern int Tx_Buffer_pop(void);
//number of elements in the Tx_Buffer
extern int Tx_Buffer_size(void);
	
/*
*FUNÇOES DE COMANDOS DE CONTROLO
*/
//?
void PrinfHelp(void);
//BCKSP
void exec_BCKSP(void);
//ESC
void exec_ESC(void);
//$
void exec_DOLLAR(void);

extern int double_to_string(double num, uint8_t preci, char* str);

/* USER CODE END Prototypes */

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
