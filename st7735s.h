/**
 *     st7735 display library
 *
 *     Copyright (c) 2020 Vitaliy Nimych (Cvetaev) @ cvetaevvitaliy@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _ST7735S_H
#define _ST7735S_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "fonts.h"
#include "picts.h"

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36 /* Memory Data Access Control */
#define ST7735_VSCSAD  0x37 /* Vertical Scroll Start Address of RAM */
#define ST7735_SCRLAR  0x33 /* Scroll Area Set */

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

//// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF
//#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

typedef uint8_t (*spi_write)(uint8_t *pData, uint16_t Size);
typedef void (*write_pin)(uint32_t port, uint32_t pin, uint8_t state);


typedef struct {
    uint32_t gpio_port;
    uint32_t gpio_pin;
} LCD_ST7735_GPIO_t;

typedef struct {
    void *handle;
    spi_write  spi_write_data;
    write_pin gpio_write_pin;
    LCD_ST7735_GPIO_t reset;
    LCD_ST7735_GPIO_t cs;
    LCD_ST7735_GPIO_t data;
    LCD_ST7735_GPIO_t backlight;
} LCD_ST7735_ctx_t;


typedef enum {
    LCD_R0,
    LCD_R90,
    LCD_R180,
    LCD_R270
} LCD_ST7735S_rotation_t;


void LCD_ST7735S_Init(LCD_ST7735_ctx_t *data);
void LCD_ST7735S_SetOrientation(LCD_ST7735S_rotation_t rotation);
void LCD_ST7735S_Scroll(uint8_t);
void LCD_ST7735S_ScrollArea(uint8_t x_start, uint8_t x_stop);
void LCD_ST7735S_Update(void);

void LCD_ST7735S_DrawPixel(int16_t x, int16_t y, uint16_t color);
void LCD_ST7735_FastDrawPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_ST7735_DrawString(const char *str, int x, int y, const tFont *font, uint32_t color);

void LCD_ST7735S_Backlight(bool enable);
void Draw_Bitmap_Mono(int x, int y, const tImage *image, uint16_t color565);
void LCD_ST7735S_Draw_RGB_Bitmap(int16_t x, int16_t y, const tImage_RGB *image);

void LCD_ST7735S_Clear(void);



#endif //_ST7735S_H
