#ifndef ST7735S_PICTS_H
#define ST7735S_PICTS_H
#include <stdint.h>

typedef struct {
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage;



typedef struct {
    const uint16_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage_RGB;

extern const tImage_RGB Image;
extern const tImage_RGB battery_big;
extern const tImage_RGB Image_Battery;
extern const tImage_RGB Image_Battery_2;
extern const tImage Image_battery_small;
extern const tImage_RGB usb_to_pc;

#endif //ST7735S_PICTS_H
