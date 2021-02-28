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
#include "st7735s.h"
#include "st7735s_settings.h"
#include "delay.h"

#define DELAY 0x80

static uint16_t ScreenBuff[ST7735_WIDTH * ST7735_HEIGHT] = {0};

static LCD_ST7735_ctx_t LCD_ST7735_ctx = {0};

typedef struct ST7735s{
    uint8_t width;
    uint8_t height;
    uint8_t xstart;
    uint8_t ystart;
} LCD_ST7735_t;

static LCD_ST7735_t LCD_ST7735 = {
        .width = ST7735_WIDTH,
        .height = ST7735_HEIGHT,
        .xstart = ST7735_XSTART,
        .ystart = ST7735_YSTART
};

static void SwapBytes(uint16_t *color);

static const uint8_t init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
        15,                       // 15 commands in list:
        ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
        150,                    //     150 ms delay
        ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
        255,                    //     500 ms delay
        ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
        0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
        0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
        0x01, 0x2C, 0x2D,       //     Dot inversion mode
        0x01, 0x2C, 0x2D,       //     Line inversion mode
        ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
        0x07,                   //     No inversion
        ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
        0xA2,
        0x02,                   //     -4.6V
        0x84,                   //     AUTO mode
        ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
        0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
        ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
        0x0A,                   //     Opamp current small
        0x00,                   //     Boost frequency
        ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
        0x8A,                   //     BCLK/2, Opamp current small & Medium low
        0x2A,
        ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
        0x8A, 0xEE,
        ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
        0x0E,
        ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
        ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
        ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
        ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
        0x05
},                 //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
init_cmds2[] = {            // Init for 7735S, part 2 (160x80 display)
    3,                        //  3 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x4F,             //     XEND = 79
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F ,            //     XEND = 159
    ST7735_INVON, 0

},        //  3: Invert colors
#endif

init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
        4,                        //  4 commands in list:
        ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
        0x02, 0x1c, 0x07, 0x12,
        0x37, 0x32, 0x29, 0x2d,
        0x29, 0x25, 0x2B, 0x39,
        0x00, 0x01, 0x03, 0x10,
        ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
        0x03, 0x1d, 0x07, 0x06,
        0x2E, 0x2C, 0x29, 0x2D,
        0x2E, 0x2E, 0x37, 0x3F,
        0x00, 0x00, 0x02, 0x10,
        ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
        10,                     //     10 ms delay
        ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
        100
};                  //     100 ms delay

static void ST7735_ExecuteCommandList(const uint8_t *addr);


uint8_t ST7735S_SPI_Transmit(uint8_t *pData, uint16_t Size)
{
    int16_t ret;
    ret = LCD_ST7735_ctx.spi_write_data(pData, Size);
    return ret;
}


static void LCD_ST7735S_Reset(void)
{
    LCD_ST7735_ctx.gpio_write_pin(LCD_ST7735_ctx.reset.gpio_port, LCD_ST7735_ctx.reset.gpio_pin, 0);
    delay_ms(10);
    LCD_ST7735_ctx.gpio_write_pin(LCD_ST7735_ctx.reset.gpio_port, LCD_ST7735_ctx.reset.gpio_pin, 1);
}


static void LCD_ST7735S_Select(void)
{
    LCD_ST7735_ctx.gpio_write_pin(LCD_ST7735_ctx.cs.gpio_port, LCD_ST7735_ctx.cs.gpio_pin, 0);
}


static void LCD_ST7735S_Unselect(void)
{
    LCD_ST7735_ctx.gpio_write_pin(LCD_ST7735_ctx.cs.gpio_port, LCD_ST7735_ctx.cs.gpio_pin, 1);
}


static void LCD_ST7735S_DC_Select(void)
{
    LCD_ST7735_ctx.gpio_write_pin(LCD_ST7735_ctx.data.gpio_port, LCD_ST7735_ctx.data.gpio_pin, 1);
}


static void LCD_ST7735S_DC_Unselect(void)
{
    LCD_ST7735_ctx.gpio_write_pin(LCD_ST7735_ctx.data.gpio_port, LCD_ST7735_ctx.data.gpio_pin, 0);
}


void LCD_ST7735S_Backlight(bool enable)
{
    if (enable)
    {
        LCD_ST7735_ctx.gpio_write_pin(LCD_ST7735_ctx.backlight.gpio_port, LCD_ST7735_ctx.backlight.gpio_pin, 1);
    }
    else
    {
        LCD_ST7735_ctx.gpio_write_pin(LCD_ST7735_ctx.backlight.gpio_port, LCD_ST7735_ctx.backlight.gpio_pin, 0);
    }
}


void ST7735_WriteCommand(uint8_t cmd)
{
    LCD_ST7735S_DC_Unselect();
    ST7735S_SPI_Transmit(&cmd, sizeof(cmd));
}


static void ST7735_WriteData(uint8_t* buff, size_t buff_size)
{
    LCD_ST7735S_DC_Select();
    ST7735S_SPI_Transmit(buff, buff_size);

}


