# Platform independent library for the ST7735S IPS display
-----
**see st7735s_settings.h list of supported displays* <br>
*`-0.96" IPS mini 160x80` <br> `-1.44" display 128x128` <br> `-WaveShare ST7735S-based 1.8" display 160x128` <br> `-AliExpress/eBay 1.8" display 160x128`*

## How to generate Fonts and Pict
-----
To generate fonts, you can use a cross-platform(Win, Linux) program [LCD-Image-Converter](https://github.com/riuson/lcd-image-converter) <br>
MacOS patched version [LCD-Image-Converter MacOS](https://github.com/cvetaevvitaliy/lcd-image-converter_MacOs) <br>

Web pages
- [Documentation](http://www.riuson.com/lcd-image-converter)
- [Home web site](http://www.riuson.com/lcd-image-converter)



## How to use library
------

Add include 
```c
#include "st7735s.h"
```

First step, need init struct LCD_ST7735_ctx_t and register callback SPI write function and GPIO

```c
LCD_ST7735_ctx_t LCD_ST7735 = {
        .spi_write_data         = SPI_Transmit,                 // register callback for write data to SPI
        .gpio_write_pin         = GPIO_Write,                   // register callback for write GPIO pin
        .reset.gpio_port        = (uint32_t) SPI_RST_GPIO_PORT, // reset pin GPIO port
        .reset.gpio_pin         = SPI_RST_PIN,                  // reset pin GPIO
        .data.gpio_port         = (uint32_t) SPI_DC_GPIO_PORT,  // data pin GPIO port
        .data.gpio_pin          = SPI_DC_PIN,                   // data pin GPIO
        .cs.gpio_port           = (uint32_t) SPI1_CS_GPIO_PORT, // chip select pin GPIO port
        .cs.gpio_pin            = SPI1_CS_PIN,                  // chip select pin GPIO
        .backlight.gpio_port    = (uint32_t) LCD_BLK_GPIO_PORT, // backlight pin GPIO port
        .backlight.gpio_pin     = LCD_BLK_PIN,                  // backlight pin GPIO

};
```

example function for write data to SPI and GPIO

```c
uint8_t SPI_Transmit(uint8_t *pData, uint16_t Size)
{
    if (HAL_SPI_Transmit(&hspi1, pData, Size,0) == HAL_OK)
        return true;
    else
        return false;
}


void GPIO_Write(uint32_t port, uint32_t pin, uint8_t state)
{
    HAL_GPIO_WritePin( (GPIO_TypeDef*)port, pin, state);
}
```

Call  LCD_ST7735S_Init() finction and init display

```c
LCD_ST7735S_Init(&LCD_ST7735);
LCD_ST7735_DrawString("Hello world", 0, 0, &Font_8x10, ST7735_WHITE);
LCD_ST7735S_Update();
```
Important note
Since a buffer is used to send data to the display, you need to call the function whenever you want to update information on the display.
```c
LCD_ST7735S_Update();
```

