#include "ssd1306.ops.h"
#include "avr_uart.h"


int main(void) {
    avr_uart_init();
    stdout = &avr_uart_output;
    stdin  = &avr_uart_input_echo;

    printf("SSD1306 OPS 'led' status test\n");

    ssd1306_init();
    ssd1306_init();
    for (uint8_t line = 0; line < SSD1306_NB_LINE; line++) {
        ssd1306_clear(line);
    }
    ssd1306_print(2, "Outlet STATUS :");
    openPowerSwitch_outlet_status(4, 0x05); // led 4&2 : on, 3&1 : off
    printf("done...\n");
    return 0;
}

