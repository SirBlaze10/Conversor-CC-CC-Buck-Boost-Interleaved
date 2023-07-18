#include "commands.h"
#include "main.h"
#include "usart.h"
#include "string.h"
#include "math.h"
#include "stdio.h"
#include "adc.h"





//variales
int last_char_valid1_string = 0;

struct_func_t funcArray[]={
				{"MR",	&MemoryRead},
				{"MW",	&MemoryWrite},
				{"MI",	&MakePinInput},
				{"MO",	&MakePinOutput},
				{"RD",	&ReadDigitalInput},
				{"WD",	&WriteDigitalOutput},
			//	{"RA",	&AnalogRead},
				{"VER", &PrintVersion},
				// {"S_FILT", &SeriesFilterParser}
				{"ADC",	&PrintADC},
				
};

//ma man
int size_array(void)
{
    return ((sizeof(funcArray))/(sizeof(funcArray[0])));
}


//
// validaçao 1
//
int Validation_1_switch(void)
{
	uint8_t arr_size = size_array();			//
	uint8_t valid1_string[CMD_MAX_LEN];
	int c, func_return;
	uint8_t i=0;

	
	while(c!= 32 && c!= 13 && i < CMD_MAX_LEN)		//copia a string ate ao carater " " OU "CR" (o i nao pode ultrapassar o limite do buffer)
	{
		c = Rx_Buffer_pop();
		if(c != -ENODATA)
			valid1_string[i] = c;
		i++;
	}
	last_char_valid1_string = c;//isto so e necessario para o Start Sample =(
	
	valid1_string[i-1]='\0';	//adiciona o carater delimitador para se poder usar o strcmp
	
	for(i=0; i<arr_size;i++)
	{
		if(!strcmp(funcArray[i].nome, (char*)valid1_string))
		{
			func_return = funcArray[i].funcPtr();						//a funçao correspondente ao comando é chamada aqui
			break;
		}
	}
	
	if(i==arr_size)
	{
		func_return = -NCMDAVL;
		Tx_StrPush("COMANDO NAO ENCONTRADO ");
	}
	
	if(!func_return)//(EXIT_SUCESS==0) //verifica se o o ultimo comando foi completamente valido
	{
		//codificar aqui a cena do ultimo comando valido
		
	}
		
	do		//isto serve para por o read a apontar para o inicio da proxima string
	{
		func_return = Rx_Buffer_pop();
	}while(func_return!='\0' && func_return!=-ENODATA);
		
	Tx_StrPush("\r > ");
	INIT_TRANSMITION;
	
	return EXIT_SUCESS;
}


//
//-------------------------------SERIE DE FUNÇOES DE MANIPULAÇAO DE STRINGS--------------------------------
//
void Rx_StrPop(uint8_t *str, char delim)
{
	uint8_t i=0;
	int c = 1;
	
	while((c!=delim && c!= '\r') && c!=-ENODATA)
	{
		c = Rx_Buffer_pop();
		str[i]=c;
		i++;
	}
	str[i-1]='\0'; //escrever por ultimo o CR
}

void Tx_StrPush(char *str)
{
	int push_ret = 0;
	int i=0;
	
	do
	{
		push_ret = Tx_Buffer_push(str[i]);
		i++;
	}while(push_ret != -ENOBUFS && str[i]!=0);
}	


int HexString_to_Int(uint8_t *str)
{
	int return_value = 0;
	int num_pow = 0;
	int i=0;
	
	while (str[i] != '\0') {	//procura pelo fim da string
		i++;
	}
	
	i--;

	while (i>-1)
	{//
		if (str[i] >= 48 && str[i] <= 57)
		{
			return_value += (str[i] - 48)*pow(16, num_pow);
		}
		else if (str[i] >= 65 && str[i] <= 70)
		{
			return_value += (str[i] - 55)*pow(16, num_pow);
		}
		else { return -EPERM; } //se a string tiver carates impossiveis retorna -1; 
		
		num_pow++;
		i--;
	}
	return return_value;
}

