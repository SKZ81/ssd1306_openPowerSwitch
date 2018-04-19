#ifndef __SSD1306_OPS_H__
#define __SSD1306_OPS_H__

#include <stdint.h>

#define SSD1306_NB_LINE 8 // 128*64 display

void ssd1306_init();

// text function
// NB : using ssd1306_print() will previously call ssd1306_clear () on the same line
void ssd1306_print(uint8_t line, char* str);

void ssd1306_clear(uint8_t line);

//void ssd1306_pgm_dump(uint8_t line, )

void openPowerSwitch_logo(void);

// display "led" icons on given line, according to the status bits
// NB : outlet 1 : bit0, outlet 2 : bit1, outlet3: bit2, outlet 4: bit3.
void openPowerSwitch_outlet_status(uint8_t line, uint8_t status);
#endif