void LCD_ST7735S_Init(LCD_ST7735_ctx_t *data)
{
    if (data == NULL)
        return;

    memcpy(&LCD_ST7735_ctx, data, sizeof(LCD_ST7735_ctx));

    LCD_ST7735S_Select();
    LCD_ST7735S_Reset();
    ST7735_ExecuteCommandList(init_cmds1);
    ST7735_ExecuteCommandList(init_cmds2);
    ST7735_ExecuteCommandList(init_cmds3);
    LCD_ST7735S_Unselect();
    LCD_ST7735S_Backlight(true);
}


static void ST7735_ExecuteCommandList(const uint8_t *addr)
{
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--)
    {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(cmd);

        numArgs = *addr++;

        /** If high bit set, delay follows args */
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs)
        {
            ST7735_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms)
        {
            ms = *addr++;
            if(ms == 255) ms = 500;
            delay_ms(ms);
        }
    }
}


static void SwapBytes(uint16_t *color)
{
    uint8_t temp = *color >> 8;
    *color = (*color << 8) | temp;
}


void LCD_ST7735S_DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= LCD_ST7735.width) || (y < 0) || (y >= LCD_ST7735.height))
        return;

    SwapBytes(&color);

    ScreenBuff[y * LCD_ST7735.width + x] = color;
}


static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // column address set
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + LCD_ST7735.xstart, 0x00, x1 + LCD_ST7735.xstart };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_RASET);
    data[1] = y0 + LCD_ST7735.ystart;
    data[3] = y1 + LCD_ST7735.ystart;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_RAMWR);
}


void LCD_ST7735_FastDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if((x >= LCD_ST7735.width) || (y >= LCD_ST7735.height))
        return;

    LCD_ST7735S_Select();

    ST7735_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    LCD_ST7735S_Unselect();
}


void LCD_ST7735S_Update(void)
{
    LCD_ST7735S_Select();
    ST7735_SetAddressWindow(0, 0, LCD_ST7735.width-1, LCD_ST7735.height - 1);

    ST7735_WriteData((uint8_t*)ScreenBuff, sizeof(ScreenBuff));

    LCD_ST7735S_Unselect();
}


void Draw_Bitmap_Mono(int x, int y, const tImage *image, uint16_t color565)
{
    uint8_t value = 0;
    int16_t x0, y0;
    int16_t counter = 0;
    const uint8_t *pdata = (const uint8_t *) image->data;
    // rows
    for (y0 = 0; y0 < image->height; y0++) {
        // columns
        for (x0 = 0; x0 < image->width; x0++) {
            // load new data
            if (counter == 0) {
                value = *pdata++;
                counter = image->dataSize;
            }
            counter--;

            // set pixel
            if ((value & 0x80) != 0) {
                LCD_ST7735S_DrawPixel(x + x0, y + y0, color565);
            } else
                LCD_ST7735S_DrawPixel(x + x0, y + y0, 0x0000);

            value = value << 1;
        }
    }
}


const tChar *find_char_by_code(int code, const tFont *font)
{
    int16_t count = font->length;
    int16_t first = 0;
    int16_t last = count - 1;
    int16_t mid = 0;

    if (count > 0) {
        if ((code >= font->chars[0].code) && (code <= font->chars[count - 1].code)) {
            while (last >= first) {
                mid = first + ((last - first) / 2);

                if (font->chars[mid].code < code)
                    first = mid + 1;
                else if (font->chars[mid].code > code)
                    last = mid - 1;
                else
                    break;
            }

            if (font->chars[mid].code == code)
                return (&font->chars[mid]);
        }
    }

    return (0);
}


int utf8_next_char(const char *str, int16_t start, int16_t *resultCode, int16_t *nextIndex)
{
    uint16_t len = 0;
    uint16_t index = 0;
    *resultCode = 0;

    while (*(str + index) != 0) {
        len++;
        index++;
    }

    uint8_t c;
    uint8_t code = 0;
    int8_t result = 0;
    uint8_t skip = 0;

    *resultCode = 0;
    *nextIndex = -1;

    if (start >= 0 && start < len) {
        index = start;

        while (index < len) {
            c = *(str + index);
            index++;

            // msb
            if (skip == 0) {
                // if range 0x00010000-0x001fffff
                if ((c & 0xf8) == 0xf0) {
                    skip = 3;
                    code = c;
                }
                    // if range 0x00000800-0x0000ffff
                else if ((c & 0xf0) == 0xe0) {
                    skip = 2;
                    code = c;
                }
                    // if range 0x00000080-0x000007ff
                else if ((c & 0xe0) == 0xc0) {
                    skip = 1;
                    code = c;
                }
                    // if range 0x00-0x7f
                else {//if ((c & 0x80) == 0x00)
                    skip = 0;
                    code = c;
                }
            } else { // not msb
                code = code << 8;
                code |= c;
                skip--;
            }
            if (skip == 0) {
                // completed
                *resultCode = code;
                *nextIndex = index;
                result = 1;
                break;
            }
        }
    }
    return (result);
}


