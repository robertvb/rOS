#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#define Binary			 	 2
#define Dec				10
#define Hexa				16


int8_t uintToString(uint32_t num, uint8_t base, uint8_t * str);

uint8_t * stringToUint(char * str, uint8_t base);

uint8_t intToString(int32_t num, uint8_t base, uint8_t * str);

int8_t * stringToInt(char * str, uint8_t base);

uint8_t stringLength(uint8_t * str);

uint32_t searchStringPattern(uint8_t * pattern, const uint8_t * str);

uint8_t * strcpy(uint8_t * dest, const uint8_t *  src);

uint8_t * strncpy(uint8_t * dest, const uint8_t * src, const uint32_t numChars);

#endif
