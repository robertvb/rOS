#include "includes/string.h"


/* Parsea num a char *, guarda el resultado en str */
int8_t uintToString(uint32_t num, uint8_t base, uint8_t * str){

	uint8_t divCont;				
	uint8_t reminder;
	uint8_t i;						

	if(base > 16)							// Se comprueba la base
		return -1;

	if(stringLength(str) < 10)					// 11 caracteres, 10 para el numero y 1 para el '\0'
		return -2;

	if(num==0) {
		
		str[0]='0';
		str[1]='\0';
		return 1;
	}

	divCont = 0;							// Llevamos la cuenta de la sucesivas divisiones

	if(base==Hexa) 
		while(num) {						// Se divide hasta que el ńumero sea 0

			reminder = num % base;

			if(reminder < 10)
				str[divCont] = '0' + reminder;
			else
				str[divCont] = 'a' + reminder - Dec;	// Números y letras no son adyacentes en ASCII
									// 'a' representa el inicio de las minúsculas en la tabla ASCII (adyacentes)
			divCont++;
			num /= base;					// Le restamos el resto y loop.
		}

	else
		while(num) {						// Se divide hasta que el ńumero sea 0

		reminder = num % base;
		str[divCont] = '0' + reminder;
		divCont++;
		num /= base;						// Le restamos el resto y loop.
	}

	str[divCont] = '\0';
									// Siempre acaba en una división más de la cuenta (D = 0)	
									// En el caso hexadecimal no incluye el '0x'

	for(i = 0; i < --divCont; i++) {				// Reordenamos el array			
		
		reminder     = str[i];					// Reutilizamos la variable, ahora como var aux para el intercambio.
		str[i] 	     = str[divCont];
		str[divCont] = reminder;
	}

	return 0;

}

/* Cuenta el número de caracteres hasta llegar al '\0' */
uint8_t stringLength(uint8_t * str) {

	uint8_t cont;
	for(cont=0; str[cont] != '\0'; cont++);

	return cont;

}

/* Busca un patron en un string, si existe, devuelve la posicion del string donde acaba el patrón, sino un 0 */
uint32_t searchStringPattern(uint8_t * pattern, const uint8_t * str){


	uint32_t i;
	uint32_t equals;

	equals = 0;
	for(i = 0; str[i] != '\0' && pattern[equals] != '\0'; i++) {
		
		if(str[i] == pattern[equals])
			equals++;
		else
			equals = 0;
	}

	return pattern[equals] == '\0' ? i : 0;

}

uint8_t * strcpy(uint8_t *dest, const uint8_t *src) {

	char *save = dest;
	while(*dest++ = *src++);
	return save;
}

uint8_t * strncpy(uint8_t *dest, const uint8_t *src, const uint32_t numChars) {

	uint8_t *save = dest;
	uint32_t i;
	for(i = 1; i < numChars && (*dest++ = *src++); i++);
	return save;
}