void LCD_ST7735_DrawString(const char *str, int x, int y, const tFont *font, uint32_t color)
{
    int16_t len = strlen(str);
    int16_t index = 0;
    int16_t code = 0;
    int16_t x1 = x;
    int16_t nextIndex;

    while (index < len) {
        if (utf8_next_char(str, index, &code, &nextIndex) != 0) {
            const tChar *ch = find_char_by_code(code, font);
            if (ch != 0) {
                Draw_Bitmap_Mono(x1, y, ch->image, color);
                x1 += ch->image->width;
            }
        }
        index = nextIndex;
        if (nextIndex < 0)
            break;
    }
}


void LCD_ST7735S_SetOrientation(LCD_ST7735S_rotation_t rotation)
{
    uint8_t madctl;

    switch ((uint8_t)rotation) {
        case   LCD_R0: { madctl = 0b01100000;
            LCD_ST7735.width = ST7735_WIDTH;
            LCD_ST7735.height = ST7735_HEIGHT;
            LCD_ST7735.xstart = ST7735_XSTART;
            LCD_ST7735.ystart = ST7735_YSTART;
            break;
        }
        case  LCD_R90: { madctl = 0b11000000;
            LCD_ST7735.width = ST7735_HEIGHT;
            LCD_ST7735.height = ST7735_WIDTH;
            LCD_ST7735.xstart = ST7735_YSTART;
            LCD_ST7735.ystart = ST7735_XSTART;
            break;
        }
        case LCD_R180: { madctl = 0b10100000;
            LCD_ST7735.width = ST7735_WIDTH;
            LCD_ST7735.height = ST7735_HEIGHT;
            LCD_ST7735.xstart = ST7735_XSTART;
            LCD_ST7735.ystart = ST7735_YSTART;

            break;
        }
        case LCD_R270: { madctl = 0b01000000;
            LCD_ST7735.width = ST7735_HEIGHT;
            LCD_ST7735.height = ST7735_WIDTH;
            LCD_ST7735.xstart = ST7735_YSTART;
            LCD_ST7735.ystart = ST7735_XSTART;
            break;
        }
    }
    LCD_ST7735S_Select();
    ST7735_WriteCommand(ST7735_MADCTL);
    ST7735_WriteData(&madctl, sizeof(madctl));
    LCD_ST7735S_Unselect();
}



void LCD_ST7735S_Scroll(uint8_t line) {

    if (line < 160) {
        LCD_ST7735S_Select();
        ST7735_WriteCommand(ST7735_VSCSAD);
        uint8_t data[] = {line >> 8, line & 0xFF};
        ST7735_WriteData(data, 2);
        LCD_ST7735S_Unselect();
    }

}


void LCD_ST7735S_ScrollArea(uint8_t x_start, uint8_t x_stop)
{
    /** tfa: top fixed area: nr of line from top of the frame mem and display) */
    uint16_t tfa = LCD_ST7735.width - x_stop + LCD_ST7735.xstart;
    /** vsa: height of the vertical scrolling area in nr of line of the frame mem
       (not the display) from the vertical scrolling address. the first line appears
       immediately after the bottom most line of the top fixed area. */
    uint16_t vsa = x_stop - x_start + LCD_ST7735.xstart;
    /** bfa: bottom fixed are in nr of lines from bottom of the frame memory and display */
    uint16_t bfa = x_start + LCD_ST7735.xstart;

    if (tfa+vsa+bfa < 160)
        return;

    uint8_t CMD[] = { tfa >> 8, tfa & 0xFF,
                      vsa >> 8, vsa & 0xFF,
                      bfa >> 8, bfa & 0xFF };

    LCD_ST7735S_Select();
    ST7735_WriteCommand(ST7735_SCRLAR);
    ST7735_WriteData(CMD, sizeof(CMD));
    LCD_ST7735S_Unselect();

}

#define pgm_read_word(addr) (*(const unsigned short *)(addr))
/**************************************************************************/
/*!
   @brief   Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) at the specified
   (x,y) position. For 16-bit display devices; no color reduction performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with 16-bit color bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void LCD_ST7735S_Draw_RGB_Bitmap(int16_t x, int16_t y, const tImage_RGB *image)
{
    uint16_t w = image->width;
    uint16_t h = image->height;
    for (int16_t j = 0; j < h; j++, y++) {
        for (int16_t i = 0; i < w; i++) {
            //LCD_ST7735S_DrawPixel(x + i, y, pgm_read_word(&image[j * w + i]));
            LCD_ST7735S_DrawPixel(x + i, y, image->data[j * w + i]);
        }
    }
}

void LCD_ST7735S_Clear(void)
{
    memset(ScreenBuff, 0, sizeof(ScreenBuff));

}
