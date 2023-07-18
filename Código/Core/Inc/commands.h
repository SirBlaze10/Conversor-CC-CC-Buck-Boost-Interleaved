#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <stdint.h>
//my defines
#define CMD_MAX_LEN 7
//#define N_CMD 8
#define NCMDAVL	66

//variables
extern int last_char_valid1_string;

//my data types
typedef struct struct_func
{
  char nome[CMD_MAX_LEN];
	int (*funcPtr)(void);

}struct_func_t;



//prototipos das funçoes de comando de interface
int size_array(void);
extern int Validation_1_switch(void);

/*
*MANIPULAÇAO DE STRINGS
*/
extern void Rx_StrPop(uint8_t *str, char delim);
extern void Tx_StrPush(char *str);
extern int HexString_to_Int(uint8_t *str);
extern int DecString_to_Int(uint8_t *str);
extern float FloatString_to_Int(uint8_t *str);
	
/*
*COMANDOS
*/
int MemoryRead(void);
int MemoryWrite(void);
int MakePinInput(void);
int MakePinOutput(void);
int ReadDigitalInput(void);
int WriteDigitalOutput(void);
int AnalogRead(void);
int PrintVersion(void);
int PrintADC(void);



#endif // _COMMANDS_H
