/*
Copyright (c) 2015 Robert A. Vazeux Blanco

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
 */

#include "../includes/hefesto/string.h"


/* Parsea num a char *, guarda el resultado en str */
int8_t  uintToStringStr(uint32_t num, uint8_t base, uint8_t * str){

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

	if(base==HEXADECIMAL)
		while(num) {						// Se divide hasta que el ńumero sea 0

			reminder = num % base;

			if(reminder < 10)
				str[divCont] = '0' + reminder;
			else
				str[divCont] = 'a' + reminder - DECIMAL;	// Números y letras no son adyacentes en ASCII
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


char * uintToString(unsigned int num, char base){

	char divCont;
	char reminder;
	char i;
	static char buff [] = {"            \0"};

	if(base > 16) {							// Se comprueba la base
		buff[0] = '\0';
		return buff;
	}
	if(num==0) {
		buff[0]='0';
		buff[1]='\0';
		return buff;
	}

	divCont = 0;							// Llevamos la cuenta de la sucesivas divisiones

	if(base==HEXADECIMAL)
		while(num) {						// Se divide hasta que el ńumero sea 0

			reminder = num % base;

			if(reminder < 10)
				buff[divCont] = '0' + reminder;
			else
				buff[divCont] = 'a' + reminder - DECIMAL;	// Números y letras no son adyacentes en ASCII
									// 'a' representa el inicio de las minúsculas en la tabla ASCII (adyacentes)
			divCont++;
			num /= base;					// Le restamos el resto y loop.
		}

	else
		while(num) {						// Se divide hasta que el ńumero sea 0

		reminder = num % base;
		buff[divCont] = '0' + reminder;
		divCont++;
		num /= base;						// Le restamos el resto y loop.
	}

	buff[divCont] = '\0';
									// Siempre acaba en una división más de la cuenta (D = 0)	
									// En el caso hexadecimal no incluye el '0x'

	for(i = 0; i < --divCont; i++) {				// Reordenamos el array			
		
		reminder      = buff[i];					// Reutilizamos la variable, ahora como var aux para el intercambio.
		buff[i] 	  = buff[divCont];
		buff[divCont] = reminder;
	}

	return buff;

}

/* Cuenta el número de caracteres hasta llegar al '\0' */
uint8_t stringLength(const uint8_t * str) {

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

uint8_t *strncat(uint8_t *dest, const uint8_t *src, uint32_t n){
    char *ret = dest;
    while (*dest)
        dest++;
    while (n--)
        if (!(*dest++ = *src++))
            return ret;
    *dest = 0;
    return ret;
}

int32_t strncmp(const char *s1, const char *s2, uint32_t n) {
    while(n--)
        if(*s1++!=*s2++)
            return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
    return 0;
}

