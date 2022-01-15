/*+===================================================================
  File:      oled.h

  Summary:   Used for OLED display.

  Kary Wall 1/12/2022.
===================================================================+*/

#include <U8g2lib.h>

#define OLED_CLOCK 15 // Pins for the OLED display
#define OLED_DATA 4
#define OLED_RESET 16

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);

int g_lineHeight = 0;
