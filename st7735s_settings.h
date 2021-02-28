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
#ifndef _ST7735S_SETTINGS_H
#define _ST7735S_SETTINGS_H
#include <stdint.h>
#include <stdbool.h>
#include "st7735s.h"


/****************************************
 * For select display, please defined display
 *
 * AliExpress/eBay 1.8" display
 * #define ST7735S_160X128
 *
 * WaveShare ST7735S-based 1.8" display
 * #define ST7735S_160X128_WAWESHARE
 *
 * 1.44" display
 * #define ST7735S_128X128
 *
 * 0.96" IPS mini 160x80
 * #define ST7735S_160X80_MINI
 * **************************************/
#define ST7735S_160X80_MINI_CHINE


#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#if defined(ST7735S_160X128)
/** AliExpress/eBay 1.8" display */
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV)
#elif defined(ST7735S_160X128_WAWESHARE)
/** WaveShare ST7735S-based 1.8" display */
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128
#define ST7735_XSTART 1
#define ST7735_YSTART 2
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_RGB)
#elif defined(ST7735S_128X128)
/** 1.44" display */
#define ST7735_IS_128X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 128
#define ST7735_XSTART 1
#define ST7735_YSTART 2
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR)
#elif defined(ST7735S_160X80_MINI_CHINE)
/** 1.44" display */
#define ST7735_IS_128X128 1
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 80
#define ST7735_XSTART 0
#define ST7735_YSTART 24
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR)
#elif defined(ST7735S_160X80_MINI)
/** 0.96" IPS mini 160x80 */
#define ST7735_IS_160X80 1
#define ST7735_XSTART 1
#define ST7735_YSTART 26
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 80
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR)
#endif

#endif //ST7735S_SETTINGS_H