int DecString_to_Int(uint8_t *str)
{
	int return_value = 0;
	int num_pow = 0;
	int i=0;
	
	while (str[i] != '\0') {	//procura pelo fim da string
		i++;
	}
	
	i--;

	while (i>-1)
	{//
		if (str[i] >= 48 && str[i] <= 57)
		{
			return_value += (str[i] - 48)*pow(10, num_pow);
		}
		else { return -EPERM; } //se a string tiver carates impossiveis retorna -1; 
		
		num_pow++;
		i--;
	}
	return return_value;
}


float FloatString_to_Int(uint8_t *str)
{
	uint8_t point_flag = 0;
	int8_t sigan_flag = 1;
	float return_value = 0;
	int num_pow = 0;
	uint16_t div_factor = 1;
	int i=0;
	
	if(str[0] == '-')	//sinal menos
	{
		str = str + sizeof(uint8_t);
		sigan_flag = -1;
	}
	while (str[i] != '\0') {	//procura pelo fim da string
		if(point_flag)
			div_factor *= 10;
		if(str[i] == '.')
			point_flag = 1;
		
		i++;
	}
	
	i--;

	while (i>-1)
	{//
		if (str[i] >= 48 && str[i] <= 57)
		{
			return_value += (str[i] - 48)*pow(10, num_pow);
		}
		else if(str[i] == '.')num_pow--;			//so para contrariar o num_pow++;
			else { return -EPERM; } //se a string tiver carates impossiveis retorna -1; 
		
		num_pow++;
		i--;
	}
	return_value = return_value / div_factor;
	return_value *= sigan_flag;
	return return_value;
}

//-------------------------------FIM SERIE DE FUNÇOES DE MANIPULAÇAO DE STRINGS--------------------------------



//
//---------------------------AQUI INICIA-SE A IMPLEMENTAÇAO DA LISTA DE COMANDOS DE INTERFACE--------------------------
//

//devolve o endereço do porto que queremos manipular
uint32_t AddrGpioPort(uint8_t gpio_N){
	uint32_t gpioAddr;
	
	gpioAddr = gpio_N * 0x400;
	gpioAddr += 0x40020000;
	
	return gpioAddr;
}

/*
*MEMORY READ
*/
int MemoryRead(void)
{
	//variaveis
	uint8_t str[8];
	uint8_t* addr;
	int addr_off;
	short int length;
	
	//descodificaçao do address
	Rx_StrPop(str, ' ');
	addr_off = HexString_to_Int(str);
	if(addr_off!=-EPERM && addr_off<0xFFFF)
	{
		addr = (uint8_t*) (addr_off + 0x20020000);	//(inicio de SRAM1 + addr(offset))
	}else
		{
			Tx_StrPush("ADDRESS ERROR\r");
			INIT_TRANSMITION;
			return -EPERM;
		}
	
	//descodificaçao do length
	Rx_StrPop(str, 13);	//CR pois e o fim da string
	length = HexString_to_Int(str);
	if(length!=-1 && length < 256) //verifica se addr e invalido e se esta dentro de 8 bit
	{
		//codigo que realmente faz o MR
		for(int i=0 ; i<length ; i++)
			{
				str[i]=*addr;
				addr++;
			}
	}else
		{
			//inserir mensagem de erro
			return -EPERM;
		}
	
		printf("%s", str);
	return EXIT_SUCESS;
}

/*
*MEMORY WRITE
*/
int MemoryWrite(void)
{
		//variaveis
	uint8_t str[8];
	uint8_t* addr;
	int addr_off;
	short int length;
	short int data;
	
	//descodificaçao do address
	Rx_StrPop(str, ' ');
	addr_off = HexString_to_Int(str);
	if(addr_off!=-EPERM && addr_off<0xFFFF)
	{
		addr = (uint8_t*) (addr_off + 0x20020000);	//(inicio de SRAM1 + addr(offset))
	}else
		{
			//inserir mensagem de erro
			return -EPERM;
		}
	
	//descodificaçao do length
	Rx_StrPop(str, '\0');	
	length = HexString_to_Int(str);
	if(!(length!=-1 && length < 256))	//caso seja invalido
	{
		//insrir mensagem de erro
		return -EPERM;
	}
	
	//descodificaçao do data
	Rx_StrPop(str, 13);	//CR pois e o fim da string
	data = HexString_to_Int(str);
	if(data!=-1 && data < 256)
	{
		for(int i=0 ; i<length ; i++)
		{
			*addr=data;
			addr++;
		}
	}else
		{
			//insrir mensagem de erro
			return -EPERM;
		}

	return EXIT_SUCESS;
}


