#ifndef ST7735S_FONTS_H
#define ST7735S_FONTS_H
#include <stdint.h>
#include "picts.h"


typedef struct {
    long int code;
    const tImage *image;
} tChar;

typedef struct {
    int length;
    const tChar *chars;
} tFont;

extern const tFont Font_8x10;
extern const tFont Font_10x20;
extern const tFont Font_11x22;
extern const tFont Font_13x16;
extern const tFont Font_16x24;
extern const tFont Font_LET_18x26;
extern const tFont Font_20x24;
extern const tFont Font_24x17;
extern const tFont Font_25x27;

#endif //ST7735S_FONTS_H
