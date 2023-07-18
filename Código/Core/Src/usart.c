/**
  ******************************************************************************
  * File Name          : USART.c
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "commands.h"



//	USART Rx
		volatile uint8_t	Rx_BufferM[BUF_LEN];
		volatile uint8_t  Rx_Bf_read=0;
		volatile uint8_t	Rx_Bf_write=0;
		
		uint8_t UART3_Rx_char=0;
		volatile uint8_t receive_flag=0;
		uint8_t Rx_ready_Process = 0;
		
//USART Tx
		volatile uint8_t	Tx_BufferM[BUF_LEN];
		volatile uint16_t  Tx_Bf_read=0;
		volatile uint16_t	Tx_Bf_write=0;
		uint8_t UART3_Tx_char = '\r';
		
		
//ESTRUTURA DE CONTROL CHAR
struct_control_char_t control_Array[]={
	{'?', &PrinfHelp},		//help
	{8, 	&exec_BCKSP},		//BCKSP
	{27, 	&exec_ESC},	//ESO
	{36, 	&exec_DOLLAR},	//$
};


/* USER CODE END 0 */

UART_HandleTypeDef huart3;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void init_UART3()
{
	HAL_UART_Receive_IT(&huart3, &UART3_Rx_char, 1);// set the interrupt for UART3 Rx
}


//implemantation of UART_RX ISR
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){
	if (huart->Instance == USART3){ //current UART?
		
		receive_flag = 1;
	
		// set the interrupt for UART3 Rx again
		HAL_UART_Receive_IT(&huart3, &UART3_Rx_char, 1);
	}
}


//implemantation of UART_TX ISR
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if (huart->Instance == USART3){ //current UART?
		
		int16_t c;
		
		c = Tx_Buffer_pop();
		
		// set the interrupt for UART3 Rx again
		if(c!=-ENODATA)
		{
			UART3_Tx_char = c;
			HAL_UART_Transmit_IT(&huart3, &UART3_Tx_char, 1);
		}
		else
		{
			UART3_Tx_char = '\0';
		}
	}
}

/*
*	Funçao que converte um numero em string
*/
int double_to_string(double num, uint8_t preci, char* str)
{
	uint8_t i = 0;
	uint16_t aux = 0; 
	uint16_t aux_exp = 1;
	uint16_t aux_aux_exp = 0;
	uint8_t signal_flag = 0;
	if(num < 0)
	{
		str[i] = '-';		//apresenta o sinal menos no inicio
		i++;
		num = (-num);
		signal_flag = 1;
	}
	aux = (uint16_t)num;;
		/**************************************************************

								  [		expoente		    ] [																					mantissa																								    ]
double register	x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x x
								^			
							siganl
		
		Se o valor do aux_exp for inicalmente 0, entao basta fazer uma oepraçao de OU com os 11 bits de expoente 
				nao funciona com floats ou double :((((((((
	**************************************************************/
	
	//+++++++++++++++++ descodificar a parte inteira +++++++++++++++++++++++
			while(aux > 10)
			{
				aux_aux_exp++;
				aux /= 10;
				aux_exp *= 10;
			}
			aux = (uint16_t)num;
			if(signal_flag)
				aux_aux_exp++;
			
			for(i = i; i < (aux_aux_exp+1); i++)
			{		
				str[i] = ((uint8_t)(aux / aux_exp)) + 0x30;
				aux = aux - (str[i] - 0x30)*aux_exp; 			//meu deus que trolhice
				aux_exp /= 10;
			}
	//+++++++++++++++++ fim descodificar a parte inteira +++++++++++++++++++++++
			
	//+++++++++++++++++ descodificar a parte decimal +++++++++++++++++++++++	
			str[i] = ',';	//ponto decimal
			i++;
			aux = (uint16_t)num;
			num = num - aux;
			for(aux_exp = 0; aux_exp < (preci - (aux_aux_exp+1)); aux_aux_exp++)
			{
				num = num * 10;
				str[i] = ((uint8_t)(num)) + 0x30;
				num = num - (str[i] - 0x30);
				i++;
			}
	//+++++++++++++++++ fim descodificar a parte decimal +++++++++++++++++++++++
		
		str[i] = '\r';
		str[i+1] = '\0';	
	return EXIT_SUCESS;
}



//---------------------------AQUI INICIA-SE A IMPLEMENTAÇAO DA GESTAO DOS BUFFERS--------------------------