/*
*MAKE PIN INPUT
*/
int MakePinInput(void)
{
	uint8_t str[8];
	int port_addr;
	uint8_t pin_setting;
	uint32_t gpio_ender;
	GPIO_InitTypeDef my_Gpio_init;

	
	//descodificaçao do port address
	Rx_StrPop(str, ' ');
	port_addr = HexString_to_Int(str);
	
	if(port_addr!=-1 && port_addr < 11) //verifica se addr e invalido e se esta dentro de 11 portos
		{
			gpio_ender = AddrGpioPort(port_addr);
		} else 
			{	
				Tx_StrPush("erro port_addr!");
				INIT_TRANSMITION;
				return -EPERM; //invalido , sai da funçao e devolde erro
			}
		
	Rx_StrPop(str, 13);
	pin_setting = HexString_to_Int(str);
	
	if(pin_setting < 256)
	{
		my_Gpio_init.Pin = pin_setting;
		my_Gpio_init.Mode = GPIO_MODE_INPUT;
		my_Gpio_init.Pull = GPIO_NOPULL;
	}else 
		{
			Tx_StrPush(" erro pin_setting ");
				INIT_TRANSMITION;
				
				return -EPERM; //invalido , sai da funçao e devolde erro
		}
	
	HAL_GPIO_Init((GPIO_TypeDef*)gpio_ender, &my_Gpio_init);
	Tx_StrPush(">MAKE PIN INPUT \r\n");
	INIT_TRANSMITION;
	
	return EXIT_SUCESS;
}


/*
*MAKE PIN OUTPUT
*/
int MakePinOutput(void)
{	
	uint8_t str[8];
	int port_addr;
	uint8_t pin_setting;
	uint32_t gpio_ender;
	GPIO_InitTypeDef my_Gpio_init;

	
	//descodificaçao do port address
	Rx_StrPop(str, ' ');
	port_addr = HexString_to_Int(str);
	
	if(port_addr!=-1 && port_addr < 11) //verifica se addr e invalido e se esta dentro de 11 portos
		{
			gpio_ender = AddrGpioPort(port_addr);
		} else 
			{	
				Tx_StrPush("erro port_addr!");
				INIT_TRANSMITION;
				return -EPERM; //invalido , sai da funçao e devolde erro
			}
		
	Rx_StrPop(str, 13);
	pin_setting = HexString_to_Int(str);
	
	if(pin_setting < 256)
	{
		my_Gpio_init.Pin = pin_setting;
		my_Gpio_init.Mode = GPIO_MODE_OUTPUT_PP;;
		my_Gpio_init.Pull = GPIO_NOPULL;
	}else 
		{
			Tx_StrPush(" erro pin_setting ");
				INIT_TRANSMITION;
				
				return -EPERM; //invalido , sai da funçao e devolde erro
		}
	
	HAL_GPIO_Init((GPIO_TypeDef*)gpio_ender, &my_Gpio_init);
	Tx_StrPush(">MAKE PIN OUTPUT \r\n");
	INIT_TRANSMITION;
	
	return EXIT_SUCESS;
}


