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

#ifndef SOURCE_APOLO_DEBUGTRACER_H_
#define SOURCE_APOLO_DEBUGTRACER_H_

#include "../hades/rpi-uart.h"

void init_debugTracer(void);
unsigned int getDebugTracerLever(void);
void setDebugTracerLever(unsigned int lvl);
void debugPrintStrV1(char * str);
void debugPrintStrV2(char * str);
void debugPrintStrV3(char * str);
void debugPrintValueV1(unsigned int value, char base);
void debugPrintValueV2(unsigned int value, char base);
void debugPrintValueV3(unsigned int value, char base);


#endif /* SOURCE_APOLO_DEBUGTRACER_H_ */