//insert an element into Rx_Buffer
int Rx_Buffer_push(char c)
{
	//is Rx_Buffer full?
	if((Rx_Bf_write-Rx_Bf_read)>=BUF_LEN)
		return -ENOBUFS;
	
	Rx_BufferM[Rx_Bf_write] = c;	//escreve no buffer e incremnta o index
	INC_BUF_IDX(Rx_Bf_write, BUF_LEN, 1);
	
	return EXIT_SUCESS;
}


//pop an element from Rx_Buffer
int Rx_Buffer_pop(void)
{
	int c;
	
	//if Rx_Buffer empty?
	if(Rx_Bf_write==Rx_Bf_read)
		return -ENODATA;
	
	c=Rx_BufferM[Rx_Bf_read];
	INC_BUF_IDX(Rx_Bf_read, BUF_LEN, 1);
	
	return c;
}

//--------------------------------------------------------------------------------------
//insert an element into Tx_Buffer
int Tx_Buffer_push(char c)
{
	//is Tx_Buffer full?
	if((Tx_Bf_write-Tx_Bf_read)>=BUF_LEN)
		return -ENOBUFS;
	
	Tx_BufferM[Tx_Bf_write & (BUF_LEN)-1] = c;
	Tx_Bf_write++;
	
	return EXIT_SUCESS;
}


//remove an element from Tx_Buffer
int Tx_Buffer_pop(void)
{
	int c;
	
	//if Tx_Buffer empty?
	if(Tx_Bf_write==Tx_Bf_read)
		return -ENODATA;
	
	c = Tx_BufferM[Tx_Bf_read & (BUF_LEN)-1];
	Tx_Bf_read++;
	
	return c;
}

//number of elements in the Tx_Buffer
int Tx_Buffer_size(void)
{			
	return (Tx_Bf_write-Tx_Bf_read);
}

//---------------------------FIM DA IMPLEMENTAÇAO DA GESTAO DOS BUFFERS--------------------------




int Rx_char_parsing(void)	//funçao responsavel pelo parsing do carater recebido
{
	
	if(!receive_flag) 
		return -NRCHAR; //sem carater
	
	receive_flag=0;		//limpa a flag de receçao
	
	
	if(!process_control())	//se o carater for um control_char entao a funçao ja foi executada e so resta sair da funçao de parsing
	{
		return EXIT_SUCESS;
	}
	

	Rx_Buffer_push(UART3_Rx_char);//escreve no buffer e incremnta o index
	
	
	if(UART3_Rx_char == 13)	//caso seja um CR entao adiciona um '\0'	para delimir a ajudar no processamento
	{
		Rx_Buffer_push('\0');//escreve no buffer e incremnta o index
		Rx_ready_Process = 1;
	}		
	
	if(Rx_ready_Process)		//redundancia (neste caso e so estupido, ver isto mais tarde)
	{
		Rx_ready_Process=0;
		Validation_1_switch();
	}
	
	
	
	return EXIT_SUCESS;
}


//---------------------------AQUI INICIA-SE A IMPLEMENTAÇAO DA LISTA DE COMANDOS DE CONTROLO--------------------------

//?
void PrinfHelp(void)
{
	Tx_StrPush("MR \r");
	Tx_StrPush("MW \r");
	Tx_StrPush("MI \r");
	Tx_StrPush("MO \r");
	Tx_StrPush("RD \r");
	Tx_StrPush("WD \r");
	Tx_StrPush("VER \r");
	INIT_TRANSMITION;
}

//BCKSP
void exec_BCKSP(void)
{
	if(Rx_Bf_write>Rx_Bf_read)
	{
		DEC_BUF_IDX(Rx_Bf_write, BUF_LEN, 1);	//decrementa e mantem o buffer dentro dos limites
	}
}

//ESC
void exec_ESC(void)
{
	Rx_Bf_read=Rx_Bf_write;
}

//$
void exec_DOLLAR(void)
{
	
}

//funçao de parssing do control char
int process_control(void)
{
	uint8_t count=0;
	
	while(count<4)
	{
		if(UART3_Rx_char == control_Array[count].c)
		{
			control_Array[count].funcPtr();
			return EXIT_SUCESS;					//se existir um control_char entao pocessa e devolve sucesso
		}
		count++;
	}
	return -EPERM;	//caso nao for um control_char devolve operaçao nao permitida
}

//---------------------------FIM DA IMPLEMENTAÇAO DA LISTA DE COMANDOS DE CONTROLO--------------------------

//redifine the stdout
int fputc(int ch, FILE *f){
	HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 100);
	return ch;
}


/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
