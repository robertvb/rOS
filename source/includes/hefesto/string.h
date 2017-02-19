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

#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#define Binary			 	 2
#define DECIMAL				10
#define HEXADECIMAL			16


int8_t uintToString(uint32_t num, uint8_t base, uint8_t * str);

uint8_t * stringToUint(char * str, uint8_t base);

uint8_t intToString(int32_t num, uint8_t base, uint8_t * str);

int8_t * stringToInt(char * str, uint8_t base);

uint8_t stringLength(const uint8_t * str);

uint32_t searchStringPattern(uint8_t * pattern, const uint8_t * str);

uint8_t * strcpy(uint8_t * dest, const uint8_t *  src);

uint8_t * strncpy(uint8_t * dest, const uint8_t * src, const uint32_t numChars);

uint8_t *strncat(uint8_t *dest, const uint8_t *src, uint32_t n);

int32_t strncmp(const char *s1, const char *s2, uint32_t n);

#endif
