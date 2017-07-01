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

#ifndef RPI_SCREEN_H
#define RPI_SCREEN_H

#include <stdint.h>

#define MAX_PWIDTH 				4096
#define MAX_PHEIGHT				4096
#define RPI_GPU_MAILBOX_BASE    0x2000B880
#define CHAR_WIDTH	  	        8
#define CHAR_HEIGHT 			16
#define MAX_VERTICAL_PIXELS		768
#define MAX_HORIZONTAL_PIXELS 	1024
//  ALGUNOS RGB HIGH-COLOR 16BIT COLORES                            
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255,   0 */
#define White           0xFFFF      /* 255, 255, 255 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */
#define Pink            0xF81F


typedef struct {			// DESCRIPTOR DEL BUFFER DE MARCO

    volatile uint32_t pWidth;	 	// anchura fisica pantalla
    volatile uint32_t pHeight;	 	// altura fisica pantalla
    volatile uint32_t vWidth;		// anchura virtual pantalla
    volatile uint32_t vHeight;		// altura virtual pantalla
    volatile uint32_t pitch;		// numero bytes en cada fila del buffer (usamos 2 × 1024 = 2048) *
    volatile uint32_t cMode;		// Colour mode (16 = high color).
    volatile uint32_t x;		// desplazamientos en X (esquina superior izquierda)
    volatile uint32_t y;		// desplazamientos en Y (esquina superior izquierda)
    volatile uint32_t pointer;		// puntero donde el DMA de la GPU va a buscar la información a mostrar *
    volatile uint32_t size;		// tamaño de la paleta *

					// (*) valores facilitados por la GPU
    } rpi_gpu_framebuffer_descriptor_t;


typedef struct {			//BUZÓN DE COMUNICACION CON LA GPU (*UNUSED, abordado en las rutinas de inicio del sistema - rpi-gpuMailbox.S - )

    volatile uint32_t read1;		//0x2000B880		**************
    volatile uint32_t read2;		//0x2000B884		* CAMPO READ *
    volatile uint32_t read3;		//0x2000B888		* TAM (128b) *
    volatile uint32_t read4;		//0x2000B88C		**************
    volatile uint32_t poll;		//0x2000B890
    volatile uint32_t sender;		//0x2000B894
    volatile uint32_t status;		//0x2000B898
    volatile uint32_t configuration;	//0x2000B89C
    volatile uint32_t write;		//0x2000B8A0
    } rpi_gpu_mailbox_t;


rpi_gpu_mailbox_t * RPI_GetGpuMailbox(void);
rpi_gpu_framebuffer_descriptor_t * RPI_GetFrameBufferDescrp(void);
void paintEntireScreen(uint16_t Colour16b);
void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t Colour16b);
void setPixelFB(unsigned int * fb, int32_t posX, int32_t posY, uint16_t Colour16b);
void setPixel(int32_t posX, int32_t posY, uint16_t Colour16b);
int8_t drawCharacterCL(uint16_t colour, uint8_t Char, uint32_t x, uint32_t y);
int8_t drawCharacter(uint8_t Char, uint32_t posX, uint32_t posY);
void eraseCharacterCL(uint16_t colour, uint32_t x, uint32_t y);
int8_t drawStringCL(uint16_t colour, uint8_t * string, uint32_t length, uint32_t x, uint32_t y);
int8_t drawString(uint8_t * string, uint32_t length, uint32_t x, uint32_t y);
void eraseString(uint16_t baseColour16b, uint32_t x, uint32_t y, uint32_t len);

#endif
