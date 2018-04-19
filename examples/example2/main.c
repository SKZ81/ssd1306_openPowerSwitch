#include "ssd1306.ops.h"
#include "avr_uart.h"


int main(void) {
    avr_uart_init();
    stdout = &avr_uart_output;
    stdin  = &avr_uart_input_echo;

    printf("SSD1306 OPS logo test\n");

    ssd1306_init();
    openPowerSwitch_logo();
    printf("done...\n");
    return 0;
}

