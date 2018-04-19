#include "ssd1306.ops.h"
#include "avr_uart.h"


int main(void) {
    avr_uart_init();
    stdout = &avr_uart_output;
    stdin  = &avr_uart_input_echo;

    printf("SSD1306 simple test\n");

    ssd1306_init();
    for (uint8_t line = 0; line < SSD1306_NB_LINE; line++) {
        ssd1306_clear(line);
    }
    ssd1306_print(0, "hello, world !!!");

    return 0;
}