/*
*READ DIGITAL INPUT
*/
int ReadDigitalInput(void)
{
	uint8_t str[8];
//	char str2[4];
	int port_addr;
	uint8_t pin_setting;
	uint8_t pin_state;
	uint32_t gpio_ender;

	//descodificaçao do port address
	Rx_StrPop(str, ' ');
	port_addr = HexString_to_Int(str);
	
	if(port_addr!=-1 && port_addr < 11) //verifica se addr e invalido e se esta dentro de 11 portos
		{
			gpio_ender = AddrGpioPort(port_addr);
		} else 
			{	
				Tx_StrPush("erro port_addr!");
				INIT_TRANSMITION;
				return -EPERM; //invalido , sai da funçao e devolde erro
			}
	
		
	Rx_StrPop(str, 13);
	pin_setting = HexString_to_Int(str);
	
	if(pin_setting < 256)
	{
		pin_state = ((GPIO_TypeDef*)gpio_ender)->IDR & pin_setting;
	}else 
		{
			Tx_StrPush("erro pin_setting!");
			INIT_TRANSMITION;
			return -EPERM; //invalido , sai da funçao e devolde erro
		}
		
		//sprintf(str2, "%x", pin_state);	//string to num
		
		Tx_StrPush(" READ DIGITAL INPUT - ");
		//Tx_Buffer_push(pin_state);
		INIT_TRANSMITION;
		HAL_Delay(5);
		printf("\r%x", pin_state);
	return EXIT_SUCESS;
}


/*
*WRITE DIGITAL OUTPUT
*/
int WriteDigitalOutput(void)
{
	uint8_t str[8];
	int port_addr;
	uint8_t pin_setting;
	uint32_t gpio_ender;
	int byte=0;
	
	//descodificaçao do port address
	Rx_StrPop(str, ' ');
	port_addr = HexString_to_Int(str);
	
	if(port_addr!=-1 && port_addr < 11) //verifica se addr e invalido e se esta dentro de 11 portos
		{
			gpio_ender = AddrGpioPort(port_addr);
		} else 
			{	
				Tx_StrPush("erro port_addr!");
				INIT_TRANSMITION;
				return -EPERM; //invalido , sai da funçao e devolde erro
			}
	
	//descodificaçao do pim_setting
	Rx_StrPop(str, ' ');
	pin_setting = HexString_to_Int(str);
	
	if(pin_setting > 255)
	{
		Tx_StrPush("erro pin_setting!");
		INIT_TRANSMITION;
		return -EPERM; //invalido , sai da funçao e devolde erro
	}
		
	//descodificaçao do pin values
	Rx_StrPop(str, 13);
	byte = HexString_to_Int(str);
	
	if(byte!=-1 && byte < 256) //verifica se byte e invalido e se esta dentro de 8 bit
	{
		HAL_GPIO_WritePin((GPIO_TypeDef*)gpio_ender, pin_setting, (GPIO_PinState)byte);
	} 
		
		Tx_StrPush("WRITE DIGITAL OUTPUT");
		INIT_TRANSMITION;
	
	return EXIT_SUCESS;
}

#if 0		//nao e necessario
/*
*ANALOG READ
*/
int AnalogRead(void)
{
	uint16_t adcValue_read;
	float adcValue_read_f;
	char str[8];
	
	HAL_ADC_Start_IT(&hadc1);
	
	while(!adc1Flag); //fica aqui preso
	adc1Flag=0;
	
	HAL_ADC_Stop_IT(&hadc1);
	
	adcValue_read = adcValue;
	
	adcValue_read = adcValue_read & 0xFFF;
	adcValue_read_f = (float)adcValue_read/(float)1241;
	
	
	sprintf(str, "%f", adcValue_read_f);	//string to num
	
	Tx_StrPush("ANALOG READ - ");
	Tx_StrPush(str);
	INIT_TRANSMITION;
	
	return EXIT_SUCESS;
}
#endif

/*
*PRINT VERSION
*/
int PrintVersion(void)
{
	Tx_StrPush("\r > V 2.9 grupo 7 LPI-II DEIC UM2020 \r");
	INIT_TRANSMITION;
	
	return EXIT_SUCESS;
}


int PrintADC(void)
{
	printf("ADC1 = %d\r\n", ADC1_chn4_value);
}
//---------------------------FIM DA IMPLEMENTAÇAO DA LISTA DE COMANDOS DE INTERFACE--------------------------


